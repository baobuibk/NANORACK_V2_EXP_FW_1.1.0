/*
 * photo_board.h
 *
 *  Created on: Mar 6, 2025
 *      Author: DELL
 */

#ifndef DEVICES_PHOTO_BOARD_PHOTO_BOARD_H_
#define DEVICES_PHOTO_BOARD_PHOTO_BOARD_H_

#include "ads8327.h"
#include "adg1414.h"
#include "main.h"

//extern ADG1414_Device_t photo_sw;

extern adg1414_chain_t photo_sw;
extern ADS8327_Device_t photo_adc;

void Photo_board_init(void);

#endif /* DEVICES_PHOTO_BOARD_PHOTO_BOARD_H_ */
