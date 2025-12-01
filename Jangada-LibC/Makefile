REQUIRED_VARS := BUILD_DIR TRIPLET ARCH BOARD
empty := $(foreach v,$(REQUIRED_VARS),\
	$(if $(strip $($(v))),,\
		$(error Variable '$(v)' is empty)\
	)\
)

CC = $(TRIPLET)gcc
AS = $(TRIPLET)as
CXX = $(TRIPLET)g++
LD = $(TRIPLET)ld
AR = $(TRIPLET)ar

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
LIBC_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))

LIBC_BUILD_DIR := $(BUILD_DIR)/Jangada-LibC

SRCS := string/memcpy.c
OBJS := $(SRCS:%=$(LIBC_BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
TARGET := $(LIBC_BUILD_DIR)/tinylibc.a

INCLUDES := -I$(LIBC_PATH)/include
CFLAGS := -MMD -MP $(INCLUDES)

all: install $(TARGET)

install:
	mkdir -p $(LIBC_BUILD_DIR)/include
	cp -r $(LIBC_PATH)/include/* $(LIBC_BUILD_DIR)/include/

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	@echo [AR] $(subst $(BUILD_DIR)/,,$@)
	$(AR) rcs $@ $^

$(LIBC_BUILD_DIR)/%.c.o: $(LIBC_PATH)/%.c
	mkdir -p $(dir $@)
	@echo [CC] $(subst $(BUILD_DIR)/,,$@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS)

-include $(DEPS)