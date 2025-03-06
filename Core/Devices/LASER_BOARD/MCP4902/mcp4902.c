/*
 * mcp4902.c
 *
 *  Created on: Mar 2, 2025
 *      Author: DELL
 */

#include "mcp4902.h"



uint8_t v2dac(uint16_t voltage)
{
	return (voltage * 2.55f)/_VREF_DAC;
}

uint16_t dac2v(uint8_t dac)
{
	return (dac*_VREF_DAC)/2.55f;
}

/* Hàm gửi dữ liệu qua SPI */
static void MCP4902_Write(MCP4902_Device_t *dev)
{
	uint16_t temp;

    while (!LL_SPI_IsActiveFlag_TXE(dev->spi));


    for (int i = 0; i < MCP4902_NUM_CHANNEL; i++)
    {
    	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
    	temp = i ? ((1<<MCP4902_AB_BIT)|(1<<MCP4902_GA_BIT)|(1<<MCP4902_SHDN_BIT)|(dev->dac_channel[i]<<4)):
				   ((1<<MCP4902_GA_BIT)|(1<<MCP4902_SHDN_BIT)|(dev->dac_channel[i]<<4));

        LL_SPI_TransmitData8(dev->spi, (uint8_t)(temp>>8));
        while (!LL_SPI_IsActiveFlag_TXE(dev->spi));  // Đợi TXE
        while (LL_SPI_IsActiveFlag_BSY(dev->spi));   // Đợi BSY

        LL_SPI_TransmitData8(dev->spi, (uint8_t)temp);
		while (!LL_SPI_IsActiveFlag_TXE(dev->spi));  // Đợi TXE
		while (LL_SPI_IsActiveFlag_BSY(dev->spi));   // Đợi BSY

        LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);

		LL_GPIO_ResetOutputPin(dev->latch_port, dev->latch_pin);
		__NOP();
		LL_GPIO_SetOutputPin(dev->latch_port, dev->latch_pin);
    }


}

void MCP4902_Shutdown(MCP4902_Device_t *dev, uint8_t channel)
{
	uint16_t temp;

    while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
    LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

	temp = channel ? ((1<<MCP4902_AB_BIT)|(1<<MCP4902_GA_BIT)) : (1<<MCP4902_GA_BIT);
	LL_SPI_TransmitData16(dev->spi, temp);

	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));  // Đợi TXE
	while (LL_SPI_IsActiveFlag_BSY(dev->spi));   // Đợi BSY

    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);

    LL_GPIO_ResetOutputPin(dev->latch_port, dev->latch_pin);
	__NOP();
	LL_GPIO_SetOutputPin(dev->latch_port, dev->latch_pin);
}

void MCP4902_Set_DAC(MCP4902_Device_t *dev, uint8_t channel, uint8_t DAC_val)
{
	dev->dac_channel[channel] = DAC_val;
	MCP4902_Write(dev);
}

void MCP4902_Set_Voltage(MCP4902_Device_t *dev, uint8_t channel, uint16_t voltage)
{
	dev->dac_channel[channel] = v2dac(voltage);
	MCP4902_Write(dev);
}

void MCP4902_Device_Init(	MCP4902_Device_t *dev,
							SPI_TypeDef *spi,
							GPIO_TypeDef *cs_port,
							uint32_t cs_pin,
							GPIO_TypeDef *latch_port,
							uint32_t latch_pin)
{
	dev->spi = spi;
	dev->cs_port = cs_port;
	dev->cs_pin = cs_pin;
	dev->latch_port = latch_port;
	dev->latch_pin = latch_pin;

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = dev->cs_pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(dev->cs_port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = dev->latch_pin;
	LL_GPIO_Init(dev->latch_port, &GPIO_InitStruct);

	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
	LL_GPIO_SetOutputPin(dev->latch_port, dev->latch_pin);

	for (int i = 0; i < MCP4902_NUM_CHANNEL; i++)
	{
		dev->dac_channel[i] = 0;
	}

	LL_SPI_Enable(dev->spi);
	while (!LL_SPI_IsEnabled(dev->spi));
	MCP4902_Write(dev);
}

