/*******************************************************************//**
 *  \file UMD.h
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

#ifndef UMD_H_
#define UMD_H_

#include <cstdint>
#include <string>

#include "Cartridges/Cartridge.h"
#include "usbd_cdc_if.h"
#include "cartfactory.h"
#include "Cartridges/Cartridge.h"

#define LED_SHIFT_DIR_LEFT		0
#define LED_SHIFT_DIR_RIGHT 	1

#define UMD_OUTPUT_ENABLE		0
#define UMD_OUTPUT_DISABLE		1

#define CMD_MAX_LEN				32

#define CE0_ADRESS		       	0x60000000U
#define CE1_ADRESS     		  	0x64000000U
#define CE2_ADRESS       		0x68000000U
#define CE3_ADRESS       		0x6C000000U

class UMD{

public:
	UMD();

	// run is the only method visible from main.cpp, it never returns
	void run(void);

private:

	// pointer to cartridge objects
	Cartridge *cart;

	// FMSC memory pointers
	__IO uint8_t * ce0_8b_ptr = (uint8_t *)(CE0_ADRESS);

	// listen for commands
	uint8_t cmd_current;
	void clr_cmd_buffer(void);
	void listen(void);

	// initializers
	void init(void);
	void set_cartridge_type(uint8_t mode);

	// USB methods
	void send_usb(std::string);
	void send_usb(uint8_t* buf, uint16_t size);
	uint16_t receive_usb(uint8_t* buf, uint16_t size);

	// LED methods
	void set_leds(uint8_t LEDs);
	void shift_leds(uint8_t dir);

	// Cartridge Methods
	typedef enum {vcart_off=0, vcart_3v3, vcart_5v} cartv_typ;
	void set_cartridge_voltage(cartv_typ voltage);
	void enable_output_translators(void);
	void disable_output_translators(void);

};

#endif /* UMD_H_ */
