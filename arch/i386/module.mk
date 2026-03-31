ifneq ($(ARCH),i386)
  $(info [arch/i386] Skipping ARCH=$(ARCH))
else

ARCH_I386_DIR := arch/i386
SRCS += $(ARCH_I386_DIR)/boot.S $(ARCH_I386_DIR)/crti.S $(ARCH_I386_DIR)/crtn.S
INCLUDES += -I$(ARCH_I386_DIR)

endif
