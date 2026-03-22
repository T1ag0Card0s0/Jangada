#include "scheduler.h"
#include <jangada/platform.h>
#include <jangada/tty.h>
#include <stdint.h>

static uint32_t g_tick_interval = 0;

static uint32_t kernel_tick_interval_init(void)
{
    uint32_t freq = platform_timer_get_freq();
    uint32_t tick = freq / 100U; /* 10 ms */

    if (tick == 0U)
    {
        tick = 1U;
    }

    return tick;
}

static void terminal_write_u32(uint32_t n)
{
    char buf[12];
    int i = 0;

    if (n == 0U)
    {
        terminal_putchar('0');
        return;
    }

    while (n != 0U)
    {
        buf[i++] = (char)('0' + (n % 10U));
        n /= 10U;
    }

    while (i-- > 0)
    {
        terminal_putchar(buf[i]);
    }
}

void timer_irq_handler(void)
{
    platform_timer_reload(g_tick_interval);
}

void uart0_irq_handler(void)
{
    platform_uart_ack_irq();
}

void irq_unhandled(uint32_t id)
{
    terminal_writestring("unhandled IRQ: ");
    terminal_write_u32(id);
    terminal_putchar('\n');
}

void data_abort_c_handler(void)
{
    uint32_t dfsr;
    uint32_t dfar;

    asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r"(dfsr));
    asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar));

    terminal_writestring("\n*** DATA ABORT ***\nDFAR: ");
    terminal_write_u32(dfar);
    terminal_writestring("\nDFSR: ");
    terminal_write_u32(dfsr);
    terminal_putchar('\n');

    while (1)
    {
    }
}

void prefetch_abort_c_handler(void)
{
    terminal_writestring("\n*** PREFETCH ABORT ***\n");

    while (1)
    {
    }
}

void undef_c_handler(void)
{
    terminal_writestring("\n*** UNDEFINED INSTRUCTION ***\n");

    while (1)
    {
    }
}

void svc_c_handler(uint32_t svc_num)
{
    terminal_writestring("SVC #");
    terminal_write_u32(svc_num);
    terminal_putchar('\n');
}

void fiq_c_handler(void)
{
    terminal_writestring("FIQ fired\n");
}

static void task_a(void)
{
    uint32_t count = 0;

    while (1)
    {
        terminal_writestring("  [A] count=");
        terminal_write_u32(count++);
        terminal_putchar('\n');

        for (volatile uint32_t i = 0; i < 1000000U; i++)
        {
        }
    }
}

static void task_b(void)
{
    uint32_t count = 0;

    while (1)
    {
        terminal_writestring("  [B] count=");
        terminal_write_u32(count++);
        terminal_putchar('\n');

        for (volatile uint32_t i = 0; i < 1000000U; i++)
        {
        }
    }
}

static void task_c(void)
{
    uint32_t count = 0;

    while (1)
    {
        terminal_writestring("  [C] count=");
        terminal_write_u32(count++);
        terminal_putchar('\n');

        for (volatile uint32_t i = 0; i < 500000U; i++)
        {
        }
    }
}

int kernel_main(void)
{
    terminal_initialize();
    terminal_writestring("Booting...\n");

    platform_interrupt_controller_init();

    scheduler_init();
    scheduler_add_task(task_a, "task_A");
    scheduler_add_task(task_b, "task_B");
    scheduler_add_task(task_c, "task_C");

    g_tick_interval = kernel_tick_interval_init();
    platform_timer_init(g_tick_interval);

    terminal_writestring("Timer started\n");
    terminal_writestring("Scheduler starting - 3 tasks\n");

    platform_enable_interrupts();

    scheduler_start();

    while (1)
    {
        platform_idle();
    }

    return 0;
}
