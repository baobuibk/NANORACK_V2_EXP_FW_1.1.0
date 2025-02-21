/*
 * mcp4902.h
 *
 *  Created on: Jan 24, 2025
 *      Author: CAO HIEU
 */

#ifndef DAC_MCP4902_H_
#define DAC_MCP4902_H_

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_spi.h"

#define _VREF_DAC        5.0f

#define MCP_BUF_BIT      14
#define MCP_GA_BIT       13
#define MCP_SHDN_BIT     12
#define MCP_AB_BIT       15

#define DAC_CHA          0
#define DAC_CHB          1

#define DAC_CS_PORT      GPIOE
#define DAC_CS_PIN       LL_GPIO_PIN_14

#define DAC_LATCH_PORT   GPIOE
#define DAC_LATCH_PIN    LL_GPIO_PIN_15

void DAC_Init(void);
uint8_t v2dac(uint16_t voltage);
uint16_t dac2v(uint8_t dac);
void DAC_Write(uint8_t channel, uint8_t DAC_Data);
void DAC_Off(uint8_t channel);

#endif /* DAC_MCP4902_H_ */
