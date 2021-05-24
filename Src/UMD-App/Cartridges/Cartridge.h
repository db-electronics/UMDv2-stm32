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

/*******************************************************************//**
 * \class cartridge
 * \brief base cartridge class from which all other arts inherit
 **********************************************************************/
class Cartridge {
public:
	Cartridge();
	virtual ~Cartridge();

	struct s_param{
		uint8_t id;
	}param;

	struct s_flash_info {
		uint8_t manufacturer;
		uint8_t device;
		uint32_t size;
	} flash_info;

	// common methods
	// Cartridge Methods
	enum eVoltage : uint8_t {
		vcart_off=0, vcart_3v3, vcart_5v
	};
	eVoltage vcart;

	void set_voltage(eVoltage v);
	void set_level_translators(bool enable);

	// memory types on cartridges
	enum e_memory_type : uint8_t {
		mem_prg=0, mem_chr, mem_ram, mem_bram, mem_ctrl
	};

	// MCP23008 id register address on cartridge adapters
	const uint8_t I2C_CART_ID_ADDRESS = 0x20 << 1;
	uint8_t I2C_CART_ID_GPIO_REG = 0x09;
	uint8_t get_adapter_id(void);

	// NOR_HandleTypeDef hnor1;
	// SRAM_HandleTypeDef hsram2;
	// NOR_HandleTypeDef hnor3;
	// SRAM_HandleTypeDef hsram4;


	// cartridge methods
	virtual void init(void);

	virtual void get_flash_id(void);
	void find_flash_size(void);

	// 8 bit operations, default to CE0, the base cart implementation ignores mem_t
	virtual uint8_t read_byte(uint16_t address, e_memory_type mem_t);
	virtual uint8_t read_byte(uint32_t address, e_memory_type mem_t);
	virtual void read_bytes(uint16_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t);
	virtual void read_bytes(uint32_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t);

	virtual void write_byte(uint16_t address, uint8_t data, e_memory_type mem_t);
	virtual void write_byte(uint32_t address, uint8_t data, e_memory_type mem_t);

	// 16 bit operations default to CE3, the base cart implementation ignores mem_t
	virtual uint16_t read_word(uint32_t address, e_memory_type mem_t);
	virtual void read_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t);
	virtual void write_word(uint32_t address, uint16_t data, e_memory_type mem_t);

protected:
	//FSMC address offsets
	const uint32_t UMD_CE0 = 0x60000000U;
	const uint32_t UMD_CE1 = 0x64000000U;
	const uint32_t UMD_CE2 = 0x68000000U;
	const uint32_t UMD_CE3 = 0x6C000000U;
	uint32_t default_ce = UMD_CE0;

};


#endif /* CARTRIDGES_CARTRIDGE_H_ */
