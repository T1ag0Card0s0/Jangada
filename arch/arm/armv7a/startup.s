.equ MODE_FIQ,       0x11
.equ MODE_IRQ,       0x12
.equ MODE_SVC,       0x13
.equ MODE_ABT,       0x17
.equ MODE_UND,       0x1B
.equ IF_BITS,        0xC0

.equ STACK_SVC_SIZE, 0x2000
.equ STACK_IRQ_SIZE, 0x1000
.equ STACK_ABT_SIZE, 0x0800
.equ STACK_UND_SIZE, 0x0800

.section .text
.global reset_handler
.extern kernel_main
.extern _vectors
.extern _sbss, _ebss
.extern _sdata, _edata, _etext
.extern _stack_top

reset_handler:
    msr cpsr_c, #(MODE_IRQ | IF_BITS)
    ldr sp, =_stack_top
    sub sp, sp, #STACK_SVC_SIZE

    msr cpsr_c, #(MODE_ABT | IF_BITS)
    ldr sp, =_stack_top
    sub sp, sp, #(STACK_SVC_SIZE + STACK_IRQ_SIZE)

    msr cpsr_c, #(MODE_UND | IF_BITS)
    ldr sp, =_stack_top
    sub sp, sp, #(STACK_SVC_SIZE + STACK_IRQ_SIZE + STACK_ABT_SIZE)

    msr cpsr_c, #(MODE_FIQ | IF_BITS)
    ldr sp, =_stack_top
    sub sp, sp, #(STACK_SVC_SIZE + STACK_IRQ_SIZE + STACK_ABT_SIZE + STACK_UND_SIZE)

    msr cpsr_c, #(MODE_SVC | IF_BITS)
    ldr sp, =_stack_top

    ldr r0, =_vectors
    mcr p15, 0, r0, c12, c0, 0
    isb

    ldr  r0, =_sbss
    ldr  r1, =_ebss
    mov  r2, #0
.Lbss_loop:
    cmp  r0, r1
    strlt r2, [r0], #4
    blt  .Lbss_loop

    ldr  r0, =_sdata
    ldr  r1, =_edata
    ldr  r2, =_etext
.Ldata_loop:
    cmp  r0, r1
    ldrlt r3, [r2], #4
    strlt r3, [r0], #4
    blt  .Ldata_loop

    bl   kernel_main
    b    .
