/*
 * i2c_slave.c
 *
 *  Created on: Oct 17, 2024
 *      Author: CAO HIEU
 */

#include "stm32f4xx_ll_i2c.h"
#include "i2c_slave.h"
#include <string.h>
#include "register.h"

#include "uart.h"
#include "stdio.h"
#include "command.h"
#define UART_CMDLINE USART1
uint8_t external_memory[256];
/*Make sure ReInit-Address is right*/

extern reg_t g_registers[REGISTER_COUNT];

volatile i2c_t I2C_slave_obj;

static void inline __attribute__((always_inline)) delayus(unsigned us_mul_5)
{
    uint32_t ticks = SYSTICKPERUS * us_mul_5;
    uint32_t start_tick = SysTick->VAL;

    while ((SysTick->VAL - start_tick) < ticks);
}

void I2C_ReInit(void)
{
    LL_I2C_Disable(I2C_slave_obj.I2Cx);
    LL_I2C_DeInit(I2C_slave_obj.I2Cx);

    LL_I2C_EnableClockStretching(I2C_slave_obj.I2Cx);
    LL_I2C_DisableGeneralCall(I2C_slave_obj.I2Cx);
    LL_I2C_DisableOwnAddress2(I2C_slave_obj.I2Cx);

    LL_I2C_InitTypeDef I2C_InitStruct = {0};
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.ClockSpeed = 100000;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
    I2C_InitStruct.OwnAddress1 = 36;  // 0x12
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C_slave_obj.I2Cx, &I2C_InitStruct);

    LL_I2C_EnableIT_EVT(I2C_slave_obj.I2Cx);
    LL_I2C_EnableIT_ERR(I2C_slave_obj.I2Cx);
    LL_I2C_Enable(I2C_slave_obj.I2Cx);
}


void i2c_slave_check_timeout(void){
    static int rx_busy_counter = 0;

    if (LL_I2C_IsActiveFlag_BUSY(I2C_slave_obj.I2Cx)){
        rx_busy_counter++;
    }
    else{
        rx_busy_counter = 0;
    }

    if (rx_busy_counter > I2C_RX_BUSY_CNTR){
        // Reset peripheral I2C
        if (I2C_slave_obj.I2Cx == I2C2){
            LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C2);
            LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C2);
        } else if (I2C_slave_obj.I2Cx == I2C3){
            LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C3);
            LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C3);
        }

        I2C_ReInit();
        i2c_slave_init(I2C2);

        rx_busy_counter = 0;
    }
}

void i2c_slave_clear(void) {
    I2C_slave_obj.reg_address = 0;
    I2C_slave_obj.curr_idx = NONE;
    I2C_slave_obj.reg_addr_rcvd = 0;
    I2C_slave_obj.ready_to_answer = 0;
    I2C_slave_obj.ready_to_write = 0;
}

int i2c_slave_init(I2C_TypeDef *I2Cx) {
    I2C_slave_obj.I2Cx = I2Cx;
    I2C_Slave_Status = I2C_Status_OK;
    i2c_slave_clear();
    memset(external_memory, 0, sizeof(external_memory));

    return 0;
}

//static uint8_t data_index = 0;

