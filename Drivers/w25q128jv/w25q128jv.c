/**
 * @file w25q128jv.c
 * @author cy023
 * @date 2023.02.15
 * @brief Flash Driver - w25q128jv
 *
 * Reference:
 * https://www.winbond.com/hq/product/code-storage-flash-memory/serial-nor-flash/?__locale=zh_TW&partNo=W25Q128JV
 */

#include "w25q128jv.h"
#include <stdint.h>
#include <stdio.h>
#include "NuMicro.h"
#include "boot_system.h"

/**
 * @brief Standard SPI Instructions
 *          ref: w25q128jv datasheet 8.1.2 Instruction Set Table 1
 */
#define W25Q128JV_WRITE_ENABLE             0x06
#define W25Q128JV_VOLATILE_SR_WRITE_ENABLE 0x50
#define W25Q128JV_WRITE_DISABLE            0x04

#define W25Q128JV_RELEASE_POWER_DOWN_ID  0xAB
#define W25Q128JV_MANUFACTURER_DEVICE_ID 0x90
#define W25Q128JV_JEDEC_ID               0x9F
#define W25Q128JV_READ_UNIQUE_ID         0x4B

#define W25Q128JV_READ_DATA 0x03
#define W25Q128JV_FAST_READ 0x0B  // TODO:

#define W25Q128JV_PAGE_PROGRAM 0x02

#define W25Q128JV_SECTOR_ERASE_4KB 0x20
#define W25Q128JV_BLOCK_ERASE_32KB 0x52
#define W25Q128JV_BLOCK_ERASE_64KB 0xD8
#define W25Q128JV_CHIP_ERASE       0xC7

#define W25Q128JV_READ_STATUS_REG1  0x05
#define W25Q128JV_WRITE_STATUS_REG1 0x01
#define W25Q128JV_READ_STATUS_REG2  0x35
#define W25Q128JV_WRITE_STATUS_REG2 0x31
#define W25Q128JV_READ_STATUS_REG3  0x15
#define W25Q128JV_WRITE_STATUS_REG3 0x11

#define W25Q128JV_READ_SFDP_REG        0x5A
#define W25Q128JV_ERASE_SECURITY_REG   0x44
#define W25Q128JV_PROGRAM_SECURITY_REG 0x42
#define W25Q128JV_READ_SECURITY_REG    0x48

#define W25Q128JV_GLOBAL_BLOCK_LOCK       0x7E
#define W25Q128JV_GLOBAL_BLOCK_UNLOCK     0x98
#define W25Q128JV_READ_BLOCK_LOCK         0x3D
#define W25Q128JV_INDIVIDUAL_BLOCK_LOCK   0x36
#define W25Q128JV_INDIVIDUAL_BLOCK_UNLOCK 0x39

#define W25Q128JV_ERASE_PROGRAM_SUSPEND 0x75
#define W25Q128JV_ERASE_PROGRAM_RESUME  0x7A
#define W25Q128JV_POWER_DOWN            0xB9

#define W25Q128JV_DTR_ENTER_QPI_MODE 0x38  // TODO: DTR
#define W25Q128JV_ENABLE_RESET       0x66
#define W25Q128JV_RESET_DEVICE       0x99

/**
 * @brief Dual/Quad SPI Instructions
 *          ref: w25q128jv datasheet 8.1.3 Instruction Set Table 2
 */
#define W25Q128JV_FAST_READ_DUAL_OUTPUT 0x3B

#define W25Q128JV_FAST_READ_DUAL_IO      0xBB
#define W25Q128JV_MFTR_DEVICE_ID_DUAL_IO 0x92

#define W25Q128JV_QUAD_INPUT_PAGE_PROGRAM 0x32
#define W25Q128JV_FAST_READ_QUAD_OUTPUT   0x6B

#define W25Q128JV_MFTR_DEVICE_ID_QUAD_IO 0x94
#define W25Q128JV_FAST_READ_QUAD_IO      0xEB
#define W25Q128JV_SET_BURST_WITH_WRAP    0x77

/**
 * @brief Dual/Quad SPI Instructions
 *          ref: w25q128jv datasheet 8.1.3 Instruction Set Table 2
 */
#define W25Q128JV_PAGE_SIZE   (256)
#define W25Q128JV_SECTOR_SIZE (4096)
#define W25Q128JV_BLOCK_SIZE  (65536)
#define W25Q128JV_FLASH_SIZE  (0x200000)

/**
 * @brief Dummy Byte for SPI swap
 */
#define W25Q128JV_DUMMY_BYTE 0x00

#define SUCCESSED 1
#define FAILED    0

/*******************************************************************************
 * Porting Layer
 ******************************************************************************/
/**
 * @brief For SPI data swap
 *
 */
