/*
 * uart.h
 *
 *  Created on: May 23, 2024
 *      Author: CAO HIEU
 */

#ifndef UART_UART_H_
#define UART_UART_H_

#include "stm32f4xx_ll_usart.h"

/* change the size of the buffer */

#define USART1_BUFFER_SIZE 1023
#define USART6_BUFFER_SIZE 255

typedef struct
{
    unsigned char *buffer;
    volatile unsigned int head;
    volatile unsigned int tail;
    unsigned int size;
} ring_buffer;

/* Initialize the ring buffer */
void UART_RingBuffer_Init(void);

/* reads the data in the rx_buffer and increment the tail count in rx_buffer */
int UART_ReadRing(USART_TypeDef *uart);

/* writes the data to the tx_buffer and increment the head count in tx_buffer */
void UART_WriteRing(USART_TypeDef *uart, int c);

/* function to send the string to the uart */
void UART_SendStringRing(USART_TypeDef *uart, const char *s);

/* Checks if the data is available to read in the rx_buffer */
int IsDataAvailable(USART_TypeDef *uart);

/* Resets the entire ring buffer, the new data will start from position 0 */
void UART_Flush_RingRx (USART_TypeDef *uart);
void UART_Flush_RingTx (USART_TypeDef *uart);


/* the ISR for the uart. put it in the IRQ handler */
void UART_Ring_ISR (USART_TypeDef *uart);

#endif /* UART_UART_H_ */
