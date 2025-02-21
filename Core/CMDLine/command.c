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
#define NAME_SHELL "EXP"
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
    {"help", Cmd_help, ": Display list of help commands | format: help"},
    /* Command for power supply */
    //								{"tec_pw_on", Cmd_tec_pw_on,": test | format: tec_pw_on 0/1"},
    //								{"led_pw_on", Cmd_led_pw_on,": test | format: led_pw_on 0/1"},
    {"temp_pw_on", Cmd_temp_pw_on, ": test | format: temp_pw_on 0/1"},
    /* Command for temperature */
    {"set_temp", Cmd_set_temp, ": set_temp | format: set_temp 20 30 40 50"},
    {"get_temp_setpoint", Cmd_get_temp_setpoint, ": get_temp_setpoint | format: get_temp_setpoint"},
    {"get_temp", Cmd_get_temp, ": get_temp | format: get_temp"},

	{"tec_init", Cmd_tec_init, ": init tec | format: tec_init"},
    {"set_tec_vol", Cmd_set_tec_vol, ": set_tec_voltage | format: set_tec_voltage vol0 vol1 vol2 vol3"},
    {"get_tec_vol", Cmd_get_tec_vol, ": get_tec_voltage | format: get_tec_vol"},
    {"tec_dir", Cmd_tec_dir, ": ----1=COOL, 0=HEAT----- | format: tec_dir 1/0 1/0 1/0 1/0"},
	{"tec_ctrl", Cmd_tec_ctrl, ": ------1=ON, 0=OFF------ | format: tec_ctl 1/0 1/0 1/0 1/0"},

    {"set_heater_duty", Cmd_set_heater_duty, ": set_heater_duty | format: set_heater_duty 0% 1% 2% 3%"},
    {"get_heater_duty", Cmd_get_heater_duty, ": get_heater_duty | format: get_heater_duty"},

    {"temp_ctrl", Cmd_temp_ctrl, ": tec_ctrl | format: tec_ctrl C H O O"},
    {"temp_auto_ctrl", Cmd_temp_auto_ctrl, ": tec_ctrl_auto | format: temp_auto_ctrl 1 1 0 0"},
    /* Command for ir led */
    {"set_ir_duty", Cmd_set_ir_duty, " | format: set_ir_duty <duty>"},
    {"get_ir_duty", Cmd_get_ir_duty, " | format: get_ir_duty"},
    /* Command for i2c sensor */
    {"get_accel_gyro", Cmd_get_acceleration_gyroscope, " | format: get_accel_gyro"},
    {"get_press", Cmd_get_pressure, " | format: get_press"},
    /* Command for system */
    {"get_all", Cmd_get_all, ":Display all | format: get_all"},

    {"dac_set", Cmd_dac_set, ": --------- | format: ------"},
    {"get_adc", Cmd_get_adc, ": --------- | format: ------"},
    {"pd_set", Cmd_pd_set, ": --------- | format: ------"},
    {"ls_set", Cmd_ls_set, ": --------- | format: ------"},

    {"auto_adc", Cmd_auto_adc, ": Instead of using <get_adc>, auto_adc = get_adc automatically"},
    {"auto_laser", Cmd_auto_laser, ": : auto_laser [interval] [interval adc] [userdelay]"},


//	{"read", Cmd_test, ": --------- | format: ------"},
    {0, 0, 0}};

