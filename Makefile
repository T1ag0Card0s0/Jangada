.DEFAULT_GOAL := all

PROJECT := jangada
VERSION := 0.1.0

SUPPORTED_ARCHS := armv7a i386
SUPPORTED_BOARDS := qemu_virt
SUPPORTED_MCUS_armv7a := cortex-a15
SUPPORTED_MCUS_i386 := pc_i386

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
MAP_DIR := $(BUILD_DIR)/map

ARCH ?= armv7a
MCU ?= cortex-a15
BOARD ?= qemu_virt
TRIPLET ?= arm-none-eabi-

CC := $(TRIPLET)gcc
AS := $(TRIPLET)as
AR := $(TRIPLET)ar
LD := $(TRIPLET)ld
OBJCOPY := $(TRIPLET)objcopy
OBJDUMP := $(TRIPLET)objdump
QEMU := qemu-system-arm

CPUFLAGS := -mcpu=$(MCU)

COMMON_FLAGS := \
    -Wall \
    -Wextra \
    -ffunction-sections \
    -fdata-sections \
    -fno-common \
    -ffreestanding \
    -nostdlib \
    -DPROJECT_VERSION=\"$(VERSION)\" \
    -DARCH_$(shell echo $(ARCH) | tr a-z A-Z) \
    -DMCU_$(shell echo $(MCU) | tr a-z A-Z | tr - _) \
    -DBOARD_$(shell echo $(BOARD) | tr a-z A-Z | tr - _)

CFLAGS := $(CPUFLAGS) \
    $(COMMON_FLAGS) \
    -O2 \
    -MMD -MP

ASFLAGS := $(CPUFLAGS)

LDFLAGS := \
    -T bsp/$(BOARD)/$(MCU)/linker.ld \
    --no-warn-rwx-segments \
    -Map=$(MAP_DIR)/$(PROJECT).map \
    --gc-sections \
    --cref

INCLUDES := \
    -I. \
    -Iinclude \
    -Iarch/$(ARCH) \
    -Iconfig

MODULE_MKS := $(shell find . -name "module.mk" \
    -not -path "$(BUILD_DIR)/*" \
    -not -path "./.git/*" | sort)

SRCS :=
OBJS :=
LIBS :=
DEPS :=

ifeq ($(strip $(MODULE_MKS)),)
  $(warning No module.mk files found under subdirectories!)
endif

include $(MODULE_MKS)

C_SRCS := $(filter %.c,$(SRCS))
AS_SRCS_s := $(filter %.s,$(SRCS))
AS_SRCS_S := $(filter %.S,$(SRCS))

OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRCS))
OBJS += $(patsubst %.s,$(OBJ_DIR)/%.o,$(AS_SRCS_s))
OBJS += $(patsubst %.S,$(OBJ_DIR)/%.o,$(AS_SRCS_S))

DEPS += $(OBJS:.o=.d)

ifeq ($(strip $(OBJS)),)
  $(error OBJS is empty — no sources were collected from module.mk files)
endif

ELF := $(BIN_DIR)/$(PROJECT).elf
BIN := $(BIN_DIR)/$(PROJECT).bin
HEX := $(BIN_DIR)/$(PROJECT).hex
LST := $(BIN_DIR)/$(PROJECT).lst

.PHONY: all clean run help

all: $(ELF) $(BIN) $(HEX) $(LST)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	@echo "  AS      $<"
	@$(AS) $(ASFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo "  AS      $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@

$(ELF): $(OBJS) $(LIBS)
	@mkdir -p $(dir $@) $(MAP_DIR)
	@echo "  LD      $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(BIN): $(ELF)
	@echo "  BIN     $@"
	@$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	@echo "  HEX     $@"
	@$(OBJCOPY) -O ihex $< $@

$(LST): $(ELF)
	@echo "  LST     $@"
	@$(OBJDUMP) -h -S $< > $@

run: $(ELF)
	$(QEMU) -M virt -cpu $(MCU) -kernel $(ELF) -nographic

clean:
	@echo "  CLEAN"
	@rm -rf $(BUILD_DIR)

help:
	@echo ""
	@echo "Jangada Build System"
	@echo "===================="
	@echo ""
	@echo "Targets:"
	@echo "  make all    Build ELF, BIN, HEX and LST"
	@echo "  make clean  Remove build artifacts"
	@echo "  make run    Run in QEMU"
	@echo "  make help   Show this help"
	@echo ""
	@echo "Current configuration:"
	@echo "  ARCH   = $(ARCH)"
	@echo "  MCU    = $(MCU)"
	@echo "  BOARD  = $(BOARD)"
	@echo "  TRIPLET= $(TRIPLET)"
	@echo ""
	@echo "Supported architectures:"
	@echo "  $(SUPPORTED_ARCHS)"
	@echo ""
	@echo "Supported boards:"
	@echo "  $(SUPPORTED_BOARDS)"
	@echo ""
	@echo "Supported MCUs by architecture:"
	@echo "  armv7a : $(SUPPORTED_MCUS_armv7a)"
	@echo "  i386   : $(SUPPORTED_MCUS_i386)"
	@echo ""

-include $(DEPS)