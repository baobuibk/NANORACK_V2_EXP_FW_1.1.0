/*
 * register.c
 *
 *  Created on: Oct 21, 2024
 *      Author: CAO HIEU
 */

#include "register.h"
#include "stdio.h"

volatile I2C_Slave_StatusTypeDef I2C_Slave_Status = I2C_Status_OK;

reg_t g_registers[REGISTER_COUNT];

void Register_Init(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        g_registers[i].reg_addr = i;
        g_registers[i].access = READ_ONLY;
        g_registers[i].value = 0xFF;
    }
    for (uint8_t i = 8; i <= 126; i++)
    {
        g_registers[i].reg_addr = i;
        g_registers[i].access = FULL_ACCESS;
        g_registers[i].value = 0;
    }
    g_registers[127].reg_addr = 127;
    g_registers[127].access = READ_ONLY;
    g_registers[127].value = 0x77;

    for (uint8_t i = 128; i < REGISTER_COUNT; i++)
    {
        g_registers[i].reg_addr = i;
        g_registers[i].access = READ_ONLY;
        g_registers[i].value = 0xFF;
    }
}


Peripheral_StatusTypeDef reg_read(uint8_t reg_address, uint8_t *value) {
    if (reg_address >= REGISTER_COUNT) return Status_ERROR;
    if (g_registers[reg_address].access == RESERVED) return Status_ERROR;
    *value = g_registers[reg_address].value;
    return Status_OK;
}

Peripheral_StatusTypeDef reg_write(uint8_t reg_address, uint8_t value) {
    if (reg_address >= REGISTER_COUNT) return Status_ERROR;
    if (g_registers[reg_address].access != FULL_ACCESS) return Status_ERROR;
    g_registers[reg_address].value = value;
    return Status_OK;
}

Peripheral_StatusTypeDef reg_read_multi(uint8_t start_addr, uint8_t length, uint8_t *values) {
    if (start_addr + length > REGISTER_COUNT) return Status_ERROR;
    for (uint8_t i = 0; i < length; i++) {
        if (g_registers[start_addr + i].access == RESERVED) return Status_ERROR;
        values[i] = g_registers[start_addr + i].value;
    }
    return Status_OK;
}

Peripheral_StatusTypeDef reg_write_multi(uint8_t start_addr, uint8_t length, const uint8_t *values) {
    if (start_addr + length > REGISTER_COUNT) return Status_ERROR;
    for (uint8_t i = 0; i < length; i++) {
        if (g_registers[start_addr + i].access != FULL_ACCESS) return Status_ERROR;
        g_registers[start_addr + i].value = values[i];
    }
    return Status_OK;
}


