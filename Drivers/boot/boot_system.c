/**
 * @file boot_system.c
 * @author cy023
 * @date 2023.03.20
 * @brief
 */

#include "boot_system.h"
#include "NuMicro.h"
#include "device.h"

/*******************************************************************************
 * Peripheral Driver - private function
 ******************************************************************************/
/**
 * @brief Clock peripheral initialization.
 */
static void system_clock_init(void)
{
    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable External XTAL (4~24 MHz) : 12MHz */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable SPI2 peripheral clock */
    CLK_EnableModuleClock(SPI2_MODULE);

    /* Select UART clock source from HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT,
                       CLK_CLKDIV0_UART0(1));

    /* Select PCLK1 as the clock source of SPI2 */
    CLK_SetModuleClock(SPI2_MODULE, CLK_CLKSEL2_SPI2SEL_PCLK1, MODULE_NoMsk);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and
     * CyclesPerUs automatically. */
    SystemCoreClockUpdate();
}

/**
 * @brief Clock peripheral initialization.
 */
static void system_clock_deinit(void)
{
    // TODO:
}

/**
 * @brief GPIO peripheral initialization.
 *
 *  - PB.5 for prog/run dectect pin
 *  - PH.4 BOOT pin as output
 *  - PH.5 LED0 pin as output
 */
static void system_gpio_init(void)
{
    // set PB5 as prog/run detect pin
    GPIO_SetMode(PB, BIT5, GPIO_MODE_INPUT);

    // set BOOT pin (PH.4) as output
    GPIO_SetMode(PH, BIT4, GPIO_MODE_OUTPUT);

    // set LED0 pin (PH.5) as output
    GPIO_SetMode(PH, BIT5, GPIO_MODE_OUTPUT);
}

/**
 * @brief GPIO peripheral deinitialization.
 */
static void system_gpio_deinit(void)
{
    // TODO:
}

/**
 * @brief UART peripheral initialization.
 */
static void system_uart0_init(void)
{
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |=
        (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Init UART to 38400-8n1 for print message */
    UART_Open(UART0, 38400);
}

/**
 * @brief UART peripheral deinitialization.
 */
static void system_uart0_deinit(void)
{
    UART_Close(UART0);
}

/**
 * @brief For External Flash - w25q128jv
 *
 *  FLASH_MOSI  : PA.8
 *  FLASH_SCK   : PA.10
 *  FLASH_MISO  : PG.4
 *  FLASH_CS    : PA.11
 */
static void system_spi_init(void)
{
    /* Setup SPI2 multi-function pins */
    // TODO: Change Flash_CS from PG4 (MFP3) to PA9 (MFP4)
    // SYS->GPA_MFPH |= SYS_GPA_MFPH_PA11MFP_SPI2_SS |
    // SYS_GPA_MFPH_PA10MFP_SPI2_CLK | SYS_GPA_MFPH_PA8MFP_SPI2_MOSI |
    // SYS_GPA_MFPH_PA9MFP_SPI2_MISO;
    SYS->GPA_MFPH |= SYS_GPA_MFPH_PA11MFP_SPI2_SS |
                     SYS_GPA_MFPH_PA10MFP_SPI2_CLK |
                     SYS_GPA_MFPH_PA8MFP_SPI2_MOSI;
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG4MFP_SPI2_MISO;

    /* Enable SPI2 clock pin (PA10) schmitt trigger */
    PA->SMTEN |= GPIO_SMTEN_SMTEN10_Msk;

    /* Enable SPI2 I/O high slew rate */
    GPIO_SetSlewCtl(PA, 0xF, GPIO_SLEWCTL_FAST);
    GPIO_SetSlewCtl(PG, 0xF, GPIO_SLEWCTL_FAST);

    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, SPI
     * Mode-0 timing, clock is 20MHz */
    SPI_Open(SPI_FLASH_PORT, SPI_MASTER, SPI_MODE_0, 8, 20000000);

    /* Disable auto SS function, control SS signal manually. */
    SPI_DisableAutoSS(SPI_FLASH_PORT);
}

static void system_spi_deinit(void)
{
    // TODO:
}

/*******************************************************************************
 * Public Function
 ******************************************************************************/

void system_init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

#if defined(BOOT_GPIO_DRIVER_ENABLE) && (BOOT_GPIO_DRIVER_ENABLE + 0)
    system_gpio_init();
#endif

#if defined(BOOT_CLOCK_DRIVER_ENABLE) && (BOOT_CLOCK_DRIVER_ENABLE + 0)
    system_clock_init();
#endif

#if defined(BOOT_UART_DRIVER_ENABLE) && (BOOT_UART_DRIVER_ENABLE + 0)
    system_uart0_init();
#endif

#if defined(BOOT_SPI_DRIVER_ENABLE) && (BOOT_SPI_DRIVER_ENABLE + 0)
    system_spi_init();
#endif

    /* Lock protected registers */
    SYS_LockReg();
}

