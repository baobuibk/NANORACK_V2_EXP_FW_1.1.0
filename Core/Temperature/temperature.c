/*
 * temperature.c
 *
 *  Created on: Dec 16, 2024
 *      Author: SANG HUYNH
 */

#include "temperature.h"

/* Private define ------------------------------------------------------------*/

//#define	TEMP0_AUTO	0
//#define	TEMP1_AUTO	1
//#define	TEMP2_AUTO	2
//#define	TEMP3_AUTO	3

#define	TEC0_EN		0
#define	TEC1_EN		1
#define	TEC2_EN		2
#define	TEC3_EN		3
#define	HEATER0_EN	4
#define	HEATER1_EN	5
#define	HEATER2_EN	6
#define	HEATER3_EN	7

#define NTC0_EN    0
#define NTC1_EN    1
#define NTC2_EN    2
#define NTC3_EN    3
#define NTC4_EN    4
#define NTC5_EN    5
#define NTC6_EN    6
#define NTC7_EN    7

#define	TEC0_DIR	0
#define	TEC1_DIR	1
#define	TEC2_DIR	2
#define	TEC3_DIR	3

#define TEC_VOL_DEFAULT		500000000	//nanoVol

/* Private function ----------------------------------------------------------*/
static void temperature_update(void);
void temperature_auto_TEC(void);
void temperature_auto_HTR(void);

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
	0,								// Pwr_status
	 0,								// Temp_change_flag
	370,							// Temp_Ref						(250 mean 25*C)
	 10,							// High_Threshold				(10 mean 1*C)
	 10,							// Low_Threshold				(30 mean 3*C)
	{TEC_VOL_DEFAULT, TEC_VOL_DEFAULT, TEC_VOL_DEFAULT, TEC_VOL_DEFAULT},	// Tec_voltage[4];  //nanoVoltage
	 0,								// Tec_dir;				// xxxx dir3 dir2 dir1 dir0 (LSB)
	{500, 500, 500, 500},			// Heater_duty[4];		// 0-999
	{0,	0, 0, 0, 0, 0, 0, 0},		// NTC_temp[8]			(250 mean 25.0*C)
	 0,								// BMP390_temp			(250 mean 25.0*C)
	 0,								// Tec_Heater_status;	// heater3_en heater2_en heater1_en heater0_en tec3_en tec2_en tec1_en tec0_en
	 0,								// NTC_Ref			// is one NTC from 0 to 7
	 0,								// Temp_auto;			// ON/OFF
};

static void temperature_update(void)
{
	if (s_Temperature_CurrentState.Temp_change_flag)
	{
		uint8_t Tec_dir = 0;
		for (uint8_t channel = 0; channel < 4; channel ++)
		{
			Tec_dir = ((s_Temperature_CurrentState.Tec_dir & (1 << channel)) == (1 << channel)) ? TEC_HEAT : TEC_COOL;
			lt8722_set_output_voltage_channel(tec_table[channel], Tec_dir, s_Temperature_CurrentState.Tec_vol[channel]);
			heater_set_duty_pwm_channel(channel, s_Temperature_CurrentState.Heater_duty[channel]);
		}
		s_Temperature_CurrentState.Temp_change_flag = 0;
	}
	NTC_get_temperature(s_Temperature_CurrentState.NTC_temp);

	if (s_Temperature_CurrentState.Temp_auto) {
		temperature_auto_TEC();
		temperature_auto_HTR();
	}
}

