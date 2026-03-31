.section .vectors, "ax"
.global _vectors

_vectors:
    ldr pc, =reset_handler
    ldr pc, =undef_handler
    ldr pc, =svc_handler
    ldr pc, =prefetch_abort_handler
    ldr pc, =data_abort_handler
    ldr pc, =reserved_handler
    ldr pc, =irq_handler
    ldr pc, =fiq_handler
