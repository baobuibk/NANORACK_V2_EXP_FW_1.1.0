/*
 * sensor_i2c.c
 *
 *  Created on: Dec 23, 2024
 *      Author: SANG HUYNH
 */

#include "sensor_i2c.h"
#include "scheduler.h"
#include "bmp390.h"
#include "command.h"

/* Private define ------------------------------------------------------------*/

/* Private function ----------------------------------------------------------*/
void sensor_i2c_update(void);

/* Private typedef -----------------------------------------------------------*/
typedef struct Sensor_TaskContextTypedef
{
	SCH_TASK_HANDLE               	taskHandle;
	SCH_TaskPropertyTypedef       	taskProperty;
	uint32_t                      	taskTick;
} Sensor_TaskContextTypedef;


/* Private variables ---------------------------------------------------------*/
static Sensor_TaskContextTypedef           s_task_context =
{
	SCH_INVALID_TASK_HANDLE,                // Will be updated by Schedular
	{
		SCH_TASK_SYNC,                      // taskType;
		SCH_TASK_PRIO_0,                    // taskPriority;
		20,                                 // taskPeriodInMS;
		sensor_i2c_update,                	// taskFunction;
		18									// taskTick
	},
};

void sensor_i2c_update(void)
{
	bmp390_temp_press_update();
	return;
}
void  sensor_i2c_init(void)
{
	BMP390_init();
}
void  sensor_i2c_create_task(void)
{
	SCH_TASK_CreateTask(&s_task_context.taskHandle, &s_task_context.taskProperty);
}
