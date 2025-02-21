/*
 * ir_led.c
 *
 *  Created on: Dec 26, 2024
 *      Author: SANG HUYNH
 */

#include "ir_led.h"


static uint16_t ir_led_duty_current[4] = {0, 0, 0, 0};

void ir_led_set_duty(uint8_t channel, uint16_t duty_pwm)
{
    if (duty_pwm > 9999) duty_pwm = 9999;
    switch (channel)
    {
        case 0:
        	ir_led_duty_current[0] = duty_pwm;
            LL_TIM_OC_SetCompareCH1(TIM3, duty_pwm);
            break;
        case 1:
        	ir_led_duty_current[1] = duty_pwm;
        	LL_TIM_OC_SetCompareCH2(TIM3, duty_pwm);
            break;
        case 2:
        	ir_led_duty_current[2] = duty_pwm;
            LL_TIM_OC_SetCompareCH3(TIM3, duty_pwm);
            break;
        case 3:
        	ir_led_duty_current[3] = duty_pwm;
            LL_TIM_OC_SetCompareCH4(TIM3, duty_pwm);
            break;
        default:
            break;
    }
}

uint16_t ir_led_get_duty(uint8_t channel)
{
	return ir_led_duty_current[channel];
}
