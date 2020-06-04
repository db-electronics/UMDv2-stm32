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

#include "MasterSystem.h"
#include "fsmc.h"

/*******************************************************************//**
 *
 **********************************************************************/
MasterSystem::MasterSystem() {}


/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::init(void){
	set_voltage(vcart_5v);
	set_level_translators(true);
	shadow.slot0 = 0;
	shadow.slot1 = 1;
	shadow.slot2 = 2;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint32_t MasterSystem::set_slot_register(const uint32_t& address, uint8_t slot_num){

	uint32_t mapped_address;

	// find which page this memory address is in
	uint8_t page = (uint8_t)(address>>14);


	switch(slot_num){
	case 0:
		mapped_address = UMD_CE0 + SLOT_0_BASE_ADDRESS + (0x00003FFF & address);
		shadow.slot0 = page;
		write_rom_byte(SLOT_0_REG_ADDRESS, page);
		break;
	case 1:
		mapped_address = UMD_CE0 + SLOT_1_BASE_ADDRESS + (0x00003FFF & address);
		shadow.slot1 = page;
		write_rom_byte(SLOT_2_REG_ADDRESS, page);
		break;
	case 2:
		mapped_address = UMD_CE0 + SLOT_2_BASE_ADDRESS + (0x00003FFF & address);
		shadow.slot2 = page;
		write_rom_byte(SLOT_2_REG_ADDRESS, page);
		break;
	default:
		mapped_address = UMD_CE0 + SLOT_2_BASE_ADDRESS + (0x00003FFF & address);
		shadow.slot2 = page;
		write_rom_byte(SLOT_2_REG_ADDRESS, page);
		break;
	}

	return mapped_address;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t MasterSystem::read_rom_byte(const uint16_t& address){
	uint8_t value;
	uint32_t fmsc_addr = address + UMD_CE0;
	value = *(__IO uint8_t *)(fmsc_addr);
	return value;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t MasterSystem::read_rom_byte(const uint32_t& address){
	uint8_t value;
	uint32_t fmsc_addr = address + UMD_CE0;
	value = *(__IO uint8_t *)(fmsc_addr);
	return value;
}

/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::read_rom_byte(const uint32_t& address, uint8_t *buf, uint16_t size){
	uint32_t fmsc_addr = address + UMD_CE0;
	for(; size != 0U; size--){
		*buf = *(__IO uint8_t *)(fmsc_addr);
		buf++;
		fmsc_addr++;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::write_rom_byte(const uint32_t& address, uint8_t data){
	uint32_t fmsc_addr = address + UMD_CE0;
	*(__IO uint8_t *)(fmsc_addr) = data;
}


