/**
 * @file flash.h
 * @author cy023
 * @date 2023.03.20
 * @brief
 * 
 * The API only supports for address in User App Section.
 */

#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

/**
 * @brief Set the flash page size.
 * @param size the size to set. should be 256 * N.
 * @return uint8_t
 *      0: successed.
 *      1: failed.
 */
uint8_t flash_set_pgsz(uint16_t size);

/**
 * @brief Get the current flash page size.
 * @return uint16_t flash page size.
 */
uint16_t flash_get_pgsz(void);

/**
 * @brief Write page buffer to flash page.
 * @param page_addr byte address of the flash page to read.
 * @param buf address of page buffer.
 * @return uint8_t
 *      0: successed.
 *      1: failed.
 */
uint8_t flash_write_app_page(const uint32_t dest, uint8_t *buf);

/**
 * @brief Read a flash page and put it into buf.
 * @param page_addr byte address of the flash page to read.
 * @param buf address of page buffer.
 * @return uint8_t
 *      0: successed.
 *      1: failed.
 */
uint8_t flash_read_app_page(const uint32_t src, uint8_t *buf);

/**
 * @brief Verify the flash page is the same as page buf.
 * @param page_addr byte address of the flash page to verify.
 * @param buf address of page buffer.
 * @return uint8_t 
 *      0: the page is the same with buf.
 *      1: the page is different from buf.
 */
uint8_t flash_verify_app_page(const uint32_t src, uint8_t *buf);

/**
 * @brief Earse the assigned flash section.
 * @param sector_num the num of flash section to clear.
 * @return uint8_t
 *      0: successed.
 *      1: failed.
 */
// uint8_t flash_earse_sector(uint8_t sector_num);

/**
 * @brief Earse all space of user app flash.
 * @return uint8_t
 *      0: successed.
 *      1: failed.
 */
uint8_t flash_earse_app_all(void);

#endif /* FLASH_H */
