/*******************************************************************//**
 *  \file UMD.h
 *  \author René Richard
 *  \brief This program provides a serial interface over USB to the
 *         Universal Mega Dumper.
 *
 *  \copyright This file is part of Universal Mega Dumper.
 *
 *   Universal Mega Dumper is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Universal Mega Dumper is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Universal Mega Dumper.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UMD_H_
#define UMD_H_

#include <cstdint>
#include <string>
#include "Cartridges/Cartridge.h"

#define LED_SHIFT_DIR_LEFT	0
#define LED_SHIFT_DIR_RIGHT 1

using namespace std;

class UMD{

public:
	UMD();

	// run is the only method visible from main.cpp, it never returns
	void run(void);

private:

	// pointer to cartridge objects
	Cartridge *cart;

	// initializers
	void init(void);
	void setCartridgeType(uint8_t mode);

	// USB transmit methods
	void sendUSB(string);
	void sendUSB(uint8_t* buf, uint16_t size);
	uint16_t receiveUSB(uint8_t* buf, uint16_t size);

	// LED methods
	void setLEDs(uint8_t LEDs);
	void shiftLEDs(uint8_t dir);

};

#endif /* UMD_H_ */
