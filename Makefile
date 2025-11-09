ifeq ($(wildcard $(PROJECT_DIR)),)
$(error "PROJECT_DIR not found!")
endif

CONFIG_FILE ?= $(PROJECT_DIR)/.config

NOCONFIG_TARGETS := menuconfig help format format-check tidy check
ifeq ($(filter $(NOCONFIG_TARGETS),$(MAKECMDGOALS)),)
  ifeq ($(wildcard $(CONFIG_FILE)),)
    $(error "$(CONFIG_FILE) not found! Please run 'make menuconfig' first.")
  endif
endif

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
RTOS_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))

ifneq ($(wildcard $(CONFIG_FILE)),)
include $(CONFIG_FILE)
TOOLCHAIN_FILE ?= $(subst ",,$(CONFIG_TOOLCHAIN_FILE))
LINKER_SCRIPT ?= $(subst ",,$(CONFIG_LINKER_SCRIPT))
ARCH ?= $(subst ",,$(CONFIG_ARCH))
endif

BUILD_DIR ?= $(PROJECT_DIR)/build
TARGET ?= $(PROJECT_DIR)/$(notdir $(PROJECT_DIR)) 

OBJS := 
DIRS := $(ARCH) $(DIRS)

ALL_OBJS := $(OBJS)
CLEAN_FILES := $(TARGET) $(OBJS)
DIST_CLEAN_FILES := $(OBJS)


rwildcard=$(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2))$(filter $(subst *,%,$(2)),$(d)))
FORMAT_SRCS := $(call rwildcard,$(DIRS),*.c) $(call rwildcard,$(DIRS),*.cpp) $(call rwildcard,$(DIRS),*.h)

default: show-info all

$(TARGET): build-subdirs $(OBJS) find-all-objs
	@mkdir -p $(dir $@)
	@echo -e "\t" CC $(CFLAGS) $(ALL_OBJS) -o $@
	@$(CC) $(CFLAGS) $(ALL_OBJS) -T $(LINKER_SCRIPT) -o $@

.PHONY: all
all: $(TARGET)
	@echo -ne "\n\n"
	@echo Target $(TARGET) build finished.

.PHONY: clean
clean: clean-subdirs
	@echo CLEAN $(CLEAN_FILES)
	@rm -f $(CLEAN_FILES)
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TARGET)

.PHONY: find-all-objs
find-all-objs:
	$(eval ALL_OBJS := $(sort $(call rwildcard,$(BUILD_DIR)/,*.o)))

show-info:
	@echo Building $(notdir $(PROJECT_DIR))
	@echo Toolchain Makefile \(TOOLCHAIN_FILE\): $(TOOLCHAIN_FILE)
	@echo Linker script \(LINKER_SCRIPT\): $(LINKER_SCRIPT)
	@echo Folder with architecture specifics \(ARCH\): $(ARCH)
	@echo Final build directory \(BUILD_DIR\): $(BUILD_DIR)
	@echo Final binary \(TARGET\): $(TARGET)
	@echo RTOS Makefile path \(RTOS_PATH\): $(RTOS_PATH)
	@echo C compiler \(CC\): $(CC)
	@echo C++ compiler \(CXX\): $(CXX)
	@echo Assembler \(AS\): $(AS)
	@echo Archive tool \(AR\): $(AR)
	@echo Linker \(LD\): $(LD)
	@echo Strip tool \(STRIP\): $(STRIP)
	@echo -ne "\n\n"

.PHONY: menuconfig 
menuconfig:
	MENUCONFIG_STYLE=aquatic menuconfig $(RTOS_PATH)/Kconfig

.PHONY: format format-check tidy check
format:
	@echo "Running clang-format on sources..."
	@for src in $(FORMAT_SRCS); do \
		[ -f $$src ] && clang-format -i $$src && echo "Formatted $$src"; \
	done
	@echo "clang-format done."

format-check:
	@echo "Checking formatting..."
	@for src in $(FORMAT_SRCS); do \
		[ -f $$src ] && clang-format --dry-run --Werror $$src && echo "Checked $$src"; \
	done
	@echo "clang-format check complete."

tidy:
	@echo "Running clang-tidy on sources..."
	@for src in $(FORMAT_SRCS); do \
		[ -f $$src ] && clang-tidy $$src -- -I$(PROJECT_DIR) $(CFLAGS) $(CXXFLAGS) && echo "Checked $$src"; \
	done
	@echo "clang-tidy done."

check: format-check tidy
	@echo "All code style and static analysis checks passed."

export BUILD_DIR
export LINKER_SCRIPT
export TOOLCHAIN_FILE
export PROJECT_DIR
export RTOS_PATH
export CONFIG_FILE
export MAKE_INCLUDE=$(RTOS_PATH)/config/make.mk
export SUB_MAKE_INCLUDE=$(RTOS_PATH)/config/submake.mk

ifneq ($(wildcard $(CONFIG_FILE)),)
include $(MAKE_INCLUDE)
endif
