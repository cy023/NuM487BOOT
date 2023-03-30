/**
 * @file test_05_littlefs.c
 * @author cy023
 * @date 2023.03.20
 * @brief
 *
 */

#include <stdio.h>
#include "boot_system.h"
#include "lfs.h"
#include "lfs_port.h"

// entry point
void lfs_test(void)
{
    // mount the filesystem
    int err = lfs_mount(&lfs_w25q128jv, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs_w25q128jv, &cfg);
        lfs_mount(&lfs_w25q128jv, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs_w25q128jv, &lfs_file_w25q128jv, "boot_count",
                  LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs_w25q128jv, &lfs_file_w25q128jv, &boot_count,
                  sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs_w25q128jv, &lfs_file_w25q128jv);
    lfs_file_write(&lfs_w25q128jv, &lfs_file_w25q128jv, &boot_count,
                   sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs_w25q128jv, &lfs_file_w25q128jv);

    // release any resources we were using
    lfs_unmount(&lfs_w25q128jv);

    // print the boot count
    printf("boot_count: %ld\n", boot_count);
}

// entry point
int main(void)
{
    system_init();
    printf("System Boot.\n");
    printf("[test05]: LittleFS ...\n\n");

    lfs_test();
    return 0;
}
