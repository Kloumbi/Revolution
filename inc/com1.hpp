/*
 * com.hpp
 *
 *  Created on: Mar 27, 2019
 *      Author: rev
 */

#ifndef COM1_HPP_
#define COM1_HPP_

#include "stm32f4xx_usart.h"
#include "hardware.h"
#include "buffer.hpp"
#include "flash.hpp"

extern "C" {
void USART1_IRQHandler(void);
}

class Com1
{
public:

	static Com1* getInstance();
	virtual ~Com1();
	void write(uint8_t data);
	void sendBytes(uint8_t *data, uint32_t nBytes);
	void sendByte8ToBinaryString(uint8_t data);
	void sendByte16ToBinaryString(uint16_t data);
	void sendByte32ToBinaryString(uint32_t data);
	void sendString(const char *s);
	void sendString(uint8_t *u);
	uint8_t read();
	void incommingDataDecoder(Flash* flash);
	bool dataAvailable();
	void setBaudeRate(uint32_t baudrate);


private:

	Com1();

	bool 					isTransmitting;
	uint8_t 				pixelColumn[1156];
	Buffer<uint8_t, 1024> 	rxBuffer;
	Buffer<uint8_t, 1024> 	txBuffer;
	static Com1*			instance;
	friend void USART1_IRQHandler(void);


};


#endif /* COM1_HPP_ */
