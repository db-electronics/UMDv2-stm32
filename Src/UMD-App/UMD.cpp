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

	// We need a cart factory but only one, and this function is the only one that needs to update
	// the cart ptr.  So we can use the static keyword to keep this across calls to the function
	set_cartridge_type(0);

	cart->init();

	uint32_t crc_input[1] = {0x00010004};
	uint32_t crc_result;
	// CRC-32/MPEG-2
	crc_result = HAL_CRC_Calculate(&hcrc, crc_input, 1);

	while(1){
		umd_millis = HAL_GetTick();
		listen();

		// check adc
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		adc_icart = HAL_ADC_GetValue(&hadc1);

		// wait a bit
		while( (HAL_GetTick() - umd_millis) < listen_interval );
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::listen(void){

	uint8_t data;

	if( usb.available() ){

		umd_command = usb.get();
		switch(umd_command){

		// COMMAND 0x01 - ID
		case 0x01:

			//usb_tx(std::string("UMDv2"));
			usb.put(std::string("UMD v2.0.0.0"));
			usb.transmit();
			break;

		// COMMAND 0x02 - SET LEDs
		case 0x02:
			// next byte contains the LED value
			if( usb.available(cmd_timeout, 1) ){
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
			if( usb.available(cmd_timeout, 1) ){
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



