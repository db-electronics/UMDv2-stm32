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

	// USB transmit methods
	void sendUSB(string);
	void sendUSB(uint8_t* buf, uint16_t size);

	void shift_LEDs(uint8_t);
};

#endif /* UMD_H_ */
