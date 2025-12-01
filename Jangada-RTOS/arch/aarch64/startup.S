    .section .text.boot
    .global _start
    .align  11                // 2KB alignment (for VBAR_EL1 if needed)
_start:
    // Set up stack pointer (defined in linker script)
    ldr     x0, =_stack_top_
    mov     sp, x0

    // Zero out .bss
    ldr     x1, =_bss_start_   // start of .bss
    ldr     x2, =_bss_end_     // end of .bss
    mov     x3, #0

bss_clear_loop:
    cmp     x1, x2
    b.ge    bss_clear_done
    str     x3, [x1], #8
    b       bss_clear_loop

bss_clear_done:
    // Call main()
    bl      main

    // If main returns, hang forever
hang:
    wfe
    b       hang

    // Optionally define weak symbol for vector table (if needed)
    .section .vectors, "ax"
    .align 11
    .weak _vectors_start
_vectors_start:
    // minimal default vector table (optional, EL1 synchronous handler, etc.)
    b .
