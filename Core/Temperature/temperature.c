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
#define	TEMP0_ENA	0
#define	TEMP0_AUTO	1
#define	TEMP1_ENA	2
#define	TEMP1_AUTO	3
#define	TEMP2_ENA	4
#define	TEMP2_AUTO	5
#define	TEMP3_ENA	6
#define	TEMP3_AUTO	7

#define	TEC0_ON		0
#define	TEC1_ON		1
#define	TEC2_ON		2
#define	TEC3_ON		3
#define	HEATER0_ON	4
#define	HEATER1_ON	5
#define	HEATER2_ON	6
#define	HEATER3_ON	7

#define	TEC0_MODE		0
#define	TEC1_MODE		1
#define	TEC2_MODE		2
#define	TEC3_MODE		3
#define	TEC0_STATUS		4
#define	TEC1_STATUS		5
#define	TEC2_STATUS		6
#define	TEC3_STATUS		7

#define IDLE	2
#define HEATING	1
#define COOLING	0

#define TEC_VOL_DEFAULT		500000000

/* Private function ----------------------------------------------------------*/
static void temperature_update(void);
void temperature_auto_ctrl(int16_t temperature_now, uint8_t channel);
void temperature_manual_ctrl(uint8_t channel);

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

static	Temperature_CurrentStateTypedef_t	s_Temperature_CurrentState =
{
	{250, 250, 250, 250},			// Temperature_setpoint[4]		(250 mean 25.0*C)
	 10,							// High_Threshold				(10 mean 1*C)
	 30,							// Low_Threshold				(30 mean 3*C)
	{TEC_VOL_DEFAULT, TEC_VOL_DEFAULT, TEC_VOL_DEFAULT, TEC_VOL_DEFAULT},	// Tec_voltage[4];  //nanoVoltage
	{3000, 3000, 3000, 3000},		// Heater_duty[4];		// 0-9999
	{0,	0, 0, 0},					// NTC_temperature[4]			(250 mean 25.0*C)
	 0,								// BMP390_temperature			(250 mean 25.0*C)
	 0,								// Temp_status; 		// temp3_auto temp3_ena temp2_auto temp2_ena temp1_auto temp1_ena temp0_auto temp0_ena
	 0,								// TEC_HEATER_status;	// heater3_on heater2_on heater1_on heater0_on tec3_on tec2_on tec1_on tec0_on
	{IDLE, IDLE, IDLE, IDLE},		//mode
	0								//
};

