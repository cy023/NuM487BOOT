/**
 * @file device.h
 * @author cy023
 * @date 2023.03.20
 * @brief
 *      Code Section Plan for M487KMCAN.
 *
 *      Flash Size              : 512 kB
 *      USER_APP Section Size   : 448 kB
 *      BOOTLOADER Section Size :  64 kB
 */

#ifndef DEVICE_H
#define DEVICE_H

#define USER_APP_START (0x00010000UL)
#define USER_APP_END   (0x0007FFFFUL)
#define USER_APP_SIZE  (0x00080000UL)

#define BOOTLOADER_START (0x00000000UL)
#define BOOTLOADER_END   (0x0000FFFFUL)
#define BOOTLOADER_SIZE  (0x00010000UL)

enum device_table {
    D_ATSAME54_DEVB = 1,
    D_NUM487KM_DEVB
};

#endif /* DEVICE_H */
