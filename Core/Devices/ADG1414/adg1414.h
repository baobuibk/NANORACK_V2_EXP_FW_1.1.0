/*
 * adg1414.h
 *
 *  Created on: Mar 13, 2025
 *      Author: HTSANG
 */

#ifndef DEVICES_ADG1414_ADG1414_H_
#define DEVICES_ADG1414_ADG1414_H_

#include "main.h"

/* Define the number of chips and switches */
#define ADG1414_CHAIN_NUM_CHIPS_MAX    16   // Maximum chips in daisy chain
#define ADG1414_DATA_BYTES             8    // 8 bytes (1 byte per chip, only 6 lower bits used)

typedef struct adg1414_dev {
	SPI_TypeDef *hspi;
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
	uint8_t channel_per_dev;
} adg1414_dev_t;

typedef struct _adg1414_chain_t {
	adg1414_dev_t dev;
	uint8_t num_of_chain;
} adg1414_chain_t;

extern volatile uint8_t adg1414_timeout;

uint8_t adg1414_init(adg1414_dev_t *dev);
uint8_t adg1414_SetSwChannel(adg1414_dev_t *dev, uint8_t channel);
uint8_t adg1414_ResetSwChannel(adg1414_dev_t *dev, uint8_t channel);

/* Function to initial one chain shift */
uint8_t adg1414_Chain_init(adg1414_chain_t *chain);
/* Function to turn on a switch */
uint8_t adg1414_Chain_SetSwChannel(adg1414_chain_t *chain, uint8_t channel);
/* Function to turn off a switch */
uint8_t adg1414_Chain_ResetSwChannel(adg1414_chain_t *chain, uint8_t channel);
/* Function to turn off all switches */
uint8_t adg1414_Chain_ResetSwAll(adg1414_chain_t *chain);


#endif /* DEVICES_ADG1414_ADG1414_H_ */
