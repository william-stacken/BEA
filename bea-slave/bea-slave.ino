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

static int byte_count = 0;
static int bit_err_count = 0;

void recvCallback(int howMany)
{
	int len = 0;
	uint8_t b;

	while (Wire.available()) {
		b = Wire.read();
		len++;
		for (int r = random(256) ^ b; r != 0; r >>= 1) {
			if (r & 0x1) {
				bit_err_count++;
			}
		}
	}
	byte_count += len;
}

// Will be called by the master after it has finished sending data
void sendCallback()
{
	uint8_t b;

	// Print resulting bit error count three times for redundancy
	Serial.println();
	Serial.print("# Bit error count in master sequence:");
	Serial.println(bit_err_count);
	Serial.println(bit_err_count);
	Serial.println(bit_err_count);
	bit_err_count = 0;

	Serial.print("# Sending ");
	Serial.print(byte_count);
	Serial.println(" bytes...");
	for (int i = 0; i < byte_count; i++) {
		Wire.write(random(256));
	}
	byte_count = 0;

	Serial.println("# Send complete.");
	Serial.println();
}

void setup()
{
	randomSeed(BEA_RAND_SEED);

	Serial.begin(9600);
	Wire.begin(BEA_I2C_ADDRESS);
	Wire.onReceive(recvCallback);
	Wire.onRequest(sendCallback);

	Serial.println("# BEA slave ready");
}

void loop()
{
	// Do nothing
	delay(100);
}
