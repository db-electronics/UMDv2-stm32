/*******************************************************************//**
 *  \file UMD_Commands.h
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

#include "UMD.h"

/*******************************************************************//**
 * 0x0000
 **********************************************************************/
uint32_t UMD::cmd_undefined(UMD_BUF *buf){


	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0001
 **********************************************************************/
uint32_t UMD::cmd_listcmd(UMD_BUF *buf){

	// generate a nice terminal friendly output of available commands
	usb.put(std::string("UMDv2 Commands:\n\n"));
	for( auto &cmd_it : cmd_table ) {
		usb.put(cmd_it.name);
		usb.put(std::string("\n"));
	}
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0002
 **********************************************************************/
uint32_t UMD::cmd_setleds(UMD_BUF *buf){

	// first byte contains the LED value
	io_set_leds(*(buf->u8));
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0003
 **********************************************************************/
uint32_t UMD::cmd_setid(UMD_BUF *buf){

	// first byte contains the LED value
	pc_assigned_id = *(buf->u32);
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0004
 **********************************************************************/
uint32_t UMD::cmd_version(UMD_BUF *buf){

	usb.put(std::string("UMD v2.0.0.0"));
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0005
 **********************************************************************/
uint32_t UMD::cmd_getcartv(UMD_BUF *buf){

	usb.put(static_cast<uint8_t>(cart->vcart));
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0006
 **********************************************************************/
uint32_t UMD::cmd_setcartv(UMD_BUF *buf){

	// first byte contains the voltage value
	cart->set_voltage(static_cast<Cartridge::eVoltage>(*(buf->u8)));
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0007
 **********************************************************************/
uint32_t UMD::cmd_getadapterid(UMD_BUF *buf){

	cart->get_adapter_id();
	usb.put(cart->param.id);
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0008
 **********************************************************************/
uint32_t UMD::cmd_getflashid(UMD_BUF *buf){

	cart->get_flash_id();
	usb.put(cart->flash_info.manufacturer);
	usb.put(cart->flash_info.device);
	usb.put(cart->flash_info.size);
	return UMD_CMD_OK;
}

/*******************************************************************//**
 * 0x0009
 **********************************************************************/
uint32_t UMD::cmd_readrom(UMD_BUF *buf){
	uint32_t address, crc, crc_len;
	uint16_t size, pad;

	// retrieve start address and size in bytes of requested read
	address = *(buf->u32);
	size = *((buf + sizeof(address))->u16);
	crc_len = size;

	// read the rom
	if( cart->param.bus_size == 8 ){
		cart->read_bytes(address, &buf->u8[0], size, Cartridge::mem_prg);
	}else{
		cart->read_words(address, &buf->u16[0], size, Cartridge::mem_prg);
	}

	// send back to host, 0 pad to nearest u32 size
	pad = size % sizeof(uint32_t);
	if( pad != 0){
		pad = 4 - pad;
		size += pad;
		crc_len += pad;
		while(pad--){
			usb.put((uint8_t)0x00);
		}
	}

	// put data in output buffer
	usb.put(&buf->u8[0], size);
	// add crc32
	crc = crc32mpeg2_calc(buf->u32, crc_len, true);
	usb.put(crc);


	return UMD_CMD_OK;
}

