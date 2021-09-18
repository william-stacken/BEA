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

extern "C" {
  #include <bea.h>
  #include <bea/twi.h>
}

static int seq_count = 0;
static int bit_err_count = 0;

void recvCallback(uint8_t *data, int len)
{
  if (data == NULL || len < 0) {
    return;
  }

  for (int i = 0; i < len; i++) {
    for (int r = random(256) ^ data[i]; r != 0; r >>= 1) {
      if (r & 0x1) {
        bit_err_count++;
      }
    }
  }
  seq_count += len;
}

// Will be called by the master after it has finished sending data
void sendCallback()
{
  uint8_t b;
  // Print resulting bit error count three times for redundancy
  Serial.println(bit_err_count);
  Serial.println(bit_err_count);
  Serial.println(bit_err_count);
  bit_err_count = 0;

  randomSeed(BEA_RAND_SEED);

  for (int i = 0; i < seq_count; i++) {
    b = random(256);
    twi_transmit(&b, 1)
  }
}

void setup()
{
	Serial.begin(9600);

  randomSeed(BEA_RAND_SEED);

	twi_init();
  twi_setAddress(BEA_I2C_ADDRESS);
  twi_attachSlaveRxEvent(recvCallback);
  twi_attachSlaveTxEvent(sendCallback);

	Serial.println("BEA slave ready");
}

void loop()
{
  delay(100);
}
