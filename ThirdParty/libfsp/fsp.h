/*
 * fsp.h
 *
 * Copyright (C) 2017-2019, Federal University of Santa Catarina.
 *
 * This file is part of FloripaSat-FSP.
 *
 * FloripaSat-FSP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FloripaSat-FSP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FloripaSat-FSP. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \brief FloripaSat Protocol library definition.
 *
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 *
 * \version 0.2.0
 *
 * \date 06/11/2017
 *
 * \defgroup fsp fsp
 * \{
 */

#ifndef FSP_H_
#define FSP_H_

#include <stdint.h>

// Packet positions
#define FSP_PKT_POS_SOD                 0       /**< SOD byte position. */
#define FSP_PKT_POS_SRC_ADR             1       /**< Source address byte position. */
#define FSP_PKT_POS_DST_ADR             2       /**< Destination address byte position. */
#define FSP_PKT_POS_LEN                 3       /**< Length position. */
#define FSP_PKT_POS_TYPE                4       /**< Type position. */

// Start Of Data byte
#define FSP_PKT_SOD                     0xCA    /**< Start Of Data byte. */

// End of Frame byte
#define FSP_PKT_EOF                     0xEF    /**< End of Frame byte. */

#define FSP_PKT_ESC						0xBD
/*-----------DEFINE FOR TSOD[ESC SOD]/TEOF[ESC EOF]/TESC[ESC ESC]------------*/
// Transposed Start Of Data byte
#define FSP_PKT_TSOD                    0xDC    /**< Start Of Data byte. */
// Transposed End Of Frame
#define FSP_PKT_TEOF					0xDE
// Transposed Frame Escape
#define FSP_PKT_TESC 					0xDB

// Addresses
#define FSP_ADR_COPC	                1       /**< COPC module address. */
#define FSP_ADR_PMU                     2       /**< PMU module address. */
#define FSP_ADR_PDU                     3       /**< PDU module address. */
#define FSP_ADR_CAM	                    4       /**< CAM module address. */
#define FSP_ADR_IOU		                5       /**< IOU module address. */
#define FSP_ADR_EXP		                2       /**< IOU module address. */

// Types of packets
#define FSP_PKT_TYPE_DATA               1       /**< Data packet. */
#define FSP_PKT_TYPE_DATA_WITH_ACK      2       /**< Data packet with acknowledgement. */
#define FSP_PKT_TYPE_CMD                3       /**< Command packet. */
#define FSP_PKT_TYPE_CMD_WITH_ACK       4       /**< Command packet with acknowledgement. */
#define FSP_PKT_TYPE_ACK                5       /**< Acknowledgement packet. */
#define FSP_PKT_TYPE_NACK               6       /**< Not-acknowledgement packet. */
#define FSP_PKT_TYPE_CMD_W_DATA     	7
#define FSP_PKT_TYPE_CMD_W_DATA_ACK		8

// Commands
#define FSP_CMD_RESPONSE_DONE								0x00		/*response ascii*/
#define FSP_CMD_CODE_EXP_TEC_PW_ON							0x01
#define FSP_CMD_CODE_EXP_LED_PW_ON							0x02
#define FSP_CMD_CODE_EXP_HEATER_PW_ON						0x03
#define FSP_CMD_CODE_EXP_SET_TEMP							0x04
#define FSP_CMD_CODE_EXP_GET_TEMP_SETPOINT					0x05
#define FSP_CMD_CODE_EXP_GET_TEMP							0x06
#define FSP_CMD_CODE_EXP_SET_TEC_VOL						0x07
#define FSP_CMD_CODE_EXP_GET_TEC_VOL						0x08
#define FSP_CMD_CODE_EXP_SET_HEATER_DUTY					0x09
#define FSP_CMD_CODE_EXP_GET_HEATER_DUTY					0x0A
#define FSP_CMD_CODE_EXP_TEMP_CTRL							0x0B
#define FSP_CMD_CODE_EXP_TEMP_AUTO_CTRL						0x0C
#define FSP_CMD_CODE_EXP_SET_IR_DUTY						0x0D
#define FSP_CMD_CODE_EXP_GET_IR_DUTY						0x0E
#define FSP_CMD_CODE_EXP_GET_ALL							0xFF

// Ack answers
#define FSP_ACK_RF_MUTEX_FREE           1       /**< RF mutex is free. */
#define FSP_ACK_RF_MUTEX_BUSY           2       /**< RF mutex is busy. */

// Min. and Max. lengths
#define FSP_PKT_MIN_LENGTH              7       /**< Minimum packet length in bytes. */
#define FSP_PKT_MAX_LENGTH              67     /**< Maximum packet length in bytes. */
#define FSP_PAYLOAD_MAX_LENGTH          60     /**< Maximum packet payload length in bytes. */

// CRC16 initial value (or seed byte)
#define FSP_CRC16_INITIAL_VALUE         0       /**< CRC16 initial value. */

// Decode states
#define FSP_PKT_NOT_READY               0       /**< Packet not ready. */
#define FSP_PKT_READY                   1       /**< Packet ready. */
#define FSP_PKT_INVALID                 2       /**< Packet invalid. */
#define FSP_PKT_WRONG_ADR               3       /**< Packet with wrong address. */
#define FSP_PKT_ERROR                   4       /**< Packet with error. */
#define FSP_PKT_CRC_FAIL                5       /**< Packet with error. */
#define FSP_PKT_WRONG_LENGTH            6       /**< Packet with error. */

