NO_REQUIRED_VARS_TARGETS := format
ifeq ($(filter $(NO_REQUIRED_VARS_TARGETS),$(MAKECMDGOALS)),)
REQUIRED_VARS := BUILD_DIR TRIPLET ARCH BOARD
empty := $(foreach v,$(REQUIRED_VARS),\
    $(if $(strip $($(v))),,\
        $(error Variable '$(v)' is empty)\
    )\
)
endif

CC = $(TRIPLET)gcc
AS = $(TRIPLET)as
CXX = $(TRIPLET)g++
LD = $(TRIPLET)ld
AR = $(TRIPLET)ar

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
RTOS_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))

RTOS_BUILD_DIR := $(BUILD_DIR)/Jangada-RTOS
LINKER_SCRIPT := $(RTOS_PATH)/boards/$(BOARD)/linker.ld

SRCS := kernel/queue.c
ifeq ($(ARCH),aarch64)
SRCS += arch/aarch64/startup.S
endif

OBJS := $(SRCS:%=$(RTOS_BUILD_DIR)/%.o)

RTOS_INCLUDE_DIR := $(RTOS_PATH)/include
RTOS_SRCS := $(SRCS:%=$(RTOS_PATH)/%)

all: install $(OBJS)

$(RTOS_BUILD_DIR)/%.c.o: $(RTOS_PATH)/%.c 
	mkdir -p $(dir $@)
	@echo [CC] $(subst $(BUILD_DIR)/,,$@)
	$(CC) $(CFLAGS) -c $< -o $@

$(RTOS_BUILD_DIR)/%.S.o: $(RTOS_PATH)/%.S 
	mkdir -p $(dir $@)
	@echo [AS] $(subst $(BUILD_DIR)/,,$@)
	$(AS) $(ASFLAGS) -c $< -o $@

install:
	mkdir -p $(RTOS_BUILD_DIR)/include
	mkdir -p $(RTOS_BUILD_DIR)/linker_sript
	cp $(LINKER_SCRIPT) $(RTOS_BUILD_DIR)/linker_sript/linker.ld

clean:
	rm $(OBJS)

format:
	@echo "Running clang-format on sources..."
	@for src in $(RTOS_SRCS); do \
		if [ -f $$src ]; then \
			clang-format -i -style=file $$src && echo "Formatted $$src"; \
		fi; \
	done
	@echo "clang-format done."


.PHONY: all clean format