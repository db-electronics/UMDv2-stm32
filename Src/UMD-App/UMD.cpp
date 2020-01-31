/*******************************************************************//**
 *  \file UMD.cpp
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "i2c.h"
#include "sdio.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "fsmc.h"

#include "UMD.h"


UMD::UMD(){

	// say hello
	std::string str = "UMDv2\n\r";
	sendUSB(str);

}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::init(void){

	int i;

	std::string str = "UMDv2 initializing...\n\r";
	sendUSB(str);


	// flash to show we're alive
	for(i=0;i<4;i++){
		setLEDs(0x05);
		HAL_Delay(250);
		setLEDs(0x0A);
		HAL_Delay(250);
	}
	setLEDs(0x00);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::run(void){

	SerialCommand cli();
	std::string str = "UMDv2 running...\n\r";
	init();

	// We need a cart factory but only one, and this function is the only one that needs to update
	// the cart ptr.  So we can use the static keyword to keep this across calls to the function
	setCartridgeType(0);

	cart->init();
	//uint8_t byte = cart->readByte(0x12345678);
	//byte++;

	while(1){
		HAL_Delay(500);
		shiftLEDs(LED_SHIFT_DIR_LEFT);
		HAL_Delay(500);
		shiftLEDs(LED_SHIFT_DIR_LEFT);
		sendUSB(str);
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::setCartridgeType(uint8_t mode){
	static CartFactory cf;
	cart = cf.getCart(static_cast<CartFactory::Mode>(mode));
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::sendUSB(std::string str){
	CDC_Transmit_FS( (uint8_t*)str.c_str(), str.length() );
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::sendUSB(uint8_t* buf, uint16_t size){
	CDC_Transmit_FS(buf, size);
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t UMD::receiveUSB(uint8_t* buf, uint16_t size){
	uint16_t data;
	data = CDC_ReadBuffer(buf, size);
	return data;
}


void UMD::vsel(void){

}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::setLEDs(uint8_t LEDs){

	(LEDs & 0x01) ? HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
	(LEDs & 0x02) ? HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	(LEDs & 0x04) ? HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	(LEDs & 0x08) ? HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::shiftLEDs(uint8_t dir){

	static uint8_t state = 1;

	setLEDs(state);
	if(dir == LED_SHIFT_DIR_LEFT){
		state <<= 1;
		if( state > 8 ) state = 1;
	}else{
		state >>= 1;
		if( state > 8 ) state = 8;
	}
}
