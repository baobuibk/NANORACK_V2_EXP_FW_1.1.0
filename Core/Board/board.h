/*
 * board.h
 *
 *  Created on: Nov 21, 2024
 *      Author: SANG HUYNH
 */

#ifndef BOARD_BOARD_H_
#define BOARD_BOARD_H_
#include <stm32f407xx.h>

//*****************************************************************************
// UART_CONSOLE
//*****************************************************************************
#define EXP_UART_HANDLE       USART6
#define EXP_UART_IRQ          USART6_IRQn


//*****************************************************************************
// UART_COPC
//*****************************************************************************
#define EXP_RS485_HANDLE       USART1
#define EXP_RS485_IRQ          USART1_IRQn


//*****************************************************************************
// I2C_SENSOR
//*****************************************************************************
#define SENSOR_I2C_HANDLE		I2C1
#define SENSOR_I2C_IRQ			I2C1_EV_IRQn


//*****************************************************************************
// I2C_COPC
//*****************************************************************************
#define EXP_I2C_HANDLE		I2C2
#define EXP_I2C_IRQ			I2C2_EV_IRQn

//*****************************************************************************
// SPI_TEC
//*****************************************************************************
#define SPI_TEC				SPI3

extern struct lt8722_dev tec_0;
extern struct lt8722_dev tec_1;
extern struct lt8722_dev tec_2;
extern struct lt8722_dev tec_3;
extern struct lt8722_dev * tec_table[];
extern struct mb85rs2mt_dev fram;
extern struct adg1414_dev exp_adg1414;

#endif /* BOARD_BOARD_H_ */
