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

/*******************************************************************//**
 *
 **********************************************************************/
USB::USB(){
	outbuf.size = 0;
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

	if( outbuf.size >= UMD_BUFFER_SIZE ){
		return true;
	}else{
		return false;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void USB::transmit(void){

	if( outbuf.size != 0){
		if( outbuf.size > UMD_BUFFER_SIZE ){
			outbuf.size = UMD_BUFFER_SIZE;
		}

		CDC_Transmit_FS(outbuf.data.bytes, outbuf.size);
		outbuf.size = 0;
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
uint16_t USB::put(std::string str){

	uint16_t str_len = str.length();
	uint16_t room_left = UMD_BUFFER_SIZE - outbuf.size;

	// is there enough room on the buffer for the string?
	if( str_len > room_left ){
		return 0;
	}else{
		const char *strp = str.c_str();
		for( int i = 0; i < str_len ; i++){
			outbuf.data.bytes[outbuf.size++] = *(strp++);
		}
	}
	return str_len;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint8_t byte){

	// is the buffer full?
	if( is_full() ){
		return 0;
	}else{
		outbuf.data.bytes[outbuf.size++] = byte;
	}
	return 1;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint16_t word){

	// is the buffer full?
	if( (UMD_BUFFER_SIZE - outbuf.size) < 2 ){
		return 0;
	}else{
		outbuf.data.words[outbuf.size] = word;
		outbuf.size += 2;
	}
	return 1;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint8_t *data, uint16_t len){

	uint16_t room_left = UMD_BUFFER_SIZE - outbuf.size;

	// is there enough room on the buffer for the string?
	if( len > room_left ){
		return 0;
	}else{
		for( int i = 0; i < len ; i++){
			outbuf.data.bytes[outbuf.size++] = *(data++);
		}
	}
	return len;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint16_t *data, uint16_t len){

	uint16_t room_left = UMD_BUFFER_SIZE - outbuf.size;

	// only proceed if we're at a word boundary
	if( outbuf.size % 2 != 0 ){
		return -1;
	}

	// is there enough room on the buffer for the string?
	if( len > room_left ){
		return 0;
	}else{
		for( int i = 0; i < (len>>2) ; i++){
			outbuf.data.words[outbuf.size] = *(data++);
			outbuf.size += 2;
		}
	}
	return len;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::get(uint8_t* data, uint16_t size){
	return CDC_ReadBuffer(data, size);
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t USB::get(void){
	return CDC_ReadBuffer_Single();
}

