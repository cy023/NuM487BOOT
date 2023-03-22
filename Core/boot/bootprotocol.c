/**
 * @file bootprotocol.c
 * @author cy023
 * @brief Packet Handler
 * @date 2023.03.20
 */

#include "commuch.h"
#include "bootprotocol.h"
#include "boot_system.h"
#include "flash.h"
#include "device.h"

// #include "lfs.h"
// #include "lfs_port.h"

#define BUFFERSIZE  516
static uint8_t bl_buffer[BUFFERSIZE] = {0};

/*******************************************************************************
 * Basic Operation
 ******************************************************************************/

uint8_t get_packet(bl_packet_t *packet)
{
    uint8_t cmd;
    uint8_t len_h, len_l;
    uint8_t chksum = 0;

    if (com_channel_getc() != HEADER) return FAILED;
    if (com_channel_getc() != HEADER) return FAILED;
    if (com_channel_getc() != HEADER) return FAILED;

    cmd = com_channel_getc();
    len_h = com_channel_getc();
    len_l = com_channel_getc();

    packet->cmd = cmd;
    packet->length = (len_h << 8) | len_l;

    for (uint16_t i = 0; i < packet->length; i++) {
        packet->data[i] = com_channel_getc();
        chksum += packet->data[i];
    }
    if (com_channel_getc() != chksum)   
        return FAILED;

    return SUCCESSED;
}

uint8_t put_packet(bl_packet_t *packet)
{
    uint8_t chksum = 0;

    com_channel_putc(HEADER);
    com_channel_putc(HEADER);
    com_channel_putc(HEADER);
    com_channel_putc(packet->cmd);
    com_channel_putc(packet->length >> 8);
    com_channel_putc(packet->length & 0xFF);

    for (uint16_t i = 0; i < packet->length; i++) {
        com_channel_putc(packet->data[i]);
        chksum += packet->data[i];
    }
    com_channel_putc(chksum);
    return SUCCESSED;
}

void send_ACK(bl_packet_t *packet)
{
    packet->length = 1;
    packet->data[0] = ACK;
    put_packet(packet);
}

void send_NACK(bl_packet_t *packet)
{
    packet->length = 1;
    packet->data[0] = NACK;
    put_packet(packet);
}

/*******************************************************************************
 * Boot Protocol
 ******************************************************************************/

void establish_connection(void)
{
    bl_packet_t pac = { .cmd = 0,
                        .length = 0,
                        .data = bl_buffer };
    while (1) {
        if (get_packet(&pac))
            continue;

        if (pac.cmd == CMD_CHK_PROTOCOL) {
            pac.length = 2;
            pac.data[0] = SUCCESSED;
            pac.data[1] = 1;
            put_packet(&pac);
            return;
        }
        else {
            send_NACK(&pac);
        }
    }
}

