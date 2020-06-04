/*******************************************************************//**
 *  \file MasterSystem.h
 *  \author René Richard
 *  \brief This program allows to read and write to various game cartridges.
 *         The UMD base class handles all generic cartridge operations, console
 *         specific operations are handled in derived classes.
 *
 * \copyright This file is part of Universal Mega Dumper.
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

#ifndef CARTRIDGES_MASTERSYSTEM_H_
#define CARTRIDGES_MASTERSYSTEM_H_

#include "Cartridge.h"

/*******************************************************************//**
 * \class MasterSystem
 * \brief
 **********************************************************************/
class MasterSystem : public Cartridge
{
public:
	/*******************************************************************//**
	 * \brief Constructor
	 **********************************************************************/
	MasterSystem();

	/*******************************************************************//**
	 * \brief setup the UMD's hardware for the current cartridge
	 * \return void
	 **********************************************************************/
	void init();

	// fixed read without any mapping consideration
	uint8_t read_rom_byte(const uint16_t& address);

	uint8_t read_rom_byte(const uint32_t& address);

	void read_rom_byte(const uint32_t& address, uint8_t *buf, uint16_t size);

	void write_rom_byte(const uint32_t& address, uint8_t data);

private:

	struct {
		uint32_t last_address;
		uint8_t slot0;
		uint8_t slot1;
		uint8_t slot2;
	}shadow;

	const uint16_t SLOT_0_BASE_ADDRESS = 0x0000;
	const uint16_t SLOT_1_BASE_ADDRESS = 0x4000;
	const uint16_t SLOT_2_BASE_ADDRESS = 0x8000;

	const uint16_t SLOT_0_REG_ADDRESS = 0xFFFD;
	const uint16_t SLOT_1_REG_ADDRESS = 0xFFFE;
	const uint16_t SLOT_2_REG_ADDRESS = 0xFFFF;

	uint32_t set_slot_register(const uint32_t& address, uint8_t slot_num);

};

#endif /* CARTRIDGES_MASTERSYSTEM_H_ */
