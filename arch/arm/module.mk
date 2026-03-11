ifneq ($(ARCH),arm)
  $(info [arch/arm] Skipping ARCH=$(ARCH))
else

ARCH_ARM_DIR := arch/arm
SRCS += $(ARCH_ARM_DIR)/startup.S
INCLUDES += -I$(ARCH_ARM_DIR)

endif
