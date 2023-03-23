/**
 * @file test_05_w25q128jv.c
 * @author cy023
 * @date 2023.03.20
 * @brief 
 * 
 */

#include <stdio.h>
#include "NuMicro.h"
#include "boot_system.h"
#include "flash.h"
#include "device.h"
#include "w25q128jv.h"

uint8_t buffer[256] = {0};

void printBuf(void)
{
    printf("==========        Buffer        ==========\n");
    for (int i = 0; i < 32; i++) {
        printf("\t");
        for (int j = 0; j < 8; j++)
            printf("%02x ", buffer[8 * i + j]);
        printf("\n");
    }
    printf("==========================================\n");
}

int main()
{
    system_init();
    printf("System Boot.\n");
    printf("[test02]: Flash W25Q128JV ...\n\n");

    w25q128jv_init();
    
    for (int i = 0; i < 256; i++) {
        buffer[i] = 0;
    }
    printBuf();

    printf("==========  Erase Chip          ==========\n");
    // w25q128jv_erase_chip();
    w25q128jv_erase_sector(0);
    printf("==========  Erase Chip Done     ==========\n");

    printf("==========  Read Data           ==========\n");
    w25q128jv_read_bytes(buffer, 0, 256);
    printf("==========  Read Data Done      ==========\n");

    printBuf();

    for (int i = 0; i < 256; i++) {
        buffer[i] = i;
    }
    printBuf();

    printf("==========  Program Data        ==========\n");
    w25q128jv_write_page(buffer, 0, 0, 256);
    printf("==========  Program Data Done   ==========\n");

    for (int i = 0; i < 256; i++) {
        buffer[i] = 0;
    }

    printf("==========  Read Data           ==========\n");
    w25q128jv_read_bytes(buffer, 0, 256);
    printf("==========  Read Data Done      ==========\n");

    printBuf();

    while (1) {
        w25q128jv_read_bytes(buffer, 0, 256);
        system_delay_ms(100);
    }

    return 0;
}
