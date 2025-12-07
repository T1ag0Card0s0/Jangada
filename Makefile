mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
RTOS_PATH = $(patsubst %/,%,$(dir $(mkfile_path)))
include $(RTOS_PATH)/make/helpers.mk

BUILD_DIR ?= $(CURDIR)/build
PREFIX ?= $(BUILD_DIR)/sysroot
RTOS_BUILD_DIR = $(BUILD_DIR)/jangada

REQUIRED_VARS := BUILD_DIR ARCH BOARD TRIPLET
$(call check-required-vars,$(REQUIRED_VARS))

include $(RTOS_PATH)/make/globals.mk

RTOS_DIRS = $(RTOS_PATH)/bsps/$(ARCH) \
			$(RTOS_PATH)/libc \
			$(RTOS_PATH)/kernel

RTOS_INCLUDE_DIR := $(RTOS_PATH)/kernel/include \
					$(RTOS_PATH)/libc/include

all: build-dirs install

sysroot:
	mkdir -p $(PREFIX)/usr/{lib,include/{kernel,sys,bsp}}

install: install-dirs

clean: clean-dirs

format: format-dirs

build-dirs: sysroot
	for dir in $(RTOS_DIRS); do \
		$(MAKE) -C $$dir all; \
	done

clean-dirs:
	for dir in $(RTOS_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done

install-dirs:
	for dir in $(RTOS_DIRS); do \
		$(MAKE) -C $$dir install; \
	done

format-dirs:
	for dir in $(RTOS_DIRS); do \
		$(MAKE) -C $$dir format; \
	done

.PHONY: all clean test format
