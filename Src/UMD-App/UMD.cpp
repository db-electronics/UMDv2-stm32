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


/*******************************************************************//**
 *
 **********************************************************************/
UMD::UMD(){

}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::init(void){

	int i;
	//uint8_t read_data;

	// std::string str = "UMDv2 initializing...\n\r";
	send_usb(std::string("UMDv2 initializing...\n\r"));
	// HAL_USART_Transmit();

	// turn off cartridge voltage source
	set_cartridge_voltage(vcart_off);

	// can write directly to the FMSC memory space
	/* NOR memory device read/write start address */
	// read_data = *(__IO uint8_t *)ce0_8b_ptr;e

	// configure outputs
	disable_output_translators();
	HAL_GPIO_WritePin(BOOT_EN_GPIO_Port, BOOT_EN_Pin, GPIO_PIN_RESET);

	// flash to show we're alive
	for(i=0;i<4;i++){
		set_leds(0x05);
		HAL_Delay(250);
		set_leds(0x0A);
		HAL_Delay(250);
	}
	set_leds(0x00);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::run(void){

	uint8_t leds_value = 8;
	std::string str = "UMDv2 running...\n\r";
	init();

	// We need a cart factory but only one, and this function is the only one that needs to update
	// the cart ptr.  So we can use the static keyword to keep this across calls to the function
	set_cartridge_type(0);

	cart->init();

	while(1){
		set_leds(leds_value);
		if( leds_value == 8){ leds_value = 4; }else{ leds_value = 8; }
		HAL_Delay(500);
		listen();
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::listen(void){

	uint8_t data;
	set_leds(0x01);

	if( CDC_BytesAvailable() ){
		set_leds(0x02);

		cmd_current = CDC_ReadBuffer_Single();
		set_leds(0x03);

		switch(cmd_current){

		// COMMAND 0x01 - ID
		case 0x01:
			send_usb(std::string("UMDv2\n\r"));
			break;

		// COMMAND 0x02 - SET LEDs
		case 0x02:
			// next byte contains the LED value
			while(CDC_BytesAvailable() == 0);
			data = CDC_ReadBuffer_Single();
			set_leds(data);
			break;

		// COMMAND 0x03 - SET CARTRIDGE VOLTAGE
		case 0x03:
			// next byte contains the voltage value
			while(CDC_BytesAvailable() == 0);
			data = CDC_ReadBuffer_Single();
			set_cartridge_voltage(static_cast<cartv_typ>(data));
			break;

		// DEFAULT REPLY
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

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::enable_output_translators(void){
	HAL_GPIO_WritePin(nOUT_EN0_GPIO_Port, nOUT_EN0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(nOUT_EN1_GPIO_Port, nOUT_EN1_Pin, GPIO_PIN_RESET);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::disable_output_translators(void){
	HAL_GPIO_WritePin(nOUT_EN0_GPIO_Port, nOUT_EN0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(nOUT_EN1_GPIO_Port, nOUT_EN1_Pin, GPIO_PIN_SET);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::set_cartridge_voltage(cartv_typ voltage){
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
void UMD::set_leds(uint8_t leds){

	(leds & 0x01) ? HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
	(leds & 0x02) ? HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	(leds & 0x04) ? HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	(leds & 0x08) ? HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::shift_leds(uint8_t dir){

	static uint8_t state = 1;

	set_leds(state);
	if(dir == LED_SHIFT_DIR_LEFT){
		state <<= 1;
		if( state > 8 ) state = 1;
	}else{
		state >>= 1;
		if( state > 8 ) state = 8;
	}
}
