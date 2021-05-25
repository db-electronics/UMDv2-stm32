/*******************************************************************//**
 *  \file Genesis.h
 *  \author btrim
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

#ifndef CARTRIDGES_GENESIS_H_
#define CARTRIDGES_GENESIS_H_

#include "Cartridge.h"


/*******************************************************************//**
 * \class Genesis
 * \brief
 **********************************************************************/
class Genesis : public Cartridge
{
public:

	/*******************************************************************//**
	 * \brief Constructor
	 **********************************************************************/
	Genesis();

	/*******************************************************************//**
	 * \brief setup the UMD's hardware for the current cartridge
	 * \return void
	 **********************************************************************/
	void init();
	void erase_flash(bool wait);
	void get_flash_id(void);
	uint16_t toggle_bit(uint16_t attempts);

	// 8 bit operations, default to CE0, the base cart implementation ignores mem_t
	void write_byte(uint32_t address, uint8_t data, e_memory_type mem_t);

	// 16 bit operations default to CE3, the base cart implementation ignores mem_t
	uint16_t read_word(uint32_t address, e_memory_type mem_t);
	void read_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t, bool dma = false);
	void write_word(uint32_t address, uint16_t data, e_memory_type mem_t);
	void program_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t);

	/*******************************************************************//**
	 * \brief Pins
	 **********************************************************************/
	// nLWR on SEL1
	#define nLWR_Pin GPIO_PIN_8
	#define nLWR_GPIO_Port GPIOG
	// nMRES on GP8
	#define nMRES_Pin GPIO_PIN_6
	#define nMRES_GPIO_Port GPIOC
	// nM3 on GP2
	#define nM3_Pin GPIO_PIN_4
	#define nM3_GPIO_Port GPIOC

private:

	//macro to flip endianness of words
	#define BIG_END_WORD(w) ((w & 0xff) << 8) | ((w & 0xff00) >> 8)
	void inline swap_bytes(uint16_t *buf, uint16_t size){
		for(; size > 0; size -= 2){
			*(buf) = BIG_END_WORD(*buf);
			buf++;
		}
	};

	// enable/disable bram latche
	void inline enable_bram_writes(void){ this->write_byte(0xA130F1, 0x03, mem_ctrl); };
	void inline disable_bram(void){ this->write_byte(0xA130F1, 0x00, mem_ctrl); };

	const uint32_t TIME_CE = UMD_CE0;
	const uint32_t TIME_LOWER_BOUND = 0xA13000;
	const uint32_t TIME_UPPER_BOUND = 0xA130FF;

	const uint32_t GEN_CE = UMD_CE3;
	const uint32_t BRAM_LOWER_BOUND = 0x200000;
	const uint32_t BRAM_UPPER_BOUND = 0x3FFFFF;

};

#endif /* CARTRIDGES_GENESIS_H_ */
