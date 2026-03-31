#include "gic.h"
#include <stdint.h>

/* QEMU virt GIC */
#define GICD_BASE 0x08000000U
#define GICC_BASE 0x08010000U

/* Distributor */
#define GICD_CTLR (*(volatile uint32_t *)(GICD_BASE + 0x000))
#define GICD_ISENABLER ((volatile uint32_t *)(GICD_BASE + 0x100))
#define GICD_ICENABLER ((volatile uint32_t *)(GICD_BASE + 0x180))
#define GICD_IPRIORITYR ((volatile uint8_t *)(GICD_BASE + 0x400))
#define GICD_ITARGETSR ((volatile uint8_t *)(GICD_BASE + 0x800))

/* CPU Interface */
#define GICC_CTLR (*(volatile uint32_t *)(GICC_BASE + 0x000))
#define GICC_PMR (*(volatile uint32_t *)(GICC_BASE + 0x004))
#define GICC_BPR (*(volatile uint32_t *)(GICC_BASE + 0x008))

#define TIMER_IRQ 30
#define UART0_IRQ 33

void gic_init(void)
{
    GICD_CTLR = 0;

    GICD_IPRIORITYR[TIMER_IRQ] = 0xA0;
    GICD_IPRIORITYR[UART0_IRQ] = 0xA0;

    GICD_ITARGETSR[TIMER_IRQ] = 0x01;
    GICD_ITARGETSR[UART0_IRQ] = 0x01;

    GICD_ISENABLER[0] = (1U << TIMER_IRQ);
    GICD_ISENABLER[1] = (1U << (UART0_IRQ - 32));

    GICD_CTLR = 1;

    GICC_PMR = 0xFF;
    GICC_BPR = 0x07;
    GICC_CTLR = 1;
}

void gic_enable_irq(uint32_t irq)
{
    GICD_ISENABLER[irq / 32] = (1U << (irq % 32));
}

void gic_disable_irq(uint32_t irq)
{
    GICD_ICENABLER[irq / 32] = (1U << (irq % 32));
}

void gic_set_priority(uint32_t irq, uint8_t priority)
{
    GICD_IPRIORITYR[irq] = priority;
}