static void temperature_update(void)
{

//	UART_SendStringRing(UART_CMDLINE, "temp");
// update temperature from BMP390
// 	s_Temperature_CurrentState.BMP390_temperature = bmp390_get_temperature();
	s_Temperature_CurrentState.BMP390_temperature = 0U;
// update temperature from NTC
 	NTC_get_temperature(NTC_Temperature);
	s_Temperature_CurrentState.NTC_temperature[0] = NTC_Temperature[0];
	s_Temperature_CurrentState.NTC_temperature[1] = NTC_Temperature[1];
	s_Temperature_CurrentState.NTC_temperature[2] = NTC_Temperature[2];
	s_Temperature_CurrentState.NTC_temperature[3] = NTC_Temperature[3];

	s_Temperature_CurrentState.NTC_temperature[4] = NTC_Temperature[4];
	s_Temperature_CurrentState.NTC_temperature[5] = NTC_Temperature[5];
	s_Temperature_CurrentState.NTC_temperature[6] = NTC_Temperature[6];
	s_Temperature_CurrentState.NTC_temperature[7] = NTC_Temperature[7];

//	If temperature is out range, auto mode will turn on
//	if (NTC_channel0_temperature < 50 || NTC_channel0_temperature > 350)
//	{
//		temperature_enable_auto_control(0);
//		temperature_enable_auto_control(1);
//	}
//	if (NTC_channel2_temperature < 50 || NTC_channel2_temperature > 350)
//	{
//		temperature_enable_auto_control_TEC(2);
//		temperature_enable_auto_control_TEC(3);
//	}


	if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC0_STATUS)) == (1 << TEC0_STATUS))	// Nếu TEC ON
	{
		lt8722_set_swen_req(0, LT8722_SWEN_REQ_ENABLED);
		if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC0_MODE)) == (1 << TEC0_MODE))	// Nếu TEC COOL
		{
			lt8722_set_output_voltage_channel(0, TEC_COOL, s_Temperature_CurrentState.Tec_voltage[0]);
		}
		else	// Nếu TEC HEAT
		{
			lt8722_set_output_voltage_channel(0, TEC_HEAT, s_Temperature_CurrentState.Tec_voltage[0]);
		}
	}
	else
		lt8722_set_swen_req(0, LT8722_SWEN_REQ_DISABLED);

	if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC1_STATUS)) == (1 << TEC1_STATUS))	// Nếu TEC ON
	{
		lt8722_set_swen_req(1, LT8722_SWEN_REQ_ENABLED);
		if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC1_MODE)) == (1 << TEC1_MODE))	// Nếu TEC COOL
		{
			lt8722_set_output_voltage_channel(1, TEC_COOL, s_Temperature_CurrentState.Tec_voltage[1]);
		}
		else	// Nếu TEC HEAT
		{
			lt8722_set_output_voltage_channel(1, TEC_HEAT, s_Temperature_CurrentState.Tec_voltage[1]);
		}
	}
	else
		lt8722_set_swen_req(1, LT8722_SWEN_REQ_DISABLED);

	if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC2_STATUS)) == (1 << TEC2_STATUS))	// Nếu TEC ON
	{
		lt8722_set_swen_req(2, LT8722_SWEN_REQ_ENABLED);
		if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC2_MODE)) == (1 << TEC2_MODE))	// Nếu TEC COOL
		{
			lt8722_set_output_voltage_channel(2, TEC_COOL, s_Temperature_CurrentState.Tec_voltage[2]);
		}
		else	// Nếu TEC HEAT
		{
			lt8722_set_output_voltage_channel(2, TEC_HEAT, s_Temperature_CurrentState.Tec_voltage[2]);
		}
	}
	else
		lt8722_set_swen_req(2, LT8722_SWEN_REQ_DISABLED);

	if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC3_STATUS)) == (1 << TEC3_STATUS))	// Nếu TEC ON
	{
		lt8722_set_swen_req(3, LT8722_SWEN_REQ_ENABLED);
		if ((s_Temperature_CurrentState.TEC_status_mode & (1 << TEC3_MODE)) == (1 << TEC3_MODE))	// Nếu TEC COOL
		{
			lt8722_set_output_voltage_channel(3, TEC_COOL, s_Temperature_CurrentState.Tec_voltage[3]);
		}
		else	// Nếu TEC HEAT
		{
			lt8722_set_output_voltage_channel(3, TEC_HEAT, s_Temperature_CurrentState.Tec_voltage[3]);
		}
	}
	else
		lt8722_set_swen_req(3, LT8722_SWEN_REQ_DISABLED);


	if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP0_AUTO)) == (1 << TEMP0_AUTO))	 //channel 0 is auto
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temperature[0], 0);
	else
		if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP0_ENA)) == (1 << TEMP0_ENA)) //channel 0 isn't auto but it is enable
			temperature_manual_ctrl(0);

	if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP1_AUTO)) == (1 << TEMP1_AUTO))	 //channel 1 is auto
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temperature[1], 1);
	else
		if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP1_ENA)) == (1 << TEMP1_ENA)) //channel 1 isn't auto but it is enable
			temperature_manual_ctrl(1);

	if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP2_AUTO)) == (1 << TEMP2_AUTO))	 //channel 2 is auto
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temperature[2], 2);
	else
		if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP2_ENA)) == (1 << TEMP2_ENA)) //channel 2 isn't auto but it is enable
			temperature_manual_ctrl(2);

	if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP3_AUTO)) == (1 << TEMP3_AUTO))	 //channel 3 is auto
		temperature_auto_ctrl(s_Temperature_CurrentState.NTC_temperature[3], 3);
	else
		if ((s_Temperature_CurrentState.Temp_status & (1 << TEMP3_ENA)) == (1 << TEMP3_ENA)) //channel 3 isn't auto but it is enable
			temperature_manual_ctrl(3);

	return;
}

