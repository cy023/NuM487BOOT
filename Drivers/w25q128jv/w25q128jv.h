/**
 * @file w25q128jv.h
 * @author cy023
 * @date 2023.02.15
 * @brief Flash Driver - w25q128jv
 *
 * Reference: https://www.winbond.com/hq/product/code-storage-flash-memory/serial-nor-flash/?__locale=zh_TW&partNo=W25Q128JV
 */

#ifndef W25Q128JV_H
#define W25Q128JV_H

#include <stdint.h>

/**
 * @brief Read JEDEC ID
 * @ref 8.1.1 Manufacturer and Device Identification
 * @ref 8.1.2 Instruction Set Table 1 (Standard SPI Instructions)
 * @return uint32_t receive order (MF7-MF0) (ID15-ID8) (ID7-ID0)
 */
uint32_t w25q128jv_read_JEDEC_ID(void);

/**
 * @brief Read Unique ID
 * @return uint32_t
 *      1 - SUCCESSED
 *      0 - FAILED, the parameter 'bytes' not 8.
 */
uint8_t w25q128jv_read_UID(uint8_t *uid, uint8_t bytes);

/**
 * @brief w25q128jv Information
 * @return uint32_t
 *      1 - SUCCESSED
 *      0 - FAILED
 */
uint8_t w25q128jv_init(void);

void w25q128jv_erase_chip(void);
void w25q128jv_erase_sector(uint32_t sector_num);
void w25q128jv_erase_block(uint32_t block_num);

void w25q128jv_read_byte(uint8_t *pbuf, uint32_t addr);
void w25q128jv_read_bytes(uint8_t *pbuf, uint32_t addr, uint32_t bytes);
void w25q128jv_read_page(uint8_t *pbuf, uint32_t page_num, uint32_t offset, uint32_t bytes);
void w25q128jv_read_sector(uint8_t *pbuf, uint32_t sector_num, uint32_t offset, uint32_t bytes);
void w25q128jv_read_block(uint8_t *pbuf, uint32_t block_num, uint32_t offset, uint32_t bytes);

void w25q128jv_write_byte(uint8_t pbuf, uint32_t addr);
void w25q128jv_write_page(uint8_t *pbuf, uint32_t page_num, uint32_t offset, uint32_t bytes);
void w25q128jv_write_sector(uint8_t *pbuf, uint32_t sector_num, uint32_t offset, uint32_t bytes);
void w25q128jv_write_block(uint8_t *pbuf, uint32_t block_num, uint32_t offset, uint32_t bytes);

#endif /* W25Q128JV_H */
