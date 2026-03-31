.equ GICC_BASE,   0x08010000
.equ GICC_IAR,    0x00C
.equ GICC_EOIR,   0x010

.equ TIMER_IRQ,   30
.equ UART0_IRQ,   33

.section .text

.global irq_handler
irq_handler:
    sub   lr, lr, #4
    stmfd sp!, {r0-r12, lr}

    ldr   r0, =GICC_BASE
    ldr   r4, [r0, #GICC_IAR]

    ldr   r2, =0x3FF
    and   r1, r4, r2

    cmp   r1, #TIMER_IRQ
    beq   _dispatch_timer

    cmp   r1, #UART0_IRQ
    beq   _dispatch_uart0

    cmp   r1, r2
    beq   _irq_eoi

    mov   r0, r1
    bl    irq_unhandled
    b     _irq_eoi

_dispatch_timer:
    bl    timer_irq_handler

    ldr   r0, =GICC_BASE
    str   r4, [r0, #GICC_EOIR]

    b     scheduler_switch_from_irq

_dispatch_uart0:
    bl    uart0_irq_handler
    b     _irq_eoi

_irq_eoi:
    ldr   r0, =GICC_BASE
    str   r4, [r0, #GICC_EOIR]
    ldmfd sp!, {r0-r12, pc}^

.global svc_handler
svc_handler:
    stmfd sp!, {r0-r12, lr}
    ldr   r0, [lr, #-4]
    bic   r0, r0, #0xFF000000
    bl    svc_c_handler
    ldmfd sp!, {r0-r12, pc}^

.global undef_handler
undef_handler:
    stmfd sp!, {r0-r12, lr}
    bl    undef_c_handler
    ldmfd sp!, {r0-r12, pc}^

.global prefetch_abort_handler
prefetch_abort_handler:
    sub   lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl    prefetch_abort_c_handler
    ldmfd sp!, {r0-r12, pc}^

.global data_abort_handler
data_abort_handler:
    sub   lr, lr, #8
    stmfd sp!, {r0-r12, lr}
    bl    data_abort_c_handler
    ldmfd sp!, {r0-r12, pc}^

.global reserved_handler
reserved_handler:
    b .

.global fiq_handler
fiq_handler:
    sub   lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl    fiq_c_handler
    ldmfd sp!, {r0-r12, pc}^

.weak timer_irq_handler
timer_irq_handler:
    bx lr

.weak uart0_irq_handler
uart0_irq_handler:
    bx lr

.weak irq_unhandled
irq_unhandled:
    bx lr

.weak svc_c_handler
svc_c_handler:
    bx lr

.weak undef_c_handler
undef_c_handler:
    b .

.weak prefetch_abort_c_handler
prefetch_abort_c_handler:
    b .

.weak data_abort_c_handler
data_abort_c_handler:
    b .

.weak fiq_c_handler
fiq_c_handler:
    bx lr
