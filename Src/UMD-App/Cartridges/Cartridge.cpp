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


/*******************************************************************//**
 *
 **********************************************************************/
Cartridge::Cartridge() {

}

/*******************************************************************//**
 *
 **********************************************************************/
Cartridge::~Cartridge() {

}

/*******************************************************************//**
 *
 **********************************************************************/
void Cartridge::init(void){
	set_voltage(vcart_off);
	set_level_translators(false);
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
uint8_t Cartridge::readByte(uint32_t address){
	uint8_t value;
	value = *(volatile uint8_t *)(FSMC_CE1_8BS_ADDR + address);
	return value;
}
