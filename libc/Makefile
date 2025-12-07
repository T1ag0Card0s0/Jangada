SRCS := string/memcpy.c
OBJS := $(SRCS:%=$(LIBC_BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
TARGET := $(LIBC_BUILD_DIR)/libc.a

INCLUDES := -I$(RTOS_PATH)/libc/include
CFLAGS := -MMD -MP $(INCLUDES)

all: $(TARGET)

install: all
	cp $(TARGET) $(PREFIX)/usr/lib
	cp -r $(RTOS_PATH)/libc/include/* $(PREFIX)/usr/include/

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	@echo [AR] $(subst $(BUILD_DIR)/,,$@)
	$(AR) rcs $@ $^

clean:
	rm $(OBJS)
	rm $(TARGET)

include $(RTOS_PATH)/make/rules.mk

-include $(DEPS)