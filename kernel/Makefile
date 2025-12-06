KERNEL_SRCS := src/queue.c
KERNEL_OBJS := $(KERNEL_SRCS:%=$(KERNEL_BUILD_DIR)/%.o)
KERNEL_INCLUDE_DIR := kernel/include

all: $(KERNEL_OBJS) 

install: all
	mkdir -p $(KERNEL_BUILD_DIR)/include
	cp -r $(RTOS_PATH)/$(KERNEL_INCLUDE_DIR) $(KERNEL_BUILD_DIR)

clean:
	rm -f $(KERNEL_OBJS)

include $(RTOS_PATH)/make/rules.mk

.PHONY: all clean format