void bl_command_process(void)
{
    bl_packet_t pac = { .cmd = 0,
                        .length = 0,
                        .data = bl_buffer };

    while (1) {
        if (get_packet(&pac))
            continue;

        switch (pac.cmd) {
            case CMD_CHK_PROTOCOL: {
                pac.length = 2;
                pac.data[0] = SUCCESSED; // ACK
                pac.data[1] = 1; // protocol vresion
                put_packet(&pac);
                break;
            }
            case CMD_CHK_DEVICE: {
                pac.length = 2;
                pac.data[0] = SUCCESSED; // ACK
                pac.data[1] = (uint8_t)D_NUM487KM_DEVB;
                put_packet(&pac);
                break;
            }
            case CMD_PROG_END: {
                send_ACK(&pac);
                return;
            }
            case CMD_PROG_EXT_FLASH_BOOT: { // TODO:
                // boot_from_fs();
                // send_ACK(&pac);
                break;
            }
            case CMD_FLASH_SET_PGSZ: {
                // TODO: only support 512 byte page size.
                if (flash_set_pgsz(*((uint16_t *)pac.data)))
                    send_NACK(&pac);
                else
                    send_ACK(&pac);
                break;
            }
            case CMD_FLASH_GET_PGSZ: {
                pac.length = 3;
                pac.data[0] = SUCCESSED; // ACK
                *((uint16_t *)(pac.data + 1)) = flash_get_pgsz();
                put_packet(&pac);
                break;
            }
            case CMD_FLASH_WRITE: {
                if (flash_write_app_page(*(uint32_t *)pac.data, (uint8_t *)(pac.data + 4)))
                    send_NACK(&pac);
                else
                    send_ACK(&pac);
                break;
            }
            case CMD_FLASH_READ: {
                if (flash_read_app_page(*(uint32_t *)pac.data, (uint8_t *)(pac.data + 1))) {
                    send_NACK(&pac);
                }
                else {
                    pac.length = 1 + flash_get_pgsz();
                    pac.data[0] = SUCCESSED; // ACK
                    put_packet(&pac);
                }
                break;
            }
            case CMD_FLASH_VERIFY: { // TODO:
                send_NACK(&pac);
                break;
            }
            case CMD_FLASH_EARSE_SECTOR: { // TODO:
                send_NACK(&pac);
                break;
            }
            case CMD_FLASH_EARSE_ALL: {
                if (flash_earse_app_all())
                    send_NACK(&pac);
                else
                    send_ACK(&pac);
                break;
            }

            /******************************************************************/

            case CMD_EXT_FLASH_FOPEN: {
                // // mount the filesystem
                // int err = lfs_mount(&lfs_w25q128jv, &cfg);

                // // reformat if we can't mount the filesystem, this should only happen on the first boot
                // if (err) {
                //     lfs_format(&lfs_w25q128jv, &cfg);
                //     lfs_mount(&lfs_w25q128jv, &cfg);
                // }

                // lfs_remove(&lfs_w25q128jv, "/boot");

                // // Open boot partition
                // lfs_file_open(&lfs_w25q128jv, &lfs_file_w25q128jv, "/boot", LFS_O_WRONLY | LFS_O_APPEND | LFS_O_CREAT);

                send_ACK(&pac);
                break;
            }
            case CMD_EXT_FLASH_FCLOSE: {
                // // Write boot image done!
                // lfs_file_close(&lfs_w25q128jv, &lfs_file_w25q128jv);

                // // release any resources we were using
                // lfs_unmount(&lfs_w25q128jv);

                send_ACK(&pac);
                break;
            }
            case CMD_EXT_FLASH_WRITE: {
                // lfs_file_rewind(&lfs_w25q128jv, &lfs_file_w25q128jv);
                // lfs_file_write(&lfs_w25q128jv, &lfs_file_w25q128jv, (uint8_t *)pac.data, BUFFERSIZE);
                // send_ACK(&pac);
                break;
            }
            case CMD_EXT_FLASH_READ:            {break;}
            case CMD_EXT_FLASH_VERIFY:          {break;}
            case CMD_EXT_FLASH_EARSE_SECTOR:    {break;}
            case CMD_EXT_FLASH_HEX_DEL:         {break;}

            /******************************************************************/

            case CMD_EEPROM_SET_PGSZ:           {break;}
            case CMD_EEPROM_GET_PGSZ:           {break;}
            case CMD_EEPROM_WRITE:              {break;}
            case CMD_EEPROM_READ:               {break;}
            case CMD_EEPROM_EARSE_ALL:          {break;}

            /******************************************************************/

            default: { // NOT supported command
                send_NACK(&pac);
                break;
            }
        }
    }
}

// void boot_from_fs(void)
// {
//     bootLED_on();

//     flash_earse_app_all();
//     memset(bl_buffer, 0, BUFFERSIZE);

//     int err = lfs_mount(&lfs_w25q128jv, &cfg);

//     // reformat if we can't mount the filesystem, this should only happen on the first boot
//     if (err) {
//         lfs_format(&lfs_w25q128jv, &cfg);
//         lfs_mount(&lfs_w25q128jv, &cfg);
//     }

//     lfs_file_open(&lfs_w25q128jv, &lfs_file_w25q128jv, "/boot", LFS_O_RDONLY | LFS_O_CREAT);

//     lfs_soff_t fsize = lfs_file_size(&lfs_w25q128jv, &lfs_file_w25q128jv);
//     // printf("/boot size is %ld\n", fsize);

//     while (fsize >= BUFFERSIZE) {
//         lfs_file_read(&lfs_w25q128jv, &lfs_file_w25q128jv, bl_buffer, BUFFERSIZE);
//         flash_write_app_page(*(uint32_t *)bl_buffer, (uint8_t *)(bl_buffer + 4));
//         fsize -= BUFFERSIZE;
//     }
//     if (fsize) {
//         memset(bl_buffer, 0, BUFFERSIZE);
//         lfs_file_read(&lfs_w25q128jv, &lfs_file_w25q128jv, bl_buffer, fsize);
//         flash_write_app_page(*(uint32_t *)bl_buffer, (uint8_t *)(bl_buffer + 4));
//     }

//     lfs_file_close(&lfs_w25q128jv, &lfs_file_w25q128jv);
//     lfs_unmount(&lfs_w25q128jv);

//     bootLED_off();
// }
