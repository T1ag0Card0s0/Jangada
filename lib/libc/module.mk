LIBC_DIR := lib/libc
LIBC_ARCHIVE := $(BUILD_DIR)/lib/libc/libc.a

LIBC_SRCS := $(LIBC_DIR)/string/memcpy.c $(LIBC_DIR)/string/memset.c $(LIBC_DIR)/string/strcmp.c
LIBC_OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(LIBC_SRCS))

LIBS += $(LIBC_ARCHIVE)
INCLUDES += -I$(LIBC_DIR)/include
DEPS += $(LIBC_OBJS:.o=.d)

$(OBJ_DIR)/$(LIBC_DIR)/%.o: $(LIBC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBC_ARCHIVE): $(LIBC_OBJS)
	@mkdir -p $(dir $@)
	@echo "  AR      $@"
	@$(AR) rcs $@ $^

.PHONY: libc_clean
clean: libc_clean
libc_clean:
	@rm -f $(LIBC_ARCHIVE) $(LIBC_OBJS)
