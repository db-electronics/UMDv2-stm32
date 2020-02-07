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
#include "gpio.h"
#include "fsmc.h"

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
	// HAL_USART_Transmit();

	// turn off cartridge voltage source
	set_cartridge_voltage(vcart_off);

	// can write directly to the FMSC memory space
	/* NOR memory device read/write start address */
	// read_data = *(__IO uint8_t *)ce0_8b_ptr;e

	// configure outputs
	io_set_level_translators(false);
	io_boot_precharge(false);

	// set proper CRC32 poly
	// https://stackoverflow.com/questions/39646441/how-to-set-stm32-to-generate-standard-crc32/39683314

	// flash to show we're alive
	for(i=0;i<4;i++){
		io_set_leds(0x05);
		HAL_Delay(250);
		io_set_leds(0x0A);
		HAL_Delay(250);
	}
	io_set_leds(0x00);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::run(void){

	uint32_t umd_millis;
	init();

	// would be nice to have a file on the SD card that could act as a script
	// to automate a UMD task like repetitive buring

	// We need a cart factory but only one, and this function is the only one that needs to update
	// the cart ptr.  So we can use the static keyword to keep this across calls to the function
	set_cartridge_type(0);

	cart->init();

	while(1){
		umd_millis = HAL_GetTick();
		listen();

		// check adc
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		adc_icart = HAL_ADC_GetValue(&hadc1);

		// wait a bit
		while( (HAL_GetTick() - umd_millis) < LISTEN_INTERVAL );
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::listen(void){

	uint8_t data;

	// first 2 bytes are command, next 2 bytes are the size of this packet
	if( usb.available(CMD_TIMEOUT, CMD_HEADER_SIZE) ){

		// read command and payload size - test value 0x08 0x00 0x03 0x04 0x4f 0x89 0x90 0x9f
		// https://crccalc.com
		usb.get(cmd.header.bytes, CMD_HEADER_SIZE);
		payload_size = cmd.header.size - CMD_HEADER_SIZE;

		// wait for rest of data if payload is not 0
		if( payload_size ){
			if( usb.available(PAYLOAD_TIMEOUT, payload_size) != payload_size ){
				usb.put(CMDREPLY.PAYLOAD_TIMEOUT);
				usb.transmit();
				// reset usb rx buffer
				usb.flush();
				return;
			}
			// command with payload, reset CRC and accumulate over command header and payload
			__HAL_CRC_DR_RESET(&hcrc);
		}else{
			// 0 payload commands, only need to check the command header
			crc_calc = HAL_CRC_Calculate(&hcrc, &cmd.header.sof, 1);
			if( crc_calc != cmd.header.crc ){
				usb.put(CMDREPLY.BAD_CRC);
				usb.transmit();
			}
		}

		umd_command = usb.get();
		switch(cmd.header.cmd){

		case 0x0000:
			usb.put(std::string("UMD v2.0.0.0"));
			usb.transmit();
			break;

		// COMMAND 0x02 - SET LEDs
		case 0x0001:
			// next byte contains the LED value
			if( usb.available(CMD_TIMEOUT, 1) ){
				data = usb.get();
				io_set_leds(data);
				cmd_put_ack();
			}else{
				cmd_put_timeout();
			}
			break;

		// COMMAND 0x03 - SET CARTRIDGE VOLTAGE
		case 0x03:
			// next byte contains the value
			if( usb.available(CMD_TIMEOUT, 1) ){
				data = usb.get();
				set_cartridge_voltage(static_cast<cartv_typ>(data));
				cmd_put_ack();
			}else{
				cmd_put_timeout();
			}
			break;

		// COMMAND 0x04 - GET CARTRIDGE VOLTAGE
		case 0x04:
			// reply with command byte followed by cartv enum
			usb.put(umd_command);
			usb.put(static_cast<uint8_t>(cartv));
			usb.transmit();
			break;

		case 0x05:
			usb.put(adc_icart);
			usb.transmit();
			break;

		// DEFAULT REPLY
		default:

			break;
		}
	}
}


/*******************************************************************//**
 *
 **********************************************************************/
void UMD::cmd_put_ack(void){
	// acknowledge by returning the command byte and its 2s complement
	usb.put(umd_command);
	usb.put((uint8_t)(-umd_command));

}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::cmd_put_timeout(void){
	// timeout by returning the command byte followed by its complement
	usb.put(umd_command);
	usb.put((uint8_t)(~umd_command));
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
void UMD::set_cartridge_voltage(cartv_typ voltage){
	switch(voltage){
	case vcart_3v3:
		cartv = vcart_3v3;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_5v:
		cartv = vcart_5v;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_RESET);
		break;
	case vcart_off:
	default:
		cartv = vcart_off;
		HAL_GPIO_WritePin(VSEL0_GPIO_Port, VSEL0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(VSEL1_GPIO_Port, VSEL1_Pin, GPIO_PIN_SET);
		break;
	}
}



