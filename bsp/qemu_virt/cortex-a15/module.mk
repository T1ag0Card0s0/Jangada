ifneq ($(BOARD),qemu_virt)
  $(info [bsp/qemu_virt] Skipping BOARD=$(BOARD))
else

ifneq ($(MCU),cortex-a15)
  $(info [bsp/qemu_virt/cortex-a15] Skipping MCU=$(MCU))
else

BSP_DIR := bsp/qemu_virt/cortex-a15
SRCS += $(BSP_DIR)/gic.c \
        $(BSP_DIR)/irq_handlers.s \
        $(BSP_DIR)/platform.c \
        $(BSP_DIR)/timer.c \
        $(BSP_DIR)/tty.c
INCLUDES += -I$(BSP_DIR)
LINKER_FILE = $(BSP_DIR)/linker.ld

endif
endif