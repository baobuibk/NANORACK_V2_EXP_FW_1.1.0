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

typedef enum {OFF = 0, COOL = 1, HEAT =2, } mode_ctrl_temp_t;

typedef struct _Temperature_CurrentStateTypedef_
{
	int16_t						Temperature_setpoint[4];
	int16_t						High_Threshold;
	int16_t						Low_Threshold;
	int64_t						Tec_voltage[4];		// nanoVoltage
	int16_t						Heater_duty[4];		// 0-9999
	int16_t						NTC_temperature[8];
	int16_t						BMP390_temperature;
	uint8_t 					Temp_status;	// temp3_auto temp3_ena temp2_auto temp2_ena temp1_auto temp1_ena temp0_auto temp0_ena
	uint8_t 					TEC_HEATER_status; // heater3_on heater2_on heater1_on heater0_on tec3_on tec2_on tec1_on tec0_on
	uint8_t						mode[4];		// 0: HEAT mode; 1: COOL mode
	uint8_t 					TEC_status_mode;
} Temperature_CurrentStateTypedef_t;


void Temperature_GetSet_Init(void);
void Temperature_GetSet_CreateTask(void);

void temperature_set_setpoint(uint8_t channel, int16_t setpoint);
int16_t temperature_get_setpoint(uint8_t channel);
int16_t temperature_get_temp_NTC(uint8_t channel);
void temperature_set_tec_vol(uint8_t channel, uint16_t voltage);
uint16_t temperature_get_tec_vol(uint8_t channel);
void temperature_set_heater_duty(uint8_t channel, uint8_t duty);
uint8_t temperature_get_heater_duty(uint8_t channel);
void temperature_set_auto_ctrl(uint8_t auto_0, uint8_t auto_1, uint8_t auto_2, uint8_t auto_3);
void temperature_set_ctrl(mode_ctrl_temp_t mode_0, mode_ctrl_temp_t mode_1, mode_ctrl_temp_t mode_2, mode_ctrl_temp_t mode_3);

void tec_set_mode(mode_ctrl_tec_t mode_0, mode_ctrl_tec_t mode_1, mode_ctrl_tec_t mode_2, mode_ctrl_tec_t mode_3);
void tec_on(uint8_t on_0, uint8_t on_1, uint8_t on_2, uint8_t on_3);

#endif /* TEMPERATURE_TEMPERATURE_H_ */
