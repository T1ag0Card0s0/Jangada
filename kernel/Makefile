KERNEL_RTOS_BUILD_DIR = $(RTOS_BUILD_DIR)/kernel
SRCS := src/queue.c
OBJS := $(SRCS:%=$(KERNEL_RTOS_BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
TARGET := $(KERNEL_RTOS_BUILD_DIR)/libjangada.a

KERNEL_INCLUDE_DIR := kernel/include

INCLUDES := -I$(RTOS_PATH)/kernel/include
CFLAGS := -MMD -MP $(INCLUDES)

all: $(TARGET)

install:
	cp -r $(RTOS_PATH)/$(KERNEL_INCLUDE_DIR)/* $(PREFIX)/usr/include/kernel
	cp -r $(TARGET) $(PREFIX)/usr/lib

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	@echo [AR] $(subst $(RTOS_BUILD_DIR)/,,$@)
	$(AR) rcs $@ $^

clean:
	rm -f $(OBJS)

include $(RTOS_PATH)/make/rules.mk

-include $(DEPS)