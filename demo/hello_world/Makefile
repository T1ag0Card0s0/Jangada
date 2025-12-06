MAKEFLAGS += --no-print-directory
MAKEFLAGS += -s

BUILD_DIR := $(CURDIR)/build

TRIPLET := /opt/aarch64-none-elf/bin/aarch64-none-elf-
CC := $(TRIPLET)gcc
AS := $(TRIPLET)as
CXX := $(TRIPLET)g++
LD := $(TRIPLET)ld
AR := $(TRIPLET)ar
ARCH := aarch64
BOARD := qemu_virt

CFLAGS := $(CFLAGS) -Wall -Os -ffreestanding -nostdlib -nostartfiles -fno-builtin -ffunction-sections -fdata-sections -MMD -MP 
LDFLAGS := -nostdlib -nostartfiles -ffreestanding -Wl,--gc-sections

SRCS := src/main.c
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

ALL_OBJS = $(shell find $(BUILD_DIR) -type f -name '*.o')
ALL_ARCHIVES = $(shell find $(BUILD_DIR) -type f -name '*.a')

TARGET := $(BUILD_DIR)/hello_world.elf
LINKER_SCRIPT := $(BUILD_DIR)/bsps/aarch64/qemu_virt/linker.ld

RUN_aarch64_qemu_virt := qemu-system-aarch64 -machine virt -cpu cortex-a53 -m 128M -nographic -kernel $(TARGET)

all: $(TARGET)

$(TARGET): Jangada-RTOS tinylibc $(OBJS)
	@echo [LD] $(subst $(BUILD_DIR)/,,$@)
	$(CC) $(ALL_OBJS) $(ALL_ARCHIVES) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@

Jangada-RTOS:
	$(MAKE) -C ../../ all \
		BUILD_DIR=$(BUILD_DIR) \
		TRIPLET=$(TRIPLET) \
		ARCH=$(ARCH) BOARD=$(BOARD)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	echo [CC] $(subst $(BUILD_DIR)/,,$@)
	$(CC) -DHELLO_WORLD_MESSAGE="\"Hello World!"\" $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: %.S 
	mkdir -p $(dir $@)
	echo [AS] $(subst $(BUILD_DIR)/,,$@)

clean:
	rm -rf $(BUILD_DIR)

run: all
	@$(RUN_$(ARCH)_$(BOARD))

.PHONY: all clean Jangada-RTOS tinylibc run
-include $(DEPS)