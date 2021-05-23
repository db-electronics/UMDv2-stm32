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
	param.id = 0;
	set_voltage(vcart_off);
	set_level_translators(false);
}

/*******************************************************************//**
 * The get_flash_size() function returns the flash size
 **********************************************************************/
void Cartridge::find_flash_size(void)
{
    switch( flash_info.manufacturer ){
        // microchip
        case 0xBF:
            switch( flash_info.device )
            {
                case 0x6D: // SST39VF6401B
                case 0x6C: // SST39VF6402B
                	flash_info.size = 0x800000;
                    break;
                case 0x5D: // SST39VF3201B
                case 0x5C: // SST39VF3202B
                case 0x5B: // SST39VF3201
                case 0x5A: // SST39VF3202
                	flash_info.size = 0x400000;
                    break;
                case 0x4F: // SST39VF1601C
                case 0x4E: // SST39VF1602C
                case 0x4B: // SST39VF1601
                case 0x4A: // SST39VF1602
                	flash_info.size = 0x200000;
                    break;
                default:
                    break;
            }
            break;

        // macronix
        case 0xC2:
            switch( flash_info.device )
            {
                // chips which will be single per board
                // 3.3V
                case 0xC9: // MX29LV640ET
                case 0xCB: // MX29LV640EB
                	flash_info.size = 0x800000;
                    break;
                case 0xA7: // MX29LV320ET
                case 0xA8: // MX29LV320EB
                	flash_info.size = 0x400000;
                    break;
                case 0xC4: // MX29LV160DT
                case 0x49: // MX29LV160DB
                	flash_info.size = 0x400000;
                    break;
                // 5V
                case 0x58: // MX29F800CT
                case 0xD6: // MX29F800CB
                	flash_info.size = 0x100000;
                    break;
                case 0x23: // MX29F400CT
                case 0xAB: // MX29F400CB
                	flash_info.size = 0x80000;
                    break;
                case 0x51: // MX29F200CT
                case 0x57: // MX29F200CB
                	flash_info.size = 0x80000;
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
		vcart = vcart_3v3;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_5v:
		vcart = vcart_5v;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_off:
	default:
		vcart = vcart_off;
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
			param.id = cart_id;
		}
	}
	return cart_id;
}

/*******************************************************************//**
 *
 **********************************************************************/
void Cartridge::get_flash_id(void){
	//mx29f800 software ID detect byte mode
	// enter software ID mode
	write_rom((uint16_t)0x0AAA, (uint8_t)0xAA);
	write_rom((uint16_t)0x0555, (uint8_t)0x55);
	write_rom((uint16_t)0x0AAA, (uint8_t)0x90);
	// read manufacturer
	read_rom((uint16_t)0x0000, &flash_info.manufacturer);
	// read device
	read_rom((uint16_t)0x0001, &flash_info.device);
	// exit software ID mode
	write_rom((uint16_t)0x0000, (uint8_t)0xF0);
	find_flash_size();
}

/*******************************************************************//**
* 8 BIT OPERATIONS
************************************************************************
 * single 8bit read at 16bit address
 **********************************************************************/
void Cartridge::read_rom(uint16_t address, uint8_t *buf){
	uint32_t fmsc_addr = UMD_CE0 + address;
	*buf = *(__IO uint8_t *)(fmsc_addr);
}

/*******************************************************************//**
 * single 8bit read at 32bit address
 **********************************************************************/
void Cartridge::read_rom(uint32_t address, uint8_t *buf){
	uint32_t fmsc_addr = UMD_CE0 + address;
	*buf = *(__IO uint8_t *)(fmsc_addr);
}

/*******************************************************************//**
 * multiple 8bit reads at 16bit address
 **********************************************************************/
void Cartridge::read_rom(uint16_t address, uint8_t *buf, uint16_t size){
	uint32_t fmsc_addr = UMD_CE0 + address;
	for(; size != 0; size--){
		*(buf++) = *(__IO uint8_t *)(fmsc_addr++);
	}
}

/*******************************************************************//**
 * multiple 8bit read at 32bit address
 **********************************************************************/
void Cartridge::read_rom(uint32_t address, uint8_t *buf, uint16_t size){
	uint32_t fmsc_addr = UMD_CE0 + address;
	for(; size != 0; size--){
		*(buf++) = *(__IO uint8_t *)(fmsc_addr++);
	}
}

/*******************************************************************//**
 * single 8 bit write at 16bit address
 **********************************************************************/
void Cartridge::write_rom(uint16_t address, uint8_t data){
	uint32_t fmsc_addr = UMD_CE0 + address;
	*(__IO uint8_t *)(fmsc_addr) = data;
}

/*******************************************************************//**
 * single 8 bit write at 32bit address
 **********************************************************************/
void Cartridge::write_rom(uint32_t address, uint8_t data){
	uint32_t fmsc_addr = UMD_CE0 + address;
	*(__IO uint8_t *)(fmsc_addr) = data;
}

/*******************************************************************//**
* 16 BIT OPERATIONS
************************************************************************
 * single 16bit read at 32bit address
 **********************************************************************/
void Cartridge::read_rom(uint32_t address, uint16_t *buf){
	uint32_t fmsc_addr = UMD_CE2 + address;
	*buf = *(__IO uint16_t *)(fmsc_addr);
}

/*******************************************************************//**
 * multiple 16bit read at 32bit address
 **********************************************************************/
void Cartridge::read_rom(uint32_t address, uint16_t *buf, uint16_t size){
	uint32_t fmsc_addr = UMD_CE2 + address;
	for(; size != 0; size -= 2){
		*(buf++) = *(__IO uint16_t *)(fmsc_addr);
		fmsc_addr += 2;
	}
}

/*******************************************************************//**
 * single 8 bit write at 32bit address
 **********************************************************************/
void Cartridge::write_rom(uint32_t address, uint16_t data){
	uint32_t fmsc_addr = UMD_CE2 + address;
	*(__IO uint16_t *)(fmsc_addr) = data;
}
