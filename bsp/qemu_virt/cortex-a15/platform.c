#include "gic.h"
#include <jangada/platform.h>
#include <stdint.h>

#define UART0_BASE 0x09000000U
#define UART0_DR (*(volatile uint32_t *)(UART0_BASE + 0x00))

void platform_interrupt_controller_init(void)
{
    gic_init();
}

void platform_enable_interrupts(void)
{
    asm volatile("mrs r0, cpsr      \n"
                 "bic r0, r0, #0x80 \n"
                 "msr cpsr_c, r0    \n" ::
                         : "r0");
}

void platform_uart_ack_irq(void)
{
    volatile uint32_t c = UART0_DR;
    (void)c;
}

void platform_idle(void)
{
    asm volatile("wfi");
}
