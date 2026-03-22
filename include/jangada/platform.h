#ifndef JANGADA_PLATFORM_H
#define JANGADA_PLATFORM_H

#include <stdint.h>

void platform_interrupt_controller_init(void);
void platform_timer_init(uint32_t ticks);
void platform_timer_reload(uint32_t ticks);
uint32_t platform_timer_get_freq(void);
void platform_enable_interrupts(void);
void platform_uart_ack_irq(void);
void platform_idle(void);

#endif /* JANGADA_PLATFORM_H */
