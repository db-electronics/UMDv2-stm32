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

#include "Cartridge.h"
#include "dma.h"
#include "fsmc.h"
#include "i2c.h"

/*******************************************************************//**
 *
 **********************************************************************/
Cartridge::Cartridge() {}

/*******************************************************************//**
 *
 **********************************************************************/
Cartridge::~Cartridge() {

}

/*******************************************************************//**
 *
 **********************************************************************/
void Cartridge::init(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	param.id = 0;
	param.bus_size = 8;
	param.dma_channel = &hdma_memtomem_dma2_stream0; // default to 8bit dma channel

	// turn off the voltage to the cart
	set_voltage(vcart_off);
	set_level_translators(false);

	// configure all GPIO as inputs to be safe

	GPIO_InitStruct.Pin = GP0_Pin|GP1_Pin|GP4_Pin|GP5_Pin|GP6_Pin|GP7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GP2_Pin|GP3_Pin|GP8_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SEL0_Pin|SEL1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/*******************************************************************//**
 *
 **********************************************************************/
void Cartridge::erase_flash(bool wait){

	// first write goes through mapper to ensure page gets reset to 0
	this->write_byte((uint32_t)0x0AAA, (uint8_t)0xAA, mem_prg);
	this->write_byte((uint16_t)0x0555, (uint8_t)0x55, mem_prg);
	this->write_byte((uint16_t)0x0AAA, (uint8_t)0x80, mem_prg);
	this->write_byte((uint16_t)0x0AAA, (uint8_t)0xAA, mem_prg);
	this->write_byte((uint16_t)0x0555, (uint8_t)0x55, mem_prg);
	this->write_byte((uint16_t)0x0AAA, (uint8_t)0x10, mem_prg);

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
void Cartridge::get_flash_id(void){
	// mx29f800 software ID detect byte mode
	// enter software ID mode
	this->write_byte((uint16_t)0x0AAA, (uint8_t)0xAA, mem_prg);
	this->write_byte((uint16_t)0x0555, (uint8_t)0x55, mem_prg);
	this->write_byte((uint16_t)0x0AAA, (uint8_t)0x90, mem_prg);
	// read manufacturer
	this->flash_info.manufacturer = read_byte((uint16_t)0x0000, mem_prg);
	// read device
	this->flash_info.device = read_byte((uint16_t)0x0001, mem_prg);
	// exit software ID mode
	this->write_byte((uint16_t)0x0000, (uint8_t)0xF0, mem_prg);
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t Cartridge::toggle_bit(uint16_t attempts){

	uint8_t check = 0;
	uint8_t val, old_val;

	// use 32bit read to start to ensure any mapper is reset to page 0
	old_val = this->read_byte((uint32_t)0x0000, mem_prg);
	for(uint16_t i = 0; i < attempts; i++){
		val = this->read_byte((uint16_t)0x0000, mem_prg);
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
 * The get_flash_size() function returns the flash size
 **********************************************************************/
void Cartridge::find_flash_size(void)
{
    switch( this->flash_info.manufacturer ){
        // microchip
        case 0xBF:
            switch( this->flash_info.device )
            {
                case 0x6D: // SST39VF6401B
                case 0x6C: // SST39VF6402B
                	this->flash_info.size = 0x800000;
                    break;
                case 0x5D: // SST39VF3201B
                case 0x5C: // SST39VF3202B
                case 0x5B: // SST39VF3201
                case 0x5A: // SST39VF3202
                	this->flash_info.size = 0x400000;
                    break;
                case 0x4F: // SST39VF1601C
                case 0x4E: // SST39VF1602C
                case 0x4B: // SST39VF1601
                case 0x4A: // SST39VF1602
                	this->flash_info.size = 0x200000;
                    break;
                default:
                    break;
            }
            break;

        // macronix
        case 0xC2:
            switch( this->flash_info.device )
            {
                // chips which will be single per board
                // 3.3V
                case 0xC9: // MX29LV640ET
                case 0xCB: // MX29LV640EB
                	this->flash_info.size = 0x800000;
                    break;
                case 0xA7: // MX29LV320ET
                case 0xA8: // MX29LV320EB
                	this->flash_info.size = 0x400000;
                    break;
                case 0xC4: // MX29LV160DT
                case 0x49: // MX29LV160DB
                	this->flash_info.size = 0x400000;
                    break;
                // 5V
                case 0x58: // MX29F800CT
                case 0xD6: // MX29F800CB
                	this->flash_info.size = 0x100000;
                    break;
                case 0x23: // MX29F400CT
                case 0xAB: // MX29F400CB
                	this->flash_info.size = 0x80000;
                    break;
                case 0x51: // MX29F200CT
                case 0x57: // MX29F200CB
                	this->flash_info.size = 0x80000;
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

/*******************************************************************//**
 *
 **********************************************************************/
void Cartridge::set_voltage(eVoltage voltage){
	switch(voltage){
	case vcart_3v3:
		this->vcart = vcart_3v3;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_5v:
		this->vcart = vcart_5v;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_off:
	default:
		this->vcart = vcart_off;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_SET);
		break;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void Cartridge::set_level_translators(bool enable){
	if( enable ){
		HAL_GPIO_WritePin(nOUT_EN0_GPIO_Port, nOUT_EN0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(nOUT_EN1_GPIO_Port, nOUT_EN1_Pin, GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(nOUT_EN0_GPIO_Port, nOUT_EN0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(nOUT_EN1_GPIO_Port, nOUT_EN1_Pin, GPIO_PIN_SET);
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t Cartridge::get_adapter_id(void){

	uint8_t cart_id = 0;
	HAL_StatusTypeDef hal_ret;
	hal_ret = HAL_I2C_Master_Transmit(&hi2c1, I2C_CART_ID_ADDRESS, &I2C_CART_ID_GPIO_REG, 1, 100);
	if( hal_ret == HAL_OK){
		hal_ret = HAL_I2C_Master_Receive(&hi2c1, I2C_CART_ID_ADDRESS, &cart_id, 1, 100);
		if( hal_ret == HAL_OK ){
			this->param.id = cart_id;
		}
	}
	return cart_id;
}

/*******************************************************************//**
* 8 BIT OPERATIONS
************************************************************************
 * single 8bit read at 16bit address
 **********************************************************************/
uint8_t Cartridge::read_byte(uint16_t address, e_memory_type mem_t){
	uint32_t fsmc_addr = UMD_CE0 | address;
	uint8_t read;
	read = *(__IO uint8_t *)(fsmc_addr);
	return read;
}

/*******************************************************************//**
 * single 8bit read at 32bit address
 **********************************************************************/
uint8_t Cartridge::read_byte(uint32_t address, e_memory_type mem_t){
	uint32_t fsmc_addr = UMD_CE0 | address;
	uint8_t read;
	read = *(__IO uint8_t *)(fsmc_addr);
	return read;
}

/*******************************************************************//**
 * multiple 8bit reads at 16bit address
 **********************************************************************/
void Cartridge::read_bytes(uint16_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t, bool dma){
	uint32_t fsmc_addr = UMD_CE0 | address;

	// do transfer with DMA
	//HAL_DMA_Start(param.dma_channel, fsmc_addr, (uint32_t)buf, size);
	//while(HAL_DMA_GetState(param.dma_channel) != HAL_DMA_STATE_READY );

	for(; size != 0; size--){
		*(buf++) = *(__IO uint8_t *)(fsmc_addr++);
	}
}

/*******************************************************************//**
 * multiple 8bit read at 32bit address
 **********************************************************************/
void Cartridge::read_bytes(uint32_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t, bool dma){
	uint32_t fsmc_addr = UMD_CE0 | address;

	// do transfer with DMA
	//HAL_DMA_Start(param.dma_channel, fsmc_addr, (uint32_t)buf, size);
	//while(HAL_DMA_GetState(param.dma_channel) != HAL_DMA_STATE_READY );

	for(; size != 0; size--){
		*(buf++) = *(__IO uint8_t *)(fsmc_addr++);
	}
}

/*******************************************************************//**
 * single 8 bit write at 16bit address
 **********************************************************************/
void Cartridge::write_byte(uint16_t address, uint8_t data, e_memory_type mem_t){
	uint32_t fsmc_addr = UMD_CE0 | address;
	*(__IO uint8_t *)(fsmc_addr) = data;
}

/*******************************************************************//**
 * single 8 bit write at 32bit address
 **********************************************************************/
void Cartridge::write_byte(uint32_t address, uint8_t data, e_memory_type mem_t){
	uint32_t fsmc_addr = UMD_CE0 | address;
	*(__IO uint8_t *)(fsmc_addr) = data;
}

/*******************************************************************//**
 * single 8 bit program at 32bit address
 **********************************************************************/
void Cartridge::program_bytes(uint32_t address, uint8_t *buf, uint16_t size, e_memory_type mem_t){

	for(; size > 0; size--){
		this->write_byte((uint32_t)0x0AAA, 0xAA, mem_prg);
		this->write_byte((uint16_t)0x0555, 0x55, mem_prg);
		this->write_byte((uint16_t)0x0AAA, 0xA0, mem_prg);
		// write the data
		this->write_byte(address, *(buf++), mem_prg);
		// wait for completion
		while(this->toggle_bit(2) != 2);
	}

}


/*******************************************************************//**
* 16 BIT OPERATIONS
************************************************************************
 * single 16bit read at 32bit address
 **********************************************************************/
uint16_t Cartridge::read_word(uint32_t address, e_memory_type mem_t){
	uint32_t fsmc_addr = UMD_CE3 | address;
	uint16_t read;
	read = *(__IO uint16_t *)(fsmc_addr);
	return read;
}

/*******************************************************************//**
 * multiple 16bit reads at 32bit address
 **********************************************************************/
void Cartridge::read_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t, bool dma){
	uint32_t fsmc_addr = UMD_CE3 | address;

	for(; size != 0; size -= 2){
		*(buf++) = *(__IO uint16_t *)(fsmc_addr);
		fsmc_addr += 2;
	}
}

/*******************************************************************//**
 * single 16 bit write at 32bit address
 **********************************************************************/
void Cartridge::write_word(uint32_t address, uint16_t data, e_memory_type mem_t){
	uint32_t fsmc_addr = UMD_CE3 | address;
	*(__IO uint16_t *)(fsmc_addr) = data;
}

/*******************************************************************//**
 * single 16 bit program at 32bit address
 **********************************************************************/
void Cartridge::program_words(uint32_t address, uint16_t *buf, uint16_t size, e_memory_type mem_t){

	for(; size > 0; size -= 2){
		this->write_word((uint32_t)0x0AAA << 1, 0x00AA, mem_prg);
		this->write_word((uint32_t)0x0555 << 1, 0x0055, mem_prg);
		this->write_word((uint32_t)0x0AAA << 1, 0x00A0, mem_prg);
		// write the data
		this->write_word(address, *(buf++), mem_prg);
		// wait for completion
		while(this->toggle_bit(2) != 2);
	}
}
