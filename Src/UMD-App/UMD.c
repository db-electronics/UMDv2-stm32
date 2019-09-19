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
#include "usb_otg.h"
#include "gpio.h"
#include "fsmc.h"

#include "UMD.h"

void UMD_app(void){
	while(1){
		HAL_Delay(500);
		UMD_shift_LEDs(0);
	}
}

void UMD_shift_LEDs(unsigned int mode){
	if( HAL_GPIO_ReadPin(LED0_GPIO_Port, LED0_Pin) ){
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, 0);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
	}else if (HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin) ){
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, 0);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
	}else if (HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin) ){
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, 0);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
	}else{
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, 1);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
	}
}
