/*
 * cli_command.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#include "cli_command.h"
#include "scheduler.h"
#include "cmdline.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct _CLI_Command_TaskContextTypedef_ {
	SCH_TASK_HANDLE taskHandle;
	SCH_TaskPropertyTypedef taskProperty;
} CLI_Command_TaskContextTypedef;

/* Private function ----------------------------------------------------------*/
static void CLI_Command_Task_Update(void);


static CLI_Command_TaskContextTypedef s_CLI_CommandTaskContext = {
		SCH_INVALID_TASK_HANDLE, 	// Will be updated by Schedular
		{ SCH_TASK_SYNC,        	// taskType;
		SCH_TASK_PRIO_0,         	// taskPriority;
		10,                      	// taskPeriodInMS;
		CLI_Command_Task_Update, 	// taskFunction;
		9 }
};

/*************************************************
 *                Command Define                 *
 *************************************************/
static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_Reset(EmbeddedCli *cli, char *args, void *context);
/*************************************************
 *                 Command  Array                *
 *************************************************/
// Guide: Command bindings are declared in the following order:
// { category, name, help, tokenizeArgs, context, binding }
// - category: Command group; set to NULL if grouping is not needed.
// - name: Command name (required)
// - help: Help string describing the command (required)
// - tokenizeArgs: Set to true to automatically split arguments when the command is called.
// - context: Pointer to a command-specific context; can be NULL.
// - binding: Callback function that handles the command.

static const CliCommandBinding cliStaticBindings_internal[] = {
    { "Ultis",		 	"help",        	"Print list of commands [Firmware: 1]",             	false,  NULL, CMD_Help,			 },
    { "Ultis",			"cls",         	"Clears the console",                               	false,  NULL, CMD_ClearCLI,  	 },
    { NULL,        		"reset",       	"Reset MCU: reset",                                 	false, 	NULL, CMD_Reset,     	 },
};

/*************************************************
 *             Command List Function             *
 *************************************************/
void CLI_Command_Init(USART_TypeDef *handle_uart) {
	SystemCLI_Init();
	UART_SendStringRing(handle_uart, "CLI_HEHE \r\n");
}

void CLI_Command_CreateTask(void) {
	SCH_TASK_CreateTask(&s_CLI_CommandTaskContext.taskHandle, &s_CLI_CommandTaskContext.taskProperty);
}

static void CLI_Command_Task_Update(void) {
	char rxData;
	if (IsDataAvailable(USART6)) {
		rxData = UART_ReadRing(USART6);
		embeddedCliReceiveChar(getUartCm4CliPointer(), (char)rxData);
		embeddedCliProcess(getUartCm4CliPointer());
	}
}
static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "\33[2J");
    embeddedCliPrint(cli, buffer);
}

static void CMD_Reset(EmbeddedCli *cli, char *args, void *context) {
	NVIC_SystemReset();
    embeddedCliPrint(cli, "");
}

/*************************************************
 *                  End CMD List                 *
 *************************************************/

/*************************************************
 *                Getter - Helper                *
 *************************************************/
const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof(cliStaticBindings_internal) / sizeof(cliStaticBindings_internal[0]);
}
