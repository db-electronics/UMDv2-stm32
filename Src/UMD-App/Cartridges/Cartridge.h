/*******************************************************************//**
 *  \file Cartridge.h
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

#ifndef CARTRIDGES_CARTRIDGE_H_
#define CARTRIDGES_CARTRIDGE_H_

#include <cstdint>

class Cartridge {
public:
	Cartridge();
	virtual ~Cartridge();

	// NOR_HandleTypeDef hnor1;
	// SRAM_HandleTypeDef hsram2;
	// NOR_HandleTypeDef hnor3;
	// SRAM_HandleTypeDef hsram4;

	//FSMC address offsets
	#define FSMC_CE0_8BF_ADDR				0x60000000U
	#define FSMC_CE1_8BS_ADDR				0x64000000U
	#define FSMC_CE2_16BF_ADDR				0x68000000U
	#define FSMC_CE3_16BS_ADDR				0x6C000000U

	// cartridge methods
	static void init();
	uint8_t readByte(uint32_t address);

};


#endif /* CARTRIDGES_CARTRIDGE_H_ */
