#include "scheduler.h"
#include <jangada/arch.h>
#include <stdint.h>

extern void scheduler_start_asm(uint32_t *sp);

static task_t tasks[MAX_TASKS];
static int task_count = 0;
static int current_task = 0;

void scheduler_init(void)
{
    task_count = 0;
    current_task = 0;

    for (int i = 0; i < MAX_TASKS; i++)
    {
        tasks[i].state = TASK_DEAD;
        tasks[i].sp = 0;
        tasks[i].name = "(empty)";
        tasks[i].tick_count = 0;
    }
}

void scheduler_add_task(void (*entry)(void), const char *name)
{
    if (task_count >= MAX_TASKS)
    {
        return;
    }

    task_t *t = &tasks[task_count];
    t->name = name;
    t->state = TASK_READY;
    t->tick_count = 0;

    t->sp = arch_task_stack_init(&t->stack[TASK_STACK_SIZE], entry,
                                 (uint32_t)task_count);

    t->stack[0] = STACK_CANARY;

    task_count++;
}

void scheduler_start(void)
{
    if (task_count == 0)
    {
        return;
    }

    tasks[0].state = TASK_RUNNING;
    scheduler_start_asm(tasks[0].sp);

    while (1)
    {
    }
}

void scheduler_tick(void)
{
    if (task_count == 0)
    {
        return;
    }

    if (tasks[current_task].stack[0] != STACK_CANARY)
    {
        volatile const char *name = tasks[current_task].name;
        (void)name;
        while (1)
        {
        }
    }

    tasks[current_task].tick_count++;

    if (tasks[current_task].state == TASK_RUNNING)
    {
        tasks[current_task].state = TASK_READY;
    }

    int next = (current_task + 1) % task_count;
    int tries = task_count;

    while (tries-- > 0)
    {
        if (tasks[next].state == TASK_READY)
        {
            break;
        }
        next = (next + 1) % task_count;
    }

    if (tasks[next].state != TASK_READY)
    {
        next = current_task;
    }

    tasks[next].state = TASK_RUNNING;
    current_task = next;
}

task_t *scheduler_current(void)
{
    return &tasks[current_task];
}

uint32_t **scheduler_get_current_sp_ptr(void)
{
    return (uint32_t **)&tasks[current_task].sp;
}

uint32_t *scheduler_get_next_sp(void)
{
    return tasks[current_task].sp;
}