static uint8_t w25q128jv_spi(uint8_t data)
{
    SPI_WRITE_TX(SPI_FLASH_PORT, data);
    while (SPI_IS_BUSY(SPI_FLASH_PORT))
        ;  // wait tx finish
    return SPI_READ_RX(SPI_FLASH_PORT) & 0xFF;
}

/**
 * @brief For SPI /CS pin enable
 *
 */
static inline void __w25q128jv_CS_ENABLE(void)
{
    SPI_SET_SS_LOW(SPI_FLASH_PORT);
}

/**
 * @brief For SPI /CS pin disable
 *
 */
static inline void __w25q128jv_CS_DISABLE(void)
{
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);
}

/*******************************************************************************
 * Static Functions
 ******************************************************************************/
static inline void w25q128jv_write_enable(void)
{
    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_WRITE_ENABLE);
    __w25q128jv_CS_DISABLE();
}

static inline void w25q128jv_write_disable(void)
{
    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_WRITE_DISABLE);
    __w25q128jv_CS_DISABLE();
}

static inline void w25q128jv_wait_for_busy(void)
{
    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_READ_STATUS_REG1);
    while (w25q128jv_spi(W25Q128JV_DUMMY_BYTE) & 0x01)
        ;
    __w25q128jv_CS_DISABLE();
}

static uint32_t w25q128jv_page2sector(uint32_t page_num)
{
    return ((page_num * W25Q128JV_PAGE_SIZE) / W25Q128JV_SECTOR_SIZE);
}

static uint32_t w25q128jv_page2block(uint32_t page_num)
{
    return ((page_num * W25Q128JV_PAGE_SIZE) / W25Q128JV_BLOCK_SIZE);
}

static uint32_t w25q128jv_sector2block(uint32_t sector_num)
{
    return ((sector_num * W25Q128JV_SECTOR_SIZE) / W25Q128JV_BLOCK_SIZE);
}

static uint32_t w25q128jv_sector2page(uint32_t sector_num)
{
    return (sector_num * W25Q128JV_SECTOR_SIZE) / W25Q128JV_PAGE_SIZE;
}

static uint32_t w25q128jv_block2sector(uint32_t block_num)
{
    return (block_num * W25Q128JV_BLOCK_SIZE) / W25Q128JV_SECTOR_SIZE;
}

static uint32_t w25q128jv_block2page(uint32_t block_num)
{
    return (block_num * W25Q128JV_BLOCK_SIZE) / W25Q128JV_PAGE_SIZE;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/
uint32_t w25q128jv_read_JEDEC_ID(void)
{
    uint32_t manufacture_id, device_id_h, device_id_l;
    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_JEDEC_ID);  // Read JEDEC ID Command
    manufacture_id = w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    device_id_h = w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    device_id_l = w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    __w25q128jv_CS_DISABLE();
    return (manufacture_id << 16) | (device_id_h << 8) | device_id_l;
}

uint8_t w25q128jv_read_UID(uint8_t *uid, uint8_t bytes)
{
    if (bytes != 8)
        return FAILED;
    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_READ_UNIQUE_ID);  // Read Unique ID Command
    for (uint8_t i = 0; i < 4; i++)
        w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    for (uint16_t i = 0; i < 8; i++)
        uid[i] = w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    __w25q128jv_CS_DISABLE();
    return SUCCESSED;
}

uint8_t w25q128jv_init(void)
{
    printf("W25Q128JV Initialization ...\n\n");

    uint32_t id = w25q128jv_read_JEDEC_ID();
    printf("Manufacture ID : 0x%lx\n", (id >> 16) & 0xFF);
    printf("Device ID      : 0x%02x%02x\n", (uint8_t)((id & 0xFF00) >> 8),
           (uint8_t)(id & 0xFF));

    uint8_t UID[8] = {0};
    w25q128jv_read_UID(UID, 8);
    printf("Unique ID      : 0x");
    for (uint8_t i = 0; i < 8; i++)
        printf("%02x", UID[i]);
    printf("\n\n");

    printf("W25Q128JV   Page Size : %8d Bytes\n", W25Q128JV_PAGE_SIZE);
    printf("W25Q128JV Sector Size : %8d Bytes\n", W25Q128JV_SECTOR_SIZE);
    printf("W25Q128JV  Block Size : %8d Bytes\n", W25Q128JV_BLOCK_SIZE);
    printf("\nW25Q128JV Initilization Done.\n\n");

    return SUCCESSED;
}

/******************************************************************************/
void w25q128jv_erase_chip(void)
{
    w25q128jv_write_enable();
    w25q128jv_wait_for_busy();  // wait

    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_CHIP_ERASE);
    __w25q128jv_CS_DISABLE();
    w25q128jv_wait_for_busy();  // wait
}

