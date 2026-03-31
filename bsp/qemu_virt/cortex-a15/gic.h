#ifndef BSP_QEMU_VIRT_CORTEX_A15_GIC_H
#define BSP_QEMU_VIRT_CORTEX_A15_GIC_H

#include <stdint.h>

void gic_init(void);
void gic_enable_irq(uint32_t irq);
void gic_disable_irq(uint32_t irq);
void gic_set_priority(uint32_t irq, uint8_t priority);

#endif /* BSP_QEMU_VIRT_CORTEX_A15_GIC_H */
