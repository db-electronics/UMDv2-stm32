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

	// say hello
	string str = "UMDv2\n\r";
	sendUSB(str);

}

void UMD::init(void){

	int i;

	string str = "UMDv2 initializing...\n\r";
	sendUSB(str);

	// flash to show we're alive
	for(i=0;i<4;i++){
		setLEDs(0,0,0,0);
		HAL_Delay(250);
		setLEDs(1,1,1,1);
		HAL_Delay(250);
	}
}

void UMD::run(void){

	string str = "UMDv2 running...\n\r";
	init();

	while(1){
		HAL_Delay(500);
		shiftLEDs(0);
		HAL_Delay(500);
		shiftLEDs(0);
		sendUSB(str);
	}
}

void UMD::sendUSB(string str){
	CDC_Transmit_FS( (uint8_t*)str.c_str(), str.length() );
}

void UMD::sendUSB(uint8_t* buf, uint16_t size){
	CDC_Transmit_FS( buf, size );
}


uint8_t UMD::readByte(uint32_t address){
	uint8_t value;
	value = *(volatile uint8_t *)(FSMC_CE1_8BS_ADDR + address);
	return value;
}

void UMD::setLEDs(uint8_t LED0, uint8_t LED1, uint8_t LED2, uint8_t LED3){

	(LED0 == 1) ? HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
	(LED1 == 1) ? HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	(LED2 == 1) ? HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	(LED3 == 1) ? HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

void UMD::shiftLEDs(uint8_t dir){

	static uint8_t state = 0;

	//HAL_GPIO_ReadPin(LED0_GPIO_Port, LED0_Pin)
	switch(state){
	case 0:
		setLEDs(1,0,0,0);
		break;
	case 1:
		setLEDs(0,1,0,0);
		break;
	case 2:
		setLEDs(0,0,1,0);
		break;
	case 3:
		setLEDs(0,0,0,1);
		break;
	default:
		break;
	}

	if(dir==0){
		state++;
	}else{
		state--;
	}
	state &= 0x03;

}
