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
#include <vector>

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

#define UMD_BUFER_SIZE			8192

#define IGNORE_CRC				1


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
	void listen(void);

	// UMD 'global' variables
	Cartridge *cart;				///< pointer to cartridge object
	USB usb;						///< USB object for communications
	FATFS SDFatFs;					///< SD Card FAT file system object
	FIL dbFile;						///< SD Card file object
	uint32_t cmd_return_code;
	uint32_t pc_assigned_id;
	uint8_t cart_id;

	struct _ADC_READINGS{
		uint16_t current;			///< latest ADC reading of cartridge current
		uint16_t avg;
		uint16_t buffer[8];
	}adc;

	// CMD REPLIES
	const struct{
		uint16_t NO_ACK = 0xFFFF;
		uint16_t CMD_FAILED = 0xFFFE;
		uint16_t PAYLOAD_TIMEOUT = 0xFFFD;
		uint16_t CRC_ERROR = 0xFFFC;
		uint16_t CRC_OK = 0xFFFB;
	}CMDREPLY;

	// FMSC memory pointers
	//__IO uint8_t * ce0_8b_ptr = (uint8_t *)(CE0_ADRESS);

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
				uint16_t cmd;
				uint16_t payload_size;
				uint16_t crc_lo;
				uint16_t crc_hi;
			};
			uint8_t bytes[8];
		}header;
	}cmd;

	// gp data buffer
	union UMD_BUF{
		uint32_t u32[UMD_BUFER_SIZE/4];
		uint16_t u16[UMD_BUFER_SIZE/2];
		uint8_t  u8[UMD_BUFER_SIZE];
	}ubuf;



    /*******************************************************************//**
     * \brief Calculate the CRC32/MPEG2 checksum
     * \param *data pointer to little endian data
     * \param len length in bytes of the data
     * \param bool reset true to begin a new CRC calc, false to accumulate
     * \return the current crc value
     **********************************************************************/
	uint32_t crc32mpeg2_calc(uint32_t *data, const uint32_t& len, bool reset);

	void set_cartridge_type(const uint8_t& mode);

	// LED methods
	void io_set_leds(const uint8_t& leds);
	void io_shift_leds(const uint8_t& dir);

	// Cartridge Methods
	void io_set_level_translators(bool enable);
	void io_boot_precharge(bool charge);

	/* Return codes for UMD commands */
	typedef enum{
		UMD_CMD_OK   = 0,
		UMD_CMD_FAIL,
	}UMD_StatusTypedef;

	// command struct includes a function pointer and a descriptive string
	struct UMD_CMD{
		uint32_t		(UMD::*command)(UMD_BUF *buf) = nullptr;    /**< function pointer implementing the command */
		std::string 	name;                           			/**< command name */
	};

	// commands are decoded by their index in the vector
	UMD_CMD exec_command;
	std::vector<UMD_CMD> cmd_table {
		{ &UMD::cmd_undefined, 		"0x0000: undefined" },
		{ &UMD::cmd_listcmd,   		"0x0001: list commands" },
		{ &UMD::cmd_setleds,   		"0x0002: set leds      [uint32_t]" },
		{ &UMD::cmd_setid,     		"0x0003: set id        [uint32_t]" },
		{ &UMD::cmd_version,   		"0x0004: get version" },
		{ &UMD::cmd_getcartv,  		"0x0005: get cartv" },
		{ &UMD::cmd_setcartv,  		"0x0006: set cartv:    [uint32_t]" },
		{ &UMD::cmd_getadapterid,	"0x0007: get adapterid" }
	};

	// Command prototypes
	uint32_t cmd_undefined(UMD_BUF *buf);
	uint32_t cmd_listcmd(UMD_BUF *buf);
	uint32_t cmd_setleds(UMD_BUF *buf);
	uint32_t cmd_setid(UMD_BUF *buf);
	uint32_t cmd_version(UMD_BUF *buf);
	uint32_t cmd_getcartv(UMD_BUF *buf);
	uint32_t cmd_setcartv(UMD_BUF *buf);
	uint32_t cmd_getadapterid(UMD_BUF *buf);

};

/*******************************************************************//**
 * \class Command
 * \brief UMD application
 **********************************************************************/
class Command{
public:

	uint16_t code;
	uint16_t payload_size;
	uint32_t crc32;

	Command(uint8_t *data);
};

#endif /* UMD_H_ */
