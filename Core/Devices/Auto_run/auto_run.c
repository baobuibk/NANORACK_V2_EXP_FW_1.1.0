/*
 * auto_run.c
 *
 *  Created on: Jan 26, 2025
 *      Author: CAO HIEU
 */

#include "../../Auto_run/auto_run.h"

#include "stdio.h"
#include "scheduler.h"

#include "mcp4902.h"
#include "uart.h"
#include "main.h"
#include "ntc.h"

uint8_t data_times = 0;
#define UART_CMDLINE USART1
//extern SPI_HandleTypeDef hspi2;

/* Private typedef -----------------------------------------------------------*/
typedef struct AUTO_RUN_TaskContextTypedef_
{
	SCH_TASK_HANDLE taskHandle;
	SCH_TaskPropertyTypedef taskProperty;
} AUTO_RUN_TaskContextTypedef;

/* Private function ----------------------------------------------------------*/
static void auto_run_task_update(void);
void read_adc(void);
void read_adc_without_LF(void);
void auto_set_ls(uint8_t ls_slot);
void auto_set_pd(uint8_t pd_slot);
void auto_get_temp(void);

static AUTO_RUN_TaskContextTypedef s_auto_runTaskContext =
	{
		SCH_INVALID_TASK_HANDLE, // Will be updated by Schedular
		{
			SCH_TASK_SYNC,		  // taskType;
			SCH_TASK_PRIO_0,	  // taskPriority;
			50,					  // taskPeriodInMS;
			auto_run_task_update, // taskFunction;
			22
		}
	};

volatile uint32_t laser_interval = 0;
volatile uint8_t times_adc = 0;
volatile uint8_t run_system = 0;
volatile uint8_t run_adc = 0;
volatile uint32_t adc_interval = 0;
volatile uint8_t pair_slot = 1;

volatile uint8_t current_column = 1;
volatile uint8_t current_row = 1;
volatile uint32_t user_delay = 0;
volatile uint32_t rest_time = 0;

volatile uint8_t first_time = 1;
volatile uint8_t first_rest = 1;
volatile uint8_t do_time = 0;
volatile uint8_t run_inf = 0;

static void auto_run_task_update(void)
{
	if (run_adc)
	{
		if (SCH_TIM_HasCompleted(SCH_TIM_AUTO_ADC))
		{
			read_adc();
			SCH_TIM_Start(SCH_TIM_AUTO_ADC, adc_interval);
		}
	}

	if (run_system)
	{

		if (SCH_TIM_HasCompleted(SCH_TIM_AUTO_LASER))
		{
			data_times = 0;

			uint8_t ld_slot = (current_column - 1) + (current_row - 1) * 6 + 1;
			if (first_time)
			{
				if (!run_inf)
				{
					if (do_time <= 0)
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
				}

				auto_set_pd(0);
				auto_set_ls(0);

				SCH_TIM_Start(SCH_TIM_USER_DELAY, user_delay);
				first_time = 0;
			}
			else
			{
				if (SCH_TIM_HasCompleted(SCH_TIM_USER_DELAY))
				{
					if (current_row == 1 && first_rest == 0)
					{
						if (current_column == 1)
						{
							auto_get_temp();
							do_time = do_time - 1;
							SCH_TIM_Start(SCH_TIM_REST, rest_time);
							first_rest = 1;
						}
					}
					if (SCH_TIM_HasCompleted(SCH_TIM_REST))
					{
						if (current_row == 1)
						{
							UART_SendStringRing(UART_CMDLINE, "\r\n");
						}

						auto_set_pd(ld_slot);
						auto_set_ls(ld_slot);
						char buffer[50];

						if (ld_slot < 10)
						{
							snprintf(buffer, sizeof(buffer), "\r\nC%d-%d | [LD0%d]", current_column, current_row, ld_slot);
						}
						else
						{
							snprintf(buffer, sizeof(buffer), "\r\nC%d-%d | [LD%d]", current_column, current_row, ld_slot);
						}

						UART_SendStringRing(UART_CMDLINE, buffer);

						current_row++;
						if (current_row > 6)
						{
							current_row = 1;
							current_column++;
							if (current_column > 6)
							{
								current_column = 1;
							}
						}
						first_rest = 0;
					}
					SCH_TIM_Start(SCH_TIM_AUTO_LASER, laser_interval);
					first_time = 1;
				}
			}
			SCH_TIM_Start(SCH_TIM_AUTO_ADC, adc_interval);
		}
		else
		{
			if (SCH_TIM_HasCompleted(SCH_TIM_REST))
			{
				if (SCH_TIM_HasCompleted(SCH_TIM_AUTO_ADC))
				{
					read_adc_without_LF();
					data_times++;
					SCH_TIM_Start(SCH_TIM_AUTO_ADC, adc_interval);
				}
			}
		}
	}
}

void auto_get_temp()
{
	int16_t temp = 0;
	char buffer[60];

	if (temp == 0x7FFF)
	{
		UART_SendStringRing(UART_CMDLINE, "\r\nTemp BMP390 = [FAIL] \r\n");
	}
	else
	{
		snprintf(buffer, sizeof(buffer), "\r\nTemp BMP390 = [%i] \r\n", temp);
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
}

void auto_set_pd(uint8_t pd_slot)
{
	if (pd_slot > (photo_sw.num_of_chain * photo_sw.dev.channel_per_dev))
		return;
	adg1414_Chain_SetSwChannel(&photo_sw, pd_slot);
}

void auto_set_ls(uint8_t ls_slot)
{
	if (ls_slot > (int_laser.num_of_chain * int_laser.dev.channel_per_dev))
		return;
	adg1414_Chain_SetSwChannel(&int_laser, ls_slot);
}

void read_adc(void)
{
	uint32_t result = 0;
	float voltage = 0.0;
	const float vref = 3.0;
	int32_t voltage_int = 0, voltage_frac = 0;


	result = ADS8327_Read_Data_Polling(&photo_adc);
	voltage = (result / 65536.0f) * vref;

	voltage_int = (int32_t)voltage;
	voltage_frac = (int32_t)((voltage - voltage_int) * 1000);

	char buffer[60];
	snprintf(buffer, sizeof(buffer), "AutoADC: %ld (Vol: %ld.%03ld V)\r\n", result, voltage_int, voltage_frac);
	UART_SendStringRing(UART_CMDLINE, buffer);
}

void read_adc_without_LF(void)
{
	uint16_t result = 0;

	result = ADS8327_Read_Data_Polling(&photo_adc);

	char buffer[50];
	snprintf(buffer, sizeof(buffer), "  [T: %d]-[ADC: %d]", data_times, result);
	UART_SendStringRing(UART_CMDLINE, buffer);
}

void AutoRun_CreateTask(void)
{
	SCH_TASK_CreateTask(&s_auto_runTaskContext.taskHandle, &s_auto_runTaskContext.taskProperty);
}
