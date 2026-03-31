ifneq ($(ARCH),armv7a)
  $(info [arch/armv7a] Skipping ARCH=$(ARCH))
else

ARCH_ARMV7A_DIR := arch/arm/armv7a
SRCS += $(ARCH_ARMV7A_DIR)/startup.s \
        $(ARCH_ARMV7A_DIR)/scheduler_switch.s \
        $(ARCH_ARMV7A_DIR)/task_context.c \
        $(ARCH_ARMV7A_DIR)/vectors.s
INCLUDES += -I$(ARCH_ARMV7A_DIR)

endif