void system_deinit(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

#if defined(BOOT_SPI_DRIVER_ENABLE) && (BOOT_SPI_DRIVER_ENABLE + 0)
    system_spi_init();
#endif

#if defined(BOOT_UART_DRIVER_ENABLE) && (BOOT_UART_DRIVER_ENABLE + 0)
    system_uart0_init();
#endif

#if defined(BOOT_CLOCK_DRIVER_ENABLE) && (BOOT_CLOCK_DRIVER_ENABLE + 0)
    system_clock_init();
#endif

#if defined(BOOT_GPIO_DRIVER_ENABLE) && (BOOT_GPIO_DRIVER_ENABLE + 0)
    system_gpio_init();
#endif

    /* Lock protected registers */
    SYS_LockReg();
}

/*******************************************************************************
 * Bootloader Operation
 ******************************************************************************/

__attribute__((always_inline)) static inline void jump2app(void)
{
    // Setting the stack pointer.
    __set_MSP(*(uint32_t *) USER_APP_START);

    // SP + 4: Reset Handler Offset.
    __ASM volatile("BLX %0" : : "r"(*(uint32_t *) (USER_APP_START + 4)));
}

void system_jump_to_app(void)
{
    // TODO If not in Privileged Mode.
    // if (CONTROL_nPRIV_Msk & __get_CONTROL()) {
    //    // not in privileged mode
    //    __asm("SVC #0\n");
    // }

    system_deinit();

    __disable_irq();

    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;
    NVIC->ICER[3] = 0xFFFFFFFF;
    NVIC->ICER[4] = 0xFFFFFFFF;
    NVIC->ICER[5] = 0xFFFFFFFF;
    NVIC->ICER[6] = 0xFFFFFFFF;
    NVIC->ICER[7] = 0xFFFFFFFF;

    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;
    NVIC->ICPR[2] = 0xFFFFFFFF;
    NVIC->ICPR[3] = 0xFFFFFFFF;
    NVIC->ICPR[4] = 0xFFFFFFFF;
    NVIC->ICPR[5] = 0xFFFFFFFF;
    NVIC->ICPR[6] = 0xFFFFFFFF;
    NVIC->ICPR[7] = 0xFFFFFFFF;

    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;  // Removes the pending status of the
                                          // SysTick exception
    SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
                    SCB_SHCSR_MEMFAULTENA_Msk);

    __DSB();
    __ISB();

    SCB->VTOR = (USER_APP_START & SCB_VTOR_TBLOFF_Msk);

    __DSB();
    __ISB();

    __enable_irq();

    jump2app();
}

uint8_t system_is_prog_mode(void)
{
    return !PB5;
}

void system_delay_ms(uint32_t ms)
{
    uint16_t delay;
    volatile uint32_t i;
    for (delay = ms; delay > 0; delay--) {
        // 1 ms loop with -O0 optimization.
        for (i = 19200; i > 0; i--) {
            ;
        }
    }
}

void bootLED_on(void)
{
    PH4 = 0;
}

void bootLED_off(void)
{
    PH4 = 1;
}

void APROM_update_enable(void)
{
    SYS_UnlockReg();        /* Unlock register lock protect */
    FMC_Open();             /* Enable FMC ISP function */
    FMC_ENABLE_AP_UPDATE(); /* Enable APROM update. */
}

void APROM_update_disable(void)
{
    FMC_DISABLE_AP_UPDATE(); /* Disable APROM update. */
    FMC_Close();             /* Disable FMC ISP function */
    SYS_LockReg();           /* Lock protected registers */
}