static Command_TaskContextTypedef s_CommandTaskContext =
    {
        SCH_INVALID_TASK_HANDLE, // Will be updated by Schedular
        {
            SCH_TASK_SYNC,           // taskType;
            SCH_TASK_PRIO_0,         // taskPriority;
            10,                      // taskPeriodInMS;
            CommandLine_Task_Update, // taskFunction;
            9}};

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

    if (rxData == 0x2D)
    { // '-' key (history up)
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
        char buffer[30];
        snprintf(buffer, sizeof(buffer), "\r%s$ ", NAME_SHELL);
        UART_SendStringRing(UART_CMDLINE, buffer);
        UART_SendStringRing(UART_CMDLINE, context->commandBuffer); // Display updated command
        return;
    }
    else if (rxData == 0x3D)
    { // '=' key (history down)
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
        char buffer[30];
        snprintf(buffer, sizeof(buffer), "\r%s$ ", NAME_SHELL);
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

    UART_SendStringRing(UART_CMDLINE, "\r\nAvailable commands:\r\n");

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
int Cmd_temp_pw_on(int argc, char *argv[])
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
    char buffer[80];

    for (uint8_t i = 0; i < 4; i++)
    {
        setpoint[i] = atoi(argv[i + 1]);
        temperature_set_setpoint(i, setpoint[i]);
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        snprintf(buffer, sizeof(buffer), "Setpoint[%d]: %i\r\n", i, setpoint[i]);
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
    //    temp = bmp390_get_temperature();
    if (temp == 0x7FFF)
    {
        UART_SendStringRing(UART_CMDLINE, "BMP390 is fail\r\n");
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "BMP390 temp: %i\r\n", temp);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    /* Temperature from NTC */
    for (uint8_t channel = 0; channel < 8; channel++)
    {
        temp = NTC_Temperature[channel];
        if (temp == 0x7FFF)
        {
            snprintf(buffer, sizeof(buffer), "NTC[%d] is fail\r\n", channel);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "NTC[%d]: %i\r\n", channel, temp);
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
    for (uint8_t channel = 0; channel < 4; channel++)
    {
        setpoint = temperature_get_setpoint(channel);
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "Setpoint[%d]:%i \r\n", channel, setpoint);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }
    return (CMDLINE_OK);
}
int Cmd_set_tec_vol(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;

    uint16_t vol[4];
    char buffer[80];

    for (uint8_t i = 0; i < 4; i++)
    {
        vol[i] = atoi(argv[i + 1]);
        if (vol[i] > 3000)
            vol[i] = 3000;
        temperature_set_tec_vol(i, vol[i]);
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        snprintf(buffer, sizeof(buffer), "\r\n--> Tec voltage[%d]: %i mV", i, vol[i]);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    return CMDLINE_OK;
}

int Cmd_get_tec_vol(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    uint16_t vol[4];
    char buffer[80];

    for (uint8_t i = 0; i < 4; i++)
    {
        vol[i] = temperature_get_tec_vol(i);
        snprintf(buffer, sizeof(buffer), "Tec voltage[%d]: %i mV\r\n", i, vol[i]);
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

    mode_ctrl_tec_t mode_0 = TEC_COOL;
    mode_ctrl_tec_t mode_1 = TEC_COOL;
    mode_ctrl_tec_t mode_2 = TEC_COOL;
    mode_ctrl_tec_t mode_3 = TEC_COOL;

    if (!atoi(argv[1])) mode_0 = TEC_HEAT;
    if (!atoi(argv[2])) mode_1 = TEC_HEAT;
    if (!atoi(argv[3])) mode_2 = TEC_HEAT;
    if (!atoi(argv[4])) mode_3 = TEC_HEAT;

    tec_set_mode(mode_0, mode_1, mode_2, mode_3);
    return CMDLINE_OK;
}

int Cmd_tec_ctrl (int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;

    uint8_t on_0 = atoi(argv[1]);
    uint8_t on_1 = atoi(argv[2]);
    uint8_t on_2 = atoi(argv[3]);
    uint8_t on_3 = atoi(argv[4]);

    tec_on(on_0, on_1, on_2, on_3);

    return CMDLINE_OK;
}

int Cmd_set_heater_duty(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;

    uint8_t duty[4];
    char buffer[80];

    for (uint8_t i = 0; i < 4; i++)
    {
        duty[i] = atoi(argv[i + 1]);
        if (duty[i] > 100)
            duty[i] = 100;
        temperature_set_heater_duty(i, duty[i]);
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        snprintf(buffer, sizeof(buffer), "Heater duty[%d]: %i%%\r\n", i, duty[i]);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    return CMDLINE_OK;
}

int Cmd_get_heater_duty(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    uint16_t duty[4];
    char buffer[80];

    for (uint8_t i = 0; i < 4; i++)
    {
        duty[i] = temperature_get_heater_duty(i);
        snprintf(buffer, sizeof(buffer), "Heater duty[%d]: %i%%\r\n", i, duty[i]);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    return CMDLINE_OK;
}

int Cmd_tec_init(int argc, char *argv[])
{
    if (argc > 2) return CMDLINE_TOO_MANY_ARGS;
	lt8722_init();
	return CMDLINE_OK;
}

int Cmd_temp_ctrl(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;
    mode_ctrl_temp_t mode_0 = OFF;
    mode_ctrl_temp_t mode_1 = OFF;
    mode_ctrl_temp_t mode_2 = OFF;
    mode_ctrl_temp_t mode_3 = OFF;

    if (!strcmp(argv[1], "C"))
        mode_0 = COOL;
    else if (!strcmp(argv[1], "H"))
        mode_0 = HEAT;
    else
        mode_0 = OFF;
    if (!strcmp(argv[2], "C"))
        mode_1 = COOL;
    else if (!strcmp(argv[2], "H"))
        mode_1 = HEAT;
    else
        mode_1 = OFF;
    if (!strcmp(argv[3], "C"))
        mode_1 = COOL;
    else if (!strcmp(argv[3], "H"))
        mode_2 = HEAT;
    else
        mode_2 = OFF;
    if (!strcmp(argv[4], "C"))
        mode_3 = COOL;
    else if (!strcmp(argv[4], "H"))
        mode_3 = HEAT;
    else
        mode_3 = OFF;

    temperature_set_ctrl(mode_0, mode_1, mode_2, mode_3);
    return (CMDLINE_OK);
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
int Cmd_set_ir_duty(int argc, char *argv[])
{
    if (argc < 6)
        return CMDLINE_TOO_FEW_ARGS;
    if (argc > 6)
        return CMDLINE_TOO_MANY_ARGS;

    uint8_t duty[4];
    char buffer[80];

    for (uint8_t i = 0; i < 4; i++)
    {
        duty[i] = atoi(argv[i + 1]);
        if (duty[i] > 100)
            duty[i] = 100;
        ir_led_set_duty(i, duty[i] * 9999 / 100);
    }

    snprintf(buffer, sizeof(buffer), "IR LED [0] duty: %i%%\r\n", duty[0]);
    UART_SendStringRing(UART_CMDLINE, buffer);
    snprintf(buffer, sizeof(buffer), "IR LED [1] duty: %i%%\r\n", duty[1]);
    UART_SendStringRing(UART_CMDLINE, buffer);
    snprintf(buffer, sizeof(buffer), "IR LED [2] duty: %i%%\r\n", duty[2]);
    UART_SendStringRing(UART_CMDLINE, buffer);
    snprintf(buffer, sizeof(buffer), "IR LED [3] duty: %i%%\r\n", duty[3]);
    UART_SendStringRing(UART_CMDLINE, buffer);

    return CMDLINE_OK;
}

int Cmd_get_ir_duty(int argc, char *argv[])
{
    if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    uint16_t duty[4];
    char buffer[80];

    for (uint8_t i = 0; i < 4; i++)
    {
        duty[i] = ir_led_get_duty(i) * 100 / 9999;
        snprintf(buffer, sizeof(buffer), "Heater duty[%d]: %i%%\r\n", i, duty[i]);
        UART_SendStringRing(UART_CMDLINE, buffer);
    }

    return CMDLINE_OK;
}

/* Command for i2c sensor */
int Cmd_get_acceleration_gyroscope(int argc, char *argv[])
{
    return (CMDLINE_OK);
}
int Cmd_get_pressure(int argc, char *argv[])
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
        while (!LL_SPI_IsActiveFlag_TXE(SPI1))
            ;
    }

    while (LL_SPI_IsActiveFlag_BSY(SPI1))
        ;

    LL_GPIO_SetOutputPin(LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin);

    return CMDLINE_OK;
}

// int Cmd_ls_set(int argc, char *argv[]) {
//     if (argc > 3) return CMDLINE_TOO_MANY_ARGS;
//     if (argc < 3) return CMDLINE_TOO_FEW_ARGS;
//
//     uint8_t ls_slot = atoi(argv[1]);
////    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
////
////    if (ls_slot > 0 && ls_slot <= 36) {
////
////        uint8_t chip_index = (ls_slot - 1) / 6;
////        uint8_t port_index = (ls_slot - 1) % 6;
////
////
////        data[chip_index] = (1 << port_index);
////    }
//
//    LL_GPIO_ResetOutputPin(LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin);
//
//
//        LL_SPI_TransmitData8(SPI1, ls_slot);
//
//    while (LL_SPI_IsActiveFlag_BSY(SPI1));
//
//    LL_GPIO_SetOutputPin(LASER_SW_INT_CS_GPIO_Port, LASER_SW_INT_CS_Pin);
//
//    return CMDLINE_OK;
//}

// PHOTO_PD_CS_Pin
// PHOTO_PD_CS_GPIO_Port
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

// auto_laser [interval] [times adc/interval]  --- Enter for stop

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


int Cmd_test(int argc, char *argv[])
{
	lt8722_init();
//	UART_SendStringRing(UART_CMDLINE, "\r\nLT8722_init \r\n");
	lt8722_set_output_voltage_channel(0, TEC_COOL, 1900000000);
	lt8722_set_output_voltage_channel(1, TEC_HEAT, 900000000);

	uint32_t data;
	char buffer[60];
	lt8722_reg_read(0, LT8722_SPIS_COMMAND, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_COMMAND: 0x%lX-%lX\r\n", data >> 16, data);
	UART_SendStringRing(UART_CMDLINE, buffer);

	lt8722_reg_read(0, LT8722_SPIS_STATUS, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_STATUS: 0x%lX-%lX\r\n", data >> 16, data);
	UART_SendStringRing(UART_CMDLINE, buffer);

	lt8722_reg_read(0, LT8722_SPIS_DAC_ILIMN, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_DAC_ILIMN: 0x%lX-%lX\r\n", data >> 16, data);
	UART_SendStringRing(UART_CMDLINE, buffer);

	lt8722_reg_read(0, LT8722_SPIS_DAC_ILIMP, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_DAC_ILIMP: 0x%lX-%lX\r\n", data >> 16, data);
	UART_SendStringRing(UART_CMDLINE, buffer);

	lt8722_reg_read(0, LT8722_SPIS_DAC, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_DAC: 0x%lX-%lX\r\n", data >> 16, data);
	UART_SendStringRing(UART_CMDLINE, buffer);

	lt8722_reg_read(0, LT8722_SPIS_OV_CLAMP, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_OV_CLAMP: 0x%lX\r\n", data);
	UART_SendStringRing(UART_CMDLINE, buffer);

	lt8722_reg_read(0, LT8722_SPIS_UV_CLAMP, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_UV_CLAMP: 0x%lX\r\n", data);
	UART_SendStringRing(UART_CMDLINE, buffer);

	lt8722_reg_read(0, LT8722_SPIS_AMUX, &data);
	snprintf(buffer, sizeof(buffer), "SPIS_AMUX: 0x%lX\r\n", data);
	UART_SendStringRing(UART_CMDLINE, buffer);
    return CMDLINE_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//int Cmd_reset(int argc, char *argv[])
//{
//    lt8722_reset();
//    return (CMDLINE_OK);
//}
//
//int Cmd_set_en_req(int argc, char *argv[])
//{
//    LL_GPIO_SetOutputPin(TEC_1_EN_GPIO_Port, TEC_1_EN_Pin);
//    //	lt8722_reg_write(LT8722_SPIS_COMMAND, 0x00004000);
//    lt8722_set_enable_req(LT8722_ENABLE_REQ_ENABLED);
//    return (CMDLINE_OK);
//}
//
//int Cmd_reset_en_req(int argc, char *argv[])
//{
//    lt8722_set_enable_req(LT8722_ENABLE_REQ_DISABLED);
//    return (CMDLINE_OK);
//}
//
//int Cmd_clear_status_reg(int argc, char *argv[])
//{
//    lt8722_reg_write(LT8722_SPIS_STATUS, 0);
//    return (CMDLINE_OK);
//}
//
//int Cmd_read(int argc, char *argv[])
//{
//    uint32_t data;
//    char buffer[60];
//
//    lt8722_reg_read(LT8722_SPIS_COMMAND, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_COMMAND: 0x%lX-%lX\r\n", data >> 16, data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_reg_read(LT8722_SPIS_STATUS, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_STATUS: 0x%lX-%lX\r\n", data >> 16, data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_reg_read(LT8722_SPIS_DAC_ILIMN, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_DAC_ILIMN: 0x%lX-%lX\r\n", data >> 16, data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_reg_read(LT8722_SPIS_DAC_ILIMP, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_DAC_ILIMP: 0x%lX-%lX\r\n", data >> 16, data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_reg_read(LT8722_SPIS_DAC, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_DAC: 0x%lX-%lX\r\n", data >> 16, data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_reg_read(LT8722_SPIS_OV_CLAMP, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_OV_CLAMP: 0x%lX\r\n", data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_reg_read(LT8722_SPIS_UV_CLAMP, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_UV_CLAMP: 0x%lX\r\n", data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    lt8722_reg_read(LT8722_SPIS_AMUX, &data);
//    snprintf(buffer, sizeof(buffer), "SPIS_AMUX: 0x%lX\r\n", data);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    return (CMDLINE_OK);
//}
//
//int Cmd_on_tec(int argc, char *argv[])
//{
//    lt8722_init();
//    return (CMDLINE_OK);
//}
//
//int Cmd_tec_set_vol(int argc, char *argv[])
//{
//    //	if (argc < 2) return CMDLINE_TOO_FEW_ARGS;
//    //	if (argc > 2) return CMDLINE_TOO_MANY_ARGS;
//
//    int64_t vol = atoi(argv[1]);
//
//    char buffer[60];
//    snprintf(buffer, sizeof(buffer), "Tec set: %lld mV\r\n", vol);
//    UART_SendStringRing(UART_CMDLINE, buffer);
//
//    vol *= 1000000;
//    lt8722_set_output_voltage(vol);
//    return (CMDLINE_OK);
//}
//
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