void temperature_auto_TEC(void) {
	if (s_Temperature_CurrentState.NTC_temp[s_Temperature_CurrentState.NTC_Ref] > s_Temperature_CurrentState.Temp_Ref + s_Temperature_CurrentState.High_Threshold) {
		heater_set_duty_pwm_channel(0, 0);
		heater_set_duty_pwm_channel(1, 0);
		heater_set_duty_pwm_channel(2, 0);
		heater_set_duty_pwm_channel(3, 0);
		if ((s_Temperature_CurrentState.Tec_Heater_status >> TEC0_EN) & 0x01) {
			s_Temperature_CurrentState.Tec_dir &= ~(1 << TEC0_EN);
			lt8722_set_output_voltage_channel(&tec_0, TEC_COOL, s_Temperature_CurrentState.Tec_vol[0]);
			lt8722_set_swen_req(&tec_0, LT8722_SWEN_REQ_ENABLED);
		}
		if ((s_Temperature_CurrentState.Tec_Heater_status >> TEC1_EN) & 0x01) {
			s_Temperature_CurrentState.Tec_dir &= ~(1 << TEC1_EN);
			lt8722_set_output_voltage_channel(&tec_1, TEC_COOL, s_Temperature_CurrentState.Tec_vol[1]);
			lt8722_set_swen_req(&tec_1, LT8722_SWEN_REQ_ENABLED);
		}
		if ((s_Temperature_CurrentState.Tec_Heater_status >> TEC2_EN) & 0x01) {
			s_Temperature_CurrentState.Tec_dir &= ~(1 << TEC2_EN);
			lt8722_set_output_voltage_channel(&tec_2, TEC_COOL, s_Temperature_CurrentState.Tec_vol[2]);
			lt8722_set_swen_req(&tec_2, LT8722_SWEN_REQ_ENABLED);
		}
		if ((s_Temperature_CurrentState.Tec_Heater_status >> TEC3_EN) & 0x01) {
			s_Temperature_CurrentState.Tec_dir &= ~(1 << TEC3_EN);
			lt8722_set_output_voltage_channel(&tec_3, TEC_COOL, s_Temperature_CurrentState.Tec_vol[3]);
			lt8722_set_swen_req(&tec_3, LT8722_SWEN_REQ_ENABLED);
		}
	}
}
void temperature_auto_HTR(void) {
	if (s_Temperature_CurrentState.NTC_temp[s_Temperature_CurrentState.NTC_Ref] < s_Temperature_CurrentState.Temp_Ref + s_Temperature_CurrentState.Low_Threshold) {
		lt8722_set_swen_req(&tec_0, LT8722_SWEN_REQ_DISABLED);
		lt8722_set_swen_req(&tec_1, LT8722_SWEN_REQ_DISABLED);
		lt8722_set_swen_req(&tec_2, LT8722_SWEN_REQ_DISABLED);
		lt8722_set_swen_req(&tec_3, LT8722_SWEN_REQ_DISABLED);
		if ((s_Temperature_CurrentState.Tec_Heater_status >> HEATER0_EN) & 0x01)
			heater_set_duty_pwm_channel(0, s_Temperature_CurrentState.Heater_duty[0]);
		if ((s_Temperature_CurrentState.Tec_Heater_status >> HEATER1_EN) & 0x01)
			heater_set_duty_pwm_channel(1, s_Temperature_CurrentState.Heater_duty[1]);
		if ((s_Temperature_CurrentState.Tec_Heater_status >> HEATER2_EN) & 0x01)
			heater_set_duty_pwm_channel(2, s_Temperature_CurrentState.Heater_duty[2]);
		if ((s_Temperature_CurrentState.Tec_Heater_status >> HEATER3_EN) & 0x01)
			heater_set_duty_pwm_channel(3, s_Temperature_CurrentState.Heater_duty[3]);
	}
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

void temperature_set_setpoint(int16_t setpoint)
{
	s_Temperature_CurrentState.Temp_Ref = setpoint;
	return;
}

int16_t temperature_get_setpoint()
{
	return s_Temperature_CurrentState.Temp_Ref;
}

int16_t temperature_get_temp_NTC(uint8_t channel)
{
	return s_Temperature_CurrentState.NTC_temp[channel];
}

void temperature_set_tec_vol(uint8_t channel, uint16_t voltage)
{
	s_Temperature_CurrentState.Temp_change_flag = 1;
	if (voltage > 3000) voltage = 3000;
	s_Temperature_CurrentState.Tec_vol[channel] = (int64_t)voltage*1000000;
	LL_mDelay(1);
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

void temperature_set_tec_dir(tec_dir_t dir_0, tec_dir_t dir_1, tec_dir_t dir_2, tec_dir_t dir_3)
{
    s_Temperature_CurrentState.Temp_change_flag = 1;
    s_Temperature_CurrentState.Tec_dir = (dir_0 << TEC0_DIR) | (dir_1 << TEC1_DIR) | (dir_2 << TEC2_DIR) | (dir_3 << TEC3_DIR);
    return;
}

void temperature_get_tec_dir(tec_dir_t *tec_dir) {
	*tec_dir = (tec_dir_t)s_Temperature_CurrentState.Tec_dir;
}

void temperature_set_tec_auto(uint8_t tec_ena) {
	s_Temperature_CurrentState.Tec_Heater_status = (s_Temperature_CurrentState.Tec_Heater_status & 0xF0) | (tec_ena & 0x0F);
}

void temperature_get_tec_auto(uint8_t *tec_ena) {
	*tec_ena = (uint8_t)(s_Temperature_CurrentState.Tec_Heater_status & 0x0F);
}

void temperature_set_heater_auto(uint8_t heater_ena) {
	s_Temperature_CurrentState.Tec_Heater_status = ((heater_ena << 4) & 0xF0) | (s_Temperature_CurrentState.Tec_Heater_status & 0x0F);
}

void temperature_get_heater_auto(uint8_t *heater_ena) {
	*heater_ena = (uint8_t)((s_Temperature_CurrentState.Tec_Heater_status >> 4) & 0x0F);
}

void temperature_set_ntc_ref(uint8_t NTC_Ref) {
	s_Temperature_CurrentState.NTC_Ref = NTC_Ref;
}

void temperature_get_ntc_ref(uint8_t *NTC_Ref) {
	*NTC_Ref = s_Temperature_CurrentState.NTC_Ref;
}

void temperature_set_auto_ctrl(uint8_t Temp_auto) {
	s_Temperature_CurrentState.Temp_auto = Temp_auto;
}
void temperature_get_auto_ctrl(uint8_t *Temp_auto) {
	*Temp_auto = s_Temperature_CurrentState.Temp_auto;
}
