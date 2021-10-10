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

static long int byte_count = 0;
static long int bit_err_count = 0;

static bool isRecv = false;

void recvCallback(int howMany)
{
	uint8_t b;

	if (byte_count <= 0) {
		Serial.println("# Receiving... ");
		BEA_DBG_PRINT("# Received: ");
		byte_count = 0;
		isRecv = true;
	}

	while (Wire.available()) {
		b = Wire.read();
		BEA_DBG_PRINT(b);
		BEA_DBG_PRINT(" ");
		byte_count++;
		for (int r = random(256) ^ b; r != 0; r >>= 1) {
			if (r & 0x1) {
				bit_err_count++;
			}
		}
	}
}

// Will be called by the master after it has finished sending data
void sendCallback()
{
	uint8_t b;
	long int chunk = (byte_count > BUFFER_LENGTH) ? BUFFER_LENGTH : byte_count;
	Serial.println("Sending chunk.");
	if (isRecv) {
		BEA_DBG_PRINTLN();
		Serial.print("# Recieve complete, sending ");
		Serial.print(byte_count);
		Serial.println(" bytes...");
		BEA_DBG_PRINT("# Sent: ");
		isRecv = false;
	}
	for (long int i = 0; i < chunk; i++) {
		b = random(256);
		BEA_DBG_PRINT(b);
		BEA_DBG_PRINT(" ");
		Wire.write(b);
	}
	byte_count -= chunk;

	if (byte_count <= 0) {
		// Print resulting number of timeouts three times for redundancy
		Serial.println("# Number of timeouts:");
		Serial.println(bea_timeouts);
		Serial.println(bea_timeouts);
		Serial.println(bea_timeouts);
		bea_timeouts = 0;

		// Print resulting bit error count three times for redundancy
		BEA_DBG_PRINTLN();
		Serial.println("# Bit error count in master sequence:");
		Serial.println(bit_err_count);
		Serial.println(bit_err_count);
		Serial.println(bit_err_count);
		bit_err_count = 0;
	}
}

void setup()
{
	randomSeed(BEA_RAND_SEED);

	Serial.begin(115200);
	Wire.begin(BEA_I2C_ADDRESS);
	Wire.setWireTimeout(100000, true);
	Wire.onReceive(recvCallback);
	Wire.onRequest(sendCallback);
	bea_timeouts = 0;

	Serial.println("# BEA slave ready");
}

void loop()
{
	// Do nothing
	delay(100);
}
