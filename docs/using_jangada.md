# Building on top of Jangada

Template Makefile.

```
BUILD_DIR := $(CURDIR)/build

CFLAGS := --sysroot=$(BUILD_DIR)/sysroot -ffreestanding -nostdlib -nostartfiles -fno-builtin -ffunction-sections -fdata-sections
LDFLAGS := --sysroot=$(BUILD_DIR)/sysroot -nostdlib -nostartfiles -ffreestanding

LDFLAGS += -lc -ljangada -larch

SRCS := $(BUILD_DIR)/src/main.c.o

LINKER_SCRIPT := $(BUILD_DIR)/jangada/bsps/aarch64/qemu_virt/linker.ld

TARGET := $(BUILD_DIR)/hello_world.elf

all: $(TARGET)

$(TARGET): Jangada $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@

Jangada:
	$(MAKE) -C ../../ all BUILD_DIR=$(BUILD_DIR) TRIPLET=aarch64-none-elf- ARCH=aarch64 BOARD=qemu_virt

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	aarch64-none-elf-gcc -DHELLO_WORLD_MESSAGE="\"Hello World!"\" $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean Jangada
```