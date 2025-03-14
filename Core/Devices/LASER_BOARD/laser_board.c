/*
 * laser_board.c
 *
 *  Created on: Mar 5, 2025
 *      Author: HTSANG
 */

#include "laser_board.h"
#include "main.h"
#include "adg1414.h"

adg1414_chain_t int_laser = {
    .dev = {
        .hspi = SPI1,
        .cs_port = LASER_SW_INT_CS_GPIO_Port,
        .cs_pin = LASER_SW_INT_CS_Pin,
        .channel_per_dev = 6   // Số kênh trên mỗi thiết bị
    },
    .num_of_chain = 6          // Số lượng thiết bị trong chuỗi
};

adg1414_dev_t ext_laser = {
	.hspi = SPI1,
	.cs_port = LASER_SW_EXT_CS_GPIO_Port,
	.cs_pin = LASER_SW_EXT_CS_Pin,
	.channel_per_dev = 8   // Số kênh trên mỗi thiết bị
};

MCP4902_Device_t DAC_device;

void Laser_board_init(void)
{
	adg1414_Chain_init(&int_laser);
	adg1414_init(&ext_laser);
	MCP4902_Device_Init(&DAC_device, SPI1, LASER_DAC_CS_GPIO_Port, LASER_DAC_CS_Pin, LASER_DAC_LATCH_GPIO_Port, LASER_DAC_LATCH_Pin);
}
