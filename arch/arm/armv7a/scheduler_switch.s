.equ MODE_SVC, 0x13
.equ MODE_IRQ, 0x12
.equ IF_BITS,  0xC0

.macro set_mode mode
    mrs   r12, cpsr
    bic   r12, r12, #0x1F
    orr   r12, r12, #(\mode | IF_BITS)
    msr   cpsr_c, r12
.endm

.section .bss
.align 3
_sched_stack_base:
    .space (256 * 4)
_sched_stack_top:

.section .text

.global scheduler_start_asm
scheduler_start_asm:
    mov   sp, r0
    ldr   r1, [sp], #4
    msr   spsr_cxsf, r1
    ldmfd sp!, {r0-r12, lr, pc}^

.global scheduler_switch_from_irq
scheduler_switch_from_irq:
    mrs   r0, spsr

    set_mode MODE_SVC
    mov   r5, lr
    set_mode MODE_IRQ
    mov   r1, sp
    add   sp, sp, #(14 * 4)
    set_mode MODE_SVC

    /*
     * Build saved task frame in SVC stack:
     *   SPSR, r0-r12, lr_svc, pc
     */
    sub   sp, sp, #(16 * 4)
    str   r0, [sp]

    add   r2, sp, #4
    mov   r3, #13
.Lcopy_irq_to_svc_regs:
    ldr   r0, [r1], #4
    str   r0, [r2], #4
    subs  r3, r3, #1
    bne   .Lcopy_irq_to_svc_regs

    str   r5, [r2], #4
    ldr   r0, [r1], #4
    str   r0, [r2], #4

    mov   r4, sp
    ldr   sp, =_sched_stack_top

    bl    scheduler_get_current_sp_ptr
    str   r4, [r0]

    bl    scheduler_tick
    bl    scheduler_get_next_sp

    ldr   r1, [r0], #4

    set_mode MODE_SVC
    ldr   lr, [r0, #(13 * 4)]
    set_mode MODE_IRQ

    msr   spsr_cxsf, r1

    sub   sp, sp, #(14 * 4)
    mov   r2, sp
    mov   r3, #13
.Lcopy_svc_to_irq_regs:
    ldr   r1, [r0], #4
    str   r1, [r2], #4
    subs  r3, r3, #1
    bne   .Lcopy_svc_to_irq_regs

    add   r0, r0, #4
    ldr   r1, [r0], #4
    str   r1, [r2], #4

    set_mode MODE_SVC
    mov   sp, r0
    set_mode MODE_IRQ

    ldmfd sp!, {r0-r12, pc}^