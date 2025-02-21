/*
 * ntc.h
 *
 *  Created on: Dec 9, 2024
 *      Author: SANG HUYNH
 */

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <rafaellcoellho@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return
 * ----------------------------------------------------------------------------
 */

/*
	@file					ntc_10k.h
	@autor					Rafael Coelho <rafaellcoellho@gmail.com>
	@brief					Header with functions for ntc 10k.
	@details
			Sensor:
				Type: NTC 10k +/- 1% 3950
				Measuring range:-20 a 105
				B-const: 3380K - / + 1%

			Circuit:

			VCC
			 |
			 |
			| |
			| |	ntc 10k
			| |
			 |
			 |____AD
			 |
			 |
			| |
			| | R1
			| |
			 |
			 |
			GND
 */

#ifndef DEVICES_NTC_NTC_H_
#define DEVICES_NTC_NTC_H_

#include <stdint.h>

#define VCC							(float)3.3f
#define R1							(unsigned int)10000

extern uint16_t NTC_ADC_value[8];
extern int16_t NTC_Temperature[8];

void NTC_DMA_ADC_Init(void);
void NTC_get_temperature(int16_t* temp);

#endif /* DEVICES_NTC_NTC_H_ */
