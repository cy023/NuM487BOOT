/**
 * @file commuch.h
 * @author cy023
 * @date 2023.03.20
 * @brief
 */

#ifndef COMMUCH_H
#define COMMUCH_H

#include <stdint.h>

// TODO: 
/**
 * @brief communication channel initialization.
 */
// void com_channel_init(void);

/**
 * @brief communication channel deinitialization.
 */
// void com_channel_deinit(void);

/**
 * @brief Send 1 byte data to communication channel.
 *
 * @param data the data to send.
 */
void com_channel_putc(uint8_t data);

/**
 * @brief Receive 1 byte data from communication channel.
 *
 * @return uint8_t received data.
 */
uint8_t com_channel_getc(void);

#endif  /* COMMUCH_H */
