ifneq ($(BOARD),qemu_virt)
  $(info [bsp/qemu_virt] Skipping BOARD=$(BOARD))
else

ifneq ($(MCU),pc_i386)
  $(info [bsp/qemu_virt/pc_i386] Skipping MCU=$(MCU))
else

BSP_DIR := bsp/qemu_virt/pc_i386
SRCS += $(BSP_DIR)/tty.c
INCLUDES += -I$(BSP_DIR)
LINKER_FILE = $(BSP_DIR)/linker.ld
endif
endif
