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

	struct _param{
		uint8_t id;
	}param;

	// common methods
	// Cartridge Methods
	enum eVoltage : uint8_t {vcart_off=0, vcart_3v3, vcart_5v};
	eVoltage vcart;
	void set_voltage(eVoltage v);
	void set_level_translators(bool enable);

	// MCP23008 id register address on cartridge adapters
	const uint8_t I2C_CART_ID_ADDRESS = 0x20 << 1;
	uint8_t I2C_CART_ID_GPIO_REG = 0x09;
	uint8_t get_adapter_id(void);

	// NOR_HandleTypeDef hnor1;
	// SRAM_HandleTypeDef hsram2;
	// NOR_HandleTypeDef hnor3;
	// SRAM_HandleTypeDef hsram4;

	//FSMC address offsets
	const uint32_t FSMC_CE0_8BS_ADDR = 0x60000000U;
	const uint32_t FSMC_CE1_8BS_ADDR = 0x64000000U;
	const uint32_t FSMC_CE2_8BS_ADDR = 0x68000000U;
	const uint32_t FSMC_CE3_16BS_ADDR = 0x6C000000U;
	const uint32_t UMD_CE0 = FSMC_CE0_8BS_ADDR;
	const uint32_t UMD_CE1 = FSMC_CE1_8BS_ADDR;
	const uint32_t UMD_CE2 = FSMC_CE2_8BS_ADDR;
	const uint32_t UMD_CE3 = FSMC_CE3_16BS_ADDR;

	// cartridge methods
	virtual void init(void);

	virtual uint8_t read_byte(uint32_t address, uint32_t ce);
	virtual void read_byte_block(uint32_t address, uint8_t *buf, uint16_t size, uint32_t ce);
	virtual void write_byte(uint32_t address, uint8_t data, uint32_t ce);

};


#endif /* CARTRIDGES_CARTRIDGE_H_ */
