/*
 * CircBuffer.h
 *
 *  Created on: Feb. 4, 2020
 *      Author: rene
 */

#ifndef USB_H_
#define USB_H_

#include <cstdint>
#include <string>

#define CIRCULAR_BUFFER_SIZE 	2048
#define CIRCULAR_BUFFER_MASK 	(CIRCULAR_BUFFER_SIZE-1)

class USB{

public:
	USB();

	struct _USB_BUFFER{
		union __databuff{
			uint8_t		byte[CIRCULAR_BUFFER_SIZE];     	///< byte access within dataBuffer
			uint16_t    word[CIRCULAR_BUFFER_SIZE/2];   	///< word access within dataBuffer
		} data;
		uint16_t	size;
	} buffer;

	void transmit(void);
	bool is_full(void);

	uint16_t available(void);
	uint16_t available(uint32_t timeout_ms, uint16_t bytes_required);

	uint16_t put(std::string str);
	uint16_t put(uint8_t byte);
	uint16_t put(uint16_t word);
	uint16_t put(uint8_t *data, uint16_t len);
	uint16_t put(uint16_t *data, uint16_t len);

	uint8_t  get(void);
	uint16_t get(uint8_t* data, uint16_t size);

private:


};


#endif /* USB_H_ */
