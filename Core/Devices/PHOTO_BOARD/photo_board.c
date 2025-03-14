/*
 * photo_board.c
 *
 *  Created on: Mar 6, 2025
 *      Author: DELL
 */

#include "photo_board.h"

//ADG1414_Device_t photo_sw;

adg1414_chain_t photo_sw = {
    .dev = {
        .hspi = SPI2,
        .cs_port = PHOTO_PD_CS_GPIO_Port,
        .cs_pin = PHOTO_PD_CS_Pin,
        .channel_per_dev = 6   // Số kênh trên mỗi thiết bị
    },
    .num_of_chain = 6          // Số lượng thiết bị trong chuỗi
};

ADS8327_Device_t photo_adc;

void Photo_board_init(void)
{
	adg1414_Chain_init(&photo_sw);
	ADS8327_Device_Init(&photo_adc, SPI2, PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin, PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin, PHOTO_ADC_EOC_GPIO_Port, PHOTO_ADC_EOC_Pin);
}
