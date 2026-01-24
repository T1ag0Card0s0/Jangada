mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
export PROJECT_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))
include config/default.config
include config/run.global

REQUIRED_VARS := TRIPLET ARCH
$(call check-required-vars,$(REQUIRED_VARS))

BUILD_DIR ?= build
TARGET := $(BUILD_DIR)/jangada
DIRS := arch/$(ARCH) kernel lib init drivers
LIB_DIRS := lib
NON_LIB_DIRS := $(filter-out $(LIB_DIRS),$(DIRS))

LDSCRIPT := include/arch/$(ARCH)/linker.ld
LDFLAGS := -T $(LDSCRIPT) -nostdlib -nostartfiles -ffreestanding -Wl,--gc-sections
export CFLAGS = -ffreestanding -I$(PROJECT_PATH)/lib/libc/include
rwildcard=$(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2))$(filter $(subst *,%,$(2)),$(d)))

default: show-info all

$(TARGET): build-subdirs find-all-objs find-all-libs
	@echo -e "\t" LD $(LDFLAGS) $(ALL_OBJS) $(ALL_LIBS) -o $@
	@$(CC) $(LDFLAGS) $(ALL_OBJS) $(ALL_LIBS) -o $@

all: $(TARGET)
	@echo Target $(TARGET) build finished.

clean: clean-subdirs
	@if [ -f "$(TARGET)" ] || [ -d "$(BUILD_DIR)" ]; then \
		echo "CLEAN $(TARGET) $(BUILD_DIR)"; \
		rm -f $(TARGET); \
		rm -rf $(BUILD_DIR); \
	fi

format: format-subdirs
	@echo Format complete.

tidy: tidy-subdirs
	@echo Tidy analysis complete.

find-all-objs:
	$(eval ALL_OBJS := $(call rwildcard,$(addprefix $(BUILD_DIR)/,$(NON_LIB_DIRS)),*.o))

find-all-libs:
	$(eval ALL_LIBS := $(call rwildcard,$(addprefix $(BUILD_DIR)/,$(LIB_DIRS)),*.a))

show-info:
	@echo Building Project

install: all install-subdirs
	mkdir -p $(SYSROOT)/boot
	cp $(TARGET) $(SYSROOT)/boot/jangada.elf

run: all 
	$(RUN_$(ARCH)_$(BOARD))

.PHONY: all clean show-info find-all-objs find-all-libs install install-subdirs format tidy tidy-check

export BUILD_DIR := $(PROJECT_PATH)/$(BUILD_DIR)
export MAKE_INCLUDE=$(PROJECT_PATH)/config/make.global
export SUB_MAKE_INCLUDE=$(PROJECT_PATH)/config/submake.global
export SYSROOT ?= $(BUILD_DIR)/sysroot
export PREFIX  ?= /usr
export INCLUDEDIR := $(PREFIX)/include
export LIBDIR := $(PREFIX)/lib
export BINDIR := $(PREFIX)/bin

include $(MAKE_INCLUDE)
include $(PROJECT_PATH)/config/triplet.global
include $(PROJECT_PATH)/config/helpers.global
