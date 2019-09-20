/*
 * UMD.c
 *
 *  Created on: Sep. 19, 2019
 *      Author: rene
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
#include <cstdint>
#include <string>
using namespace std;

UMD::UMD(){

	string str = "UMDv2\n";

	sendUSB(str);
}

void UMD::run(void){

	string str = "UMDv2 running...\n";

	while(1){
		HAL_Delay(500);
		shift_LEDs(0);
		HAL_Delay(500);
		shift_LEDs(0);
		sendUSB(str);
	}
}

void UMD::sendUSB(string str){
	//CDC_Transmit_FS( (uint8_t*)str.c_str(), str.length() );
	//CDC_Transmit_FS((uint8_t*)"UMDv2\n", 6);
	CDC_Transmit_FS( (uint8_t*)str.c_str(), str.length() );
}

void UMD::sendUSB(uint8_t* buf, uint16_t size){
	CDC_Transmit_FS( buf, size );
}

void UMD::shift_LEDs(uint8_t mode){
	if( HAL_GPIO_ReadPin(LED0_GPIO_Port, LED0_Pin) ){
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
	}else if (HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin) ){
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
	}else if (HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin) ){
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
	}
}