void w25q128jv_erase_sector(uint32_t sector_num)
{
    w25q128jv_write_enable();
    w25q128jv_wait_for_busy();  // wait

    uint32_t sector_addr = sector_num * W25Q128JV_SECTOR_SIZE;

    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_SECTOR_ERASE_4KB);
    w25q128jv_spi((sector_addr & 0xFF0000) >> 16);
    w25q128jv_spi((sector_addr & 0xFF00) >> 8);
    w25q128jv_spi(sector_addr & 0xFF);
    __w25q128jv_CS_DISABLE();
    w25q128jv_wait_for_busy();  // wait
}

void w25q128jv_erase_block(uint32_t block_num)
{
    w25q128jv_write_enable();
    w25q128jv_wait_for_busy();  // wait

    uint32_t block_addr = block_num * W25Q128JV_BLOCK_SIZE;

    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_BLOCK_ERASE_64KB);
    w25q128jv_spi((block_addr & 0xFF0000) >> 16);
    w25q128jv_spi((block_addr & 0xFF00) >> 8);
    w25q128jv_spi(block_addr & 0xFF);
    __w25q128jv_CS_DISABLE();
    w25q128jv_wait_for_busy();  // wait
}

/******************************************************************************/
void w25q128jv_write_byte(uint8_t pbuf, uint32_t addr)
{
    w25q128jv_write_enable();
    w25q128jv_wait_for_busy();  // wait

    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_PAGE_PROGRAM);
    w25q128jv_spi((addr & 0xFF0000) >> 16);
    w25q128jv_spi((addr & 0xFF00) >> 8);
    w25q128jv_spi(addr & 0xFF);
    w25q128jv_spi(pbuf);
    __w25q128jv_CS_DISABLE();
    w25q128jv_wait_for_busy();  // wait
}

void w25q128jv_write_page(uint8_t *pbuf,
                          uint32_t page_num,
                          uint32_t offset,
                          uint32_t bytes)
{
    uint32_t page_addr = page_num * W25Q128JV_PAGE_SIZE + offset;

    if (((bytes + offset) > W25Q128JV_PAGE_SIZE) || (bytes == 0))
        bytes = W25Q128JV_PAGE_SIZE - offset;
    if ((bytes + offset) > W25Q128JV_PAGE_SIZE)
        bytes = W25Q128JV_PAGE_SIZE - offset;

    w25q128jv_write_enable();
    w25q128jv_wait_for_busy();  // wait

    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_PAGE_PROGRAM);
    w25q128jv_spi((page_addr & 0xFF0000) >> 16);
    w25q128jv_spi((page_addr & 0xFF00) >> 8);
    w25q128jv_spi(page_addr & 0xFF);
    for (uint32_t i = 0; i < bytes; i++) {
        w25q128jv_spi(pbuf[i]);
    }
    __w25q128jv_CS_DISABLE();
    w25q128jv_wait_for_busy();  // wait
}

void w25q128jv_write_sector(uint8_t *pbuf,
                            uint32_t sector_num,
                            uint32_t offset,
                            uint32_t bytes)
{
    if ((bytes > W25Q128JV_SECTOR_SIZE) || (bytes == 0))
        bytes = W25Q128JV_SECTOR_SIZE;
    if (offset >= W25Q128JV_SECTOR_SIZE)
        return;

    uint32_t wpage;
    int32_t wbytes;
    uint32_t ofs;

    if ((bytes + offset) > W25Q128JV_SECTOR_SIZE)
        wbytes = W25Q128JV_SECTOR_SIZE - offset;
    else
        wbytes = bytes;

    wpage = w25q128jv_sector2page(sector_num) + (offset / W25Q128JV_PAGE_SIZE);
    ofs = offset % W25Q128JV_PAGE_SIZE;
    do {
        w25q128jv_write_page(pbuf, wpage, ofs, wbytes);
        wpage++;
        wbytes -= W25Q128JV_PAGE_SIZE - ofs;
        pbuf += W25Q128JV_PAGE_SIZE - ofs;
        ofs = 0;
    } while (wbytes > 0);
}

void w25q128jv_write_block(uint8_t *pbuf,
                           uint32_t block_num,
                           uint32_t offset,
                           uint32_t bytes)
{
    if ((bytes > W25Q128JV_BLOCK_SIZE) || (bytes == 0))
        bytes = W25Q128JV_BLOCK_SIZE;
    if (offset >= W25Q128JV_BLOCK_SIZE)
        return;

    uint32_t wpage;
    int32_t wbytes;
    uint32_t ofs;

    if ((bytes + offset) > W25Q128JV_BLOCK_SIZE)
        wbytes = W25Q128JV_BLOCK_SIZE - offset;
    else
        wbytes = bytes;

    wpage = w25q128jv_sector2page(block_num) + (offset / W25Q128JV_PAGE_SIZE);
    ofs = offset % W25Q128JV_PAGE_SIZE;
    do {
        w25q128jv_write_page(pbuf, wpage, ofs, wbytes);
        wpage++;
        wbytes -= W25Q128JV_PAGE_SIZE - ofs;
        pbuf += W25Q128JV_PAGE_SIZE - ofs;
        ofs = 0;
    } while (wbytes > 0);
}

