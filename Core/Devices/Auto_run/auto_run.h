/*
 * auto_run.h
 *
 *  Created on: Jan 26, 2025
 *      Author: CAO HIEU
 */

#ifndef DEVICES_AUTO_RUN_AUTO_RUN_H_
#define DEVICES_AUTO_RUN_AUTO_RUN_H_
#include "main.h"
#include "laser_board.h"
#include "photo_board.h"

extern volatile uint32_t laser_interval;
extern volatile uint8_t times_adc;
extern volatile uint8_t run_system;
extern volatile uint8_t run_adc;
extern volatile uint32_t adc_interval;
extern volatile uint8_t pair_slot;
extern volatile uint8_t current_column;
extern volatile uint8_t current_row;
extern volatile uint32_t user_delay;
extern volatile uint32_t rest_time;
extern volatile uint8_t do_time;
extern volatile uint8_t run_inf;

void AutoRun_CreateTask(void);

#endif /* DEVICES_AUTO_RUN_AUTO_RUN_H_ */
