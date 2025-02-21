/*
 * i2c_slave.h
 *
 *  Created on: Oct 17, 2024
 *      Author: CAO HIEU
 */

#ifndef SRC_I2C_SLAVE_H_
#define SRC_I2C_SLAVE_H_


#include "main.h"
#include "register.h"

#define NONE -1
#define ECHO 0
#include "stm32f4xx_ll_i2c.h"

#define SYSTICKCLOCK 80000000ULL
#define SYSTICKPERUS (SYSTICKCLOCK / 1000000UL)
#define BYTE_TIMEOUT_US   (SYSTICKPERUS * 3 * 10)

#define I2C_RX_BUSY_CNTR	150

typedef struct i2c_s {
    volatile int curr_idx;
    volatile uint8_t reg_addr_rcvd;
    volatile uint8_t reg_address;
    volatile uint8_t ready_to_answer;
    volatile uint8_t ready_to_write;
    I2C_TypeDef *I2Cx;
} i2c_t;

int i2c_slave_init();
void i2c_slave_check_timeout(void);
void I2C_event_IRQ(void);
void I2C_error_IRQ(void);

#endif /* SRC_I2C_SLAVE_H_ */
