/*******************************************************************//**
 *  \file Cartridge.cpp
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

#include "Cartridge.h"
#include "fsmc.h"

using namespace std;


Cartridge::Cartridge() {
	// TODO Auto-generated constructor stub

}

Cartridge::~Cartridge() {
	// TODO Auto-generated destructor stub
}

void Cartridge::init(void){

}

uint8_t Cartridge::readByte(uint32_t address){
	uint8_t value;
	value = *(volatile uint8_t *)(FSMC_CE1_8BS_ADDR + address);
	return value;
}
