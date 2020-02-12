/*
 * UMD_io.cpp
 *
 *  Created on: Feb 3, 2020
 *      Author: rene
 */

#include "UMD.h"



/*******************************************************************//**
 *
 **********************************************************************/
void UMD::io_set_leds(uint8_t leds){

	(leds & 0x01) ? HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
	(leds & 0x02) ? HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	(leds & 0x04) ? HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	(leds & 0x08) ? HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::io_shift_leds(uint8_t dir){

	static uint8_t state = 1;

	io_set_leds(state);
	if(dir == LED_SHIFT_DIR_LEFT){
		state <<= 1;
		if( state > 8 ) state = 1;
	}else{
		state >>= 1;
		if( state > 8 ) state = 8;
	}
}

/*******************************************************************//**
 *
 **********************************************************************/
void UMD::io_boot_precharge(bool charge){
	if( charge ){
		HAL_GPIO_WritePin(BOOT_EN_GPIO_Port, BOOT_EN_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(BOOT_EN_GPIO_Port, BOOT_EN_Pin, GPIO_PIN_RESET);
	}
}

