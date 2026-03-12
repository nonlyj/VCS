#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>


#define SERIAL_UART_CLK   	RCC_APB2Periph_USART1
#define SERIAL_GPIO_CLK   	RCC_APB2Periph_GPIOA
#define SERIAL_GPIO_PORT  	GPIOA
#define SERIAL_TX						GPIO_Pin_9
#define SERIAL_RX						GPIO_Pin_10
#define SERIAL_BAUDRATE			115200
#define SERIAL_PARITY				USART_Parity_No
#define SERIAL_STOPBITS			USART_StopBits_1
#define SERIAL_WORDLENGTH		USART_WordLength_8b



extern char Serial_RxPacket[];
extern volatile uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

#endif
