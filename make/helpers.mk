print-help = \
    $(info Required variables: BUILD_DIR, ARCH, BOARD, TRIPLET) \
    $(info ARCH      BOARD) \
    $(info aarch64   qemu_virt) 


check-required-vars = \
    $(foreach v,$(1), \
        $(if $(strip $($(v))),, \
            $(call print-help); \
            $(error Variable '$(v)' is empty) \
        ) \
    )
