/*
 * UMD_usb.cpp
 *
 *  Created on: Feb 3, 2020
 *      Author: rene
 */

// these are basically just to wrap all the CDC functions
// and to make them friendlier with overloading

#include "UMD.h"

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::usb_tx(std::string str){
	CDC_Transmit_FS( (uint8_t*)str.c_str(), str.length() );
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::usb_tx(uint8_t* buf, uint16_t size){
	CDC_Transmit_FS(buf, size);
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t UMD::usb_rx(uint8_t* buf, uint16_t size){
	return CDC_ReadBuffer(buf, size);
}

/*******************************************************************//**
 *
 **********************************************************************/
uint8_t UMD::usb_rx(void){
	return CDC_ReadBuffer_Single();
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t UMD::usb_available(void){
	return CDC_BytesAvailable();
}

/*******************************************************************//**
 *
 **********************************************************************/
uint16_t UMD::usb_available(uint32_t timeout_ms, uint16_t bytes_required){
	return CDC_BytesAvailableTimeout(timeout_ms, bytes_required);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::usb_init_buffer(void){
	CDC_InitBuffer();
}