// Config.
#define FSP_PKT_WITH_ACK                1       /**< Packet with acknowledgement. */
#define FSP_PKT_WITHOUT_ACK             0       /**< Packet without acknowledgement. */

#define FSP_PKT_HEADER_LENGTH			4
#define FSP_PKT_CRC_LENGTH              2

/**
 * \brief FSP packet struct.
 */
typedef struct
{
    uint8_t sod;                                /**< Start of data. */
    uint8_t src_adr;                            /**< Source address. */
    uint8_t dst_adr;                            /**< Destination address. */
    uint8_t length;                             /**< Length of the packet payload. */
    uint8_t type;                               /**< Type of packet. */
    uint8_t payload[FSP_PAYLOAD_MAX_LENGTH];    /**< Payload of the packet. */
    uint16_t crc16;                             /**< CRC16-CCITT bytes. */
	uint8_t eof;
} fsp_packet_t;

/**
 * \brief The address of the module running this library.
 */
extern uint8_t fsp_my_adr;

/**
 * \brief Decode byte position (From the internal "state machine"),
 */
extern uint8_t fsp_decode_pos;

/**
 * \brief Initializes the FSP library.
 *
 * \param module_adr is the address of the module running this library.
 *
 * \return None
 */
void fsp_init(uint8_t module_adr);

/**
 * \brief Resets the FSP internal state machine (The position counter) for decoding.
 *
 * \return None
 */
void fsp_reset(void);

/**
 * \brief Generates a FSP data packet.
 *
 * \param data is the paylod of the desired data packet.
 * \param data_len is the length of the payload of the desired data packet.
 * \param dst_adr is the destination address.
 * \param ack is the acknowledge option (FSP_PKT_WITH_ACK or FSP_PKT_WITHOUT_ACK).
 * \param fsp is a pointer to a fsp_packet_t struct to store the new data packet.
 *
 * \return None
 */
void fsp_gen_data_pkt(uint8_t *data, uint8_t data_len, uint8_t dst_adr, uint8_t ack, fsp_packet_t *fsp);

/**
 * \brief Generates a FSP command packet.
 *
 * \param cmd is the command of the desired command packet.
 * \param dst_adr is the destination address.
 * \param ack is the acknowledge option (FSP_PKT_WITH_ACK or FSP_PKT_WITHOUT_ACK).
 * \param fsp is a pointer to a fsp_packet_t struct to store the new data packet.
 *
 * \return None
 */
void fsp_gen_cmd_pkt(uint8_t cmd, uint8_t dst_adr, uint8_t ack, fsp_packet_t *fsp);

/**
 * \brief Generates a FSP ack. packet.
 *
 * The ack. packet does not have a payload (The length field is zero).
 *
 * \param dst_adr is the destination address.
 * \param fsp is a pointer to a fsp_packet_t struct to store the new data packet.
 *
 * \return None
 */
void fsp_gen_ack_pkt(uint8_t dst_adr, fsp_packet_t *fsp);

/**
 * \brief Generates a FSP nack. packet.
 *
 * The nack. packet does not have a payload (The length field is zero).
 *
 * \param dst_adr is the destination address.
 * \param fsp is a pointer to a fsp_packet_t struct to store the new data packet.
 *
 * \return None
 */
void fsp_gen_nack_pkt(uint8_t dst_adr, fsp_packet_t *fsp);

/**
 * \brief Generates a generic FSP packet from a given data.
 *
 * \param payload is the payload of the desired packet.
 * \param payload_len is the length of the payload of the desired packet.
 * \param dst_adr is the destination address.
 * \param type is the type of the packet.
 *
 * \return None
 */
void fsp_gen_pkt(uint8_t *cmd, uint8_t *payload, uint8_t payload_len, uint8_t dst_adr, uint8_t type, fsp_packet_t *fsp);

/**
 * \brief Encodes a fsp_packet_t struct into a array of bytes.
 *
 * \param fsp is a pointer to a fsp_packet_t struct with all the packet data.
 * \param pkt is an array to store the packet.
 * \param pkt_len is a pointer to the packet array length.
 *
 * \return None
 */
void fsp_encode(fsp_packet_t *fsp, uint8_t *pkt, uint8_t *pkt_len);

/**
 * \brief Decodes a array with the FSP packet into a fsp_packet_t struct.
 *
 * \param byte is the incoming byte from a FSP packet.
 * \param fsp is a pointer to a fsp_packet_t struct to store the packet data.
 *
 * \return The state of the decoding process. It can be:
 * \parblock
 *              -\b FSP_PKT_NOT_READY when all the packet data were not received yet.
 *              -\b FSP_PKT_READY when the packet is ready to be used and the decoding process is over.
 *              -\b FSP_PKT_INVALID when an invalid packet was received.
 *              -\b FSP_PKT_WRONG_ADR when the destination address of the packet is not equal to host address.
 *              -\b FSP_PKT_ERROR when an error occurs during the decoding process.
 *              .
 * \endparblock
 */
uint8_t fsp_decode(uint8_t byte, fsp_packet_t *fsp);

void fsp_gen_cmd_w_data_pkt(uint8_t cmd, uint8_t *data, uint8_t data_len, uint8_t dst_adr, uint8_t ack, fsp_packet_t *fsp);
int frame_decode(uint8_t *buffer, uint8_t length, fsp_packet_t *pkt);
#endif // FSP_H_

//! \} End of fsp group
