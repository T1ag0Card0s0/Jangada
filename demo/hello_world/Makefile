MAKEFLAGS += --no-print-directory
MAKEFLAGS += -s

BUILD_DIR := $(CURDIR)/build

TRIPLET := aarch64-none-elf-
CC := $(TRIPLET)gcc
AS := $(TRIPLET)as
CXX := $(TRIPLET)g++
LD := $(TRIPLET)ld
AR := $(TRIPLET)ar
ARCH := aarch64
BOARD := qemu_virt

CFLAGS := --sysroot=$(BUILD_DIR)/sysroot -Wall -Os -ffreestanding -nostdlib -nostartfiles -fno-builtin -ffunction-sections -fdata-sections -MMD -MP 
LDFLAGS := --sysroot=$(BUILD_DIR)/sysroot -nostdlib -nostartfiles -ffreestanding -Wl,--gc-sections
LDFLAGS += -lc -ljangada -lbsp

SRCS := src/main.c
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

LINKER_SCRIPT := $(BUILD_DIR)/jangada/bsps/aarch64/qemu_virt/linker.ld

TARGET := $(BUILD_DIR)/hello_world.elf

RUN_aarch64_qemu_virt := qemu-system-aarch64 -machine virt -cpu cortex-a53 -m 128M -nographic -kernel $(TARGET)

all: $(TARGET)

$(TARGET): Jangada $(OBJS)
	@echo [LD] $(OBJS) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@

	$(CC) $(OBJS) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@

Jangada:
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

.PHONY: all clean Jangada run
-include $(DEPS)