/*
 * laser_board.h
 *
 *  Created on: Mar 5, 2025
 *      Author: HTSANG
 */

#ifndef DEVICES_LASER_BOARD_LASER_BOARD_H_
#define DEVICES_LASER_BOARD_LASER_BOARD_H_

#include "main.h"
#include "adg1414.h"
#include "mcp4902.h"

extern MCP4902_Device_t DAC_device;
extern adg1414_chain_t int_laser;
extern adg1414_dev_t ext_laser;

//extern ADG1414_Device_t laser_int;
//extern ADG1414_Device_t laser_ext;

void Laser_board_init(void);

#endif /* DEVICES_LASER_BOARD_LASER_BOARD_H_ */
