/*
 * laser_board.c
 *
 *  Created on: Mar 5, 2025
 *      Author: HTSANG
 */

#include "laser_board.h"
#include "main.h"

ADG1414_Device_t laser_int;
ADG1414_Device_t laser_ext;
MCP4902_Device_t DAC_device;

void Laser_board_init(void)
{
	ADG1414_Chain_Init(&laser_int, SPI1, LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin, INTERNAL_CHAIN_SWITCH_NUM);
	ADG1414_Chain_Init(&laser_ext, SPI1, LASER_SW_EXT_CS_GPIO_Port, LASER_SW_EXT_CS_Pin, EXTERNAL_CHAIN_SWITCH_NUM);
	MCP4902_Device_Init(&DAC_device, SPI1, LASER_DAC_CS_GPIO_Port, LASER_DAC_CS_Pin, LASER_DAC_LATCH_GPIO_Port, LASER_DAC_LATCH_Pin);
}