void temperature_auto_ctrl(int16_t temperature_now, uint8_t channel)
{
	// Case: temperature is higher than expected temperature
	// Using: TEC
	if (temperature_now > s_Temperature_CurrentState.Temperature_setpoint[channel] + s_Temperature_CurrentState.High_Threshold)
	{
		// turn off heater
		heater_set_duty_pwm_channel(channel, 0);
		// turn on tec
		lt8722_set_output_voltage_channel(channel, TEC_COOL, s_Temperature_CurrentState.Tec_voltage[channel]);
		// update status
		s_Temperature_CurrentState.mode[channel] = COOLING;
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << channel);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << (channel+4));
	}
	// Case: temperature is lower than expected temperature
	// Using: Heater
	else if (temperature_now < s_Temperature_CurrentState.Temperature_setpoint[channel] - s_Temperature_CurrentState.Low_Threshold)
	{
		// turn off tec
		lt8722_set_output_voltage_channel(channel, TEC_COOL, 0);
		// turn on heater
		heater_set_duty_pwm_channel(channel, s_Temperature_CurrentState.Heater_duty[channel]);
		// update status
		s_Temperature_CurrentState.mode[channel] = HEATING;
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << (channel+4));
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << channel);
	}
	// Case: temperature is closed expected temperature
	// Using: none
	else
	{
		// turn off both tec and heater
		lt8722_set_output_voltage_channel(channel, TEC_COOL, 0);
		heater_set_duty_pwm_channel(channel, 0);
		// update status
		s_Temperature_CurrentState.mode[channel] = IDLE;
		s_Temperature_CurrentState.TEC_HEATER_status &= ~((1 << (channel+4)) | (1 << channel));
	}
	return;
}

void temperature_manual_ctrl(uint8_t channel)
{
	if ((s_Temperature_CurrentState.TEC_HEATER_status & (1 << channel)) == (1 << channel))
	{
		// Turn off Heater
		heater_set_duty_pwm_channel(channel, 0);
		// Turn on TEC
		lt8722_set_output_voltage_channel(channel, TEC_COOL, s_Temperature_CurrentState.Tec_voltage[channel]);
		s_Temperature_CurrentState.mode[channel] = COOLING;
	}
	else if ((s_Temperature_CurrentState.TEC_HEATER_status & (1 << (channel + 4))) == (1 << (channel + 4)))
	{
		// Turn off TEC
		lt8722_set_output_voltage_channel(channel, TEC_COOL, 0);
		// Turn on Heater
		heater_set_duty_pwm_channel(channel, s_Temperature_CurrentState.Heater_duty[channel]);
		s_Temperature_CurrentState.mode[channel] = HEATING;
	}
	return;
}

void Temperature_GetSet_Init(void)
{
//	lt8722_init(channel);
	return;
}

void Temperature_GetSet_CreateTask(void)
{
	SCH_TASK_CreateTask(&temp_task_context.taskHandle, &temp_task_context.taskProperty);
	return;
}

void temperature_set_setpoint(uint8_t channel, int16_t setpoint)
{
	s_Temperature_CurrentState.Temperature_setpoint[channel] = setpoint;
	return;
}

int16_t temperature_get_setpoint(uint8_t channel)
{
	return s_Temperature_CurrentState.Temperature_setpoint[channel];
}

int16_t temperature_get_temp_NTC(uint8_t channel)
{
	return s_Temperature_CurrentState.NTC_temperature[channel];
}

void temperature_set_tec_vol(uint8_t channel, uint16_t voltage)
{
	s_Temperature_CurrentState.Tec_voltage[channel] = voltage*1000000;
}

uint16_t temperature_get_tec_vol(uint8_t channel)
{
	return (s_Temperature_CurrentState.Tec_voltage[channel]/1000000);
}

