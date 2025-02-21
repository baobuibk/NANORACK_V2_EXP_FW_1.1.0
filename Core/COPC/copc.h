/*
 * copc.h
 *
 *  Created on: Dec 18, 2024
 *      Author: SANG HUYNH
 */

#ifndef COPC_COPC_H_
#define COPC_COPC_H_

#include "stdint.h"
#define COPC_CMD_MAX_LEN	32

typedef struct _COMMON_FRAME_
{
	uint8_t Cmd;
} COMMON_FRAME;


// FROM COPC TO EXP
//--------------------------------------------------------------------------------------------
typedef struct _COPC_SET_PW_FRAME_
{
	uint8_t Cmd;
	uint8_t Status_PW;
} COPC_SET_PW_FRAME;

typedef struct _COPC_SET_TEMP_COMMAND_FRAME_
{
	uint8_t		Cmd;
	uint8_t		Setpoint_0_High;
	uint8_t		Setpoint_0_Low;
	uint8_t		Setpoint_1_High;
	uint8_t		Setpoint_1_Low;
	uint8_t		Setpoint_2_High;
	uint8_t		Setpoint_2_Low;
	uint8_t		Setpoint_3_High;
	uint8_t		Setpoint_3_Low;
} COPC_SET_TEMP_COMMAND_FRAME;

#define COPC_GET_TEMP_SETPOINT_COMMAND_FRAME		COMMON_FRAME
#define COPC_GET_TEMP_COMMAND_FRAME		 			COMMON_FRAME

typedef struct _COPC_SET_TEC_VOLTAGE_COMMAND_FRAME_
{
	uint8_t		Cmd;
	uint8_t		Voltage_0_High;
	uint8_t		Voltage_0_Low;
	uint8_t		Voltage_1_High;
	uint8_t		Voltage_1_Low;
	uint8_t		Voltage_2_High;
	uint8_t		Voltage_2_Low;
	uint8_t		Voltage_3_High;
	uint8_t		Voltage_3_Low;
} COPC_SET_TEC_VOLTAGE_COMMAND_FRAME;

#define COPC_GET_TEC_VOLTAGE_COMMAND_FRAME			COMMON_FRAME

typedef struct _COPC_SET_HEATER_DUTY_COMMAND_FRAME_
{
	uint8_t		Cmd;
	uint8_t		Duty_0;
	uint8_t		Duty_1;
	uint8_t		Duty_2;
	uint8_t		Duty_3;
} COPC_SET_HEATER_DUTY_COMMAND_FRAME;

#define COPC_GET_HEATER_DUTY_COMMAND_FRAME			COMMON_FRAME

typedef struct _COPC_TEMP_CTRL_COMMAND_FRAME_
{
	uint8_t		Cmd;
	uint8_t		Mode_0;
	uint8_t		Mode_1;
	uint8_t		Mode_2;
	uint8_t		Mode_3;
} COPC_TEMP_CTRL_COMMAND_FRAME;

typedef struct _COPC_TEMP_AUTO_CTRL_COMMAND_FRAME_
{
	uint8_t		Cmd;
	uint8_t		Auto_0;
	uint8_t		Auto_1;
	uint8_t		Auto_2;
	uint8_t		Auto_3;
} COPC_TEMP_AUTO_CTRL_COMMAND_FRAME;

#define COPC_SET_IR_DUTY_COMMAND_FRAME		COPC_SET_HEATER_DUTY_COMMAND_FRAME
#define COPC_GET_IR_DUTY_COMMAND_FRAME		COMMON_FRAME
#define COPC_GET_ALL_COMMAND_FRAME			COMMON_FRAME
// Union to encapsulate all frame types
typedef union _COPC_Sfp_Payload_
{
	COMMON_FRAME							commonFrame;
	COPC_SET_PW_FRAME						setPowerCommandFrame;
	COPC_SET_TEMP_COMMAND_FRAME				setTempCommandFrame;
	COPC_GET_TEMP_SETPOINT_COMMAND_FRAME	getTempSetpointCommandFrame;
	COPC_GET_TEMP_COMMAND_FRAME				getTempCommandFrame;
	COPC_SET_TEC_VOLTAGE_COMMAND_FRAME		setTecVoltageCommandFrame;
	COPC_GET_TEC_VOLTAGE_COMMAND_FRAME		getTecVoltageCommandFrame;
	COPC_SET_HEATER_DUTY_COMMAND_FRAME		setHeaterDutyCommandFrame;
	COPC_GET_HEATER_DUTY_COMMAND_FRAME		getHeaterDutyCommandFrame;
	COPC_TEMP_CTRL_COMMAND_FRAME			tempCtrlCommandFrame;
	COPC_TEMP_AUTO_CTRL_COMMAND_FRAME		tempAutoCtrlCommandFrame;
	COPC_SET_IR_DUTY_COMMAND_FRAME			setIrDutyCommandFrame;
	COPC_GET_IR_DUTY_COMMAND_FRAME			getIrDutyCommandFrame;
	COPC_GET_ALL_COMMAND_FRAME				getAllCommandFrame;
} COPC_Sfp_Payload_t;


