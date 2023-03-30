/**
 * @file test_04_flash.c
 * @author cy023
 * @date 2023.03.20
 * @brief
 *
 * NOTICE: If run this test code in .App section,
 *          the flash_earse_app_all() boundary should be modified.
 *         Otherwise, it will erase itself.
 */

#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "boot_system.h"
#include "device.h"
#include "flash.h"

#define BUFFSIZE 512
uint8_t page_buffer[BUFFSIZE] = {0};

void printPage(char *s)
{
    printf("\n\t[%s]\n", s);
    printf("\n\t====================== printPage ======================\n");
    for (int i = 0; i < 32; i++) {
        printf("\t|   ");
        for (int j = 0; j < 16; j++)
            printf("%02x ", page_buffer[(i << 4) + j]);
        printf("  |\n");
    }
    printf("\t=======================================================\n");
}

int main()
{
    system_init();

    SYS_UnlockReg();        /* Unlock register lock protect */
    FMC_Open();             /* Enable FMC ISP function */
    FMC_ENABLE_AP_UPDATE(); /* Enable APROM update. */

    printf("System Boot.\n");
    printf("[test04]: flash ...\n");

    // ********************************************************************** //

    flash_set_pgsz(BUFFSIZE);
    printf("flash_set_pgsz(BUFFSIZE);\n");
    printf("flash_get_pgsz(): %d\n\n", flash_get_pgsz());

    // ********************************************************************** //

    printPage("Original page_buffer");

    FMC_Erase(USER_APP_START);

    flash_read_app_page(0, page_buffer);
    printPage("After flash_read_app_page()");

    // ********************************************************************** //

    for (int i = 0; i < BUFFSIZE; i++)
        page_buffer[i] = i;

    printPage("Set page_buffer from 0 to 511");
    flash_write_app_page(0, page_buffer);
    memset(page_buffer, 0, BUFFSIZE);
    printPage("Clean buffer.");

    flash_read_app_page(0, page_buffer);
    printPage("Read after write.");

    // ********************************************************************** //

    printf("verify_app_page: %d\n", flash_verify_app_page(0, page_buffer));
    page_buffer[100] = 0;
    printf("verify_app_page after change one byte: %d\n",
           flash_verify_app_page(0, page_buffer));

    printf("\t===================== earase all ======================\n");
    printPage("Before Erase all");
    // This action will erase all the app section !
    // Including this program !
    flash_earse_app_all();
    flash_read_app_page(0, page_buffer);
    printPage("Read after Erase all");

    FMC_DISABLE_AP_UPDATE(); /* Disable APROM update. */
    FMC_Close();             /* Disable FMC ISP function */
    SYS_LockReg();           /* Lock protected registers */

    while (1) {
        ;
    }
    return 0;
}
