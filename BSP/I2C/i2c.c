/*
 * i2c.c
 *
 *  Created on: Nov 29, 2024
 *      Author: SANG HUYNH
 */


#include "i2c.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_i2c.h"

#define I2C_TIMEOUT 5000 // Timeout value in milliseconds

/**
 * @brief  Sends data via I2C using the LL library.
 * @param  I2Cx: Pointer to the I2C instance (e.g., I2C1, I2C2, ...)
 * @param  addr: Address of the slave device (7-bit, no left shift required)
 * @param  reg: Address of the register of slave device
 * @param  data: Pointer to the data to be sent
 * @retval 0: Success, -1: Error
 */
uint8_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t data)
{
	uint32_t timeout = I2C_TIMEOUT;
    while(LL_I2C_IsActiveFlag_BUSY(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_TransmitData8(I2Cx, addr << 1);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_ClearFlag_ADDR(I2Cx);
    LL_I2C_TransmitData8(I2Cx, reg);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_TXE(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_TransmitData8(I2Cx, data);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_TXE(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_GenerateStopCondition(I2Cx);
    return 0;
}

/**
 * @brief  Sends data via I2C using the LL library.
 * @param  I2Cx: Pointer to the I2C instance (e.g., I2C1, I2C2, ...)
 * @param  addr: Address of the slave device (7-bit, no left shift required)
 * @param  reg: Address of the register of slave device
 * @param  pData: Pointer to the data to receive
 * @retval 0: Success, -1: Error
 */
uint8_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t* pData)
{
    uint8_t data = 0;
    uint32_t timeout = I2C_TIMEOUT;

    while(LL_I2C_IsActiveFlag_BUSY(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_TransmitData8(I2Cx, (addr << 1));
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_ClearFlag_ADDR(I2Cx);
    LL_I2C_TransmitData8(I2Cx, reg);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_TXE(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_TransmitData8(I2Cx, (addr << 1) | 1);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    LL_I2C_ClearFlag_ADDR(I2Cx);
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
    timeout = I2C_TIMEOUT;
    while(!LL_I2C_IsActiveFlag_RXNE(I2Cx) && timeout--);
    if (timeout == 0) return -1;

    data = LL_I2C_ReceiveData8(I2Cx);
    LL_I2C_GenerateStopCondition(I2Cx);
    *pData = data;
    return 0;
}