// FROM EXP TO COPC
//--------------------------------------------------------------------------------------------
#define EXP_SET_TEMP_RESPONSE_FRAME			COMMON_FRAME
#define EXP_SET_PW_RESPONSE_FRAME			COMMON_FRAME

typedef	struct _EXP_GET_TEMP_SETPOINT_RESPONSE_FRAME_
{
	uint8_t		Cmd;
	uint8_t		Setpoint_0_High;
	uint8_t		Setpoint_0_Low;
	uint8_t		Setpoint_1_High;
	uint8_t		Setpoint_1_Low;
	uint8_t		Setpoint_2_High;
	uint8_t		Setpoint_2_Low;
	uint8_t		Setpoint_3_High;
	uint8_t		Setpoint_3_Low;
} EXP_GET_TEMP_SETPOINT_RESPONSE_FRAME;

typedef struct _EXP_GET_TEMP_RESPONSE_FRAME_
{
	uint8_t		Cmd;
	uint8_t		NTC_0_High;
	uint8_t		NTC_0_Low;
	uint8_t		NTC_1_High;
	uint8_t		NTC_1_Low;
	uint8_t		NTC_2_High;
	uint8_t		NTC_2_Low;
	uint8_t		NTC_3_High;
	uint8_t		NTC_3_Low;
} EXP_GET_TEMP_RESPONSE_FRAME;

#define EXP_SET_TEC_VOLTAGE_RESPONSE_FRAME			COMMON_FRAME
#define EXP_GET_TEC_VOLTAGE_RESPONSE_FRAME			COPC_SET_TEC_VOLTAGE_COMMAND_FRAME
#define EXP_SET_HEATER_DUTY_RESPONSE_FRAME			COMMON_FRAME
#define EXP_GET_HEATER_DUTY_RESPONSE_FRAME			COPC_SET_HEATER_DUTY_COMMAND_FRAME
#define EXP_TEMP_CTRL_RESPONSE_FRAME				COMMON_FRAME
#define EXP_TEMP_AUTO_CTRL_RESPONSE_FRAME			COMMON_FRAME
#define EXP_SET_IR_DUTY_RESPONSE_FRAME				COMMON_FRAME
#define EXP_GET_IR_DUTY_RESPONSE_FRAME				COPC_SET_IR_DUTY_COMMAND_FRAME

//typedef struct _EXP_GET_ACCEL_GYRO_RESPONSE_FRAME_
//{
//	uint8_t		Cmd;
//	uint8_t		accel_x_High;
//	uint8_t		accel_x_Low;
//	uint8_t		accel_y_High;
//	uint8_t		accel_y_Low;
//	uint8_t		accel_z_High;
//	uint8_t		accel_z_Low;
//	uint8_t		gyro_x_High;
//	uint8_t		gyro_x_Low;
//	uint8_t		gyro_y_High;
//	uint8_t		gyro_y_Low;
//	uint8_t		gyro_z_High;
//	uint8_t		gyro_z_Low;
//} EXP_GET_ACCEL_GYRO_RESPONSE_FRAME;

//typedef struct _EXP_GET_PRESS_RESPONSE_FRAME_
//{
//	uint8_t		Cmd;
//	uint8_t		pressure_High;
//	uint8_t		pressure_Low;
//} EXP_GET_PRESS_RESPONSE_FRAME;

