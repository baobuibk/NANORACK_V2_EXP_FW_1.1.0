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
/* Command for power supply */
int Cmd_temp_pw_on(int argc, char *argv[]);
/* Command for temperature */
int Cmd_set_temp(int argc, char *argv[]);
int Cmd_get_temp(int argc, char *argv[]);
int Cmd_get_temp_setpoint(int argc, char *argv[]);

int Cmd_tec_init(int argc, char *argv[]);
int Cmd_set_tec_vol(int argc, char *argv[]);
int Cmd_get_tec_vol(int argc, char *argv[]);
int Cmd_tec_dir (int argc, char *argv[]);
int Cmd_tec_ctrl (int argc, char *argv[]);

int Cmd_set_heater_duty(int argc, char *argv[]);
int Cmd_get_heater_duty(int argc, char *argv[]);

int Cmd_temp_ctrl(int argc, char *argv[]);
int Cmd_temp_auto_ctrl(int argc, char *argv[]);
/* Command for ir led */
int Cmd_set_ir_duty(int argc, char *argv[]);
int Cmd_get_ir_duty(int argc, char *argv[]);
/* Command for i2c sensor */
int Cmd_get_acceleration_gyroscope(int argc, char *argv[]);
int Cmd_get_pressure(int argc, char *argv[]);
/* Command for system */
int Cmd_get_all(int argc, char *argv[]);

int Cmd_test(int argc, char *argv[]);

int Cmd_reset(int argc, char *argv[]);
int Cmd_set_en_req(int argc, char *argv[]);
int Cmd_reset_en_req(int argc, char *argv[]);
int Cmd_clear_status_reg(int argc, char *argv[]);
int Cmd_read(int argc, char *argv[]);
int Cmd_on_tec(int argc, char *argv[]);
int Cmd_tec_set_vol(int argc, char *argv[]);
int Cmd_get_status(int argc, char *argv[]);
int Cmd_set_ov_clamp(int argc, char *argv[]);
int Cmd_set_uv_clamp(int argc, char *argv[]);

#endif /* CMDLINE_COMMAND_H_ */
