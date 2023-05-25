/**
 * @file flash.c
 * @author cy023
 * @date 2023.03.20
 * @brief
 *
 * NOTICE: No buffer bounds checks are done here.
 */

#include <stdint.h>
#include "NuMicro.h"
#include "commuch.h"
#include "device.h"

/*******************************************************************************
 * Macro
 ******************************************************************************/
#define BL_FLASH_PACSIZE (512U)

#define FAILED    1
#define SUCCESSED 0

/*******************************************************************************
 * Public function
 ******************************************************************************/
uint8_t flash_set_pgsz(uint16_t size)
{
    if (size != 512)  // TODO: only support 512 bytes now
        return FAILED;
    else
        return SUCCESSED;
}

uint16_t flash_get_pgsz(void)
{
    return BL_FLASH_PACSIZE;
}

uint8_t flash_write_app_page(const uint32_t dest, uint8_t *buf)
{
    // TODO: FMC_WriteMultiple(0x00020000UL, (uint32_t *)page_buffer, BUFFSIZE);
    for (uint32_t addr = dest, i = 0; i < BL_FLASH_PACSIZE;
         addr += 4, i += 4) {
        uint32_t tmp = buf[i];
        tmp |= buf[i + 1] << 8;
        tmp |= buf[i + 2] << 16;
        tmp |= buf[i + 3] << 24;
        if (FMC_Write(addr, tmp) != 0)
            return FAILED;
    }
    return SUCCESSED;
}

uint8_t flash_read_app_page(const uint32_t src, uint8_t *buf)
{
    for (uint32_t addr = src, i = 0; i < BL_FLASH_PACSIZE;
         addr += 4, i += 4) {
        uint32_t res = FMC_Read(addr);
        buf[i + 0] = res & 0xFF;
        buf[i + 1] = (res >> 8) & 0xFF;
        buf[i + 2] = (res >> 16) & 0xFF;
        buf[i + 3] = (res >> 24) & 0xFF;
        if (g_FMC_i32ErrCode != 0)
            return FAILED;
    }
    return SUCCESSED;
}

uint8_t flash_verify_app_page(const uint32_t src, uint8_t *buf)
{
    uint32_t *pbuf32 = (uint32_t *) buf;
    for (uint32_t addr = src, i = 0; i < BL_FLASH_PACSIZE;
         addr += 4, i += 4) {
        uint32_t res = FMC_Read(addr);
        if (res != pbuf32[i / 4] || g_FMC_i32ErrCode != 0)
            return FAILED;
    }
    return SUCCESSED;
}

// uint8_t flash_erase_sector(uint8_t sector_num)
// {
//     // TODO:
//     // uint32_t addr = sector_num * NVMCTRL_FLASH_BLOCKSIZE;
//     // nvmctrl_erase_block(addr);
//     // return NVMCTRL_PROGE;
//     return 0;
// }

uint8_t flash_erase_app_all(void)
{
    uint8_t res = 0;
    for (uint32_t addr = USER_APP_START; addr < USER_APP_END;
         addr += (FMC_FLASH_PAGE_SIZE << 2))
        res |= FMC_Erase_Block(addr);  // erase 4 pages
    // TODO: res |= FMC_Erase(addr);
    return res;
}
