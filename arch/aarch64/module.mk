ifneq ($(ARCH),aarch64)
  $(info [arch/aarch64] Skipping ARCH=$(ARCH))
else

ARCH_AARCH64_DIR := arch/aarch64
SRCS += $(ARCH_AARCH64_DIR)/startup.S
INCLUDES += -I$(ARCH_AARCH64_DIR)

endif
