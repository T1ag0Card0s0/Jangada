#include <jangada/arch.h>
#include <stdint.h>

uint32_t *arch_task_stack_init(uint32_t *stack_top, void (*entry)(void),
                               uint32_t arg)
{
    uint32_t *sp = stack_top;

    *--sp = (uint32_t)entry; /* pc  */
    *--sp = (uint32_t)entry; /* lr  */
    *--sp = 0U;              /* r12 */
    *--sp = 0U;              /* r11 */
    *--sp = 0U;              /* r10 */
    *--sp = 0U;              /* r9  */
    *--sp = 0U;              /* r8  */
    *--sp = 0U;              /* r7  */
    *--sp = 0U;              /* r6  */
    *--sp = 0U;              /* r5  */
    *--sp = 0U;              /* r4  */
    *--sp = 0U;              /* r3  */
    *--sp = 0U;              /* r2  */
    *--sp = 0U;              /* r1  */
    *--sp = arg;             /* r0  */
    *--sp = 0x00000053U;     /* SPSR */

    return sp;
}
