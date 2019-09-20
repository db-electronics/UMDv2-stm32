/*
 * UMD.h
 *
 *  Created on: Sep. 19, 2019
 *      Author: rene
 */

#ifndef UMD_H_
#define UMD_H_

#include <cstdint>
#include <string>
using namespace std;

class UMD{

public:
	UMD();

	void run(void);

private:

	// initializers
	void init(void);

	// USB transmit methods
	void sendUSB(string);
	void sendUSB(uint8_t* buf, uint16_t size);

	// LED methods
	void setLEDs(uint8_t LED0, uint8_t LED1, uint8_t LED2, uint8_t LED3);
	void shiftLEDs(uint8_t dir);

	// cartridge methods
	uint8_t readByte(uint32_t address);

	//FSMC address offsets
	#define FSMC_CE0_8BF_ADDR				0x60000000U
	#define FSMC_CE1_8BS_ADDR				0x64000000U
	#define FSMC_CE2_16BF_ADDR				0x68000000U
	#define FSMC_CE3_16BS_ADDR				0x6C000000U
};

#endif /* UMD_H_ */
