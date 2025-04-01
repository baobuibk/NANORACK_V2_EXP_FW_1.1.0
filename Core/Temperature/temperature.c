/*
 * temperature.c
 *
 *  Created on: Dec 16, 2024
 *      Author: SANG HUYNH
 */

#include "temperature.h"
#include "scheduler.h"
#include "uart.h"
#include "command.h"
#include "ntc.h"
#include "lt8722.h"
#include "bmp390.h"
#include "heater.h"

/* Private define ------------------------------------------------------------*/
#define	TEC0_DIR	0
#define	TEC1_DIR	1
#define	TEC2_DIR	2
#define	TEC3_DIR	3

#define	TEMP0_AUTO	0
#define	TEMP1_AUTO	1
#define	TEMP2_AUTO	2
#define	TEMP3_AUTO	3

#define	TEC0_ON		0
#define	TEC1_ON		1
#define	TEC2_ON		2
#define	TEC3_ON		3
#define	HEATER0_ON	4
#define	HEATER1_ON	5
#define	HEATER2_ON	6
#define	HEATER3_ON	7

#define	TEC0_DIR	0
#define	TEC1_DIR	1
#define	TEC2_DIR	2
#define	TEC3_DIR	3

#define TEC_VOL_DEFAULT		500000000	//nanoVol

/* Private function ----------------------------------------------------------*/
static void temperature_update(void);
void temperature_auto_ctrl(int16_t temperature_now, uint8_t channel);

/* Private typedef -----------------------------------------------------------*/
typedef struct Temp_TaskContextTypedef
{
	SCH_TASK_HANDLE               	taskHandle;
	SCH_TaskPropertyTypedef       	taskProperty;
	uint32_t                      	taskTick;
} Temp_TaskContextTypedef;

/* Private variable -----------------------------------------------------------*/
static Temp_TaskContextTypedef           temp_task_context =
{
	SCH_INVALID_TASK_HANDLE,                // Will be updated by Scheduler
	{
		SCH_TASK_SYNC,                      // taskType;
		SCH_TASK_PRIO_0,                    // taskPriority;
		100,                                // taskPeriodInMS;
		temperature_update,                	// taskFunction;
		98									// taskTick
	},
};

Temperature_CurrentStateTypedef_t	s_Temperature_CurrentState =
{
	 0,								// Temp_change_flag
	{250, 250, 250, 250},			// Temp_setpoint[4]		(250 mean 25.0*C)
	 10,							// High_Threshold				(10 mean 1*C)
	 10,							// Low_Threshold				(30 mean 3*C)
	{TEC_VOL_DEFAULT, TEC_VOL_DEFAULT, TEC_VOL_DEFAULT, TEC_VOL_DEFAULT},	// Tec_voltage[4];  //nanoVoltage
	 0,								// Tec_dir;				// xxxx dir3 dir2 dir1 dir0 (LSB)
	{500, 500, 500, 500},			// Heater_duty[4];		// 0-999
	{0,	0, 0, 0, 0, 0, 0, 0},		// NTC_temp[8]			(250 mean 25.0*C)
	 0,								// BMP390_temp			(250 mean 25.0*C)
	 0,								// Temp_auto; 			// xxxx temp3_auto temp2_auto temp1_auto temp0_auto (LSB)
	 0,								// Tec_Heater_status;	// heater3_on heater2_on heater1_on heater0_on tec3_on tec2_on tec1_on tec0_on
};

static void temperature_update(void)
{
	if (s_Temperature_CurrentState.Temp_change_flag)
	{
		uint8_t Tec_dir = 0;
		for (uint8_t channel = 0; channel < 4; channel ++)
		{
			Tec_dir = ((s_Temperature_CurrentState.Tec_dir & (1 << channel)) == (1 << channel)) ? TEC_HEAT : TEC_COOL;
			lt8722_set_output_voltage_channel(channel, Tec_dir, s_Temperature_CurrentState.Tec_vol[channel]);
			heater_set_duty_pwm_channel(channel, s_Temperature_CurrentState.Heater_duty[channel]);
		}
		s_Temperature_CurrentState.Temp_change_flag = 0;
	}
	NTC_get_temperature(s_Temperature_CurrentState.NTC_temp);
	if ((s_Temperature_CurrentState.Temp_auto & (1 << TEMP0_AUTO)) == (1 << TEMP0_AUTO))
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temp[0], 0);
	if ((s_Temperature_CurrentState.Temp_auto & (1 << TEMP1_AUTO)) == (1 << TEMP1_AUTO))
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temp[1], 1);
	if ((s_Temperature_CurrentState.Temp_auto & (1 << TEMP2_AUTO)) == (1 << TEMP2_AUTO))
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temp[2], 2);
	if ((s_Temperature_CurrentState.Temp_auto & (1 << TEMP3_AUTO)) == (1 << TEMP3_AUTO))
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temp[3], 3);
}

