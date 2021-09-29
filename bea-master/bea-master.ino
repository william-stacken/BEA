//
// Copyright (C) 2021 William Stackenäs <w.stackenas@gmail.com>
//
// This file is part of BEA.
//
// BEA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BEA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <bea.h>
#include <bea/Wire.h>

long int readInt()
{
	char c;
	char buf[11];
	int pos;
	long int res = 0;
	long int e = 1;
	for (pos = 0; pos < sizeof(buf) / sizeof(buf[0]); pos++) {
		while (Serial.available() == 0);

		c = Serial.read();
		if (c == '\n' || c == '\r') {
			if (pos == 0) {
				// empty string
				return -1;
			} else {
				break;
			}
		} else if (c > '9' || c < '0') {
			// Bad integer
			return -1;
		}
		buf[pos] = c;
	}
	for (int i = pos - 1; i >= 0; i--) {
		res += (long int)(buf[i] - '0') * e;
		e *= 10;
	}

	return res;
}

void setup()
{
	randomSeed(BEA_RAND_SEED);

	Serial.begin(9600);
	Wire.begin();

	Serial.println("# BEA master ready");
}

void loop()
{
	long int byte_count;
	long int bit_err_count = 0;
	long int chunk;
	int attempts;
	uint8_t b;
	int start, finish;

	// Clear the serial receive buffer
	while (Serial.available() != 0) {
		(void) Serial.read();
	}

	Serial.println("# Enter the amount of data in bytes to send for each frequency and press the ");
	Serial.print("# enter key, or simply press the enter key for the default amount of ");
	Serial.print(BEA_DEFAULT_LEN);
	Serial.println(" bytes.");
	byte_count = readInt();
	if (byte_count < 0) {
		byte_count = BEA_DEFAULT_LEN;
	}

	Serial.println("# Enter the frequency id and press the enter key. The following frequencies are available.");
	Serial.println("# id\tfrequence");
	for (int i = 0; i < sizeof(BEA_FREQs) / sizeof(BEA_FREQs[0]); i++) {
		Serial.print("# ");
		Serial.print(i);
		Serial.print("\t");
		Serial.print(BEA_FREQs[i]);
		Serial.println(" Hz");
	}
	Serial.println("# Or simply press enter for ALL frequencies.");
	start = readInt();
	if (start < 0 || start >= sizeof(BEA_FREQs) / sizeof(BEA_FREQs[0])) {
		start = 0;
		finish = sizeof(BEA_FREQs) / sizeof(BEA_FREQs[0]);
	} else {
		finish = start + 1;
	}

	for (int f = start; f < finish; f++) {
		Wire.setClock(BEA_FREQs[f]);

		// Write prng sequence to slave
		Serial.print("# Sending ");
		Serial.print(byte_count);
		Serial.print(" bytes with frequency ");
		Serial.print(BEA_FREQs[f]);
		Serial.println(" Hz...");
		BEA_DBG_PRINT("# Sent: ");
		for (long int j = byte_count; j > 0; j -= BUFFER_LENGTH) {
			chunk = (j > BUFFER_LENGTH) ? BUFFER_LENGTH : j;
			for (attempts = 0; attempts < BEA_I2C_ATTEMPTS; attempts++) {
				Wire.beginTransmission(BEA_I2C_ADDRESS);
				for (long int i = 0; i < chunk; i++) {
					b = random(256);
					BEA_DBG_PRINT(b);
					BEA_DBG_PRINT(" ");
					Wire.write(b);
				}
				if (Wire.endTransmission() == 0) {
					break;
				}
			}
			if (attempts >= BEA_I2C_ATTEMPTS) {
				BEA_DBG_PRINTLN();
				Serial.print("ERROR: Could not write to slave device after ");
				Serial.print(attempts);
				Serial.println(" attempts");
				return;
			}
		}
		BEA_DBG_PRINTLN();

		// Read prng sequence from slave and count bit errors
		Serial.println("# Send complete, receiving...");
		BEA_DBG_PRINT("# Received: ");
		for (long int j = byte_count; j > 0; j -= BUFFER_LENGTH) {
			chunk = (j > BUFFER_LENGTH) ? BUFFER_LENGTH : j;
			for (attempts = 0; attempts < BEA_I2C_ATTEMPTS; attempts++) {
				if (Wire.requestFrom(BEA_I2C_ADDRESS, chunk) <= 0) {
					continue;
				}
				while (Wire.available()) {
					b = Wire.read();
					BEA_DBG_PRINT(b);
					BEA_DBG_PRINT(" ");
					for (int r = random(256) ^ b; r != 0; r >>= 1) {
						if (r & 0x1) {
							bit_err_count++;
						}
					}
				}
				break;
			}
			if (attempts >= BEA_I2C_ATTEMPTS) {
				BEA_DBG_PRINTLN();
				Serial.print("ERROR: Could not read from slave device after ");
				Serial.print(attempts);
				Serial.println(" attempts");
				return;
			}
		}
		BEA_DBG_PRINTLN();

		// Print resulting bit error count three times for redundancy
		Serial.println("# Bit error count in slave sequence:");
		Serial.println(bit_err_count);
		Serial.println(bit_err_count);
		Serial.println(bit_err_count);
		bit_err_count = 0;
	}

	Serial.println("# Experiment complete!");
	Serial.println();
}
