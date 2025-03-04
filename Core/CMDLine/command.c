/*
 * command.c
 *
 *  Created on: Nov 21, 2024
 *      Author: SANG HUYNH
 */

#include "command.h"
#include "cmdline.h"
#include "scheduler.h"
#include "uart.h"
#include "main.h"
#include "board.h"
#include "lt8722.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"
#include <stdlib.h>
#include <stdio.h>
#include "temperature.h"
#include "bmp390.h"
#include "ntc.h"
#include "ir_led.h"
#include "mcp4902.h"
#include "auto_run.h"
#include "date_time.h"


//@CaoHieu: Change CommandLine to use Simple UART RingBuffer
// Add history
// Add some utils to CommandLine
#define NAME_SHELL "EXP_110"
#define KEY_ENTER '\r'       /* [enter] key */
#define KEY_BACKSPACE '\x7f' /* [backspace] key */

typedef struct
{
    char commandBuffer[COMMAND_MAX_LENGTH];
    uint16_t commandBufferIndex;
    char commandHistory[MAX_HISTORY][COMMAND_MAX_LENGTH];
    uint16_t historyCount;
    uint16_t historyIndex;
} CMDLine_Context;

CMDLine_Context pContext = {0};
USART_TypeDef *UART_CMDLINE;

/* Private typedef -----------------------------------------------------------*/
typedef struct _Command_TaskContextTypedef_
{
    SCH_TASK_HANDLE taskHandle;
    SCH_TaskPropertyTypedef taskProperty;
} Command_TaskContextTypedef;

/* Private function ----------------------------------------------------------*/
static void CommandLine_Task_Update(void);
void process_command(char rxData, CMDLine_Context *context);
static void ResetConfig(void);

/* Private variable -----------------------------------------------------------*/

const char *ErrorCode[6] = {"OK\r\n", "CMDLINE_BAD_CMD\r\n", "CMDLINE_TOO_MANY_ARGS\r\n",
                            "CMDLINE_TOO_FEW_ARGS\r\n", "CMDLINE_INVALID_ARG\r\n", "CMD_OK_BUT_PENDING...\r\n"};

extern SPI_HandleTypeDef hspi2;

int Cmd_dac_set(int argc, char *argv[]);
int Cmd_get_adc(int argc, char *argv[]);
int Cmd_pd_set(int argc, char *argv[]);
int Cmd_ls_set(int argc, char *argv[]);
int Cmd_auto_laser(int argc, char *argv[]);
int Cmd_auto_adc(int argc, char *argv[]);

static char s_commandBuffer[COMMAND_MAX_LENGTH];
static uint8_t s_commandBufferIndex = 0;


tCmdLineEntry g_psCmdTable[] = {
    /* Command support */
    {"help", Cmd_help, ": Display list of available commands | format: help"},

    /* Command for power supply */
    {"temp_pw", Cmd_temp_pw, ": Control power for TEC and Heater (1=ON, 0=OFF) | format: temp_pw 1/0"},

    /* Command for temperature */
    {"set_temp", Cmd_set_temp, ": Set desired temperature for multiple zones | format: set_temp <T1> <T2> <T3> <T4>"},
    {"get_temp_setpoint", Cmd_get_temp_setpoint, ": Retrieve the previously set temperature | format: get_temp_setpoint"},
    {"get_temp", Cmd_get_temp, ": Get the current temperature of all zones | format: get_temp"},

    /* Command for TEC (Thermo Electric Cooler) */
    {"tec_init", Cmd_tec_init, ": Initialize TEC module | format: tec_init"},
    {"tec_vol", Cmd_tec_set_vol, ": Set TEC voltage for multiple channels | format: tec_vol <V1> <V2> <V3> <V4>"},
    {"tec_get_vol", Cmd_tec_get_vol, ": Get the current TEC voltage | format: tec_get_vol"},
    {"tec_dir", Cmd_tec_dir, ": Set TEC direction (1=COOL, 0=HEAT) for multiple channels | format: tec_dir <1/0> <1/0> <1/0> <1/0>"},
    {"tec_ctrl", Cmd_tec_ctrl, ": Enable or disable TEC (1=ON, 0=OFF) for multiple channels | format: tec_ctrl <1/0> <1/0> <1/0> <1/0>"},

    /* Command for Heater */
    {"heater_duty", Cmd_heater_set_duty, ": Set heater duty cycle (in percentage) | format: heater_duty <D1> <D2> <D3> <D4>"},
    {"heater_get_duty", Cmd_heater_get_duty, ": Get the current heater duty cycle | format: heater_get_duty"},

    /* Command for Temperature Control */
    {"temp_auto", Cmd_temp_auto_ctrl, ": Enable automatic temperature control | format: temp_auto <1=Enable, 0=Disable> <1/0> <1/0> <1/0>"},

    /* Command for IR LED */
    {"ir_duty", Cmd_ir_set_duty, ": Set IR LED duty cycle | format: ir_duty <duty>"},
    {"ir_get_duty", Cmd_ir_get_duty, ": Get the current IR LED duty cycle | format: ir_get_duty"},

    /* Command for I2C Sensor */
    {"accel_gyro_get", Cmd_acceleration_gyroscope_get, ": Get accelerometer and gyroscope data | format: accel_gyro_get"},
    {"press_get", Cmd_pressure_get, ": Get pressure sensor data | format: press_get"},

    /* Command for system */
    {"get_all", Cmd_get_all, ": Retrieve all available sensor and system data | format: get_all"},

    /* Miscellaneous commands */
    {"dac_set", Cmd_dac_set, ": Set DAC output (Details not provided) | format: dac_set <value>"},
    {"get_adc", Cmd_get_adc, ": Get ADC readings (Details not provided) | format: get_adc"},
    {"pd_set", Cmd_pd_set, ": Set photodiode parameters (Details not provided) | format: pd_set <value>"},
    {"ls_set", Cmd_ls_set, ": Set light sensor parameters (Details not provided) | format: ls_set <value>"},
    {"auto_adc", Cmd_auto_adc, ": Instead of using <get_adc>, auto_adc = get_adc automatically"},
    {"auto_laser", Cmd_auto_laser, ": Auto_laser [interval] [interval adc] [userdelay]"},

//	{"reset", Cmd_reset, ": :"},
//	{"set_en", Cmd_set_en_req, ": :"},
//	{"reset_en", Cmd_reset_en_req, ": :"},
//	{"set_swen", Cmd_set_swen_req, ": :"},
//	{"reset_swen", Cmd_reset_swen_req, ": :"},
	{"read", Cmd_read, ": :"},
//	{"c_0", Cmd_cool_0, ": --------- | format: ------"},
//	{"h_0", Cmd_heat_0, ": --------- | format: ------"},
//	{"c_1", Cmd_cool_1, ": --------- | format: ------"},
//	{"h_1", Cmd_heat_1, ": --------- | format: ------"},
    {0, 0, 0}};

