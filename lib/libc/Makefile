SRCS := string/memcpy.c string/memset.c string/strcmp.c
DIRS := 

LIB_NAME := libc.a

include $(SUB_MAKE_INCLUDE)

all: $(BUILD_DIR)/lib/libc/$(LIB_NAME)

$(BUILD_DIR)/lib/libc/$(LIB_NAME): $(OBJS)
	@echo -e "\t" AR $@
	@mkdir -p $(dir $@)
	@$(AR) rcs $@ $(OBJS)

install: all
	$(info Installing libc to $(SYSROOT)$(LIBDIR))
	@mkdir -p $(SYSROOT)$(LIBDIR)
	@cp $(BUILD_DIR)/lib/libc/$(LIB_NAME) $(SYSROOT)$(LIBDIR)
	@cp -r include/ $(SYSROOT)$(INCLUDEDIR)/

.PHONY: all clean install
