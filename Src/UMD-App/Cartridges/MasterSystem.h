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

	virtual void get_flash_id(void);

	// fixed read without any mapping consideration
	// 16 bit address reads ignore the mapping scheme
	uint8_t read_byte(uint16_t address, e_memory_type mem_t);
	uint8_t read_byte(uint32_t address, e_memory_type mem_t);
	void read_bytes(uint16_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t);
	void read_bytes(uint32_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t);

	void write_byte(uint16_t address, uint8_t data, e_memory_type mem_t);
	void write_byte(uint32_t address, uint8_t data, e_memory_type mem_t);

private:

	const uint32_t SMS_CE = UMD_CE0;

	struct {
		const uint32_t BASE_ADDRESS[3] = {0x00000000, 0x00004000, 0x00008000};
		const uint16_t REG_ADDRESS[3] = {0xFFFD, 0xFFFE, 0xFFFF};
		const uint16_t REG_CTRL = 0xFFFC;
		const uint8_t DEFAULT = 2;
		const uint16_t SIZE = 0x4000;
		const uint16_t MASK = 0x3FFF;
		uint8_t shadow[3];
	}slot;

	uint32_t set_slot_register(const uint32_t& address, uint8_t slot_num);

};

#endif /* CARTRIDGES_MASTERSYSTEM_H_ */
