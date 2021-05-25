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

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	param.bus_size = 8;

	GPIO_InitStruct.Pin = GP0_Pin|GP1_Pin|GP4_Pin|GP5_Pin|GP6_Pin|GP7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	set_voltage(vcart_5v);
	set_level_translators(true);

	// initialize cartridge slot registers
	for(int i = 0; i < 3; i++){
		slot.shadow[i] = i;
		write_byte(slot.REG_ADDRESS[i], (uint32_t)(i * slot.SIZE), mem_prg);
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
uint32_t MasterSystem::set_slot_register(const uint32_t& address, uint8_t slot_num){

	// find which page this memory address is in
	uint8_t page = (uint8_t)(address>>14);
	uint32_t mapped_address = SMS_CE + slot.BASE_ADDRESS[slot_num] + (slot.MASK & address);
	if( slot.shadow[slot_num] != page ){
		slot.shadow[slot_num] = page;
		write_byte(slot.REG_ADDRESS[slot_num], page, mem_prg);
	}

	return mapped_address;
}

/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::get_flash_id(void){

	// enable writes to ROM
	write_byte(slot.REG_CTRL, 0x80, mem_prg);
	// call super
	Cartridge::get_flash_id();
	// disable writes to ROM
	write_byte(slot.REG_CTRL, 0x00, mem_prg);
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t MasterSystem::read_byte(uint16_t address, e_memory_type memt){
	uint32_t fsmc_addr = SMS_CE + address;
	uint8_t read;
	read = *(__IO uint8_t *)(fsmc_addr);
	return read;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t MasterSystem::read_byte(uint32_t address, e_memory_type memt){
	uint32_t fsmc_addr = set_slot_register(address, slot.DEFAULT);
	uint8_t read;
	read = *(__IO uint8_t *)(fsmc_addr);
	return read;
}

/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::read_bytes(uint16_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t){
	uint32_t fsmc_addr = SMS_CE + address;
	for(; size != 0; size--){
		*(buf++) = *(__IO uint8_t *)(fsmc_addr++);
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::read_bytes(uint32_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t){
	uint32_t fsmc_addr;
	for(; size != 0; size--){
		fsmc_addr = set_slot_register(address++, slot.DEFAULT);
		*(buf++) = *(__IO uint8_t *)(fsmc_addr);
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::write_byte(uint16_t address, uint8_t data, e_memory_type mem_t){
	uint32_t fsmc_addr = SMS_CE + address;
	*(__IO uint8_t *)(fsmc_addr) = data;
}

/*******************************************************************//**
 *
 **********************************************************************/
void MasterSystem::write_byte(uint32_t address, uint8_t data, e_memory_type mem_t){
	uint32_t fsmc_addr = set_slot_register(address, slot.DEFAULT);
	*(__IO uint8_t *)(fsmc_addr) = data;
}