void temperature_set_heater_duty(uint8_t channel, uint8_t duty)
{
	s_Temperature_CurrentState.Heater_duty[channel] = duty*9999/100;
}
uint8_t temperature_get_heater_duty(uint8_t channel)
{
	return (s_Temperature_CurrentState.Heater_duty[channel]*100/9999);
}

void temperature_set_auto_ctrl(uint8_t auto_0, uint8_t auto_1, uint8_t auto_2, uint8_t auto_3)
{
	s_Temperature_CurrentState.Temp_status |= (auto_0 << 1) | (auto_1 << 3) | (auto_2 << 5) | (auto_3 << 7);
	return;
}

void temperature_set_ctrl(mode_ctrl_temp_t mode_0, mode_ctrl_temp_t mode_1, mode_ctrl_temp_t mode_2, mode_ctrl_temp_t mode_3)
{
	if (mode_0 == HEAT)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP0_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC0_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << HEATER0_ON);
	}
	else if (mode_0 == COOL)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP0_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER0_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << TEC0_ON);
	}
	else
	{
		s_Temperature_CurrentState.Temp_status &= ~(1 << TEMP0_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC0_ON);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER0_ON);
		lt8722_set_output_voltage_channel(0, TEC_COOL, 0);
	}
	if (mode_1 == HEAT)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP1_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC1_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << HEATER1_ON);
	}
	else if (mode_1 == COOL)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP1_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER1_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << TEC1_ON);

	}
	else
	{
		s_Temperature_CurrentState.Temp_status &= ~(1 << TEMP1_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC1_ON);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER1_ON);
		lt8722_set_output_voltage_channel(1, TEC_COOL, 0);
	}
	if (mode_2 == HEAT)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP2_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC2_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << HEATER2_ON);
	}
	else if (mode_2 == COOL)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP2_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER2_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << TEC2_ON);

	}
	else
	{
		s_Temperature_CurrentState.Temp_status &= ~(1 << TEMP2_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC2_ON);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER2_ON);
		lt8722_set_output_voltage_channel(2, TEC_COOL, 0);
	}
	if (mode_3 == HEAT)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP3_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC3_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << HEATER3_ON);
	}
	else if (mode_3 == COOL)
	{
		s_Temperature_CurrentState.Temp_status |= (1 << TEMP3_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER3_ON);
		s_Temperature_CurrentState.TEC_HEATER_status |= (1 << TEC3_ON);

	}
	else
	{
		s_Temperature_CurrentState.Temp_status &= ~(1 << TEMP3_ENA);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << TEC3_ON);
		s_Temperature_CurrentState.TEC_HEATER_status &= ~(1 << HEATER3_ON);
		lt8722_set_output_voltage_channel(3, TEC_COOL, 0);
	}
	return;
}

void tec_set_mode(mode_ctrl_tec_t mode_0, mode_ctrl_tec_t mode_1, mode_ctrl_tec_t mode_2, mode_ctrl_tec_t mode_3)
{
	s_Temperature_CurrentState.TEC_status_mode |= ((mode_0 << TEC0_MODE) | (mode_1 << TEC1_MODE) | (mode_2 <<TEC2_MODE ) | (mode_3 << TEC3_MODE));
	if (!mode_0)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC0_MODE);
	if (!mode_1)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC1_MODE);
	if (!mode_2)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC2_MODE);
	if (!mode_3)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC3_MODE);
}

void tec_on(uint8_t on_0, uint8_t on_1, uint8_t on_2, uint8_t on_3)
{
	s_Temperature_CurrentState.TEC_status_mode |= ((on_0 << TEC0_STATUS)  | (on_1 << TEC1_STATUS) | (on_2 <<TEC2_STATUS ) | (on_3 << TEC3_STATUS));
	if (!on_0)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC0_STATUS);
	if (!on_1)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC1_STATUS);
	if (!on_2)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC2_STATUS);
	if (!on_3)
		s_Temperature_CurrentState.TEC_status_mode &= ~(1 << TEC3_STATUS);
}