/******************************************************************************/
void w25q128jv_read_byte(uint8_t *pbuf, uint32_t addr)
{
    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_FAST_READ);
    w25q128jv_spi((addr & 0xFF0000) >> 16);
    w25q128jv_spi((addr & 0xFF00) >> 8);
    w25q128jv_spi(addr & 0xFF);
    w25q128jv_spi(0);
    *pbuf = w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    __w25q128jv_CS_DISABLE();
}

void w25q128jv_read_bytes(uint8_t *pbuf, uint32_t addr, uint32_t bytes)
{
    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_FAST_READ);
    w25q128jv_spi((addr & 0xFF0000) >> 16);
    w25q128jv_spi((addr & 0xFF00) >> 8);
    w25q128jv_spi(addr & 0xFF);
    w25q128jv_spi(0);
    for (uint32_t i = 0; i < bytes; i++) {
        pbuf[i] = w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    }
    __w25q128jv_CS_DISABLE();
}

void w25q128jv_read_page(uint8_t *pbuf,
                         uint32_t page_num,
                         uint32_t offset,
                         uint32_t bytes)
{
    uint32_t page_addr = page_num * W25Q128JV_PAGE_SIZE + offset;

    if ((bytes > W25Q128JV_PAGE_SIZE) || (bytes == 0))
        bytes = W25Q128JV_PAGE_SIZE;
    if ((bytes + offset) > W25Q128JV_PAGE_SIZE)
        bytes = W25Q128JV_PAGE_SIZE - offset;

    __w25q128jv_CS_ENABLE();
    w25q128jv_spi(W25Q128JV_FAST_READ);
    w25q128jv_spi((page_addr & 0xFF0000) >> 16);
    w25q128jv_spi((page_addr & 0xFF00) >> 8);
    w25q128jv_spi(page_addr & 0xFF);
    w25q128jv_spi(0);
    for (uint32_t i = 0; i < bytes; i++) {
        pbuf[i] = w25q128jv_spi(W25Q128JV_DUMMY_BYTE);
    }
    __w25q128jv_CS_DISABLE();
}

void w25q128jv_read_sector(uint8_t *pbuf,
                           uint32_t sector_num,
                           uint32_t offset,
                           uint32_t bytes)
{
    if ((bytes > W25Q128JV_SECTOR_SIZE) || (bytes == 0))
        bytes = W25Q128JV_SECTOR_SIZE;
    if (offset >= W25Q128JV_SECTOR_SIZE)
        return;

    uint32_t rpage;
    int32_t rbytes;
    uint32_t ofs;

    if ((bytes + offset) > W25Q128JV_SECTOR_SIZE)
        rbytes = W25Q128JV_SECTOR_SIZE - offset;
    else
        rbytes = bytes;

    rpage = w25q128jv_sector2page(sector_num) + (offset / W25Q128JV_PAGE_SIZE);
    ofs = offset % W25Q128JV_PAGE_SIZE;
    do {
        w25q128jv_read_page(pbuf, rpage, ofs, rbytes);
        rpage++;
        rbytes -= W25Q128JV_PAGE_SIZE - ofs;
        pbuf += W25Q128JV_PAGE_SIZE - ofs;
        ofs = 0;
    } while (rbytes > 0);
}

void w25q128jv_read_block(uint8_t *pbuf,
                          uint32_t block_num,
                          uint32_t offset,
                          uint32_t bytes)
{
    if ((bytes > W25Q128JV_BLOCK_SIZE) || (bytes == 0))
        bytes = W25Q128JV_BLOCK_SIZE;
    if (offset >= W25Q128JV_BLOCK_SIZE)
        return;

    uint32_t rpage;
    int32_t rbytes;
    uint32_t ofs;

    if ((bytes + offset) > W25Q128JV_BLOCK_SIZE)
        rbytes = W25Q128JV_BLOCK_SIZE - offset;
    else
        rbytes = bytes;

    rpage = w25q128jv_block2page(block_num) + (offset / W25Q128JV_PAGE_SIZE);
    ofs = offset % W25Q128JV_PAGE_SIZE;
    do {
        w25q128jv_read_page(pbuf, rpage, ofs, rbytes);
        rpage++;
        rbytes -= W25Q128JV_PAGE_SIZE - ofs;
        pbuf += W25Q128JV_PAGE_SIZE - ofs;
        ofs = 0;
    } while (rbytes > 0);
}
