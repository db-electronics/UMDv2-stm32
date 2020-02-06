/*
 * USB.cpp
 *
 *  Created on: Feb. 4, 2020
 *      Author: rene
 */

#include "USB.h"
#include "usbd_cdc_if.h"

/*******************************************************************//**
 *
 **********************************************************************/
USB::USB(){
	buffer.size = 0;
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

	if( buffer.size >= CIRCULAR_BUFFER_SIZE ){
		return true;
	}else{
		return false;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void USB::transmit(void){

	if( buffer.size != 0){
		if( buffer.size > CIRCULAR_BUFFER_SIZE ){
			buffer.size = CIRCULAR_BUFFER_SIZE;
		}

		CDC_Transmit_FS(buffer.data.byte, buffer.size);
		buffer.size = 0;
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
	uint16_t room_left = CIRCULAR_BUFFER_SIZE - buffer.size;

	// is there enough room on the buffer for the string?
	if( str_len > room_left ){
		return 0;
	}else{
		const char *strp = str.c_str();
		for( int i = 0; i < str_len ; i++){
			buffer.data.byte[buffer.size++] = *(strp++);
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
		buffer.data.byte[buffer.size++] = byte;
	}
	return 1;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint16_t word){

	// is the buffer full?
	if( (CIRCULAR_BUFFER_SIZE - buffer.size) < 2 ){
		return 0;
	}else{
		buffer.data.word[buffer.size] = word;
		buffer.size += 2;
	}
	return 1;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint8_t *data, uint16_t len){

	uint16_t room_left = CIRCULAR_BUFFER_SIZE - buffer.size;

	// is there enough room on the buffer for the string?
	if( len > room_left ){
		return 0;
	}else{
		for( int i = 0; i < len ; i++){
			buffer.data.byte[buffer.size++] = *(data++);
		}
	}
	return len;
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t USB::put(uint16_t *data, uint16_t len){

	uint16_t room_left = CIRCULAR_BUFFER_SIZE - buffer.size;

	// only proceed if we're at a word boundary
	if( buffer.size % 2 != 0 ){
		return -1;
	}

	// is there enough room on the buffer for the string?
	if( len > room_left ){
		return 0;
	}else{
		for( int i = 0; i < (len>>2) ; i++){
			buffer.data.word[buffer.size] = *(data++);
			buffer.size += 2;
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