static Command_TaskContextTypedef s_CommandTaskContext =
{
	SCH_INVALID_TASK_HANDLE, // Will be updated by Schedular
	{
		SCH_TASK_SYNC,           // taskType;
		SCH_TASK_PRIO_0,         // taskPriority;
		10,                      // taskPeriodInMS;
		CommandLine_Task_Update, // taskFunction;
		9
	}
};

void CommandLine_Init(USART_TypeDef *handle_uart)
{
    UART_CMDLINE = handle_uart;
    memset((void *)s_commandBuffer, 0, sizeof(s_commandBuffer));
    s_commandBufferIndex = 0;
    Command_SendSplash();
    UART_SendStringRing(UART_CMDLINE, "EXP FIRMWARE V1.1.0\r\n");
    UART_SendStringRing(UART_CMDLINE, "DAC Set to [100]\r\n");
    UART_Flush_RingRx(UART_CMDLINE);
    DAC_Write(0, 100);
    HAL_Delay(100);
    DAC_Write(0, 100);
}

static void CommandLine_Task_Update(void)
{
    char rxData;
    if (IsDataAvailable(UART_CMDLINE))
    {
        rxData = UART_ReadRing(UART_CMDLINE);

        if (rxData == 27)
        {
            UART_SendStringRing(UART_CMDLINE, "\033[2J");
        }
        else
        {
            UART_WriteRing(UART_CMDLINE, rxData);
        }
        process_command(rxData, &pContext);
    }
}

