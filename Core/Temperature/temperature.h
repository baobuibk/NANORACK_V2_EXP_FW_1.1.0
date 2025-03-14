/*
 * temperature.h
 *
 *  Created on: Dec 16, 2024
 *      Author: SANG HUYNH
 */

#ifndef TEMPERATURE_TEMPERATURE_H_
#define TEMPERATURE_TEMPERATURE_H_

#include "main.h"
#include "board.h"
#include "lt8722.h"

//typedef enum {OFF = 0, COOL = 1, HEAT =2, } mode_ctrl_temp_t;

typedef struct _Temperature_CurrentStateTypedef_
{
	uint8_t						Temp_change_flag;	// 0: done; 1: need update
	int16_t						Temp_setpoint[4];
	int16_t						High_Threshold;
	int16_t						Low_Threshold;
	int64_t						Tec_vol[4];			// nanoVoltage
//	int64
	uint8_t						Tec_dir;			// xxxx dir3 dir2 dir1 dir0 (LSB)
	int16_t						Heater_duty[4];		// 0-999
	int16_t						NTC_temp[8];
	int16_t						BMP390_temp;
	uint8_t 					Temp_auto;	// xxxx temp3_auto temp2_auto temp1_auto temp0_auto (LSB)
	uint8_t 					Tec_Heater_status;	// heater3_on heater2_on heater1_on heater0_on tec3_on tec2_on tec1_on tec0_on (LSB)
} Temperature_CurrentStateTypedef_t;

extern Temperature_CurrentStateTypedef_t	s_Temperature_CurrentState;


void Temperature_GetSet_Init(void);
void Temperature_GetSet_CreateTask(void);

void temperature_set_setpoint(uint8_t channel, int16_t setpoint);
int16_t temperature_get_setpoint(uint8_t channel);
int16_t temperature_get_temp_NTC(uint8_t channel);
void temperature_set_tec_vol(uint8_t channel, uint16_t voltage);
uint16_t temperature_get_tec_vol_set(uint8_t channel);
uint16_t temperature_get_tec_vol_adc(uint8_t channel);
void temperature_set_heater_duty(uint8_t channel, uint8_t duty);
uint8_t temperature_get_heater_duty(uint8_t channel);
void temperature_set_auto_ctrl(uint8_t auto_0, uint8_t auto_1, uint8_t auto_2, uint8_t auto_3);

void tec_set_dir(tec_dir_t dir_0, tec_dir_t dir_1, tec_dir_t dir_2, tec_dir_t dir_3);

#endif /* TEMPERATURE_TEMPERATURE_H_ */
