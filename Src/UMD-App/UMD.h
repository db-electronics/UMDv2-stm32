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

#include "main.h"

#include "USB.h"
#include "Cartridges/Cartridge.h"
#include "cartfactory.h"


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

	// initializers
	void init(void);

	// main loop executes at millisecond intervals of this value
	const uint32_t listen_interval = 100;
	const uint32_t cmd_timeout = 250;

	// pointer to cartridge objects
	Cartridge *cart;
	USB usb;

	// FMSC memory pointers
	__IO uint8_t * ce0_8b_ptr = (uint8_t *)(CE0_ADRESS);

	// listen for commands, data buffers for small transfer
	uint8_t umd_command;
	uint8_t data_buf[32];
	void listen(void);
	void ack_cmd(bool success);


	void set_cartridge_type(uint8_t mode);


	// LED methods
	void io_set_leds(uint8_t leds);
	void io_shift_leds(uint8_t dir);

	// Cartridge Methods
	typedef enum {vcart_off=0, vcart_3v3, vcart_5v} cartv_typ;
	uint16_t adc_icart;
	cartv_typ cartv;
	void set_cartridge_voltage(cartv_typ voltage);
	void io_set_level_translators(bool enable);
	void io_boot_precharge(bool charge);

};

#endif /* UMD_H_ */
