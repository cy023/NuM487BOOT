/**
 * @file bootprotocol.h
 * @author cy023
 * @brief Packet Handler
 * @date 2023.03.20
 *
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                     HEADER                    |    COMMAND    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |            LENGTH             |                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 *  |                                                               |
 *  +                                                               +
 *  |                     DATA (variable length)                    |
 *  +                                               +-+-+-+-+-+-+-+-+
 *  |                                               |    CHECKSUM   |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * HEADER   : 0xA5A5A5
 * COMMAND  : See details in the following macros
 * LENGTH   : Dara length
 * DATA     : Variable length
 * CHECKSUM : sum(data) % 255
 *
 */

#ifndef BOOTPROTOCOL_H
#define BOOTPROTOCOL_H

/* bootprotocol common macros */
#define HEADER 0xA5

#define SUCCESSED 0
#define FAILED    1

#define ACK  0
#define NACK 1

/* bootprotocol commands */
#define CMD_CHK_PROTOCOL        0x01
#define CMD_CHK_DEVICE          0x02
#define CMD_PROG_END            0x03
#define CMD_PROG_EXT_FLASH_BOOT 0x04

#define CMD_FLASH_SET_PGSZ     0x10
#define CMD_FLASH_GET_PGSZ     0x11
#define CMD_FLASH_WRITE        0x12
#define CMD_FLASH_READ         0x13
#define CMD_FLASH_VERIFY       0x14
#define CMD_FLASH_EARSE_SECTOR 0x15
#define CMD_FLASH_EARSE_ALL    0x16

#define CMD_EEPROM_SET_PGSZ     0x20
#define CMD_EEPROM_GET_PGSZ     0x21
#define CMD_EEPROM_WRITE        0x22
#define CMD_EEPROM_READ         0x23
#define CMD_EEPROM_VERIFY       0x24
#define CMD_EEPROM_EARSE_SECTOR 0x25
#define CMD_EEPROM_EARSE_ALL    0x26

#define CMD_EXT_FLASH_FOPEN        0x30
#define CMD_EXT_FLASH_FCLOSE       0x31
#define CMD_EXT_FLASH_WRITE        0x32
#define CMD_EXT_FLASH_READ         0x33
#define CMD_EXT_FLASH_VERIFY       0x34
#define CMD_EXT_FLASH_EARSE_SECTOR 0x35
#define CMD_EXT_FLASH_HEX_DEL      0x36

#include <stdint.h>

/**
 * @brief packet format struct
 * @param cmd    Packet command.
 * @param data   Data buffer pointer.
 * @param length Data length.
 */
typedef struct __bootloader_packet {
    uint8_t cmd;
    uint16_t length;
    uint8_t *data;
} bl_packet_t;

/**
 * @brief Receive a packet from the communication channel.
 * @param packet
 * @return uint8_t
 *      0: successed.
 *      1: failed.
 */
uint8_t get_packet(bl_packet_t *packet);

/**
 * @brief Send a packet to the communication channel.
 * @param packet
 * @return uint8_t
 *      0: successed.
 *      1: failed.
 */
uint8_t put_packet(bl_packet_t *packet);

/**
 * @brief Send a "ACK" packet to the communication channel.
 *
 * NOTICE: command entry no effect.
 */
void send_ACK(bl_packet_t *packet);

/**
 * @brief Send a "NACK" packet to the communication channel.
 *
 * NOTICE: command entry no effect.
 */
void send_NACK(bl_packet_t *packet);

/**
 * @brief Waiting for 'CMD_CHK_PROTOCOL' command and response ACK or NACK
 *
 */
void establish_connection(void);

/**
 * @brief Waiting for programmer's command and response ACK or NACK
 *
 */
void bl_command_process(void);

/**
 * @brief Boot the program. Load the application image from /boot partition
 *
 */
uint8_t boot_from_fs(void);

#endif /* BOOTPROTOCOL_H */
