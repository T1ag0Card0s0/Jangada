ARCH_RTOS_BUILD_DIR = $(RTOS_BUILD_DIR)/arch
SRCS = aarch64/startup.S
OBJS = $(SRCS:%=$(ARCH_RTOS_BUILD_DIR)/%.o)
TARGET = $(ARCH_RTOS_BUILD_DIR)/libarch.a

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	@echo [AR] $(subst $(RTOS_BUILD_DIR)/,,$@)
	$(AR) rcs $@ $^

install: all
	cp $(TARGET) $(PREFIX)/usr/lib

clean:
	rm $(OBJS)

include $(COMPILE_RULES) 

.PHONY: all install clean
