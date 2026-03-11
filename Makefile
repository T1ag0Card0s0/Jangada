.DEFAULT_GOAL := all

PROJECT := jangada
VERSION := 0.1.0

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
MAP_DIR := $(BUILD_DIR)/map

ARCH ?= aarch64
MCU ?= cortex-a53
BOARD ?= qemu_virt
TRIPLET ?= aarch64-none-elf-

CC := $(TRIPLET)gcc
AS := $(TRIPLET)as
AR := $(TRIPLET)ar
LD := $(TRIPLET)gcc
OBJCOPY := $(TRIPLET)objcopy
OBJDUMP := $(TRIPLET)objdump
SIZE := $(TRIPLET)size
NM := $(TRIPLET)nm

COMMON_FLAGS := \
    -Wall \
    -Wextra \
    -Werror \
    -ffunction-sections \
    -fdata-sections \
    -fno-common \
    -ffreestanding \
    -nostdlib \
    -DPROJECT_VERSION=\"$(VERSION)\" \
    -DARCH_$(shell echo $(ARCH) | tr a-z A-Z) \
    -DMCU_$(shell echo $(MCU) | tr a-z A-Z | tr - _) \
    -DBOARD_$(shell echo $(BOARD) | tr a-z A-Z | tr - _)

CFLAGS := $(COMMON_FLAGS) \
    -std=c11 \
    -Os \
    -g3 \
    -MMD -MP

ASFLAGS := $(COMMON_FLAGS) \
    -g3

LDFLAGS := \
    $(COMMON_FLAGS) \
    -T arch/$(ARCH)/linker.ld \
    -Wl,--gc-sections \
    -Wl,-Map=$(MAP_DIR)/$(PROJECT).map \
    -Wl,--print-memory-usage \
    -Wl,--cref

INCLUDES := \
    -I. \
    -Iinclude \
    -Iarch/$(ARCH) \
    -Iconfig

MODULE_MKS := $(shell find . -name "module.mk" -not -path "$(BUILD_DIR)/*" -not -path "./.git/*" | sort)

# Accumulators — modules append to these
SRCS :=
OBJS :=
LIBS :=
DEPS :=

ifeq ($(strip $(MODULE_MKS)),)
  $(warning No module.mk files found under subdirectories!)
endif

include $(MODULE_MKS)

# OBJS and DEPS are computed HERE, after all module.mk files have been
# included and fully appended to SRCS. Using := (immediate) is correct
# because SRCS is now complete.
C_SRCS   := $(filter %.c, $(SRCS))
AS_SRCS_s := $(filter %.s, $(SRCS))
AS_SRCS_S := $(filter %.S, $(SRCS))

OBJS += $(patsubst %.c, $(OBJ_DIR)/%.o, $(C_SRCS))
OBJS += $(patsubst %.s, $(OBJ_DIR)/%.o, $(AS_SRCS_s))
OBJS += $(patsubst %.S, $(OBJ_DIR)/%.o, $(AS_SRCS_S))

DEPS += $(OBJS:.o=.d)

# Sanity check — fail early with a clear message rather than a silent no-op
ifeq ($(strip $(OBJS)),)
  $(error OBJS is empty — no sources were collected from module.mk files. \
  Run: find . -name "module.mk" -not -path "./build/*" to verify discovery)
endif

ELF := $(BIN_DIR)/$(PROJECT).elf
BIN := $(BIN_DIR)/$(PROJECT).bin
HEX := $(BIN_DIR)/$(PROJECT).hex
LST := $(BIN_DIR)/$(PROJECT).lst

.PHONY: all
all: $(ELF) $(BIN) $(HEX) $(LST) size

.PHONY: directories
directories:
	@mkdir -p $(BIN_DIR) $(MAP_DIR)

# C sources, each rule creates its own obj subdirectory
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Assembly
$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	@echo "  AS      $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Assembly
$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@

$(ELF): $(OBJS) $(LIBS)
	@mkdir -p $(dir $@) $(MAP_DIR)
	@echo "  LD      $@"
	@$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

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

.PHONY: flash
flash: $(BIN)
	openocd \
	  -f interface/stlink.cfg \
	  -f target/stm32f4x.cfg \
	  -c "program $(BIN) verify reset exit 0x08000000"

.PHONY: debug
debug: $(ELF)
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg &
	$(TRIPLET)gdb -tui $(ELF) \
	  -ex "target remote :3333" \
	  -ex "monitor reset halt" \
	  -ex "load"

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
	@for m in $(MODULE_MKS);   do echo "  $$m"; done
	@echo ""
	@echo "=== Raw SRCS accumulator ==="
	@for s in $(SRCS);         do echo "  $$s"; done
	@echo ""
	@echo "=== C sources ==="
	@for s in $(C_SRCS);       do echo "  $$s"; done
	@echo ""
	@echo "=== ASM sources (.s) ==="
	@for s in $(AS_SRCS_s);    do echo "  $$s"; done
	@echo ""
	@echo "=== ASM sources (.S) ==="
	@for s in $(AS_SRCS_S);    do echo "  $$s"; done
	@echo ""
	@echo "=== Object files ==="
	@for o in $(OBJS);         do echo "  $$o"; done
	@echo ""
	@echo "=== Static libraries ==="
	@for l in $(LIBS);         do echo "  $$l"; done
	@echo ""

.PHONY: help
help:
	@echo ""
	@echo "Jangada Build System"
	@echo "--------------------"
	@echo "  make               Build all"
	@echo "  make info          Show discovered modules, sources, objects"
	@echo "  make flash         Flash via OpenOCD"
	@echo "  make debug         GDB + OpenOCD"
	@echo "  make size          Memory usage"
	@echo "  make symbols       Symbol table by size"
	@echo "  make disasm        Disassemble ELF"
	@echo "  make clean         Remove build/"
	@echo "  make distclean     Remove build/ + tags"
	@echo ""
	@echo "  ARCH=$(ARCH)   MCU=$(MCU)   BOARD=$(BOARD)"
	@echo ""

-include $(DEPS)

