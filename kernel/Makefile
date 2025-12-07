SRCS := src/queue.c
OBJS := $(SRCS:%=$(KERNEL_BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
TARGET := $(KERNEL_BUILD_DIR)/libjangada.a

KERNEL_INCLUDE_DIR := kernel/include

INCLUDES := -I$(RTOS_PATH)/kernel/include
CFLAGS := -MMD -MP $(INCLUDES)

all: $(TARGET)

install:
	cp -r $(RTOS_PATH)/$(KERNEL_INCLUDE_DIR)/* $(PREFIX)/usr/include/kernel
	cp -r $(TARGET) $(PREFIX)/usr/lib

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	@echo [AR] $(subst $(BUILD_DIR)/,,$@)
	$(AR) rcs $@ $^

clean:
	rm -f $(OBJS)

include $(RTOS_PATH)/make/rules.mk

-include $(DEPS)