typedef struct _EXP_GET_ALL_RESPONSE_FRAME_
{
	uint8_t		Cmd;
	uint8_t		Temp_NTC_channel_0_High;
	uint8_t		Temp_NTC_channel_0_Low;
	uint8_t		Temp_NTC_channel_1_High;
	uint8_t		Temp_NTC_channel_1_Low;
	uint8_t		Temp_NTC_channel_2_High;
	uint8_t		Temp_NTC_channel_2_Low;
	uint8_t		Temp_NTC_channel_3_High;
	uint8_t		Temp_NTC_channel_3_Low;
	uint8_t		Temp_onewire_channel_0_High;
	uint8_t		Temp_onewire_channel_0_Low;
	uint8_t		Temp_onewire_channel_1_High;
	uint8_t		Temp_onewire_channel_1_Low;
	uint8_t		Temp_i2c_sensor_High;
	uint8_t		Temp_i2c_sensor_Low;
	uint8_t		Temp_setpoint_channel_0_High;
	uint8_t		Temp_setpoint_channel_0_Low;
	uint8_t		Temp_setpoint_channel_1_High;
	uint8_t		Temp_setpoint_channel_1_Low;
	uint8_t		Temp_setpoint_channel_2_High;
	uint8_t		Temp_setpoint_channel_2_Low;
	uint8_t		Temp_setpoint_channel_3_High;
	uint8_t		Temp_setpoint_channel_3_Low;
	uint8_t		Voltage_out_tec_channel_0_High;
	uint8_t		Voltage_out_tec_channel_0_Low;
	uint8_t		Voltage_out_tec_channel_1_High;
	uint8_t		Voltage_out_tec_channel_1_Low;
	uint8_t		Voltage_out_tec_channel_2_High;
	uint8_t		Voltage_out_tec_channel_2_Low;
	uint8_t		Voltage_out_tec_channel_3_High;
	uint8_t		Voltage_out_tec_channel_3_Low;
	uint8_t		Neo_led_R;
	uint8_t		Neo_led_G;
	uint8_t		Neo_led_B;
	uint8_t		Neo_led_W;
	uint8_t		IRled_duty;
	uint16_t	accel_x;
	uint16_t	accel_y;
	uint16_t	accel_z;
	uint16_t	gyro_x;
	uint16_t	gyro_y;
	uint16_t	gyro_z;
	uint16_t	press;
} EXP_GET_ALL_RESPONSE_FRAME;

typedef struct _EXP_GET_TEC_STATUS_RESPONSE_FRAME_
{
	uint8_t			Cmd;

	uint16_t		Temp_NTC_channel_0;
	uint16_t		Temp_NTC_channel_1;
	uint16_t		Temp_NTC_channel_2;
	uint16_t		Temp_NTC_channel_3;

	uint16_t		Temp_i2c_sensor;

	uint16_t		Temp_setpoint_channel_0;
	uint16_t		Temp_setpoint_channel_1;
	uint16_t		Temp_setpoint_channel_2;
	uint16_t		Temp_setpoint_channel_3;

	uint16_t		Voltage_out_now_tec_channel_0;
	uint16_t		Voltage_out_now_tec_channel_1;
	uint16_t		Voltage_out_now_tec_channel_2;
	uint16_t		Voltage_out_now_tec_channel_3;

	uint8_t			TEC_auto_channel_0;
	uint8_t			TEC_auto_channel_1;
	uint8_t			TEC_auto_channel_2;
	uint8_t			TEC_auto_channel_3;

	uint8_t			TEC_ena_channel_0;
	uint8_t			TEC_ena_channel_1;
	uint8_t			TEC_ena_channel_2;
	uint8_t			TEC_ena_channel_3;

	uint8_t			TEC_mode_channel_0;
	uint8_t			TEC_mode_channel_1;
	uint8_t			TEC_mode_channel_2;
	uint8_t			TEC_mode_channel_3;
} EXP_GET_TEC_STATUS_RESPONSE_FRAME;

typedef union _EXP_Sfp_Payload_{
	COMMON_FRAME							commonFrame;
	EXP_SET_PW_RESPONSE_FRAME				setPowerResponseFrame;
	EXP_SET_TEMP_RESPONSE_FRAME				setTempResponseFrame;
	EXP_GET_TEMP_SETPOINT_RESPONSE_FRAME	getTempSetpointResponseFrame;
	EXP_GET_TEMP_RESPONSE_FRAME				getTempResponseFrame;
	EXP_SET_TEC_VOLTAGE_RESPONSE_FRAME		setTecVoltageResponeFrame;
	EXP_GET_TEC_VOLTAGE_RESPONSE_FRAME		getTecVoltageResponeFrame;
	EXP_SET_HEATER_DUTY_RESPONSE_FRAME		setHeaterDutyResponeFrame;
	EXP_GET_HEATER_DUTY_RESPONSE_FRAME		getHeaterDutyResponeFrame;
	EXP_TEMP_CTRL_RESPONSE_FRAME			tempCtrlResponeFrame;
	EXP_TEMP_AUTO_CTRL_RESPONSE_FRAME		tempAutoCtrlResponeFrame;
	EXP_SET_IR_DUTY_RESPONSE_FRAME			setIrDutyResponeFrame;
	EXP_GET_IR_DUTY_RESPONSE_FRAME			getIrDutyResponeFrame;

//	EXP_GET_ACCEL_GYRO_RESPONSE_FRAME		getAccelGyroResponseFrame;
//	EXP_GET_PRESS_RESPONSE_FRAME			getPressResponseFrame;
	EXP_GET_ALL_RESPONSE_FRAME				getAllResponseFrame;
	EXP_GET_TEC_STATUS_RESPONSE_FRAME		getTECResponseFrame;
} EXP_Sfp_Payload_t;

void copc_init(void);
void copc_create_task(void);


void COPC_UART_IRQHandler(void);
#endif /* COPC_COPC_H_ */
