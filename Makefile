ifeq ($(wildcard $(PROJECT_DIR)),)
$(error "PROJECT_DIR not found!")
endif

CONFIG_FILE ?= $(PROJECT_DIR)/.config

NOCONFIG_TARGETS := menuconfig help
ifeq ($(filter $(NOCONFIG_TARGETS),$(MAKECMDGOALS)),)
  ifeq ($(wildcard $(CONFIG_FILE)),)
    $(error "$(CONFIG_FILE) not found! Please run 'make menuconfig' first.")
  endif
endif

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
RTOS_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))

ifneq ($(wildcard $(CONFIG_FILE)),)
include $(CONFIG_FILE)
TOOLCHAIN_FILE := $(RTOS_PATH)/$(subst ",,$(CONFIG_TOOLCHAIN_FILE))
LINKER_SCRIPT := $(RTOS_PATH)/$(subst ",,$(CONFIG_LINKER_SCRIPT))
ARCH := $(subst ",,$(CONFIG_ARCH))
endif

BUILD_DIR ?= $(PROJECT_DIR)/build
TARGET ?= $(PROJECT_DIR)/$(notdir $(PROJECT_DIR)) 

OBJS := 
DIRS := $(RTOS_PATH)/arch/$(ARCH) $(PROJECT_DIR)

ALL_OBJS := $(OBJS)
CLEAN_FILES := $(TARGET) $(OBJS)
DIST_CLEAN_FILES := $(OBJS)

rwildcard=$(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2))$(filter $(subst *,%,$(2)),$(d)))

default: show-info all

$(TARGET): build-subdirs $(OBJS) find-all-objs
	@mkdir -p $(dir $@)
	@echo -e "\t" CC $(CFLAGS) $(ALL_OBJS) -o $@
	@$(CC) $(CFLAGS) $(ALL_OBJS) -T $(LINKER_SCRIPT) -o $@

.PHONY: all
all: $(TARGET)
	@echo Target $(TARGET) build finished.

.PHONY: clean
clean: clean-subdirs
	@echo CLEAN $(CLEAN_FILES)
	@rm -f $(CLEAN_FILES)

.PHONY: distclean
distclean: clean-subdirs
	@echo CLEAN $(DIST_CLEAN_FILES)
	@rm -f $(DIST_CLEAN_FILES)
	@rm -rf $(BUILD_DIR)

.PHONY: find-all-objs
find-all-objs:
	$(eval ALL_OBJS := $(sort $(call rwildcard,$(BUILD_DIR)/,*.o)))

.PHONY: show-info
show-info:
	@echo Building $(notdir $(PROJECT_DIR))
	@echo Toolchain file: $(TOOLCHAIN_FILE)
	@echo Linker script: $(LINKER_SCRIPT)
	@echo Architecture: $(ARCH)
	@echo Build directory: $(BUILD_DIR)
	@echo Final binary file: $(TARGET)

.PHONY: menuconfig 
menuconfig:
	MENUCONFIG_STYLE=aquatic menuconfig Kconfig
	@mv .config $(PROJECT_DIR)

export BUILD_DIR
export LINKER_SCRIPT
export TOOLCHAIN_FILE
export PROJECT_DIR
export RTOS_PATH
export MAKE_INCLUDE=$(RTOS_PATH)/config/make.mk
export SUB_MAKE_INCLUDE=$(RTOS_PATH)/config/submake.mk

ifneq ($(wildcard $(CONFIG_FILE)),)
include $(MAKE_INCLUDE)
endif
