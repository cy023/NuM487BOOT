/**
 * @file commuch.c
 * @author cy023
 * @date 2023.03.20
 * @brief
 */

#include "commuch.h"
#include "NuMicro.h"

void com_channel_putc(uint8_t data)
{
    UART_Write(UART0, &data, 1);
}

uint8_t com_channel_getc(void)
{
    uint8_t data;
    UART_Read(UART0, &data, 1);
    return data;
}
