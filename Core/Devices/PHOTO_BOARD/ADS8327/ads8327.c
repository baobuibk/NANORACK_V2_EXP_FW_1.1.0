/*
 * ads8327.c
 *
 *  Created on: Mar 2, 2025
 *      Author: DELL
 */
#include "ads8327.h"

static uint16_t CFR_user_default = 0x0EFD;		//0000 1110 1111 1101


static void ADS8327_Transmit16(ADS8327_Device_t *dev, uint16_t data)
{
	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));

	LL_SPI_TransmitData8(dev->spi, (uint8_t)(data>>8));
	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));  // Đợi TXE
	while (LL_SPI_IsActiveFlag_BSY(dev->spi));   // Đợi BSY

	LL_SPI_TransmitData8(dev->spi, (uint8_t)data);
	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));  // Đợi TXE
	while (LL_SPI_IsActiveFlag_BSY(dev->spi));   // Đợi BSY

}


void ADS8327_Wake_Up(ADS8327_Device_t *dev)
{
	dev->CMD = ADS8327_CMD_WAKE_UP;
    LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

    ADS8327_Transmit16(dev, dev->CMD);

    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}

void ADS8327_Write_CFR(ADS8327_Device_t *dev, uint16_t CFR)
{
	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

	dev->CMD = ADS8327_CMD_WRITE_CONFIG;
	dev->CFR_value = CFR;
	uint16_t temp = (dev->CMD & 0xF000) | (dev->CFR_value & 0x0FFF);

	ADS8327_Transmit16(dev, temp);

	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}


void ADS8327_Default_CFR(ADS8327_Device_t *dev, CFR_default_t CFR_default)
{
	if (CFR_default == USER_DEFAULT)
	{
		ADS8327_Write_CFR(dev, CFR_user_default);
	}

	else if (CFR_default == FACTORY_DEFAULT)
	{
		LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
		dev->CMD = ADS8327_CMD_DEFAULT_MODE;
		dev->CFR_value = ADS8327_FACTORY_CFR_DEFAULT;
		ADS8327_Transmit16(dev, dev->CMD);
		LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
	}
}


uint16_t ADS8327_Read_Data_Polling(ADS8327_Device_t *dev)
{
	uint16_t data;
	LL_GPIO_ResetOutputPin(dev->convst_port, dev->convst_pin);

//	while(LL_GPIO_IsInputPinSet(dev->EOC_port, dev->EOC_pin));
//	while(!LL_GPIO_IsInputPinSet(dev->EOC_port, dev->EOC_pin));
//
//	LL_GPIO_SetOutputPin(dev->convst_port, dev->convst_pin);
//
//	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
//
//	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
//	LL_SPI_TransmitData8(dev->spi, 0);
//	while (LL_SPI_IsActiveFlag_BSY(dev->spi));
//
//	while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
//	data = LL_SPI_ReceiveData8(dev->spi) << 8;
//	while (LL_SPI_IsActiveFlag_BSY(dev->spi));
//
//	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
//	LL_SPI_TransmitData8(dev->spi, 0);
//	while (LL_SPI_IsActiveFlag_BSY(dev->spi));
//
//	while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
//	data |= LL_SPI_ReceiveData8(dev->spi);
//	//ADS8327_Transmit16(dev, dev->CMD);
//	//dev->ADC_val = ADS8327_Receive16(dev);
//	while (LL_SPI_IsActiveFlag_BSY(dev->spi));
//	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
	dev->ADC_val = data;
	return dev->ADC_val;
}


void ADS8327_Device_Init(	ADS8327_Device_t *dev,
							SPI_TypeDef *spi,
							GPIO_TypeDef *cs_port,
							uint32_t cs_pin,
							GPIO_TypeDef *convst_port,
							uint32_t convst_pin,
							GPIO_TypeDef *EOC_port,
							uint32_t EOC_pin)
{

	dev->spi = spi;
	dev->cs_port = cs_port;
	dev->cs_pin = cs_pin;
	dev->convst_port = convst_port;
	dev->convst_pin = convst_pin;
	dev->EOC_port = EOC_port;
	dev->EOC_pin = EOC_pin;

//	LL_SPI_Enable(dev->spi);
	while (!LL_SPI_IsEnabled(dev->spi));

	ADS8327_Default_CFR(dev, USER_DEFAULT);
	ADS8327_Wake_Up(dev);
}







