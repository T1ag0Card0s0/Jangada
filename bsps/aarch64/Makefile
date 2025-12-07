include $(RTOS_PATH)/bsps/aarch64/$(BOARD)/Makefile
BSPS_RTOS_BUILD_DIR = $(RTOS_BUILD_DIR)/bsps
SRCS = aarch64/common/startup.S
OBJS = $(SRCS:%=$(BSPS_RTOS_BUILD_DIR)/%.o)
TARGET = $(BSPS_RTOS_BUILD_DIR)/libbsp.a

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	@echo [AR] $(subst $(RTOS_BUILD_DIR)/,,$@)
	$(AR) rcs $@ $^

install: all
	mkdir -p $(BSPS_RTOS_BUILD_DIR)/$(dir $(LINKER_SCRIPT))
	cp $(RTOS_PATH)/bsps/$(LINKER_SCRIPT) $(BSPS_RTOS_BUILD_DIR)/$(LINKER_SCRIPT)
	cp $(TARGET) $(PREFIX)/usr/lib

clean:
	rm $(OBJS)
	rm -f $(BSPS_RTOS_BUILD_DIR)/$(LINKER_SCRIPT)

include $(RTOS_PATH)/make/rules.mk

.PHONY: all install clean