void temperature_auto_ctrl(int16_t temperature_now, uint8_t channel)
{
	// Case: temperature is higher than expected temperature
	// Using: TEC
	if (temperature_now > s_Temperature_CurrentState.Temp_setpoint[channel] + s_Temperature_CurrentState.High_Threshold)
	{
		// UART_SendStringRing(UART_CMDLINE, "nhiet cao");
		// turn off heater
		heater_set_duty_pwm_channel(channel, 0);
		// turn on tec with COOL
		s_Temperature_CurrentState.Tec_dir &= ~(1 << channel);
		lt8722_set_output_voltage_channel(channel, TEC_COOL, s_Temperature_CurrentState.Tec_vol[channel]);
		lt8722_set_swen_req(channel, LT8722_SWEN_REQ_ENABLED);
		// update status
		s_Temperature_CurrentState.Tec_Heater_status |= (1 << channel);
		s_Temperature_CurrentState.Tec_Heater_status &= ~(1 << (channel + 4));
	}
	// Case: temperature is lower than expected temperature
	// Using: Heater
	else if (temperature_now < s_Temperature_CurrentState.Temp_setpoint[channel] - s_Temperature_CurrentState.Low_Threshold)
	{
		// UART_SendStringRing(UART_CMDLINE, "nhiet thap");
		// turn off tec
		lt8722_set_swen_req(channel, LT8722_SWEN_REQ_DISABLED);
		// turn on heater
		heater_set_duty_pwm_channel(channel, s_Temperature_CurrentState.Heater_duty[channel]);
		// update status
		s_Temperature_CurrentState.Tec_Heater_status |= (1 << (channel + 4));
		s_Temperature_CurrentState.Tec_Heater_status &= ~(1 << channel);
	}
	// Case: temperature is closed to expected temperature
	// Using: none
	else
	{
		// UART_SendStringRing(UART_CMDLINE, "nhiet bang");
		// turn off both tec and heater
		lt8722_set_swen_req(channel, LT8722_SWEN_REQ_DISABLED);
		heater_set_duty_pwm_channel(channel, 0);
		// update status
		s_Temperature_CurrentState.Tec_Heater_status &= ~((1 << (channel + 4)) | (1 << channel));
	}
	return;
}

void Temperature_GetSet_Init(void)
{
	return;
}

void Temperature_GetSet_CreateTask(void)
{
	SCH_TASK_CreateTask(&temp_task_context.taskHandle, &temp_task_context.taskProperty);
	return;
}

void temperature_set_setpoint(uint8_t channel, int16_t setpoint)
{
	s_Temperature_CurrentState.Temp_setpoint[channel] = setpoint;
	return;
}

int16_t temperature_get_setpoint(uint8_t channel)
{
	return s_Temperature_CurrentState.Temp_setpoint[channel];
}

int16_t temperature_get_temp_NTC(uint8_t channel)
{
	return s_Temperature_CurrentState.NTC_temp[channel];
}

void temperature_set_tec_vol(uint8_t channel, uint16_t voltage)
{
	s_Temperature_CurrentState.Temp_change_flag = 1;
	if (voltage > 3000) voltage = 3000;
	s_Temperature_CurrentState.Tec_vol[channel] = voltage*1000000;
}

uint16_t temperature_get_tec_vol_set(uint8_t channel)
{
	return (s_Temperature_CurrentState.Tec_vol[channel]/1000000);
}

uint16_t temperature_get_tec_vol_adc(uint8_t channel)
{
	return (s_Temperature_CurrentState.Tec_vol[channel]/1000000);
}

void temperature_set_heater_duty(uint8_t channel, uint8_t duty)
{
	s_Temperature_CurrentState.Temp_change_flag = 1;
    if (duty > 100) duty = 100;
	s_Temperature_CurrentState.Heater_duty[channel] = duty*10;
}
uint8_t temperature_get_heater_duty(uint8_t channel)
{
	return (s_Temperature_CurrentState.Heater_duty[channel]/10);
}

void temperature_set_auto_ctrl(uint8_t auto_0, uint8_t auto_1, uint8_t auto_2, uint8_t auto_3)
{
    s_Temperature_CurrentState.Temp_auto = (auto_0 << TEMP0_AUTO) | (auto_1 << TEMP1_AUTO) | (auto_2 << TEMP2_AUTO) | (auto_3 << TEMP3_AUTO);
	lt8722_set_swen_req(&tec_0, LT8722_SWEN_REQ_DISABLED);
	heater_set_duty_pwm_channel(0, 0);
	lt8722_set_swen_req(&tec_1, LT8722_SWEN_REQ_DISABLED);
	heater_set_duty_pwm_channel(1, 0);
	lt8722_set_swen_req(&tec_2, LT8722_SWEN_REQ_DISABLED);
	heater_set_duty_pwm_channel(2, 0);
	lt8722_set_swen_req(&tec_3, LT8722_SWEN_REQ_DISABLED);
	heater_set_duty_pwm_channel(3, 0);
    return;
}

void tec_set_dir(tec_dir_t dir_0, tec_dir_t dir_1, tec_dir_t dir_2, tec_dir_t dir_3)
{
    s_Temperature_CurrentState.Temp_change_flag = 1;
    s_Temperature_CurrentState.Tec_dir = (dir_0 << TEC0_DIR) | (dir_1 << TEC1_DIR) | (dir_2 << TEC2_DIR) | (dir_3 << TEC3_DIR);
    return;
}
