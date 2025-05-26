/*
 * min_command.h
 *
 *  Created on: May 5, 2025
 *      Author: HTSANG
 */

#ifndef MIN_PROTO_MIN_APP_MIN_COMMAND_H_
#define MIN_PROTO_MIN_APP_MIN_COMMAND_H_

#include <stdint.h>
#include "min_app.h"

// =================================================================
// Min IDs
// =================================================================
#define MIN_SET		0x20
#define MIN_GET		0x21

// =================================================================
// Status data
// =================================================================
#define ON			0x01
#define OFF			0x00

// =================================================================
// Command IDs (Maximum ID: 63)
// =================================================================
#define DEV_STATUS_CMD			0x00  ///< Trạng thái của board
#define NTC_TEMP_CMD 			0x01  ///< Đọc nhiệt độ từ NTC
#define PWR_5V_CMD   			0x02  ///< Bật/tắt hoặc đọc trạng thái nguồn 5V
#define TEC_STATUS_CMD 			0x03  ///< Khởi tạo TEC hoặc lấy trạng thái TEC
#define TEC_VOLT_CMD 			0x04  ///< Cài đặt hoặc đọc điện áp TEC
#define TEC_DIR_CMD  			0x05  ///< Cài đặt hoặc đọc chiều TEC
#define HTR_DUTY_CMD 			0x06  ///< Cài đặt hoặc đọc duty của heater
#define REF_TEMP_CMD 			0x07  ///< Cài đặt hoặc đọc nhiệt độ tham chiếu
#define REF_NTC_CMD  			0x08  ///< Cài đặt hoặc đọc mã NTC tham chiếu
#define AUTO_TEC_CMD 			0x09  ///< Cài đặt hoặc đọc chế độ auto TEC
#define AUTO_HTR_CMD 			0x0A  ///< Cài đặt hoặc đọc chế độ auto heater
#define AUTO_TEMP_CMD			0x0B  ///< Cài đặt hoặc đọc nhiệt độ tự động
#define LSM_SENS_CMD 			0x0C  ///< Đọc cảm biến LSM
#define H3L_SENS_CMD 			0x0D  ///< Đọc cảm biến H3L
#define BME_SENS_CMD 			0x0E  ///< Đọc cảm biến BME


#define COLLECT_DATA_CMD	  0x12
#define PRE_CHUNK_CMD		  0x13
#define PRE_CHUNK_ACK		  0x14
#define PRE_DATA_CMD		  0x15
#define PRE_DATA_ACK		  0x16
#define SAMPLERATE_SET_CMD 	  0x17
#define SAMPLERATE_GET_CMD    0x18
#define SAMPLERATE_GET_ACK	  0x19
#define COLLECT_PACKAGE_CMD   0x1A
#define COLLECT_PACKAGE_ACK   0x1B

#define OVER				  0x3B
#define ACK					  0x3C
#define WRONG				  0x3D
#define FAIL				  0x3E
#define	DONE				  0x3F

/**
 * @brief Command handler function type.
 * @param ctx Pointer to the MIN context.
 * @param payload Pointer to the received payload data.
 * @param len Length of the payload.
 */
typedef void (*MIN_CommandHandler)(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len);

/**
 * @brief Structure to map command IDs to their handlers.
 */
typedef struct MIN_Command {
    uint8_t id;                    ///< Command ID
    MIN_CommandHandler handler;    ///< Handler function for the command
} MIN_Command_t;

/**
 * @brief Gets the command table.
 * @return Pointer to the command table.
 */
const MIN_Command_t *MIN_GetCommandTable(void);

/**
 * @brief Gets the size of the command table.
 * @return Number of entries in the command table.
 */
int MIN_GetCommandTableSize(void);


void MIN_Handler_NTC_TEMP_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len);

#endif /* MIN_PROTO_MIN_APP_MIN_COMMAND_H_ */
