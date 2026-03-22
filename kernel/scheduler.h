#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include <stdint.h>

#define MAX_TASKS 8
#define TASK_STACK_SIZE 2048
#define STACK_CANARY 0xDEADBEEFU

typedef enum
{
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_DEAD
} task_state_t;

typedef struct
{
    uint32_t *sp;
    uint32_t stack[TASK_STACK_SIZE];
    task_state_t state;
    const char *name;
    uint32_t tick_count;
} task_t;

void scheduler_init(void);
void scheduler_add_task(void (*entry)(void), const char *name);
void scheduler_start(void);
void scheduler_tick(void);
task_t *scheduler_current(void);

uint32_t **scheduler_get_current_sp_ptr(void);
uint32_t *scheduler_get_next_sp(void);

#endif /* KERNEL_SCHEDULER_H */
