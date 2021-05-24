/*******************************************************************//**
 *  \file Genesis.cpp
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

#include "Genesis.h"
#include "fsmc.h"

/*******************************************************************//**
 *
 **********************************************************************/
Genesis::Genesis() {}


/*******************************************************************//**
 *
 **********************************************************************/
void Genesis::init(void){
	set_voltage(vcart_5v);
	set_level_translators(true);
}

/*******************************************************************//**
 *
 **********************************************************************/
void Genesis::get_flash_id(void){
	//mx29f800 software ID detect word mode
	// enter software ID mode
	write_word((uint32_t)0x0AAA<<1, 0x00AA, mem_prg);
	write_word((uint32_t)0x0555<<1, 0x0055, mem_prg);
	write_word((uint32_t)0x0AAA<<1, 0x0090, mem_prg);
	// read manufacturer
	flash_info.manufacturer = (uint8_t)read_word((uint32_t)0x0000, mem_prg);
	// read device
	flash_info.device = (uint8_t)read_word((uint32_t)0x0001<<1, mem_prg);
	// exit software ID mode
	write_word((uint32_t)0x0000, 0x00F0, mem_prg);
	find_flash_size();
}


/*******************************************************************//**
* 16 BIT OPERATIONS
************************************************************************
 * single 16bit read at 32bit address
 **********************************************************************/
uint16_t Genesis::read_word(uint32_t address, e_memory_type mem_t){
	uint32_t fsmc_addr;
	uint16_t read;

	switch(mem_t){
	case mem_prg:
		fsmc_addr = GEN_CE + address;
		read = *(__IO uint16_t *)(fsmc_addr);
		break;
	default:
		fsmc_addr = GEN_CE + address;
		read = *(__IO uint16_t *)(fsmc_addr);
		break;
	}

	read = SWAP_BYTES(read);
	return read;
}

/*******************************************************************//**
 * multiple 16bit reads at 32bit address
 **********************************************************************/
void Genesis::read_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t){
	uint32_t fsmc_addr;
	uint16_t read;

	switch(mem_t){
	case mem_prg:
		fsmc_addr = GEN_CE + address;
		for(; size != 0; size -= 2){
			read = *(__IO uint16_t *)(fsmc_addr);
			*(buf++) = SWAP_BYTES(read);
			fsmc_addr += 2;
		}
		break;
	default:
		fsmc_addr = GEN_CE + address;
		for(; size != 0; size -= 2){
			read = *(__IO uint16_t *)(fsmc_addr);
			*(buf++) = SWAP_BYTES(read);
			fsmc_addr += 2;
		}
		break;
	}

}

/*******************************************************************//**
 * single 16 bit write at 32bit address
 **********************************************************************/
void Genesis::write_word(uint32_t address, uint16_t data, e_memory_type mem_t){
	uint32_t fsmc_addr;

	switch(mem_t){
	case mem_prg:
		fsmc_addr = GEN_CE + address;
		*(__IO uint16_t *)(fsmc_addr) = SWAP_BYTES(data);
		break;
	default:
		fsmc_addr = GEN_CE + address;
		*(__IO uint16_t *)(fsmc_addr) = SWAP_BYTES(data);
		break;
	}

}
