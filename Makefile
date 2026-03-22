.DEFAULT_GOAL := all

PROJECT := jangada
VERSION := 0.1.0

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
SIZE := $(TRIPLET)size
NM := $(TRIPLET)nm
GDB := $(TRIPLET)gdb
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

MODULE_MKS := $(shell find . -name "module.mk" -not -path "$(BUILD_DIR)/*" -not -path "./.git/*" | sort)

SRCS :=
OBJS :=
LIBS :=
DEPS :=

ifeq ($(strip $(MODULE_MKS)),)
  $(warning No module.mk files found under subdirectories!)
endif

include $(MODULE_MKS)

C_SRCS    := $(filter %.c,$(SRCS))
AS_SRCS_s := $(filter %.s,$(SRCS))
AS_SRCS_S := $(filter %.S,$(SRCS))

OBJS += $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRCS))
OBJS += $(patsubst %.s,$(OBJ_DIR)/%.o,$(AS_SRCS_s))
OBJS += $(patsubst %.S,$(OBJ_DIR)/%.o,$(AS_SRCS_S))

DEPS += $(OBJS:.o=.d)

ifeq ($(strip $(OBJS)),)
  $(error OBJS is empty — no sources were collected from module.mk files. Run: find . -name "module.mk" -not -path "./build/*")
endif

ELF := $(BIN_DIR)/$(PROJECT).elf
BIN := $(BIN_DIR)/$(PROJECT).bin
HEX := $(BIN_DIR)/$(PROJECT).hex
LST := $(BIN_DIR)/$(PROJECT).lst

.PHONY: all
all: $(ELF) $(BIN) $(HEX) $(LST) size

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

.PHONY: size
size: $(ELF)
	@echo ""
	@echo "====== Memory Usage ======"
	@$(SIZE) --format=berkeley $(ELF)
	@echo "=========================="

.PHONY: symbols
symbols: $(ELF)
	@$(NM) --print-size --size-sort --radix=d $(ELF)

.PHONY: disasm
disasm: $(ELF)
	@$(OBJDUMP) -d -S $(ELF) | less

.PHONY: run
run: $(ELF)
	$(QEMU) -M virt -cpu $(MCU) -kernel $(ELF) -nographic

.PHONY: debug
debug: $(ELF)
	$(QEMU) -M virt -cpu $(MCU) -kernel $(ELF) \
	        -S -gdb tcp::1234 -nographic

.PHONY: gdb
gdb: $(ELF)
	$(GDB) -tui $(ELF) \
	       -ex "target remote :1234" \
	       -ex "break reset_handler" \
	       -ex "continue"

.PHONY: clean
clean:
	@echo "  CLEAN"
	@rm -rf $(BUILD_DIR)

.PHONY: distclean
distclean: clean
	@rm -f tags cscope.*

.PHONY: info
info:
	@echo ""
	@echo "=== Module files found by find ==="
	@for m in $(MODULE_MKS); do echo "  $$m"; done
	@echo ""
	@echo "=== Raw SRCS accumulator ==="
	@for s in $(SRCS); do echo "  $$s"; done
	@echo ""
	@echo "=== C sources ==="
	@for s in $(C_SRCS); do echo "  $$s"; done
	@echo ""
	@echo "=== ASM sources (.s) ==="
	@for s in $(AS_SRCS_s); do echo "  $$s"; done
	@echo ""
	@echo "=== ASM sources (.S) ==="
	@for s in $(AS_SRCS_S); do echo "  $$s"; done
	@echo ""
	@echo "=== Object files ==="
	@for o in $(OBJS); do echo "  $$o"; done
	@echo ""
	@echo "=== Static libraries ==="
	@for l in $(LIBS); do echo "  $$l"; done
	@echo ""

.PHONY: help
help:
	@echo ""
	@echo "Jangada Build System"
	@echo "--------------------"
	@echo "  make               Build all"
	@echo "  make run           Run in QEMU"
	@echo "  make debug         Run QEMU waiting for GDB on :1234"
	@echo "  make gdb           Connect GDB to QEMU"
	@echo "  make info          Show discovered modules, sources, objects"
	@echo "  make size          Memory usage"
	@echo "  make symbols       Symbol table by size"
	@echo "  make disasm        Disassemble ELF"
	@echo "  make clean         Remove build/"
	@echo "  make distclean     Remove build/ + tags"
	@echo ""
	@echo "  ARCH=$(ARCH)   MCU=$(MCU)   BOARD=$(BOARD)"
	@echo ""

-include $(DEPS)