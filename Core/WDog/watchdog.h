/*
 * watchdog.h
 *
 *  Created on: Nov 20, 2024
 *      Author: SANG HUYNH
 */

#ifndef WATCHDOG_WATCHDOG_H_
#define WATCHDOG_WATCHDOG_H_

#include "scheduler.h"
#include "basetypedef.h"
#include "stm32f4xx_ll_gpio.h"
#include "main.h"


void Ex_Watchdog_Init(void);
void Ex_Watchdog_CreateTask(void);


#endif /* WATCHDOG_WATCHDOG_H_ */