void process_command(char rxData, CMDLine_Context *context)
{
    if (rxData == 27)
    {
        s_DateTime rtcTime = {0};
        DateTime_GetRTC(&rtcTime);
        char x_timeBuffer[30];
        snprintf(x_timeBuffer, sizeof(x_timeBuffer),
                 "[%02u:%02u:%02u]",
                 rtcTime.hour, rtcTime.minute, rtcTime.second);
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "\r\n%s%s$ ", x_timeBuffer, NAME_SHELL);
        UART_SendStringRing(UART_CMDLINE, buffer);
        context->commandBufferIndex = 0;
        context->commandBuffer[0] = '\0';
    }

    if (rxData == 0x2D) // '-' key (history up)
    {
        // Get Software DateTime
        s_DateTime rtcTime = {0};
        DateTime_GetRTC(&rtcTime);
        char x_timeBuffer[30];
        snprintf(x_timeBuffer, sizeof(x_timeBuffer),
                 "[%02u:%02u:%02u]",
                 rtcTime.hour, rtcTime.minute, rtcTime.second);
        if (context->historyIndex > 0)
        {
            context->historyIndex--;
        }

        // Load history command
        if (context->historyIndex < context->historyCount)
        {
            strcpy(context->commandBuffer, context->commandHistory[context->historyIndex]);
            context->commandBufferIndex = strlen(context->commandBuffer);
        }
        else
        {
            context->commandBuffer[0] = '\0';
            context->commandBufferIndex = 0;
        }

        // Clear current line and display updated command
        UART_SendStringRing(UART_CMDLINE, "\033[2K"); // Clear entire line
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "\r%s%s$ ", x_timeBuffer, NAME_SHELL);
        UART_SendStringRing(UART_CMDLINE, buffer);
        UART_SendStringRing(UART_CMDLINE, context->commandBuffer); // Display updated command
        return;
    }
    else if (rxData == 0x3D) // '=' key (history down)
    {
        // Get Software DateTime
        s_DateTime rtcTime = {0};
        DateTime_GetRTC(&rtcTime);
        char x_timeBuffer[30];
        snprintf(x_timeBuffer, sizeof(x_timeBuffer),
                 "[%02u:%02u:%02u]",
                 rtcTime.hour, rtcTime.minute, rtcTime.second);
        if (context->historyIndex < context->historyCount)
        {
            context->historyIndex++;
        }

        // Load history command
        if (context->historyIndex < context->historyCount)
        {
            strcpy(context->commandBuffer, context->commandHistory[context->historyIndex]);
            context->commandBufferIndex = strlen(context->commandBuffer);
        }
        else
        {
            context->commandBuffer[0] = '\0';
            context->commandBufferIndex = 0;
        }

        // Clear current line and display updated command
        UART_SendStringRing(UART_CMDLINE, "\033[2K"); // Clear entire line
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "\r%s%s$ ", x_timeBuffer, NAME_SHELL);
        UART_SendStringRing(UART_CMDLINE, buffer);
        UART_SendStringRing(UART_CMDLINE, context->commandBuffer); // Display updated command
        return;
    }

    // Handle individual key presses
    if (((rxData >= 32 && rxData <= 126) || rxData == KEY_ENTER || rxData == KEY_BACKSPACE) && rxData != 0x2D && rxData != 0x3D && rxData != 0x5C)
    {
        // Get Software DateTime
        s_DateTime rtcTime = {0};
        DateTime_GetRTC(&rtcTime);
        char x_timeBuffer[30];
        snprintf(x_timeBuffer, sizeof(x_timeBuffer),
                 "[%02u:%02u:%02u]",
                 rtcTime.hour, rtcTime.minute, rtcTime.second);
        if (rxData == KEY_ENTER)
        {
            if (context->commandBufferIndex > 0)
            {
                context->commandBuffer[context->commandBufferIndex] = '\0';
                // Save to history
                if (context->historyCount == 0 ||
                    strcmp(context->commandHistory[context->historyCount - 1], context->commandBuffer) != 0)
                {
                    if (context->historyCount < MAX_HISTORY)
                    {
                        strcpy(context->commandHistory[context->historyCount], context->commandBuffer);
                        context->historyCount++;
                    }
                    else
                    {
                        for (int i = 0; i < MAX_HISTORY - 1; i++)
                        {
                            strcpy(context->commandHistory[i], context->commandHistory[i + 1]);
                        }
                        strcpy(context->commandHistory[MAX_HISTORY - 1], context->commandBuffer);
                    }
                }
                context->historyIndex = context->historyCount;

                // Process command
                int8_t ret_val = CmdLineProcess(context->commandBuffer);
                if (ret_val == CMDLINE_NONE_RETURN)
                {
                }
                else
                {
                    char buffer[60];
                    snprintf(buffer, sizeof(buffer), "\r\n--> Return: ");
                    UART_SendStringRing(UART_CMDLINE, buffer);
                    UART_SendStringRing(UART_CMDLINE, ErrorCode[ret_val]); //
                    snprintf(buffer, sizeof(buffer), "%s%s$ ", x_timeBuffer, NAME_SHELL);
                    UART_SendStringRing(UART_CMDLINE, buffer);
                    context->commandBufferIndex = 0;
                }
            }
            else
            {
                ResetConfig();
                char buffer[60];
                snprintf(buffer, sizeof(buffer), "\r\n%s%s$ ", x_timeBuffer, NAME_SHELL);
                UART_SendStringRing(UART_CMDLINE, buffer);
            }
        }
        else if (rxData == KEY_BACKSPACE)
        {
            if (context->commandBufferIndex > 0)
            {
                context->commandBufferIndex--;
                context->commandBuffer[context->commandBufferIndex] = '\0';
            }
            else
            {
                UART_SendStringRing(UART_CMDLINE, " ");
            }
        }
        else
        {
            if (context->commandBufferIndex < COMMAND_MAX_LENGTH - 1)
            {
                context->commandBuffer[context->commandBufferIndex++] = rxData;
                context->commandBuffer[context->commandBufferIndex] = '\0';
            }
            else
            {
                // Command too long
                UART_SendStringRing(UART_CMDLINE, "\r\nError: Command too long.");
                char buffer[60];
                snprintf(buffer, sizeof(buffer), "\r\n%s%s$ ", x_timeBuffer, NAME_SHELL);
                UART_SendStringRing(UART_CMDLINE, buffer);
                context->commandBufferIndex = 0;
                context->commandBuffer[0] = '\0';
            }
        }
    }
}

static void ResetConfig(void)
{
    laser_interval = 0;
    run_system = 0;
    run_adc = 0;
    adc_interval = 0;
    pair_slot = 1;
    current_column = 1;
    current_row = 1;
    user_delay = 0;
    rest_time = 0;
    run_inf = 0;
    do_time = 0;
}

