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
#include "usbd_cdc_if.h"

#include "UMD.h"


UMD::UMD(){

}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::init(void){

	int i;
	uint8_t read_data;

	std::string str = "UMDv2 initializing...\n\r";
	send_usb(str);

	// turn off cartridge voltage source
	vcart_select(vcart_off);

	// can write directly to the FMSC memory space
	/* NOR memory device read/write start address */
	read_data = *(__IO uint8_t *)ce0_8b_ptr;

	// configure outputs
	// NOR_HandleTypeDef hnor1;
	// SRAM_HandleTypeDef hsram2;
	// NOR_HandleTypeDef hnor3;
	// SRAM_HandleTypeDef hsram4;
	disable_output_translators();
	HAL_GPIO_WritePin(BOOT_EN_GPIO_Port, BOOT_EN_Pin, GPIO_PIN_RESET);

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

	std::string str = "UMDv2 running...\n\r";
	init();

	// We need a cart factory but only one, and this function is the only one that needs to update
	// the cart ptr.  So we can use the static keyword to keep this across calls to the function
	set_cartridge_type(0);

	cart->init();

	while(1){
		// HAL_Delay(500);
		// shiftLEDs(LED_SHIFT_DIR_LEFT);
		// HAL_Delay(500);
		// shiftLEDs(LED_SHIFT_DIR_LEFT);
		// send_usb(str);
		listen();
	}
}

void UMD::listen(void){

	if( CDC_BytesAvailable() ){

		cmd_current = CDC_ReadBuffer_Single();
		switch(cmd_current){
		case 0x00:

			break;
		default:

			send_usb(std::string("unimplemented cmd byte\n\r"));
			break;
		}
	}
}


/*******************************************************************//**
 *
 **********************************************************************/
void UMD::set_cartridge_type(uint8_t mode){
	static CartFactory cf;
	cart = cf.getCart(static_cast<CartFactory::Mode>(mode));
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::send_usb(std::string str){
	CDC_Transmit_FS( (uint8_t*)str.c_str(), str.length() );
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::send_usb(uint8_t* buf, uint16_t size){
	CDC_Transmit_FS(buf, size);
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t UMD::receive_usb(uint8_t* buf, uint16_t size){
	uint16_t data;
	data = CDC_ReadBuffer(buf, size);
	return data;
}

void UMD::enable_output_translators(void){
	HAL_GPIO_WritePin(nOUT_EN0_GPIO_Port, nOUT_EN0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(nOUT_EN1_GPIO_Port, nOUT_EN1_Pin, GPIO_PIN_RESET);
}

void UMD::disable_output_translators(void){
	HAL_GPIO_WritePin(nOUT_EN0_GPIO_Port, nOUT_EN0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(nOUT_EN1_GPIO_Port, nOUT_EN1_Pin, GPIO_PIN_SET);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::vcart_select(cartv_typ voltage){
	switch(voltage){
	case vcart_3v3:
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_5v:
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_off:
	default:
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_SET);
		break;
	}
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