void I2C_event_IRQ(void)
{
	I2C_Slave_Status = I2C_Status_BUSY;
    if (LL_I2C_IsActiveFlag_ADDR(I2C_slave_obj.I2Cx))
    {
        LL_I2C_ClearFlag_ADDR(I2C_slave_obj.I2Cx);
        if (LL_I2C_GetTransferDirection(I2C_slave_obj.I2Cx) == LL_I2C_DIRECTION_WRITE)
        {
            I2C_slave_obj.reg_addr_rcvd = 0;
            LL_I2C_EnableIT_BUF(I2C_slave_obj.I2Cx);
        }
        else
        {
//            I2C_slave_obj.curr_idx = reg_get_index(I2C_slave_obj.reg_address);
//            data_index = 0;
//	          UART_SendStringRing(UART_CMDLINE, "FREAL\r\n");
            LL_I2C_EnableIT_BUF(I2C_slave_obj.I2Cx);
        }
    }

    // Data Register Empty (Trans)
    if (LL_I2C_IsActiveFlag_TXE(I2C_slave_obj.I2Cx))
    {
        char buffer[30];
        uint8_t data_to_send = 0x00;
        snprintf(buffer, sizeof(buffer), "\r\nI2C Before Index:[%d]\r\n", I2C_slave_obj.curr_idx);
        UART_SendStringRing(UART_CMDLINE, buffer);
		snprintf(buffer, sizeof(buffer), "\r\nI2C Index:[%d]\r\n", I2C_slave_obj.curr_idx);
		UART_SendStringRing(UART_CMDLINE,buffer);
		data_to_send = g_registers[I2C_slave_obj.curr_idx].value;
		snprintf(buffer, sizeof(buffer), "\r\nI2C GetDataIndex:[%d]\r\n", data_to_send);
		UART_SendStringRing(UART_CMDLINE,buffer);
        snprintf(buffer, sizeof(buffer), "\r\nI2C Response:[%d]\r\n", data_to_send);
        UART_SendStringRing(UART_CMDLINE,buffer);
        LL_I2C_TransmitData8(I2C_slave_obj.I2Cx, data_to_send);
        LL_I2C_DisableIT_BUF(I2C_slave_obj.I2Cx);
    }
    // Data Register Not Empty (Recv)
    if (LL_I2C_IsActiveFlag_RXNE(I2C_slave_obj.I2Cx)) {
        uint8_t received = LL_I2C_ReceiveData8(I2C_slave_obj.I2Cx);
        if (!I2C_slave_obj.reg_addr_rcvd)
        {
            I2C_slave_obj.reg_address = received;
            char buffer[50];
            snprintf(buffer, sizeof(buffer), "Address:[%d]\r\n", received);
            UART_SendStringRing(UART_CMDLINE, buffer);
            I2C_slave_obj.reg_addr_rcvd = 1;
            snprintf(buffer, sizeof(buffer), "GET INDEX:[%d]\r\n", I2C_slave_obj.curr_idx);
            UART_SendStringRing(UART_CMDLINE, buffer);
        }
        else
        {
            if (g_registers[I2C_slave_obj.curr_idx].access == FULL_ACCESS)
            {
                 g_registers[I2C_slave_obj.curr_idx].value = received;
            }
            I2C_slave_obj.reg_addr_rcvd = 0;
            LL_I2C_DisableIT_BUF(I2C_slave_obj.I2Cx);
        }
    }

    // STOP condition detected
    if (LL_I2C_IsActiveFlag_STOP(I2C_slave_obj.I2Cx)) {
        LL_I2C_ClearFlag_STOP(I2C_slave_obj.I2Cx);
        I2C_slave_obj.reg_addr_rcvd = 0;
//        I2C_slave_obj.curr_idx = NONE;
//        data_index = 0;
        LL_I2C_DisableIT_BUF(I2C_slave_obj.I2Cx);
        I2C_Slave_Status = I2C_Status_OK;
    }
}

void I2C_error_IRQ(void) {
    // Handle errors
	UART_SendStringRing(UART_CMDLINE,"I2C ERROR!!!\r\n");
	I2C_Slave_Status = I2C_Status_OK;
    if (LL_I2C_IsActiveFlag_BERR(I2C_slave_obj.I2Cx)) {

        LL_I2C_ClearFlag_BERR(I2C_slave_obj.I2Cx);
    }

    if (LL_I2C_IsActiveFlag_ARLO(I2C_slave_obj.I2Cx)) {
        LL_I2C_ClearFlag_ARLO(I2C_slave_obj.I2Cx);
    }

    if (LL_I2C_IsActiveFlag_AF(I2C_slave_obj.I2Cx)) {
        LL_I2C_ClearFlag_AF(I2C_slave_obj.I2Cx);
    }

    if (LL_I2C_IsActiveFlag_OVR(I2C_slave_obj.I2Cx)) {
        LL_I2C_ClearFlag_OVR(I2C_slave_obj.I2Cx);
    }
    I2C_ReInit();
    i2c_slave_init(I2C_slave_obj.I2Cx);
}
