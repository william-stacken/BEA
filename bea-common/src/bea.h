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

#ifndef BEA_INCLUDE_H_
#define BEA_INCLUDE_H_

#define BEA_I2C_ADDRESS    (0x12)

#define BEA_RAND_SEED      (1234567)

#define BEA_DEFAULT_LEN    (1000000)

// Bus clock frequencies in Hz
static const long int BEA_FREQs[] = {50000, 100000, 200000, 400000};

#endif // BEA_INCLUDE_H_
