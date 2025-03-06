/*
 * photo_board.c
 *
 *  Created on: Mar 6, 2025
 *      Author: DELL
 */

#include "photo_board.h"

ADG1414_Device_t photo_sw;
ADS8327_Device_t photo_adc;

void Photo_board_init(void)
{
	ADG1414_Chain_Init(&photo_sw, SPI2, PHOTO_PD_CS_GPIO_Port, PHOTO_PD_CS_Pin, INTERNAL_CHAIN_SWITCH_NUM);
	ADS8327_Device_Init(&photo_adc, SPI2, PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin, PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin, PHOTO_ADC_EOC_GPIO_Port, PHOTO_ADC_EOC_Pin);
}
