///*
// * copc.c
// *
// *  Created on: Dec 18, 2024
// *      Author: SANG HUYNH
// */
//
//#include "copc.h"
//#include "scheduler.h"
//#include "main.h"
//#include "uart.h"
//#include "board.h"
//#include "fsp.h"
//#include "command.h"
//#include "temperature.h"
//
///* Private define ----------------------------------------------------------*/
//#define COPC_UART_HANDLE			EXP_RS485_HANDLE
//#define COPC_UART_IRQ				EXP_RS485_IRQ
//
///* Private function ----------------------------------------------------------*/
//static void copc_task_update(void);
//void copc_process_command(fsp_packet_t	*s_COPC_FspPacket);
//
//void OK_Send(void);
//void copc_exp_tec_pw_on(void);
//void copc_exp_led_pw_on(void);
//void copc_exp_heater_pw_on(void);
//void copc_exp_set_temp(void);
//void copc_exp_get_temp_setpoint(void);
//void copc_exp_get_temp(void);
//void copc_exp_set_tec_vol(void);
//void copc_exp_get_tec_vol(void);
//void copc_exp_set_heater_duty(void);
//void copc_exp_get_heater_duty(void);
//void copc_exp_temp_ctrl(void);
//void copc_exp_temp_auto_ctrl(void);
//void copc_exp_set_ir_duty(void);
//void copc_exp_get_ir_duty(void);
//
//void copc_exp_get_all(void);
//
///* Private variables ---------------------------------------------------------*/
//uart_stdio_typedef  COPC_UART;
//char                g_COPC_UART_TX_buffer[64];
//char                g_COPC_UART_RX_buffer[64];
//
////volatile static	ringbuffer_t *p_COPCBuffer;
//static	fsp_packet_t	s_COPC_FspPacket;
//static	fsp_packet_t	s_EXP_FspPacket;
//static	COPC_Sfp_Payload_t	*s_pCOPC_Sfp_Payload;
//static	EXP_Sfp_Payload_t	*s_pEXP_Sfp_Payload;
//volatile uint8_t swap_byte = 0;
//
//volatile uint8_t COPC_RX_Buff[COPC_CMD_MAX_LEN];
//volatile uint8_t receiving = 0;
//volatile uint8_t COPC_RX_Index = 0;
//
///* Private typedef -----------------------------------------------------------*/
//typedef struct COPC_TaskContextTypedef
//{
//	SCH_TASK_HANDLE               	taskHandle;
//	SCH_TaskPropertyTypedef       	taskProperty;
//	uint32_t                      	taskTick;
//} COPC_TaskContextTypedef;
//
//static COPC_TaskContextTypedef           s_task_context =
//{
//	SCH_INVALID_TASK_HANDLE,                // Will be updated by Scheduler
//	{
//		SCH_TASK_SYNC,                      // taskType;
//		SCH_TASK_PRIO_0,                    // taskPriority;
//		10,                                 // taskPeriodInMS;
//		copc_task_update,                	// taskFunction;
//		9									// taskTick
//	},
//};
//
//void copc_init(void)
//{
//    UART_Init(&COPC_UART, COPC_UART_HANDLE, COPC_UART_IRQ,
//				g_COPC_UART_TX_buffer, g_COPC_UART_RX_buffer,
//				sizeof(g_COPC_UART_TX_buffer), sizeof(g_COPC_UART_RX_buffer));
//	fsp_init(FSP_ADR_EXP);
//}
//
//void copc_create_task(void)
//{
//	SCH_TASK_CreateTask(&s_task_context.taskHandle, &s_task_context.taskProperty);
//	return;
//}
//
//static void copc_task_update(void)
//{
//	char rxData;
//	while (!RX_BUFFER_EMPTY(&COPC_UART))
//	{
//		rxData = UART_Get_Char(&COPC_UART);
//
////		rxData = rbuffer_remove(p_COPCBuffer);
//
//		if (!receiving)
//		{
//			if(rxData  == (char)(FSP_PKT_SOD))
//			{
//				receiving  =  1;
//				COPC_RX_Index =  0;
//			}
//		}
//		else
//		{
//			if(rxData  == (char)(FSP_PKT_EOF))
//			{
//				receiving  =  0;
//				switch (frame_decode((uint8_t  *)COPC_RX_Buff, COPC_RX_Index,  &s_COPC_FspPacket))
//				{
//					//process command
//					case FSP_PKT_NOT_READY:
//						UART_Send_String(&CONSOLE_UART, "Packet not ready...\r\n");
//						break;
//					case FSP_PKT_READY:
//						UART_Send_String(&CONSOLE_UART, "Received COPC CMD\n");
//						copc_process_command(&s_COPC_FspPacket);
//						break;
//					case FSP_PKT_INVALID:
//						UART_Send_String(&CONSOLE_UART, "Packet invalid...\n");
//						break;
//					case FSP_PKT_WRONG_ADR:
//						UART_Send_String(&CONSOLE_UART, "Wrong module adr...\n");
//						UART_Write(&CONSOLE_UART, (const char *)COPC_RX_Buff, COPC_RX_Index);
//						break;
//					case FSP_PKT_ERROR:
//						UART_Send_String(&CONSOLE_UART, "Packet error...\n");
//						UART_Write(&CONSOLE_UART, (const char *)COPC_RX_Buff, COPC_RX_Index);
//						break;
//					default:
//						break;
//				}
//			}
//			else
//			{
//				if(rxData == (char)(FSP_PKT_ESC)){
//					swap_byte = 1;
//					break;
//				}
//				if(swap_byte) {
//					swap_byte = 0;
//					if(rxData == (char)(FSP_PKT_TSOD))	rxData = FSP_PKT_SOD;
//					if(rxData == (char)(FSP_PKT_TESC))	rxData = FSP_PKT_ESC;
//					if(rxData == (char)(FSP_PKT_TEOF))	rxData = FSP_PKT_EOF;
//				}
//				COPC_RX_Buff[COPC_RX_Index++]  =  rxData;
//			}
//			if  (COPC_RX_Index >= FSP_PKT_MAX_LENGTH)
//			{
//				receiving  =  0;
//			}
//		}
//	}
//}
//
//void copc_process_command(fsp_packet_t	*s_COPC_FspPacket)
//{
//	s_pEXP_Sfp_Payload->commonFrame.Cmd = s_pCOPC_Sfp_Payload->commonFrame.Cmd;
//	switch (s_pCOPC_Sfp_Payload->commonFrame.Cmd)
//	{
//		case FSP_CMD_CODE_EXP_TEC_PW_ON:
//			copc_exp_tec_pw_on();
//			break;
//		case FSP_CMD_CODE_EXP_LED_PW_ON:
//			copc_exp_led_pw_on();
//			break;
//		case FSP_CMD_CODE_EXP_HEATER_PW_ON:
//			copc_exp_heater_pw_on();
//			break;
//		case FSP_CMD_CODE_EXP_SET_TEMP:
//			copc_exp_set_temp();
//			break;
//		case FSP_CMD_CODE_EXP_GET_TEMP_SETPOINT:
//			copc_exp_get_temp_setpoint();
//			break;
//		case FSP_CMD_CODE_EXP_GET_TEMP:
//			copc_exp_get_temp();
//			break;
//		case FSP_CMD_CODE_EXP_SET_TEC_VOL:
//			copc_exp_set_tec_vol();
//			break;
//		case FSP_CMD_CODE_EXP_GET_TEC_VOL:
//			copc_exp_get_tec_vol();
//			break;
//		case FSP_CMD_CODE_EXP_SET_HEATER_DUTY:
//			copc_exp_set_heater_duty();
//			break;
//		case FSP_CMD_CODE_EXP_GET_HEATER_DUTY:
//			copc_exp_get_heater_duty();
//			break;
//		case FSP_CMD_CODE_EXP_TEMP_CTRL:
//			copc_exp_temp_ctrl();
//			break;
//		case FSP_CMD_CODE_EXP_TEMP_AUTO_CTRL:
//			copc_exp_temp_auto_ctrl();
//			break;
//		case FSP_CMD_CODE_EXP_SET_IR_DUTY:
//			copc_exp_set_ir_duty();
//			break;
//		case FSP_CMD_CODE_EXP_GET_IR_DUTY:
//			copc_exp_get_ir_duty();
//			break;
///*---------------------------------------------------------------------------*/
//		case FSP_CMD_CODE_EXP_GET_ALL:
//			copc_exp_get_all();
//			break;
//		default:
//			break;
//	}
//}
//
//void OK_Send()
//{
//	uint8_t frame_len;
//	uint8_t payload[15];
//	memset((void*)payload, 0, sizeof(payload));
//	payload[0] = s_pCOPC_Sfp_Payload->commonFrame.Cmd;
//	fsp_gen_cmd_w_data_pkt(FSP_CMD_RESPONSE_DONE, payload, 1, FSP_ADR_COPC, FSP_PKT_WITHOUT_ACK,  &s_EXP_FspPacket);
//	fsp_encode(&s_EXP_FspPacket,  payload,  &frame_len);
//	UART_Write(&COPC_UART, (char*)payload, frame_len);
//	return;
//}
//void copc_exp_tec_pw_on(void)
//{
//	uint8_t Status_PW = s_pCOPC_Sfp_Payload->setPowerCommandFrame.Status_PW ? 1 : 0;
//	if (Status_PW)
//		LL_GPIO_SetOutputPin(EF_TEC_EN_GPIO_Port, EF_TEC_EN_Pin);
//	else
//		LL_GPIO_ResetOutputPin(EF_TEC_EN_GPIO_Port, EF_TEC_EN_Pin);
//	OK_Send();
//	return;
//}
//void copc_exp_led_pw_on(void)
//{
//	uint8_t Status_PW = s_pCOPC_Sfp_Payload->setPowerCommandFrame.Status_PW  ? 1 : 0;
//	if (Status_PW)
//		LL_GPIO_SetOutputPin(EF_LED_EN_GPIO_Port, EF_LED_EN_Pin);
//	else
//		LL_GPIO_ResetOutputPin(EF_LED_EN_GPIO_Port, EF_LED_EN_Pin);
//	OK_Send();
//	return;
//}
//void copc_exp_heater_pw_on(void)
//{
//	uint8_t Status_PW = s_pCOPC_Sfp_Payload->setPowerCommandFrame.Status_PW  ? 1 : 0;
//	if (Status_PW)
//		LL_GPIO_SetOutputPin(EF_HEATER_EN_GPIO_Port, EF_HEATER_EN_Pin);
//	else
//		LL_GPIO_ResetOutputPin(EF_HEATER_EN_GPIO_Port, EF_HEATER_EN_Pin);
//	OK_Send();
//	return;
//}
//void copc_exp_set_temp(void)
//{
//	int16_t setpoint_0 = ((s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_0_High) << 8) | (s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_0_Low);
//	int16_t setpoint_1 = ((s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_1_High) << 8) | (s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_1_Low);
//	int16_t setpoint_2 = ((s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_2_High) << 8) | (s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_2_Low);
//	int16_t setpoint_3 = ((s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_3_High) << 8) | (s_pCOPC_Sfp_Payload->setTempCommandFrame.Setpoint_3_Low);
//	temperature_set_setpoint(setpoint_0, 0);
//	temperature_set_setpoint(setpoint_1, 1);
//	temperature_set_setpoint(setpoint_2, 2);
//	temperature_set_setpoint(setpoint_3, 3);
//	OK_Send();
//	return;
//}
//void copc_exp_get_temp_setpoint(void)
//{
//	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
//	uint8_t frame_len;
//	int16_t setpoint_0 = (int16_t)temperature_get_setpoint(0);
//	int16_t setpoint_1 = (int16_t)temperature_get_setpoint(1);
//	int16_t setpoint_2 = (int16_t)temperature_get_setpoint(2);
//	int16_t setpoint_3 = (int16_t)temperature_get_setpoint(3);
//
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Cmd = s_pCOPC_Sfp_Payload->getTempSetpointCommandFrame.Cmd;
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_0_High = (uint8_t)(setpoint_0 >> 8);
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_0_Low = (uint8_t)setpoint_0;
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_1_High = (uint8_t)(setpoint_1 >> 8);
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_1_Low = (uint8_t)setpoint_1;
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_2_High = (uint8_t)(setpoint_2 >> 8);
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_2_Low = (uint8_t)setpoint_2;
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_3_High = (uint8_t)(setpoint_3 >> 8);
//	s_pEXP_Sfp_Payload->getTempSetpointResponseFrame.Setpoint_3_Low = (uint8_t)setpoint_3;
//
//	fsp_gen_pkt((void*)0, (uint8_t *)&s_pEXP_Sfp_Payload->getTempSetpointResponseFrame, 9, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_EXP_FspPacket);
//	fsp_encode(&s_EXP_FspPacket, encode_frame, &frame_len);
//	UART_Write(&COPC_UART, (const char *)&encode_frame, frame_len);
//	return;
//}
//void copc_exp_get_temp(void)
//{
//	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
//	uint8_t frame_len;
//	int16_t temp_NTC_0 = (int16_t)temperature_get_temp_NTC(0);
//	int16_t temp_NTC_1 = (int16_t)temperature_get_temp_NTC(1);
//	int16_t temp_NTC_2 = (int16_t)temperature_get_temp_NTC(2);
//	int16_t temp_NTC_3 = (int16_t)temperature_get_temp_NTC(3);
//
//	s_pEXP_Sfp_Payload->getTempResponseFrame.Cmd = s_pCOPC_Sfp_Payload->getTempCommandFrame.Cmd;
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_0_High = (uint8_t)(temp_NTC_0 >> 8);
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_0_Low = (uint8_t)(temp_NTC_0);
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_1_High = (uint8_t)(temp_NTC_1 >> 8);
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_1_Low = (uint8_t)(temp_NTC_1);
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_2_High = (uint8_t)(temp_NTC_2 >> 8);
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_2_Low = (uint8_t)(temp_NTC_2);
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_3_High = (uint8_t)(temp_NTC_3 >> 8);
//	s_pEXP_Sfp_Payload->getTempResponseFrame.NTC_3_Low = (uint8_t)(temp_NTC_3);
//
//	fsp_gen_pkt((void*)0, (uint8_t *)&s_pEXP_Sfp_Payload->getTempResponseFrame, 9, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_EXP_FspPacket);
//	fsp_encode(&s_EXP_FspPacket, encode_frame, &frame_len);
//	UART_Write(&COPC_UART, (const char *)&encode_frame, frame_len);
//	return;
//}
//void copc_exp_set_tec_vol(void)
//{
//	uint16_t vol_0 = ((s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_0_High) << 8) | (s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_0_Low);
//	uint16_t vol_1 = ((s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_1_High) << 8) | (s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_1_Low);
//	uint16_t vol_2 = ((s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_2_High) << 8) | (s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_2_Low);
//	uint16_t vol_3 = ((s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_3_High) << 8) | (s_pCOPC_Sfp_Payload->setTecVoltageCommandFrame.Voltage_3_Low);
//	temperature_set_tec_vol(0, vol_0);
//	temperature_set_tec_vol(1, vol_1);
//	temperature_set_tec_vol(2, vol_2);
//	temperature_set_tec_vol(3, vol_3);
//	OK_Send();
//	return;
//}
//void copc_exp_get_tec_vol(void)
//{
//	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
//	uint8_t frame_len;
//	uint16_t vol_0 = temperature_get_tec_vol(0);
//	uint16_t vol_1 = temperature_get_tec_vol(1);
//	uint16_t vol_2 = temperature_get_tec_vol(2);
//	uint16_t vol_3 = temperature_get_tec_vol(3);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Cmd = s_pCOPC_Sfp_Payload->getTecVoltageCommandFrame.Cmd;
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_0_High = (uint8_t)(vol_0 >> 8);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_0_Low = (uint8_t)(vol_0);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_1_High = (uint8_t)(vol_1 >> 8);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_1_Low = (uint8_t)(vol_1);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_2_High = (uint8_t)(vol_2 >> 8);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_2_Low = (uint8_t)(vol_2);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_3_High = (uint8_t)(vol_3 >> 8);
//	s_pEXP_Sfp_Payload->getTecVoltageResponeFrame.Voltage_3_Low = (uint8_t)(vol_3);
//	fsp_gen_pkt((void*)0, (uint8_t *)&s_pEXP_Sfp_Payload->getTecVoltageResponeFrame, 9, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_EXP_FspPacket);
//	fsp_encode(&s_EXP_FspPacket, encode_frame, &frame_len);
//	UART_Write(&COPC_UART, (const char *)&encode_frame, frame_len);
//	return;
//}
//void copc_exp_set_heater_duty(void)
//{
//	uint8_t duty_0 = s_pCOPC_Sfp_Payload->setHeaterDutyCommandFrame.Duty_0;
//	if (duty_0 > 100) duty_0 = 100;
//	uint8_t duty_1 = s_pCOPC_Sfp_Payload->setHeaterDutyCommandFrame.Duty_1;
//	if (duty_1 > 100) duty_1 = 100;
//	uint8_t duty_2 = s_pCOPC_Sfp_Payload->setHeaterDutyCommandFrame.Duty_2;
//	if (duty_2 > 100) duty_2 = 100;
//	uint8_t duty_3 = s_pCOPC_Sfp_Payload->setHeaterDutyCommandFrame.Duty_3;
//	if (duty_3 > 100) duty_3 = 100;
//	temperature_set_heater_duty(0, duty_0);
//	temperature_set_heater_duty(1, duty_1);
//	temperature_set_heater_duty(2, duty_2);
//	temperature_set_heater_duty(3, duty_3);
//	OK_Send();
//	return;
//}
//void copc_exp_get_heater_duty(void)
//{
//	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
//	uint8_t frame_len;
//	uint8_t duty_0 = temperature_get_heater_duty(0);
//	uint8_t duty_1 = temperature_get_heater_duty(1);
//	uint8_t duty_2 = temperature_get_heater_duty(2);
//	uint8_t duty_3 = temperature_get_heater_duty(3);
//	s_pEXP_Sfp_Payload->getHeaterDutyResponeFrame.Cmd = s_pCOPC_Sfp_Payload->getHeaterDutyCommandFrame.Cmd;
//	s_pEXP_Sfp_Payload->getHeaterDutyResponeFrame.Duty_0 = duty_0;
//	s_pEXP_Sfp_Payload->getHeaterDutyResponeFrame.Duty_1 = duty_1;
//	s_pEXP_Sfp_Payload->getHeaterDutyResponeFrame.Duty_2 = duty_2;
//	s_pEXP_Sfp_Payload->getHeaterDutyResponeFrame.Duty_3 = duty_3;
//	fsp_gen_pkt((void*)0, (uint8_t *)&s_pEXP_Sfp_Payload->getHeaterDutyResponeFrame, 5, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_EXP_FspPacket);
//	fsp_encode(&s_EXP_FspPacket, encode_frame, &frame_len);
//	UART_Write(&COPC_UART, (const char *)&encode_frame, frame_len);
//	return;
//}
//void copc_exp_temp_ctrl(void)
//{
//	mode_ctrl_temp_t mode_0 = s_pCOPC_Sfp_Payload->tempCtrlCommandFrame.Mode_0;
//	if (mode_0 != COOL && mode_0 != HEAT) mode_0 = OFF;
//	mode_ctrl_temp_t mode_1 = s_pCOPC_Sfp_Payload->tempCtrlCommandFrame.Mode_1;
//	if (mode_1 != COOL && mode_1 != HEAT) mode_1 = OFF;
//	mode_ctrl_temp_t mode_2 = s_pCOPC_Sfp_Payload->tempCtrlCommandFrame.Mode_2;
//	if (mode_2 != COOL && mode_2 != HEAT) mode_2 = OFF;
//	mode_ctrl_temp_t mode_3 = s_pCOPC_Sfp_Payload->tempCtrlCommandFrame.Mode_3;
//	if (mode_3 != COOL && mode_3 != HEAT) mode_3 = OFF;
//
//	temperature_set_ctrl(mode_0, mode_1, mode_2, mode_3);
//	OK_Send();
//	return;
//}
//void copc_exp_temp_auto_ctrl(void)
//{
//	uint8_t auto_0 = s_pCOPC_Sfp_Payload->tempAutoCtrlCommandFrame.Auto_0 ? 1 : 0;
//	uint8_t auto_1 = s_pCOPC_Sfp_Payload->tempAutoCtrlCommandFrame.Auto_1 ? 1 : 0;
//	uint8_t auto_2 = s_pCOPC_Sfp_Payload->tempAutoCtrlCommandFrame.Auto_2 ? 1 : 0;
//	uint8_t auto_3 = s_pCOPC_Sfp_Payload->tempAutoCtrlCommandFrame.Auto_3 ? 1 : 0;
//	temperature_set_auto_ctrl(auto_0, auto_1, auto_2, auto_3);
//	OK_Send();
//	return;
//}
//void copc_exp_set_ir_duty(void)
//{
////	uint8_t duty_0 = s_pCOPC_Sfp_Payload->setIrDutyCommandFrame.Duty_0;
////	if (duty_0 > 100) duty_0 = 100;
////	uint8_t duty_1 = s_pCOPC_Sfp_Payload->setIrDutyCommandFrame.Duty_1;
////	if (duty_1 > 100) duty_1 = 100;
////	uint8_t duty_2 = s_pCOPC_Sfp_Payload->setIrDutyCommandFrame.Duty_2;
////	if (duty_2 > 100) duty_2 = 100;
////	uint8_t duty_3 = s_pCOPC_Sfp_Payload->setIrDutyCommandFrame.Duty_3;
////	if (duty_3 > 100) duty_3 = 100;
////	set_ir_duty(0, duty_0);
////	set_ir_duty(1, duty_1);
////	set_ir_duty(2, duty_2);
////	set_ir_duty(3, duty_3);
////	OK_Send();
//	return;
//}
//void copc_exp_get_ir_duty(void)
//{
////	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
////	uint8_t frame_len;
////	uint8_t duty_0 = get_ir_duty(0);
////	uint8_t duty_1 = get_ir_duty(1);
////	uint8_t duty_2 = get_ir_duty(2);
////	uint8_t duty_3 = get_ir_duty(3);
////	s_pEXP_Sfp_Payload->getIrDutyResponeFrame.Cmd = s_pCOPC_Sfp_Payload->getIrDutyCommandFrame.Cmd;
////	s_pEXP_Sfp_Payload->getIrDutyResponeFrame.Duty_0 = duty_0;
////	s_pEXP_Sfp_Payload->getIrDutyResponeFrame.Duty_1 = duty_1;
////	s_pEXP_Sfp_Payload->getIrDutyResponeFrame.Duty_2 = duty_2;
////	s_pEXP_Sfp_Payload->getIrDutyResponeFrame.Duty_3 = duty_3;
////	fsp_gen_pkt((void*)0, (uint8_t *)&s_pEXP_Sfp_Payload->getIrDutyResponeFrame, 5, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_EXP_FspPacket);
////	fsp_encode(&s_EXP_FspPacket, encode_frame, &frame_len);
////	UART_Write(&COPC_UART, (const char *)&encode_frame, frame_len);
//	return;
//}
//
//
//
////void copc_exp_tec_status(void)
////{
////	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
////	uint8_t frame_len;
////	uint16_t temp;
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Cmd = s_pCOPC_Sfp_Payload->commonFrame.Cmd;
////
////	//temp
////	temp = temperature_get_NTC(0);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_NTC_channel_0 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_NTC(1);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_NTC_channel_1 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_NTC(2);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_NTC_channel_2 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_NTC(3);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_NTC_channel_3 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_bmp390();
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_i2c_sensor	= (temp << 8) | (temp >> 8);
////	//setpoint
////	temp = temperature_get_setpoint(0);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_setpoint_channel_0 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_setpoint(1);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_setpoint_channel_1 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_setpoint(2);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_setpoint_channel_2 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_setpoint(3);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Temp_setpoint_channel_3 = (temp << 8) | (temp >> 8);
////	//voltage
////	temp = temperature_get_voltage_now(0);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Voltage_out_now_tec_channel_0 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_voltage_now(1);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Voltage_out_now_tec_channel_1 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_voltage_now(2);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Voltage_out_now_tec_channel_2 = (temp << 8) | (temp >> 8);
////	temp = temperature_get_voltage_now(3);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.Voltage_out_now_tec_channel_3 = (temp << 8) | (temp >> 8);
////	// TEC status
////	temp = temperature_tec_get_status();
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_auto_channel_0 = (temp & (1 << TEC0_AUTO)) >> TEC0_AUTO;
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_auto_channel_1 = (temp & (1 << TEC1_AUTO)) >> TEC1_AUTO;
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_auto_channel_2 = (temp & (1 << TEC2_AUTO)) >> TEC2_AUTO;
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_auto_channel_3 = (temp & (1 << TEC3_AUTO)) >> TEC3_AUTO;
////
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_ena_channel_0 = (temp & (1 << TEC0_ENA)) >> TEC0_ENA;
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_ena_channel_1 = (temp & (1 << TEC1_ENA)) >> TEC1_ENA;
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_ena_channel_2 = (temp & (1 << TEC2_ENA)) >> TEC2_ENA;
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_ena_channel_3 = (temp & (1 << TEC3_ENA)) >> TEC3_ENA;
////
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_mode_channel_0 = temperature_get_mode(0);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_mode_channel_1 = temperature_get_mode(1);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_mode_channel_2 = temperature_get_mode(2);
////	s_pexp_Sfp_Payload->expGetTECResponseFrame.TEC_mode_channel_3 = temperature_get_mode(3);
////	fsp_gen_pkt((void*)0, (uint8_t *)&s_pexp_Sfp_Payload->expGetTECResponseFrame, 39, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_exp_FspPacket);
////	fsp_encode(&s_exp_FspPacket, encode_frame, &frame_len);
////	usart1_send_array((const char *)&encode_frame, frame_len);
////	return;
////}
//
////void copc_exp_get_accel_gyro(void)
////{
////	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
////	uint8_t frame_len;
////	Accel_Gyro_DataTypedef accel_data = get_acceleration();
////	Accel_Gyro_DataTypedef gyro_data = get_gyroscope();
////	s_pexp_Sfp_Payload->expIRledGetBrightResponseFrame.Cmd = s_pCOPC_Sfp_Payload->commonFrame.Cmd;
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.accel_x_high = (uint8_t)(accel_data.x >> 8);
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.accel_x_low = (uint8_t)accel_data.x;
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.accel_y_high = (uint8_t)(accel_data.y >> 8);
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.accel_y_low = (uint8_t)accel_data.y;
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.accel_z_high = (uint8_t)(accel_data.z >> 8);
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.accel_z_low = (uint8_t)accel_data.z;
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.gyro_x_high = (uint8_t)(gyro_data.x >> 8);
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.gyro_x_low = (uint8_t)gyro_data.x;
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.gyro_y_high = (uint8_t)(gyro_data.y >> 8);
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.gyro_y_low = (uint8_t)gyro_data.y;
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.gyro_z_high = (uint8_t)(gyro_data.z >> 8);
////	s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame.gyro_z_low = (uint8_t)gyro_data.z;
////	fsp_gen_pkt((void*)0, (uint8_t *)&s_pexp_Sfp_Payload->expGetAccelGyroResponseFrame, 13, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_exp_FspPacket);
////	fsp_encode(&s_exp_FspPacket, encode_frame, &frame_len);
////	usart1_send_array((const char *)&encode_frame, frame_len);
////}
//
////void copc_exp_get_press(void)
////{
////	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
////	uint8_t frame_len;
////	int16_t pressure = get_pressure();
////	s_pEXP_Sfp_Payload->expGetPressResponseFrame.Cmd = s_pCOPC_Sfp_Payload->commonFrame.Cmd;
////	s_pEXP_Sfp_Payload->expGetPressResponseFrame.pressure_high = pressure >> 8;
////	s_pEXP_Sfp_Payload->expGetPressResponseFrame.pressure_low = pressure;
////	fsp_gen_pkt((void*)0, (uint8_t *)&s_pEXP_Sfp_Payload->expGetPressResponseFrame, 3, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_exp_FspPacket);
////	fsp_encode(&s_exp_FspPacket, encode_frame, &frame_len);
////	usart1_send_array((const char *)&encode_frame, frame_len);
////}
//
//void copc_exp_get_all(void)
//{
////	uint8_t encode_frame[FSP_PKT_MAX_LENGTH];
////	uint8_t frame_len;
////	s_pEXP_Sfp_Payload->getAllResponseFrame.Cmd = s_pCOPC_Sfp_Payload->getAllCommandFrame.Cmd;
////	// GET TEMP NTC
////	uint16_t temp = temperature_get_NTC(0);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_0_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_0_low = (uint8_t)temp;
////	temp = temperature_get_NTC(1);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_1_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_1_low = (uint8_t)temp;
////	temp = temperature_get_NTC(2);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_2_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_2_low = (uint8_t)temp;
////	temp = temperature_get_NTC(3);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_3_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_NTC_channel_3_low = (uint8_t)temp;
////	// GET TEMP ONEWIRE
////	temp = temperature_get_onewire(0);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_onewire_channel_0_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_onewire_channel_0_low = (uint8_t)temp;
////	temp = temperature_get_onewire(1);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_onewire_channel_1_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_onewire_channel_1_low = (uint8_t)temp;
////	// GET TEMP BMP390
////	temp = temperature_get_bmp390();
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_i2c_sensor_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_i2c_sensor_low = (uint8_t)temp;
////	// GET TEMP SETPOINT
////	temp = temperature_get_setpoint(0);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_0_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_0_low = (uint8_t)temp;
////	temp = temperature_get_setpoint(1);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_1_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_1_low = (uint8_t)temp;
////	temp = temperature_get_setpoint(2);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_2_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_2_low = (uint8_t)temp;
////	temp = temperature_get_setpoint(3);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_3_high = (uint8_t)(temp >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Temp_setpoint_channel_3_low = (uint8_t)temp;
////	// GET VOLTAGE OUTPUT TEC
////	uint16_t voltage = temperature_get_voltage(0);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_0_high = (uint8_t)(voltage >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_0_low = (uint8_t)voltage;
////	voltage = temperature_get_voltage(1);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_1_high = (uint8_t)(voltage >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_1_low = (uint8_t)voltage;
////	voltage = temperature_get_voltage(2);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_2_high = (uint8_t)(voltage >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_2_low = (uint8_t)voltage;
////	voltage = temperature_get_voltage(3);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_3_high = (uint8_t)(voltage >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Voltage_out_tec_channel_3_low = (uint8_t)voltage;
////	// GET NEO LED DATA
////	rgbw_color RGBW = ringled_get_RGBW();
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Neo_led_R = RGBW.red;
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Neo_led_G = RGBW.green;
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Neo_led_B = RGBW.blue;
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.Neo_led_W = RGBW.white;
////	// GET IR LED BRIGHT
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.IRled_duty = (uint8_t)IR_led_get_Current_DutyCyclesPercent();
////	// GET ACCEL
////	Accel_Gyro_DataTypedef a_g_data = get_acceleration();
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.accel_x = (a_g_data.x << 8) | (a_g_data.x >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.accel_y = (a_g_data.y << 8) | (a_g_data.y >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.accel_z = (a_g_data.z << 8) | (a_g_data.z >> 8);
////	// GET GYRO
////	a_g_data = get_gyroscope();
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.gyro_x = (a_g_data.x << 8) | (a_g_data.x >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.gyro_y = (a_g_data.y << 8) | (a_g_data.y >> 8);
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.gyro_z = (a_g_data.z << 8) | (a_g_data.z >> 8);
////	// GET PRESS
////	temp = get_pressure();
////	s_pexp_Sfp_Payload->expGetParamResponseFrame.press = (temp << 8) | (temp >> 8);
////	// DECODE FRAME
////	fsp_gen_pkt((void*)0, (uint8_t *)&s_pexp_Sfp_Payload->expGetParamResponseFrame, 50, FSP_ADR_COPC, FSP_PKT_TYPE_CMD_W_DATA, &s_exp_FspPacket);
////	fsp_encode(&s_exp_FspPacket, encode_frame, &frame_len);
////	usart1_send_array((const char *)&encode_frame, frame_len);
//	return;
//}
//
///* :::::::::: COPC_UART_IRQHandler ::::::::::::: */
//void COPC_UART_IRQHandler(void)
//{
//	// Transmit
//    if(LL_USART_IsActiveFlag_TXE(COPC_UART.handle) == true)
//    {
//        if(TX_BUFFER_EMPTY(&COPC_UART))
//        {
//            // Buffer empty, so disable interrupts
//            LL_USART_DisableIT_TXE(CONSOLE_UART.handle);
//        }
//        else
//        {
//            // There is more data in the output buffer. Send the next byte
//            UART_Prime_Transmit(&COPC_UART);
//        }
//    }
//    // Receive
//    if(LL_USART_IsActiveFlag_RXNE(COPC_UART.handle) == true)
//    {
//    	COPC_UART.RX_irq_char = LL_USART_ReceiveData8(COPC_UART.handle);
//    	if (!RX_BUFFER_FULL(&COPC_UART))
//    	{
//    		COPC_UART.p_RX_buffer[COPC_UART.RX_write_index] = COPC_UART.RX_irq_char;
//    	}
//    }
//    return;
//}
