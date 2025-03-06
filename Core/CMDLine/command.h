/*
 * command.h
 *
 *  Created on: Nov 21, 2024
 *      Author: SANG HUYNH
 */

#ifndef CMDLINE_COMMAND_H_
#define CMDLINE_COMMAND_H_

#include "cmdline.h"
#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define	COMMAND_MAX_LENGTH	64
#define MAX_HISTORY 8
#define MAX_CMD_LENGTH COMMAND_MAX_LENGTH

extern USART_TypeDef *UART_CMDLINE;

void CommandLine_Init(USART_TypeDef *handle_uart);
void CommandLine_CreateTask(void);
void Command_SendSplash(void);

/* Command support */
int Cmd_help(int argc, char *argv[]);
int Cmd_system_reset(int argc, char *argv[]);
int Cmd_alive_check(int argc, char *argv[]);
int Cmd_ota_boot(int argc, char *argv[]);

/* Command for power supply */
int Cmd_temp_pw(int argc, char *argv[]);

/* Command for temperature */
int Cmd_set_temp(int argc, char *argv[]);
int Cmd_get_temp(int argc, char *argv[]);
int Cmd_get_temp_setpoint(int argc, char *argv[]);

/* Command for TEC */
int Cmd_tec_init(int argc, char *argv[]);
int Cmd_tec_set_vol(int argc, char *argv[]);
int Cmd_tec_get_vol(int argc, char *argv[]);
int Cmd_tec_dir (int argc, char *argv[]);
int Cmd_tec_ctrl (int argc, char *argv[]);
int Cmd_tec_read(int argc, char *argv[]);

/* Command for heater */
int Cmd_heater_set_duty(int argc, char *argv[]);
int Cmd_heater_get_duty(int argc, char *argv[]);

/* Command auto temperature */
int Cmd_temp_set_auto(int argc, char *argv[]);
int Cmd_temp_auto_0(int argc, char *argv[]);
int Cmd_temp_auto_1(int argc, char *argv[]);
int Cmd_temp_auto_2(int argc, char *argv[]);
int Cmd_temp_auto_3(int argc, char *argv[]);

/* Command for ir led */
int Cmd_ir_set_duty(int argc, char *argv[]);
int Cmd_ir_get_duty(int argc, char *argv[]);

/* Command for i2c sensor */
int Cmd_acceleration_gyroscope_get(int argc, char *argv[]);
int Cmd_pressure_get(int argc, char *argv[]);

/* Internal laser board commands */
int Cmd_int_ls_dac(int argc, char *argv[]);
int Cmd_int_ls_set(int argc, char *argv[]);
int Cmd_int_ls_auto(int argc, char *argv[]);

/* External laser board commands */
int Cmd_ext_ls_dac(int argc, char *argv[]);
int Cmd_ext_ls_set(int argc, char *argv[]);
int Cmd_ext_ls_auto(int argc, char *argv[]);

/* Photo board commands */
int Cmd_pd_set(int argc, char *argv[]);
int Cmd_pd_get_adc(int argc, char *argv[]);
int Cmd_pd_auto(int argc, char *argv[]);

#endif /* CMDLINE_COMMAND_H_ */
