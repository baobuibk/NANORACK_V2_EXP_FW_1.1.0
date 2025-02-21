/*
 * auto_run.c
 *
 *  Created on: Jan 26, 2025
 *      Author: CAO HIEU
 */

#include "auto_run.h"
#include "stdio.h"
#include "scheduler.h"

#include "mcp4902.h"
#include "uart.h"
#include "main.h"
#include "ntc.h"

uint8_t data_times = 0;
#define UART_CMDLINE USART1
extern SPI_HandleTypeDef hspi2;

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
}

void auto_set_ls(uint8_t ls_slot)
{
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
}

void read_adc(void)
{
	uint8_t rxData[2] = {0};
	uint32_t result = 0;
	float voltage = 0.0;
	const float vref = 3.0;
	int32_t voltage_int = 0, voltage_frac = 0;

	LL_GPIO_ResetOutputPin(PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin);
	__asm__("NOP");
	LL_GPIO_SetOutputPin(PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin);

	LL_GPIO_ResetOutputPin(PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin);
	HAL_SPI_Receive(&hspi2, rxData, 2, 1000);
	LL_GPIO_SetOutputPin(PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin);

	result = ((uint32_t)rxData[0] << 8) | rxData[1];
	voltage = (result / 65536.0f) * vref;

	voltage_int = (int32_t)voltage;
	voltage_frac = (int32_t)((voltage - voltage_int) * 1000);

	char buffer[60];
	snprintf(buffer, sizeof(buffer), "AutoADC: %ld (Vol: %ld.%03ld V)\r\n", result, voltage_int, voltage_frac);
	UART_SendStringRing(UART_CMDLINE, buffer);
}

void read_adc_without_LF(void)
{
	uint8_t rxData[2] = {0};
	uint16_t result = 0;

	LL_GPIO_ResetOutputPin(PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin);
	__asm__("NOP");
	LL_GPIO_SetOutputPin(PHOTO_ADC_CONV_GPIO_Port, PHOTO_ADC_CONV_Pin);

	LL_GPIO_ResetOutputPin(PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin);
	HAL_SPI_Receive(&hspi2, rxData, 2, 1000);
	LL_GPIO_SetOutputPin(PHOTO_ADC_CS_GPIO_Port, PHOTO_ADC_CS_Pin);

	result = ((uint16_t)rxData[0] << 8) | rxData[1];

	char buffer[50];
	snprintf(buffer, sizeof(buffer), "  [T: %d]-[ADC: %d]", data_times, result);
	UART_SendStringRing(UART_CMDLINE, buffer);
}

void AutoRun_CreateTask(void)
{
	SCH_TASK_CreateTask(&s_auto_runTaskContext.taskHandle, &s_auto_runTaskContext.taskProperty);
}
