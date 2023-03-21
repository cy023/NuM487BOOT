/**
 * @file test_03_delay.c
 * @author cy023
 * @date 2023.03.20
 * @brief 
 * 
 */

#include <stdio.h>
#include "NuMicro.h"
#include "boot_system.h"

int main()
{
    system_init();
    printf("System Boot.\n");
    printf("[test03]: delay_ms() ...\n");

    uint32_t sec = 0;
    while (1) {
        printf("Time: %ld sec\n", sec++);
        system_delay_ms(1000);
        // CLK_SysTickDelay(1000000);
    }
    return 0;
}
