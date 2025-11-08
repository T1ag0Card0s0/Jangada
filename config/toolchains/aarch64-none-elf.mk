PREFIX := /opt/aarch64-none-elf/bin/aarch64-none-elf-

CC     := $(PREFIX)gcc
CXX    := $(PREFIX)g++
AR     := $(PREFIX)ar
LD     := $(PREFIX)ld
STRIP  := $(PREFIX)strip
AS     := $(CC)

CFLAGS   := -Wall -O2 -ffreestanding -nostdlib -nostartfiles -fno-builtin
CXXFLAGS := -Wall -O2 -std=c++17 -ffreestanding -nostdlib -nostartfiles -fno-builtin

LDFLAGS  := -nostdlib -nostartfiles -ffreestanding
