/**
 * @file test_00_uart.c
 * @author cy023
 * @date 2023.03.20
 * @brief 
 */

#include <stdio.h>
#include "NuMicro.h"
#include "boot_system.h"
#include "commuch.h"

int main()
{
    system_init();
    printf("System Boot.\n");
    printf("[test00]: uart ...\n");

    char c;
    while (1) {
        printf("Please input a character: \n");
        c = com_channel_getc();
        com_channel_putc(c);
        printf("Your input character is %c\n\n", c);
    }
    return 0;
}
