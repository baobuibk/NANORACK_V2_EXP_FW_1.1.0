/*
 * ir_led.h
 *
 *  Created on: Dec 26, 2024
 *      Author: SANG HUYNH
 */

#ifndef DEVICES_IR_LED_IR_LED_H_
#define DEVICES_IR_LED_IR_LED_H_

#include "main.h"

void ir_led_set_duty(uint8_t channel, uint16_t duty_pwm);
uint16_t ir_led_get_duty(uint8_t channel);

#endif /* DEVICES_IR_LED_IR_LED_H_ */
