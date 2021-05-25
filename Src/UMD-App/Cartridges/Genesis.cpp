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
#include "dma.h"
#include "fsmc.h"

/*******************************************************************//**
 *
 **********************************************************************/
Genesis::Genesis() {}


/*******************************************************************//**
 *
 **********************************************************************/
void Genesis::init(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	param.bus_size = 16;
	param.dma_channel = &hdma_memtomem_dma2_stream1;

	// set nMRES to output and drive low for now to reset cart
	GPIO_InitStruct.Pin = nMRES_Pin|nM3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(nM3_GPIO_Port, nM3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(nMRES_GPIO_Port, nLWR_Pin, GPIO_PIN_RESET);

	// set nLWR to output and drive high
	GPIO_InitStruct.Pin = nLWR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(nLWR_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(nLWR_GPIO_Port, nLWR_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GP0_Pin|GP1_Pin|GP4_Pin|GP5_Pin|GP6_Pin|GP7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	set_voltage(vcart_5v);
	set_level_translators(true);

	HAL_GPIO_WritePin(nMRES_GPIO_Port, nLWR_Pin, GPIO_PIN_SET);
}

/*******************************************************************//**
 *
 **********************************************************************/
void Genesis::erase_flash(bool wait){

	this->write_word((uint32_t)0x0AAA << 1, 0xAA00, mem_prg);
	this->write_word((uint32_t)0x0555 << 1, 0x5500, mem_prg);
	this->write_word((uint32_t)0x0AAA << 1, 0x8000, mem_prg);
	this->write_word((uint32_t)0x0AAA << 1, 0xAA00, mem_prg);
	this->write_word((uint32_t)0x0555 << 1, 0x5500, mem_prg);
	this->write_word((uint32_t)0x0AAA << 1, 0x1000, mem_prg);

	if(wait){
		uint32_t umd_millis = HAL_GetTick();
		while( this->toggle_bit(4) != 4 ){
			if( (HAL_GetTick() - umd_millis) > 250 ){
				umd_millis = HAL_GetTick();
			}
		}
	}

}

/*******************************************************************//**
 *
 **********************************************************************/
void Genesis::get_flash_id(void){
	// mx29f800 software ID detect word mode
	// enter software ID mode
	// genesis writes in big endian mode
	this->write_word((uint32_t)0x0AAA << 1, 0xAA00, mem_prg);
	this->write_word((uint32_t)0x0555 << 1, 0x5500, mem_prg);
	this->write_word((uint32_t)0x0AAA << 1, 0x9000, mem_prg);
	// read manufacturer
	this->flash_info.manufacturer = (uint8_t)this->read_word((uint32_t)0x0000, mem_prg);
	// read device
	this->flash_info.device = (uint8_t)this->read_word((uint32_t)0x0001<<1, mem_prg);
	// exit software ID mode
	this->write_word((uint32_t)0x0000, 0xF000, mem_prg);
	this->find_flash_size();
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t Genesis::toggle_bit(uint16_t attempts){

	uint8_t check = 0;
	uint16_t val, old_val;

	// use 32bit read to start to ensure any mapper is reset to page 0
	old_val = this->read_word((uint32_t)0x0000, mem_prg);
	for(uint16_t i = 0; i < attempts; i++){
		val = this->read_word((uint32_t)0x0000, mem_prg);
		if( old_val == val ){
			check++;
		}else{
			check = 0;
		}
		old_val = val;
	}
	return check;
}

/*******************************************************************//**
* 8 BIT OPERATIONS
************************************************************************
 * single 8 bit write at 32bit address
 **********************************************************************/
void Genesis::write_byte(uint32_t address, uint8_t data, e_memory_type mem_t){

	uint32_t fsmc_addr;

	switch(mem_t){
	case mem_prg:
		break;
	case mem_ctrl:
		if( address >= TIME_LOWER_BOUND and address <= TIME_UPPER_BOUND ){
			// ensure byte writes are always on odd addresses
			fsmc_addr = TIME_CE | address | 1;
			HAL_GPIO_WritePin(nLWR_GPIO_Port, nLWR_Pin, GPIO_PIN_RESET);
			*(__IO uint8_t *)(fsmc_addr) = data;
			HAL_GPIO_WritePin(nLWR_GPIO_Port, nLWR_Pin, GPIO_PIN_SET);
		}
		break;
	default:
		break;
	}
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
	case mem_bram:
		if( address >= BRAM_LOWER_BOUND and address <= BRAM_UPPER_BOUND ){
			this->enable_bram_writes();
			fsmc_addr = GEN_CE | address;
			read = *(__IO uint16_t *)(fsmc_addr);
			this->disable_bram();
		}

	default:
		fsmc_addr = GEN_CE | address;
		read = *(__IO uint16_t *)(fsmc_addr);
		break;
	}

	// convert endianness
	read = BIG_END_WORD(read);
	return read;
}

/*******************************************************************//**
 * multiple 16bit reads at 32bit address
 **********************************************************************/
void Genesis::read_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t, bool dma){
	uint32_t fsmc_addr;
	uint16_t read;

	switch(mem_t){
	case mem_prg:
	default:
		fsmc_addr = GEN_CE | address;
		if(dma){
			HAL_DMA_Start(param.dma_channel, fsmc_addr, (uint32_t)buf, size);
			while(HAL_DMA_GetState(param.dma_channel) != HAL_DMA_STATE_READY );
			this->swap_bytes(buf, size);
		}else{
			for(; size > 0; size -= 2){
				read = *(__IO uint16_t *)(fsmc_addr);
				*(buf++) = BIG_END_WORD(read);
				fsmc_addr += 2;
			}
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
		*(__IO uint16_t *)(fsmc_addr) = BIG_END_WORD(data);
		break;
	default:
		fsmc_addr = GEN_CE + address;
		*(__IO uint16_t *)(fsmc_addr) = BIG_END_WORD(data);
		break;
	}
}

/*******************************************************************//**
 * single 16 bit program at 32bit address
 **********************************************************************/
void Genesis::program_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t){

	for(; size > 0; size -= 2){
		this->write_word((uint32_t)0x0AAA << 1, 0xAA00, mem_t);
		this->write_word((uint32_t)0x0555 << 1, 0x5500, mem_t);
		this->write_word((uint32_t)0x0AAA << 1, 0xA000, mem_t);
		// write the data
		this->write_word(address, BIG_END_WORD(*buf), mem_t);
		buf++;
		// wait for completion
		while(this->toggle_bit(2) != 2);
	}
}
