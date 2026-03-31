#ifndef JANGADA_ARCH_H
#define JANGADA_ARCH_H

#include <stdint.h>

uint32_t *arch_task_stack_init(uint32_t *stack_top, void (*entry)(void),
                               uint32_t arg);

#endif /* JANGADA_ARCH_H */
