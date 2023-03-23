/**
 * @file boot_system.h
 * @author cy023
 * @date 2023.03.20
 * @brief
 */


#ifndef BOOT_SYSTEM_H
#define BOOT_SYSTEM_H

#include <stdint.h>

#define PLL_CLOCK           192000000
#define SPI_FLASH_PORT      SPI2

/*******************************************************************************
 * Peripheral Driver Enable
 ******************************************************************************/
#ifndef BOOT_GPIO_DRIVER_ENABLE
#define BOOT_GPIO_DRIVER_ENABLE   1
#endif

#ifndef BOOT_CLOCK_DRIVER_ENABLE
#define BOOT_CLOCK_DRIVER_ENABLE  1
#endif

#ifndef BOOT_UART_DRIVER_ENABLE
#define BOOT_UART_DRIVER_ENABLE   1
#endif

#ifndef BOOT_SPI_DRIVER_ENABLE
#define BOOT_SPI_DRIVER_ENABLE    1
#endif

/*******************************************************************************
 * Public Function
 ******************************************************************************/
/**
 * @brief System initialization.
 * 
 *  - system_gpio_init()
 *  - system_clock_init()
 *  - system_uart_init()
 *  - system_spi_init()
 */
void system_init(void);

/**
 * @brief System deinitialization.
 * 
 *  - system_gpio_deinit()
 *  - system_clock_deinit()
 *  - system_uart_deinit()
 *  - system_spi_deinit()
 */
void system_deinit(void);

/**
 * @brief Jump to APP section from bootloader section.
 * 
 *  - system_deinit()
 *  - Reset Core register
 *  - Change the vector table offset to application vector table
 *  - Set the MSP
 *  - Jump to application's Reset_Handler()
 */
void system_jump_to_app(void);

/**
 * @brief Check whether the MCU is in "Prog" mode.
 * @return uint8_t
 *      1: True, in PROG mode.
 *      0: False, in RUN mode.
 */
uint8_t system_is_prog_mode(void);

/**
 * @brief Delay by polling.
 * @param uint32_t ms   delay times in millisecond.
 */
void system_delay_ms(uint32_t ms);

/**
 * @brief Trun on the Boot LED.
 *
 *  NOTE: When MCU is botting from flash. The Boot LED will be turn on.
 */
void bootLED_on(void);

/**
 * @brief Trun off the Boot LED.
 */
void bootLED_off(void);

/**
 * @brief Enable APROM update.
 *
 *  - Unlock register lock protect
 *  - Enable FMC ISP function
 *  - Enable APROM update
 */
void APROM_update_enable(void);

/**
 * @brief Disable APROM update.
 *
 *  - Disable APROM update
 *  - Disable FMC ISP function
 *  - Lock protected registers
 */
void APROM_update_disable(void);

#endif  /* BOOT_SYSTEM_H */