/*-----------------------COMMAND FUNCTION LIST---------------------------*/
/* Command support */
int Cmd_help(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    UART_SendStringRing(UART_CMDLINE, "\r\nAvailable commands:");

    tCmdLineEntry *pEntry = &g_psCmdTable[0];
    size_t maxCmdLength = 0;
    while (pEntry->pcCmd)
    {
        size_t cmdLength = strlen(pEntry->pcCmd);
        if (cmdLength > maxCmdLength)
        {
            maxCmdLength = cmdLength;
        }
        pEntry++;
    }
    pEntry = &g_psCmdTable[0];
    while (pEntry->pcCmd)
    {
        char buffer[256];
        size_t cmdLength = strlen(pEntry->pcCmd);
        int padding = (int)(maxCmdLength - cmdLength + 4);
        snprintf(buffer, sizeof(buffer), "\r\n[%s]%*s: %s",
                 pEntry->pcCmd, padding, "", pEntry->pcHelp);

        UART_SendStringRing(UART_CMDLINE, buffer);
        pEntry++;
    }

    return (CMDLINE_OK);
}

/* Command for power supply */
int Cmd_temp_pw(int argc, char *argv[])
{
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    if (atoi(argv[1]))
        LL_GPIO_SetOutputPin(EF_5_EN_GPIO_Port, EF_5_EN_Pin);
    else
        LL_GPIO_ResetOutputPin(EF_5_EN_GPIO_Port, EF_5_EN_Pin);
    return (CMDLINE_OK);
}
/* Command for temperature */
int Cmd_set_temp(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    int16_t setpoint[4];
    char buffer[40];
    for (uint8_t i = 0; i < 4; i++)
    {
        setpoint[i] = atoi(argv[i + 1]);
        temperature_set_setpoint(i, setpoint[i]);
        snprintf(buffer, sizeof(buffer), "\r\n--> Setpoint[%d]: %i", i, setpoint[i]);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return CMDLINE_OK;
}

int Cmd_get_temp(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int16_t temp = 0;
    char buffer[80];

    /* Temperature from BMP390 */
    // temp = bmp390_get_temperature();
    if (temp == 0x7FFF)
    {
        UART_SendStringRing(UART_CMDLINE, "\r\n--> BMP390 is fail");
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "\r\n--> BMP390 temp: %i", temp);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    /* Temperature from NTC */
    NTC_get_temperature(NTC_Temperature);
    for (uint8_t channel = 0; channel < 8; channel++)
    {
        temp = NTC_Temperature[channel];
        if (temp == 0x7FFF)
        {
            snprintf(buffer, sizeof(buffer), "\r\n--> NTC[%d] is fail", channel);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "\r\n--> NTC[%d]: %i", channel, temp);
        }
        UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return CMDLINE_OK;
}

int Cmd_get_temp_setpoint(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;
    int16_t setpoint = 0;
    char buffer[60];
    for (uint8_t channel = 0; channel < 4; channel++)
    {
        setpoint = temperature_get_setpoint(channel);
        snprintf(buffer, sizeof(buffer), "\r\n--> Setpoint[%d]:%i", channel, setpoint);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return (CMDLINE_OK);
}
int Cmd_tec_set_vol(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    char buffer[80];
    for (uint8_t i = 0; i < 4; i++)
    {
        temperature_set_tec_vol(i, atoi(argv[i + 1]));
        snprintf(buffer, sizeof(buffer), "\r\n--> Tec voltage[%d]: %i mV", i, atoi(argv[i + 1]));
        UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return CMDLINE_OK;
}

int Cmd_tec_get_vol(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;
    uint16_t vol[4];
    char buffer[80];
    for (uint8_t i = 0; i < 4; i++)
    {
        vol[i] = temperature_get_tec_vol(i);
        snprintf(buffer, sizeof(buffer), "\r\n--> Tec voltage[%d]: %i mV", i, vol[i]);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return CMDLINE_OK;
}

int Cmd_tec_dir (int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    tec_dir_t dir_0 = atoi(argv[1]) ? TEC_HEAT : TEC_COOL;
    tec_dir_t dir_1 = atoi(argv[2]) ? TEC_HEAT : TEC_COOL;
    tec_dir_t dir_2 = atoi(argv[3]) ? TEC_HEAT : TEC_COOL;
    tec_dir_t dir_3 = atoi(argv[4]) ? TEC_HEAT : TEC_COOL;
    tec_set_dir(dir_0, dir_1, dir_2, dir_3);
    return CMDLINE_OK;
}

int Cmd_tec_ctrl (int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    if (atoi(argv[1]))
    	lt8722_set_swen_req(0, LT8722_SWEN_REQ_ENABLED);
    else
    	lt8722_set_swen_req(0, LT8722_SWEN_REQ_DISABLED);
    if (atoi(argv[2]))
    	lt8722_set_swen_req(1, LT8722_SWEN_REQ_ENABLED);
    else
    	lt8722_set_swen_req(1, LT8722_SWEN_REQ_DISABLED);
    if (atoi(argv[3]))
    	lt8722_set_swen_req(2, LT8722_SWEN_REQ_ENABLED);
    else
    	lt8722_set_swen_req(2, LT8722_SWEN_REQ_DISABLED);
    if (atoi(argv[4]))
    	lt8722_set_swen_req(3, LT8722_SWEN_REQ_ENABLED);
    else
    	lt8722_set_swen_req(3, LT8722_SWEN_REQ_DISABLED);
    return CMDLINE_OK;
}

int Cmd_heater_set_duty(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    uint8_t duty;
    char buffer[80];
    for (uint8_t i = 0; i < 4; i++)
    {
        duty = atoi(argv[i + 1]);
        if (duty > 100) duty = 100;
        temperature_set_heater_duty(i, duty);
        snprintf(buffer, sizeof(buffer), "Heater duty[%d]: %i%%\r\n", i, duty);
		UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return CMDLINE_OK;
}

int Cmd_heater_get_duty(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;
    uint8_t duty;
    char buffer[80];
    for (uint8_t i = 0; i < 4; i++)
    {
        duty = temperature_get_heater_duty(i);
        snprintf(buffer, sizeof(buffer), "Heater duty[%d]: %i%%\r\n", i, duty);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return CMDLINE_OK;
}

int Cmd_tec_init(int argc, char *argv[])
{
    if (argc > 3) return CMDLINE_TOO_MANY_ARGS;
	uint32_t data = 1;
	char buffer[60];
	int8_t tec_init_channel = 0;
    if (argc == 2)
    {
    	for (uint8_t channel = 0; channel < 4; channel++)
    	{
    		tec_init_channel = lt8722_init(channel);
			LL_mDelay(10);
			if (!tec_init_channel) lt8722_set_swen_req(channel, LT8722_SWEN_REQ_DISABLED);
			lt8722_reg_read(channel, LT8722_SPIS_STATUS, &data);
			if (!data)
				snprintf(buffer, sizeof(buffer), "\r\n--> Tec %d init success", channel);
			else
				snprintf(buffer, sizeof(buffer), "\r\n--> Tec %d init fail", channel);
			UART_SendStringRing(UART_CMDLINE, buffer);
    	}
        return CMDLINE_OK;
    }
    if (argc == 3)
    {
    	uint8_t channel = atoi(argv[1]);
    	tec_init_channel = lt8722_init(channel);
    	LL_mDelay(10);
    	if (!tec_init_channel) lt8722_set_swen_req(channel, LT8722_SWEN_REQ_DISABLED);
    	lt8722_reg_read(channel, LT8722_SPIS_STATUS, &data);
    	if (!data)
    		snprintf(buffer, sizeof(buffer), "\r\n--> Tec %d init success", channel);
    	else
    		snprintf(buffer, sizeof(buffer), "\r\n--> Tec %d init fail", channel);
    	UART_SendStringRing(UART_CMDLINE, buffer);
    	return CMDLINE_OK;
    }
    return CMDLINE_OK;
}

int Cmd_temp_auto_ctrl(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    uint8_t auto_0 = atoi(argv[1]) ? 1 : 0;
    uint8_t auto_1 = atoi(argv[2]) ? 1 : 0;
    uint8_t auto_2 = atoi(argv[3]) ? 1 : 0;
    uint8_t auto_3 = atoi(argv[4]) ? 1 : 0;
    temperature_set_auto_ctrl(auto_0, auto_1, auto_2, auto_3);
    return (CMDLINE_OK);
}
/* Command for ir led */
int Cmd_ir_set_duty(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    char buffer[80];
	uint8_t duty = atoi(argv[1]);
	if (duty > 100) duty = 100;
	ir_led_set_duty(duty);
    snprintf(buffer, sizeof(buffer), "IR LED duty: %i%%\r\n", duty);
    UART_SendStringRing(UART_CMDLINE, buffer);
    return CMDLINE_OK;
}

int Cmd_ir_get_duty(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;
    char buffer[40];
    uint8_t duty = ir_led_get_duty();
	snprintf(buffer, sizeof(buffer), "Heater duty: %i%%\r\n", duty);
	UART_SendStringRing(UART_CMDLINE, buffer);
    return CMDLINE_OK;
}

/* Command for i2c sensor */
int Cmd_acceleration_gyroscope_get(int argc, char *argv[])
{
    return (CMDLINE_OK);
}
int Cmd_pressure_get(int argc, char *argv[])
{
    return (CMDLINE_OK);
}
/* Command for system */
int Cmd_get_all(int argc, char *argv[])
{
    UART_SendStringRing(UART_CMDLINE, "Get all \r\n");
    return (CMDLINE_OK);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Cmd_dac_set(int argc, char *argv[])
{
    if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;

    uint8_t dac_point = atoi(argv[1]);

    if (dac_point > 255)
        return CMDLINE_INVALID_ARG;

    char buffer[60];
    snprintf(buffer, sizeof(buffer), "DAC Point: %d \r\n", dac_point);
    UART_SendStringRing(UART_CMDLINE, buffer);

    DAC_Write(0, dac_point);

    return (CMDLINE_OK);
}

// LASER_SW_INT_CS_Pin
// LASER_SW_INT_CS_GPIO_Port
int Cmd_ls_set(int argc, char *argv[])
{
    if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    uint8_t ls_slot = atoi(argv[1]);
    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    if (ls_slot > 0 && ls_slot <= 36)
    {
        uint8_t chip_index = (ls_slot - 1) / 6;
        uint8_t port_index = (ls_slot - 1) % 6;
        data[chip_index] = (1 << port_index);
    }
    LL_GPIO_ResetOutputPin(LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin);
    for (int i = 5; i >= 0; i--)
    {
        LL_SPI_TransmitData8(SPI1, data[i]);
        while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    }
    while (LL_SPI_IsActiveFlag_BSY(SPI1));
    LL_GPIO_SetOutputPin(LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin);
    return CMDLINE_OK;
}

// int Cmd_ls_set(int argc, char *argv[]) {
//     if (argc > 3) return CMDLINE_TOO_MANY_ARGS;
//     if (argc < 3) return CMDLINE_TOO_FEW_ARGS;
//     uint8_t ls_slot = atoi(argv[1]);
////    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
////    if (ls_slot > 0 && ls_slot <= 36) {
////        uint8_t chip_index = (ls_slot - 1) / 6;
////        uint8_t port_index = (ls_slot - 1) % 6;
////        data[chip_index] = (1 << port_index);
////    }
//    LL_GPIO_ResetOutputPin(LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin);
//        LL_SPI_TransmitData8(SPI1, ls_slot);
//    while (LL_SPI_IsActiveFlag_BSY(SPI1));
//    LL_GPIO_SetOutputPin(LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin);
//    return CMDLINE_OK;
//}

int Cmd_pd_set(int argc, char *argv[])
{
    if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    uint8_t pd_slot = atoi(argv[1]);
    if (pd_slot > 36)
        return CMDLINE_INVALID_ARG;
    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    if (pd_slot > 0 && pd_slot <= 36)
    {
        uint8_t chip_index = (pd_slot - 1) / 6;
        uint8_t port_index = (pd_slot - 1) % 6;
        data[chip_index] = (1 << port_index);
    }
    uint8_t reversed_data[6];
    for (int i = 0; i < 6; i++)
    {
        reversed_data[i] = data[5 - i];
    }
    LL_GPIO_ResetOutputPin(PHOTO_PD_CS_GPIO_Port, PHOTO_PD_CS_Pin);
    HAL_SPI_Transmit(&hspi2, reversed_data, 6, 1000);
    LL_GPIO_SetOutputPin(PHOTO_PD_CS_GPIO_Port, PHOTO_PD_CS_Pin);
    return (CMDLINE_OK);
}

int Cmd_get_adc(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    uint8_t rxData[2] = {0};
    uint32_t result = 0;
    LL_GPIO_ResetOutputPin(PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin);
    __asm__("NOP");
    LL_GPIO_SetOutputPin(PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin);
    LL_GPIO_ResetOutputPin(PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin);
    HAL_SPI_Receive(&hspi2, rxData, 2, 1000);
    LL_GPIO_SetOutputPin(PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin);
    result = ((uint32_t)rxData[0] << 8) | rxData[1];

    char buffer[60];
    snprintf(buffer, sizeof(buffer), "Got ADC: %ld \r\n", result);
    UART_SendStringRing(UART_CMDLINE, buffer);
    return (CMDLINE_OK);
}

int Cmd_auto_laser(int argc, char *argv[])
{
    if (argc > 8)
        return CMDLINE_TOO_MANY_ARGS;
    if (argc < 8)
        return CMDLINE_TOO_FEW_ARGS;

    uint32_t interval = atoi(argv[1]);
    uint32_t times = atoi(argv[2]);
    uint32_t udelay = atoi(argv[3]);
    uint8_t s_do_time = atoi(argv[4]);
    uint32_t s_rest_time = atoi(argv[5]);
    uint32_t dac = atoi(argv[6]);

    char buffer[80];

    if (interval % 100 != 0 || interval < 400)
    {
        snprintf(buffer, sizeof(buffer), "Error: Interval must be a multiple of 100ms and > 400ms.\r\n");
        UART_SendStringRing(UART_CMDLINE, buffer);
        return CMDLINE_INVALID_ARG;
    }

    if (times < 200 || times % 100 != 0 || times > interval)
    {
        snprintf(buffer, sizeof(buffer), "Error: Times must be <= interval, > 200ms, mulof100ms.\r\n");
        UART_SendStringRing(UART_CMDLINE, buffer);
        return CMDLINE_INVALID_ARG;
    }

    if (udelay > 500)
    {
        snprintf(buffer, sizeof(buffer), "Error: udelay <= 500\r\n");
        UART_SendStringRing(UART_CMDLINE, buffer);
        return CMDLINE_INVALID_ARG;
    }

    if (s_do_time > 200)
    {
        snprintf(buffer, sizeof(buffer), "Error: Do only < 200 or = 0 to infinity\r\n");
        UART_SendStringRing(UART_CMDLINE, buffer);
        return CMDLINE_INVALID_ARG;
    }

    run_inf = (s_do_time == 0) ? 1 : 0;

    if (dac > 255)
        return CMDLINE_INVALID_ARG;

    snprintf(buffer, sizeof(buffer), "DAC Point: %ld\r\n", dac);
    UART_SendStringRing(UART_CMDLINE, buffer);

    DAC_Write(0, dac);
    HAL_Delay(100);
    DAC_Write(0, dac);

    int16_t temp = 0;

    if (temp == 0x7FFF)
    {
        UART_SendStringRing(UART_CMDLINE, "\r\nTemp BMP390 = [FAIL]\r\n");
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "\r\nTemp BMP390 = [%i]\r\n", temp);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    for (uint8_t channel = 0; channel < 8; channel++)
    {
        temp = NTC_Temperature[channel];
        if (temp == 0x7FFF)
        {
            snprintf(buffer, sizeof(buffer), " | NTC[%d] = [FAIL]\r\n", channel);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), " | NTC[%d] = [%i]\r\n", channel, temp);
        }
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    laser_interval = interval;
    adc_interval = times;
    user_delay = udelay;
    rest_time = s_rest_time;
    do_time = s_do_time;
    run_system = 1;

    snprintf(buffer, sizeof(buffer), "Run system with interval: %ld ms, times: %ld, delay: %ld. Enter to End\r\n", interval, times, udelay);
    UART_SendStringRing(UART_CMDLINE, buffer);

    return CMDLINE_OK;
}

int Cmd_auto_adc(int argc, char *argv[])
{
    if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;

    uint32_t interval = atoi(argv[1]);
    char buffer[80];

    if (interval % 100 != 0 || interval < 500)
    {
        snprintf(buffer, sizeof(buffer), "Error: Interval must be a multiple of 100ms and > 500ms.\r\n");
        UART_SendStringRing(UART_CMDLINE, buffer);
        return CMDLINE_INVALID_ARG;
    }

    adc_interval = interval;
    run_adc = 1;

    snprintf(buffer, sizeof(buffer), "Run auto ADC with interval: %ld ms. Enter to End\r\n", interval);
    UART_SendStringRing(UART_CMDLINE, buffer);

    return CMDLINE_OK;
}

//int Cmd_dis(int argc, char *argv[])
//{
//	uint8_t channel = atoi(argv[1]);
//	LL_GPIO_ResetOutputPin((GPIO_TypeDef*)en_port[channel], en_pin[channel]);
//	lt8722_set_enable_req(channel, LT8722_ENABLE_REQ_DISABLED);
//	return CMDLINE_OK;
//}
//
//int Cmd_cool_0(int argc, char *argv[])
//{
////	lt8722_init();
//	lt8722_set_output_voltage_channel(0, TEC_COOL, 1000000000);
////	HAL_Delay(500);
//	lt8722_set_output_voltage_channel(1, TEC_HEAT, 1500000000);
//
//    return (CMDLINE_OK);
//}
//
//int Cmd_heat_0(int argc, char *argv[])
//{
////	lt8722_init();
////	lt8722_set_output_voltage_channel(1, TEC_COOL, 1000000000);
//	lt8722_set_output_voltage_channel(0, TEC_HEAT, 1000000000);
//
//    return (CMDLINE_OK);
//}
//int Cmd_cool_1(int argc, char *argv[])
//{
////	lt8722_init();
//	lt8722_set_output_voltage_channel(1, TEC_COOL, 1500000000);
////	lt8722_set_output_voltage_channel(1, TEC_HEAT, 1000000000);
//
//    return (CMDLINE_OK);
//}
//
//int Cmd_heat_1(int argc, char *argv[])
//{
////	lt8722_init();
////	lt8722_set_output_voltage_channel(1, TEC_COOL, 1000000000);
//	lt8722_set_output_voltage_channel(1, TEC_HEAT, 1500000000);
//
//    return (CMDLINE_OK);
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//int Cmd_reset(int argc, char *argv[])
//{
//	uint8_t channel = atoi(argv[1]);
//    lt8722_reset(channel);
//    return (CMDLINE_OK);
//}
//
//int Cmd_set_en_req(int argc, char *argv[])
//{
//	uint8_t channel = atoi(argv[1]);
//    LL_GPIO_SetOutputPin((GPIO_TypeDef*)en_port[channel], en_pin[channel]);
//    lt8722_set_enable_req(channel, LT8722_ENABLE_REQ_ENABLED);
//    return (CMDLINE_OK);
//}
//
//int Cmd_reset_en_req(int argc, char *argv[])
//{
//	uint8_t channel = atoi(argv[1]);
//	LL_GPIO_ResetOutputPin((GPIO_TypeDef*)en_port[channel], en_pin[channel]);
//    lt8722_set_enable_req(channel, LT8722_ENABLE_REQ_DISABLED);
//    return (CMDLINE_OK);
//}
//
//int Cmd_set_swen_req(int argc, char *argv[])
//{
//	uint8_t channel = atoi(argv[1]);
////	LL_GPIO_ResetOutputPin((GPIO_TypeDef*)swen_port[channel], swen_pin[channel]);
//	lt8722_set_swen_req(channel, LT8722_SWEN_REQ_ENABLED);
//    return (CMDLINE_OK);
//}
//
//int Cmd_reset_swen_req(int argc, char *argv[])
//{
//	uint8_t channel = atoi(argv[1]);
////	LL_GPIO_ResetOutputPin((GPIO_TypeDef*)swen_port[channel], swen_pin[channel]);
//	lt8722_set_swen_req(channel, LT8722_SWEN_REQ_DISABLED);
//	return (CMDLINE_OK);
//}
//
//int Cmd_clear_status_reg(int argc, char *argv[])
//{
//    lt8722_reg_write(LT8722_SPIS_STATUS, 0);
//    return (CMDLINE_OK);
//}
//
int Cmd_read(int argc, char *argv[])
{
    uint32_t data;
    char buffer[60];

    uint8_t channel = atoi(argv[1]);

    lt8722_reg_read(channel, LT8722_SPIS_COMMAND, &data);
    snprintf(buffer, sizeof(buffer), "\r\nSPIS_COMMAND: 0x%lX-%lX\r\n", data >> 16, data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    lt8722_reg_read(channel, LT8722_SPIS_STATUS, &data);
    snprintf(buffer, sizeof(buffer), "SPIS_STATUS: 0x%lX-%lX\r\n", data >> 16, data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    lt8722_reg_read(channel, LT8722_SPIS_DAC_ILIMN, &data);
    snprintf(buffer, sizeof(buffer), "SPIS_DAC_ILIMN: 0x%lX-%lX\r\n", data >> 16, data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    lt8722_reg_read(channel, LT8722_SPIS_DAC_ILIMP, &data);
    snprintf(buffer, sizeof(buffer), "SPIS_DAC_ILIMP: 0x%lX-%lX\r\n", data >> 16, data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    lt8722_reg_read(channel, LT8722_SPIS_DAC, &data);
    snprintf(buffer, sizeof(buffer), "SPIS_DAC: 0x%lX-%lX\r\n", data >> 16, data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    lt8722_reg_read(channel, LT8722_SPIS_OV_CLAMP, &data);
    snprintf(buffer, sizeof(buffer), "SPIS_OV_CLAMP: 0x%lX\r\n", data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    lt8722_reg_read(channel, LT8722_SPIS_UV_CLAMP, &data);
    snprintf(buffer, sizeof(buffer), "SPIS_UV_CLAMP: 0x%lX\r\n", data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    lt8722_reg_read(channel, LT8722_SPIS_AMUX, &data);
    snprintf(buffer, sizeof(buffer), "SPIS_AMUX: 0x%lX\r\n", data);
    UART_SendStringRing(UART_CMDLINE, buffer);

    return (CMDLINE_OK);
}
//int Cmd_get_status(int argc, char *argv[])
//{
//    uint16_t status;
//    lt8722_get_status(channel, &status);
//
//    char buffer[60];
//    snprintf(buffer, sizeof(buffer), "status: 0x%X\r\n", status);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    return (CMDLINE_OK);
//}
//
//int Cmd_set_ov_clamp(uint8_t channel, int argc, char *argv[])
//{
//    uint8_t over_vol = atoi(argv[1]);
//
//    char buffer[60];
//    snprintf(buffer, sizeof(buffer), "OV_CLAMP: %X\r\n", over_vol);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_set_spis_ov_clamp(channel, over_vol);
//    return CMDLINE_OK;
//}
//int Cmd_set_uv_clamp(uint8_t channel, int argc, char *argv[])
//{
//    uint8_t uper_vol = atoi(argv[1]);
//
//    char buffer[60];
//    snprintf(buffer, sizeof(buffer), "UV_CLAMP: %X\r\n", uper_vol);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_set_spis_uv_clamp(channel, uper_vol);
//    return CMDLINE_OK;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CommandLine_CreateTask(void)
{
    SCH_TASK_CreateTask(&s_CommandTaskContext.taskHandle, &s_CommandTaskContext.taskProperty);
}

void Command_SendSplash(void)
{
    UART_SendStringRing(UART_CMDLINE, "┌──────────────────────────────────────────────────────────────┐\r\n");
    UART_SendStringRing(UART_CMDLINE, "│        █▀ █▀█ ▄▀█ █▀▀ █▀▀ █   █ █ █▄ █ ▀█▀ █▀▀ █▀▀ █ █       │\r\n");
    UART_SendStringRing(UART_CMDLINE, "│        ▄█ █▀▀ █▀█ █▄▄ ██▄ █▄▄ █ █ █ ▀█  █  ██▄ █▄▄ █▀█       │\r\n");
    UART_SendStringRing(UART_CMDLINE, "└──────────────────────────────────────────────────────────────┘\r\n");
    UART_SendStringRing(UART_CMDLINE, "███████╗██╗  ██╗██████╗░░░░░░░░░██╗░░░██╗ ██╗░░░██╗░░░░██████╗░░\r\n");
    UART_SendStringRing(UART_CMDLINE, "██╔════╝╚██╗██╔╝██╔══██╗░░░░░░░░██║░░░██║███║░░███║░░░██╔═████╗░\r\n");
    UART_SendStringRing(UART_CMDLINE, "█████╗░░░╚███╔╝░██████╔╝░█████╗░██║░░░██║╚██║░░╚██║░░░██║██╔██║░\r\n");
    UART_SendStringRing(UART_CMDLINE, "██╔══╝░░░██╔██╗░██╔═══╝░░╚════╝░╚██╗░██╔╝░██║░░░██║░░░████╔╝██║░\r\n");
    UART_SendStringRing(UART_CMDLINE, "███████╗██╔╝ ██╗██║░░░░░░░░░░░░░░╚████╔╝░░██║██╗██║██╗╚██████╔╝░\r\n");
    UART_SendStringRing(UART_CMDLINE, "╚══════╝╚═╝░░╚═╝╚═╝░░░░░░░░░░░░░░░╚═══╝░░░╚═╝╚═╝╚═╝╚═╝░╚═════╝░░\r\n");
    UART_SendStringRing(UART_CMDLINE, "├──────────────────────────────────────────────────────────────┤\r\n");
}
