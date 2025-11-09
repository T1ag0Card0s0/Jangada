PREFIX := /opt/aarch64-none-elf/bin/aarch64-none-elf-

CC     := $(PREFIX)gcc
CXX    := $(PREFIX)g++
AR     := $(PREFIX)ar
LD     := $(PREFIX)ld
STRIP  := $(PREFIX)strip
AS     := $(CC)

CFLAGS   := $(CFLAGS) -Wall -Os -ffreestanding -nostdlib -nostartfiles -fno-builtin -ffunction-sections -fdata-sections
CXXFLAGS := $(CXXFLAGS) -Wall -Os -std=c++17 -ffreestanding -nostdlib -nostartfiles -fno-builtin -ffunction-sections -fdata-sections

LDFLAGS := -nostdlib -nostartfiles -ffreestanding -Wl,--gc-sections

