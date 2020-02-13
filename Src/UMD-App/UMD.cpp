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

	// We need a cart factory but only one, and this function is the only one that needs to update
	// the cart ptr.  So we can use the static keyword to keep this across calls to the function
	set_cartridge_type(0); // type 0 = UNDEFINED
	cart->init();

	// pc assigned id defaults to 0
	pc_assigned_id = 0;

	// can write directly to the FMSC memory space
	/* NOR memory device read/write start address */
	// read_data = *(__IO uint8_t *)ce0_8b_ptr;e

	// configure outputs
	io_boot_precharge(false);


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

	// test CRC results
	// swapped = 0xF9 14 D5 D2
	// regular = 0x0F 10 B2 E2
	// uint32_t crc_inputs[4] = {0x01020304, 0x02030405, 0x03040506, 0x04050607};
	// uint32_t crc_calc = HAL_CRC_Calculate(&hcrc, crc_inputs, 4);
	// crc_calc = crc32mpeg2_calc(crc_inputs, 16, true);

	// would be nice to have a file on the SD card that could act as a script
	// to automate a UMD task like repetitive burning


	while(1){
		umd_millis = HAL_GetTick();
		listen();

		// check adc
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		adc.current = HAL_ADC_GetValue(&hadc1);

		// wait a bit
		while( (HAL_GetTick() - umd_millis) < LISTEN_INTERVAL );
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::listen(void){

	uint16_t payload_size = 0;
	uint32_t crc_calc;

	// first 2 bytes are command, next 2 bytes are the size of this packet
	if( usb.available(CMD_TIMEOUT, CMD_HEADER_SIZE) ){

		// read command and payload size - test value 0x08 0x00 0x03 0x04 0x4f 0x89 0x90 0x9f
		// https://crccalc.com
		usb.get(cmd.header.bytes, CMD_HEADER_SIZE);
		payload_size = cmd.header.size - CMD_HEADER_SIZE;
		crc_calc = crc32mpeg2_calc(&cmd.header.sof, 4, true);

		// wait for rest of data if payload is not 0
		if( payload_size ){
			if( usb.available(PAYLOAD_TIMEOUT, payload_size) != payload_size ){
				usb.put(CMDREPLY.PAYLOAD_TIMEOUT);
				usb.transmit();
				// reset usb rx buffer
				usb.flush();
				return;
			}
			// command with payload, accumulate over payload
			usb.get(ubuf.u8, payload_size);
			crc_calc = crc32mpeg2_calc(ubuf.u32, payload_size, false);
		}

		// compare with received CRC
		if( crc_calc != cmd.header.crc ){
			// reply with CRC error
			usb.put(CMDREPLY.CRC_ERROR);
		}else{
			// decode command
			switch(cmd.header.cmd){
			case CMD_UMD:
				usb.put(std::string("UMD v2.0.0.0"));
				usb.transmit();
				break;

			case CMD_SETID:
				// pc assigns a unique 32bit id to this UMD
				pc_assigned_id = ubuf.u32[0];
				usb.put(CMDREPLY.ACK);
				break;

			case CMD_SETLEDS:
				// first byte contains the LED value
				io_set_leds(ubuf.u8[0]);
				usb.put(CMDREPLY.ACK);
				break;

			case CMD_SETCARTV:
				// first byte contains the voltage value
				cart->set_voltage(static_cast<Cartridge::eVoltage>(ubuf.u8[0]));
				usb.put(CMDREPLY.ACK);
				break;

			case CMD_GETCARTV:
				usb.put(static_cast<uint16_t>(cart->vcart));
				break;

			// DEFAULT REPLY
			default:
				usb.put(CMDREPLY.NO_ACK);
				break;
			}
		}
		usb.transmit();
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
uint32_t UMD::crc32mpeg2_calc(uint32_t *data, uint32_t len, bool reset){
	uint32_t swapped, result, i;

	// swapping the endianness of each u32 gets the same results as pythons:
	// from crccheck.crc import Crc32Mpeg2
	// I figure the STM32 can reverse endianness much faster than Python can so let's do it here
	if(reset){
		__HAL_CRC_DR_RESET(&hcrc);
	}

	for( i = 0 ; i<(len>>2) ; i++){
		swapped = ( *(data + i) & 0x000000FF ) << 24;
		swapped |= ( *(data + i) & 0x0000FF00 ) << 8;
		swapped |= ( *(data + i) & 0x00FF0000 ) >> 8;
		swapped |= ( *(data + i) & 0xFF000000 ) >> 24;
		result = HAL_CRC_Accumulate(&hcrc, &swapped, 1);
	}
	return result;
}





