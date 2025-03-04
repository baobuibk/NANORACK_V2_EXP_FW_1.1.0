/*
 * delay.c
 *
 *  Created on: Nov 25, 2024
 *      Author: SANG HUYNH
 */


#include "delay.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_cortex.h"

void delay_us(uint8_t micro_seconds)
{
    uint32_t ticks = micro_seconds * (SystemCoreClock / 1000000);
    while (ticks > 0)
    {
        if (LL_SYSTICK_IsActiveCounterFlag())
        {
            ticks--;
        }
    }
}

