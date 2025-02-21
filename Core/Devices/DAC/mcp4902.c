/*
 * mcp4902.c
 *
 *  Created on: Jan 24, 2025
 *      Author: CAO HIEU
 */

#include "MCP4902.h"

void DAC_Init(void)
{
//    // Initialize CS and LATCH pins as output
//    LL_GPIO_SetPinMode(DAC_CS_PORT, DAC_CS_PIN, LL_GPIO_MODE_OUTPUT);
//    LL_GPIO_SetPinMode(DAC_LATCH_PORT, DAC_LATCH_PIN, LL_GPIO_MODE_OUTPUT);
//
//    // Set CS and LATCH high by default
//    LL_GPIO_SetOutputPin(DAC_CS_PORT, DAC_CS_PIN);
//    LL_GPIO_SetOutputPin(DAC_LATCH_PORT, DAC_LATCH_PIN);

}

uint8_t v2dac(uint16_t voltage)
{
    return (voltage * 2.55f) / _VREF_DAC;
}

uint16_t dac2v(uint8_t dac)
{
    return (dac * _VREF_DAC) / 2.55f;
}

void DAC_Write(uint8_t channel, uint8_t DAC_Data)
{
    // Pull CS low
    LL_GPIO_ResetOutputPin(DAC_CS_PORT, DAC_CS_PIN);

    // Prepare the 16-bit data
    uint16_t temp = channel ? ((1 << MCP_AB_BIT) | (1 << MCP_GA_BIT) | (1 << MCP_SHDN_BIT) | (DAC_Data << 4))
                            : ((1 << MCP_GA_BIT) | (1 << MCP_SHDN_BIT) | (DAC_Data << 4));

    // Send high byte
    LL_SPI_TransmitData8(SPI1, (uint8_t)(temp >> 8));
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));

    // Send low byte
    LL_SPI_TransmitData8(SPI1, (uint8_t)temp);
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));

    // Wait until SPI is not busy
    while (LL_SPI_IsActiveFlag_BSY(SPI1));

    // Pull CS high
    LL_GPIO_SetOutputPin(DAC_CS_PORT, DAC_CS_PIN);

    // Toggle LATCH pin
    LL_GPIO_ResetOutputPin(DAC_LATCH_PORT, DAC_LATCH_PIN);
    for (volatile int i = 0; i < 100; i++); // Small delay
    LL_GPIO_SetOutputPin(DAC_LATCH_PORT, DAC_LATCH_PIN);
}

void DAC_Off(uint8_t channel)
{
    // Pull CS low
    LL_GPIO_ResetOutputPin(DAC_CS_PORT, DAC_CS_PIN);

    // Prepare the 16-bit data
    uint16_t temp = channel ? ((1 << MCP_AB_BIT) | (1 << MCP_GA_BIT)) : (1 << MCP_GA_BIT);

    // Send high byte
    LL_SPI_TransmitData8(SPI1, (uint8_t)(temp >> 8));
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));

    // Send low byte
    LL_SPI_TransmitData8(SPI1, (uint8_t)temp);
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));

    // Wait until SPI is not busy
    while (LL_SPI_IsActiveFlag_BSY(SPI1));

    // Pull CS high
    LL_GPIO_SetOutputPin(DAC_CS_PORT, DAC_CS_PIN);

    // Toggle LATCH pin
    LL_GPIO_ResetOutputPin(DAC_LATCH_PORT, DAC_LATCH_PIN);
    for (volatile int i = 0; i < 100; i++); // Small delay
    LL_GPIO_SetOutputPin(DAC_LATCH_PORT, DAC_LATCH_PIN);
}

