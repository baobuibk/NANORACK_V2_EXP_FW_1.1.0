/*
 * adg1414.c
 *
 *  Created on: Mar 13, 2025
 *      Author: HTSANG
 */


#include "adg1414.h"
#include "stm32f4xx_ll_spi.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//private function
///////////////////////////////////////////////////////////////////////////////////////////////
volatile uint8_t adg1414_timeout = 0;

static void adg1414_set_timeout(uint8_t mS)
{
	adg1414_timeout = mS;
}

static uint8_t adg1414_get_timeout(void)
{
	return adg1414_timeout;
}

static inline void adg1414_Select(adg1414_dev_t *dev)
{
    LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
}

static inline void adg1414_Deselect(adg1414_dev_t *dev)
{
    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}

static void adg1414_SPI_Transmit(adg1414_dev_t *dev, uint8_t *data, uint16_t size)
{
    for (int8_t i = size - 1; i >= 0; i--)
    {
        adg1414_set_timeout(10);
        while ((!LL_SPI_IsActiveFlag_TXE(dev->hspi)) && adg1414_get_timeout());
        LL_SPI_TransmitData8(dev->hspi, data[i]);
        adg1414_set_timeout(10);
        while (LL_SPI_IsActiveFlag_BSY(dev->hspi) && adg1414_get_timeout());   // Đợi BSY
    }
}

//static void adg1414_SPI_Receive(adg1414_dev_t *dev, uint8_t *data, uint16_t size)
//{
//    for (uint16_t i = 0; i < size; i++)
//    {
//		adg1414_set_timeout(10);
//		while ((!LL_SPI_IsActiveFlag_TXE(dev->hspi)) && adg1414_get_timeout());
//		LL_SPI_TransmitData8(dev->hspi, 0xFF);
//		adg1414_set_timeout(10);
//		while ((!LL_SPI_IsActiveFlag_RXNE(dev->hspi)) && adg1414_get_timeout());
//		data[i] = LL_SPI_ReceiveData8(dev->hspi);
//    }
//}

///////////////////////////////////////////////////////////////////////////////////////////////
//public function
///////////////////////////////////////////////////////////////////////////////////////////////
uint8_t adg1414_init(adg1414_dev_t *dev)
{
	adg1414_Deselect(dev);
	uint8_t TxData = 0x00;
	adg1414_Select(dev);
	adg1414_SPI_Transmit(dev, &TxData, 1);
	adg1414_Deselect(dev);
	return 0;
}

uint8_t adg1414_SetSwChannel(adg1414_dev_t *dev, uint8_t channel)
{
	if (channel > dev->channel_per_dev) return 1;
	uint8_t TxData = (1 << (channel - 1));
	adg1414_Select(dev);
	adg1414_SPI_Transmit(dev, &TxData, 1);
	adg1414_Deselect(dev);
	return 0;
}

uint8_t adg1414_ResetSwChannel(adg1414_dev_t *dev, uint8_t channel)
{
	if (channel > dev->channel_per_dev) return 1;
	uint8_t TxData = 0x00;
	adg1414_Select(dev);
	adg1414_SPI_Transmit(dev, &TxData, 1);
	adg1414_Deselect(dev);
	return 0;
}


/* Function to initial one chain shift */
uint8_t adg1414_Chain_init(adg1414_chain_t *chain)
{
	adg1414_Deselect(&chain->dev);
	uint8_t TxData[chain->num_of_chain];
	for (uint8_t i = 0; i < chain->num_of_chain; i++)
	{
		TxData[i] = 0x00;
	}
	adg1414_Select(&chain->dev);
	adg1414_SPI_Transmit(&chain->dev, TxData, chain->num_of_chain);
	adg1414_Deselect(&chain->dev);
	return 0;
}

/* Function to turn on a switch */
uint8_t adg1414_Chain_SetSwChannel(adg1414_chain_t *chain, uint8_t channel)
{
	if (channel > (chain->num_of_chain * chain->dev.channel_per_dev)) return 1;

	uint8_t TxData[chain->num_of_chain];
	for (uint8_t i = 0; i < chain->num_of_chain; i++)
	{
		TxData[i] = 0x00;
	}
	uint8_t chip_idx = (channel-1) / chain->dev.channel_per_dev;
	uint8_t bit_idx  = (channel-1) % chain->dev.channel_per_dev;
	TxData[(uint8_t)chip_idx] = (1 << bit_idx) & 0x3F;
	adg1414_Select(&chain->dev);
	adg1414_SPI_Transmit(&chain->dev, TxData, chain->num_of_chain);
	adg1414_Deselect(&chain->dev);
	return 0;
}
/* Function to turn off a switch */
uint8_t adg1414_Chain_ResetSwChannel(adg1414_chain_t *chain, uint8_t channel)
{
	if (channel > (chain->num_of_chain * chain->dev.channel_per_dev)) return 1;

	uint8_t TxData[chain->num_of_chain];
	for (uint8_t i; i < chain->num_of_chain; i++)
	{
		TxData[i] = 0x00;
	}
	uint8_t chip_idx = (channel-1) / chain->dev.channel_per_dev;
	uint8_t bit_idx  = (channel-1) % chain->dev.channel_per_dev;
	TxData[(uint8_t)chip_idx] &= ~(1 << bit_idx);
	adg1414_Select(&chain->dev);
	adg1414_SPI_Transmit(&chain->dev, TxData, chain->num_of_chain);
	adg1414_Deselect(&chain->dev);
	return 0;
}
/* Function to turn off all switches */
uint8_t adg1414_Chain_ResetSwAll(adg1414_chain_t *chain)
{
	uint8_t TxData[chain->num_of_chain];
	for (uint8_t i = 0; i < (chain->num_of_chain); i++)
	{
		TxData[i] = 0x00;
	}
	adg1414_Select(&chain->dev);
	adg1414_SPI_Transmit(&chain->dev, TxData, chain->num_of_chain);
	adg1414_Deselect(&chain->dev);
	return 0;
}
