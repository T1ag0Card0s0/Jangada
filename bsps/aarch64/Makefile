SRCS := aarch64/common/startup.S
BSPS_OBJS := $(SRCS:%=$(BSPS_BUILD_DIR)/%.o)

all: $(BSPS_OBJS)

install: all

clean:
	rm $(BSPS_OBJS)

include $(RTOS_PATH)/make/rules.mk

.PHONY: all install clean