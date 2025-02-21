/*
 * i2c.h
 *
 *  Created on: Nov 29, 2024
 *      Author: SANG HUYNH
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

#include "stm32f4xx_ll_i2c.h"

uint8_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t reg, uint8_t data);
uint8_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t *pData);

#endif /* I2C_I2C_H_ */
