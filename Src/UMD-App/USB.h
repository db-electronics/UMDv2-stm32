/*******************************************************************//**
 *  \file USB.h
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

#ifndef USB_H_
#define USB_H_

#include <cstdint>
#include <string>
#include <vector>

#define UMD_BUFFER_SIZE 	2048
#define UMD_BUFFER_MASK 	(UMD_BUFFER_SIZE-1)

class USB{

public:
	USB();

	struct _USB_BUFFER{
		union __databuff{
			uint8_t		bytes[UMD_BUFFER_SIZE];     	///< byte access within dataBuffer
			uint16_t    words[UMD_BUFFER_SIZE/2];   	///< word access within dataBuffer
		}data;
		uint16_t	size;
	} outbuf;

	union _VBUF{
		struct _VBYTES{
			uint8_t	lo;
			uint8_t	hi;
		}byte;
		uint16_t	word;
	};

	std::vector<_VBUF> vbuf;

	bool is_full(void);
	void transmit(void);
	void flush(void);

	uint16_t available(void);
	uint16_t available(uint32_t timeout_ms, uint16_t bytes_required);

	uint16_t put(std::string str);
	uint16_t put(uint8_t byte);
	uint16_t put(uint16_t word);
	uint16_t put(uint8_t *data, uint16_t len);
	uint16_t put(uint16_t *data, uint16_t len);

	uint8_t  get(void);
	uint16_t get(uint8_t* data, uint16_t size);
	uint16_t peak(uint8_t* data, uint16_t size);

private:


};


#endif /* USB_H_ */
