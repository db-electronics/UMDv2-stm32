/*******************************************************************//**
 *  \file USB.cpp
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

#include "USB.h"
#include "usbd_cdc_if.h"
#include "crc.h"

/*******************************************************************//**
 *
 **********************************************************************/
USB::USB(){
	usbbuf.size = 0;
	CDC_InitBuffer();
}

/*******************************************************************//**
 *
 **********************************************************************/
void USB::flush(void){
	CDC_InitBuffer();
}

/*******************************************************************//**
 *
 **********************************************************************/
bool USB::is_full(void){

	if( usbbuf.size >= USB_BUFFER_SIZE ){
		return true;
	}else{
		return false;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
bool USB::usbbuf_enough_room(uint16_t size){

	// check if size + 4 (crc) fits in the buffer
	if( USB_BUFFER_SIZE - (usbbuf.size + size + 4) >= 0 ){
		return true;
	}else{
		return false;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void USB::transmit(void){

	uint32_t crc, swapped;

	if( usbbuf.size != 0){
		if( usbbuf.size > USB_BUFFER_SIZE ){
			usbbuf.size = USB_BUFFER_SIZE;
		}

		// add size of trailing CRC before calculation
		usbbuf.data.packet_size += 4;
		// swapping the endianness of each u32 gets the same results as python's:
		// from crccheck.crc import Crc32Mpeg2
		// I figure the STM32 can reverse endianness much faster than Python can so let's do it here
		__HAL_CRC_DR_RESET(&hcrc);


		for( uint16_t i = 0 ; i < usbbuf.size ; i += 4){
			swapped = usbbuf.data.bytes[i] << 24;
			swapped |= usbbuf.data.bytes[i + 1] << 16;
			swapped |= usbbuf.data.bytes[i + 2] << 8;
			swapped |= usbbuf.data.bytes[i + 3];
			crc = HAL_CRC_Accumulate(&hcrc, &swapped, 1);
		}

		// add crc and size of crc to output
		put(crc);

		CDC_Transmit_FS(usbbuf.data.bytes, usbbuf.size);
		usbbuf.size = 0;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::available(void){
	return CDC_BytesAvailable();
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::available(uint32_t timeout_ms, uint16_t bytes_required){
	return CDC_BytesAvailableTimeout(timeout_ms, bytes_required);
}

/*******************************************************************//**
 *
 **********************************************************************/
void USB::put_header(uint16_t reply){
	// reset size to 4
	usbbuf.size = 4;
	usbbuf.data.ack = reply + 0x4000;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(std::string str){

	uint16_t str_len = str.length();

	// is there enough room on the buffer for the string?
	if( usbbuf_enough_room(str_len) ){
		return 0;
	}else{
		const char *strp = str.c_str();
		for( int i = 0; i < str_len ; i++){
			usbbuf.data.bytes[usbbuf.size++] = *(strp++);
		}
	}
	return str_len;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint8_t byte){

	// wil
	if( usbbuf_enough_room(sizeof(byte)) ){
		return 0;
	}else{
		usbbuf.data.bytes[usbbuf.size++] = byte;
	}
	return 1;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint16_t word){

	// is the buffer full?
	if( usbbuf_enough_room(sizeof(word)) ){
		return 0;
	}else{
		// put byte a time in case we're not at an even boundary
		usbbuf.data.bytes[usbbuf.size++] = (uint8_t)word;
		usbbuf.data.bytes[usbbuf.size++] = (uint8_t)(word>>8);
	}
	return 2;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint32_t lword){

	// is the buffer full?
	if( usbbuf_enough_room(sizeof(lword)) ){
		return 0;
	}else{
		// put byte a time in case we're not at an even boundary
		usbbuf.data.bytes[usbbuf.size++] = (uint8_t)lword;
		usbbuf.data.bytes[usbbuf.size++] = (uint8_t)(lword>>8);
		usbbuf.data.bytes[usbbuf.size++] = (uint8_t)(lword>>16);
		usbbuf.data.bytes[usbbuf.size++] = (uint8_t)(lword>>24);
	}
	return 4;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint8_t *data, uint16_t len){

	uint16_t room_left = USB_BUFFER_SIZE - usbbuf.size;

	// is there enough room on the buffer for the string?
	if( len > room_left ){
		return 0;
	}else{
		for( int i = 0; i < len ; i++){
			usbbuf.data.bytes[usbbuf.size++] = *(data++);
		}
	}
	return len;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint16_t *data, uint16_t len){

	uint16_t room_left = USB_BUFFER_SIZE - usbbuf.size;

	// only proceed if we're at a word boundary
	if( usbbuf.size % 2 != 0 ){
		return -1;
	}

	// is there enough room on the buffer for the string?
	if( len > room_left ){
		return 0;
	}else{
		for( int i = 0; i < (len>>2) ; i++){
			// put byte a time in case we're not at an even boundary
			usbbuf.data.bytes[usbbuf.size++] = (uint8_t)(*(data));
			usbbuf.data.bytes[usbbuf.size++] = (uint8_t)((*(data++)>>8));
		}
	}
	return len;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t USB::get(void){
	return CDC_ReadBuffer_Single();
}


/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::get(uint8_t* data, uint16_t size){
	return CDC_ReadBuffer(data, size);
}

