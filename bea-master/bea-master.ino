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
	char c = '\0';
	char buf[11];
	int pos;
	for (pos = 0; pos < sizeof(buf) / sizeof(buf[0]); pos++) {
		while (Serial.available() == 0);

		c = Serial.read();
		if (c == '\n' || c == '\r') {
			if (pos == 0) {
				// empty string
				return BEA_DEFAULT_LEN;
			} else {
				break;
			}
		} else if (c > '9' || c < '0') {
			// Bad integer
			return BEA_DEFAULT_LEN;
		}
		buf[pos] = c;
	}
	buf[pos] = '\0';

	while (Serial.available() != 0) {
		(void) Serial.read();
	}
	return atoi(buf);
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
	int byte_count;
	int bit_err_count = 0;
  int chunk;
	uint8_t b;

	Serial.println("# Enter the amount of data in bytes to send for each frequency and press the ");
	Serial.print("# enter key, or simply press the enter key for the default amount of ");
	Serial.print(BEA_DEFAULT_LEN);
	Serial.println(" bytes.");
	byte_count = readInt();

	for (int f = 0; f < sizeof(BEA_FREQs) / sizeof(BEA_FREQs[0]); f++) {
		Wire.setClock(BEA_FREQs[f]);

		// Write prng sequence to slave
		Serial.print("# Sending ");
		Serial.print(byte_count);
		Serial.print(" bytes with frequency ");
		Serial.print(BEA_FREQs[f]);
		Serial.println(" Hz...");
    for (int j = byte_count; j > 0; j -= BUFFER_LENGTH) {
      chunk = (byte_count > BUFFER_LENGTH) ? BUFFER_LENGTH : byte_count;
  		Wire.beginTransmission(BEA_I2C_ADDRESS);
  		for (int i = 0; i < chunk; i++) {
  			Wire.write(random(256));
  		}
  		Wire.endTransmission();
    }
		// Read prng sequence from slave and count bit errors
		Serial.println("# Send complete, receiving...");
    for (int j = byte_count; j > 0; j -= BUFFER_LENGTH) {
      chunk = (byte_count > BUFFER_LENGTH) ? BUFFER_LENGTH : byte_count;
  		Wire.requestFrom(BEA_I2C_ADDRESS, chunk);
  		while (Wire.available()) {
  			b = Wire.read();
  			for (int r = random(256) ^ b; r != 0; r >>= 1) {
  				if (r & 0x1) {
  					bit_err_count++;
  				}
  			}
  		}
    }
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
