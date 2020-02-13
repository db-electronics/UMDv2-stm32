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
#include "fatfs.h"

#include "USB.h"
#include "Cartridges/Cartridge.h"
#include "CartFactory.h"


#define LED_SHIFT_DIR_LEFT		0
#define LED_SHIFT_DIR_RIGHT 	1


#define CE0_ADRESS		       	0x60000000U
#define CE1_ADRESS     		  	0x64000000U
#define CE2_ADRESS       		0x68000000U
#define CE3_ADRESS       		0x6C000000U

#define UBUF_SIZE				8192

/*******************************************************************//**
 * \class UMD
 * \brief UMD application
 **********************************************************************/
class UMD{

public:
	UMD();

    /*******************************************************************//**
     * \brief the only method visible from main.cpp, it never returns
     **********************************************************************/
	void run(void);

private:

	/*******************************************************************//**
	 * \brief initialize UMD hardware and variables
	 **********************************************************************/
	void init(void);

	// UMD 'global' variables
	Cartridge *cart;				///< pointer to cartridge object
	USB usb;						///< USB object for communications
	FATFS SDFatFs;					///< SD Card FAT file system object
	FIL dbFile;						///< SD Card file object

	struct _ADC_READINGS{
		uint16_t current;			///< latest ADC reading of cartridge current
		uint16_t avg;
		uint16_t buffer[8];
	}adc;

	uint32_t pc_assigned_id;

	// CMD WORDS
	enum COMMANDS : uint16_t {
		CMD_UMD = 		0x0000,		///< UMD replies with "UMD v2.0.0.0" - only command which does not ACK
		CMD_SETID = 	0x0001,		///< assigned pc_assigned_id for multi-umd setup
		CMD_SETLEDS = 	0x0002,
		CMD_SETCARTV =  0x0003,
		CMD_GETCARTV =  0x0004
	};

	// CMD REPLIES
	const struct{
		uint16_t ACK = 0xDBDB;
		uint16_t NO_ACK = 0xFFFF;
		uint16_t PAYLOAD_TIMEOUT = 0xDEAD;
		uint16_t CRC_ERROR = 0xCBAD;
		uint16_t CRC_OK = 0xC000;
	}CMDREPLY;

	// FMSC memory pointers
	__IO uint8_t * ce0_8b_ptr = (uint8_t *)(CE0_ADRESS);

	// main loop executes at millisecond intervals of this value
	const uint32_t LISTEN_INTERVAL = 100;
	const uint32_t CMD_TIMEOUT = 100;
	const uint32_t PAYLOAD_TIMEOUT = 200;

	// listen for commands, data buffers for small transfer
	const uint16_t CMD_HEADER_SIZE = 8;
	/*******************************************************************//**
     * \brief cmd
     * cmd union to properly receive commands from the PC
     **********************************************************************/
	struct{
		union{
			struct{
				uint32_t sof;
				uint32_t crc;
			};
			struct{
				uint16_t size;
				uint16_t cmd;
				uint16_t crc_lo;
				uint16_t crc_hi;
			};
			uint8_t bytes[8];
		}header;
	}cmd;

	// gp data buffer
	union _UMD_BUF{
		uint32_t u32[UBUF_SIZE/4];
		uint16_t u16[UBUF_SIZE/2];
		uint8_t  u8[UBUF_SIZE];
	}ubuf;

	void listen(void);

    /*******************************************************************//**
     * \brief Calculate the CRC32/MPEG2 checksum
     * \param *data pointer to little endian data
     * \param len length in bytes of the data
     * \param bool reset true to begin a new CRC calc, false to accumulate
     * \return the current crc value
     **********************************************************************/
	uint32_t crc32mpeg2_calc(uint32_t *data, uint32_t len, bool reset);

	void set_cartridge_type(uint8_t mode);

	// LED methods
	void io_set_leds(uint8_t leds);
	void io_shift_leds(uint8_t dir);

	// Cartridge Methods
	void io_set_level_translators(bool enable);
	void io_boot_precharge(bool charge);

};

#endif /* UMD_H_ */
