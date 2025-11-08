MAKEFLAGS += --no-print-directory
MAKEFLAGS += -s

ifeq ($(wildcard $(TOOLCHAIN_FILE)),)
    $(error Toolchain file not found: $(TOOLCHAIN_FILE).)
endif

include $(TOOLCHAIN_FILE)

ifeq ($(wildcard $(LINKER_SCRIPT)),)
    $(error Linker script not found: $(LINKER_SCRIPT).)
endif

CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS)
ASFLAGS := $(ASFLAGS)

OBJCFLAGS := $(CFLAGS) -c
OBJCXXFLAGS := $(CXXFLAGS) -c
OBJASFLAGS := $(ASFLAGS) -c 

.PHONY: build-subdirs
build-subdirs: $(DIRS)

.PHONY: $(DIRS)
$(DIRS):
	@$(MAKE) -C $@ all
	
.PHONY: clean-subdirs
clean-subdirs:
	@for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo -e "\t" CC $(OBJCFLAGS) $< -o $@
	@$(CC) $(OBJCFLAGS) $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	@echo -e "\t" CXX $(OBJCXXFLAGS) $< -o $@
	@$(CXX) $(OBJCXXFLAGS) $< -o $@

$(BUILD_DIR)/%.S.o: %.S
	@mkdir -p $(dir $@)
	@echo -e "\t" AS $(OBJASFLAGS) $< -o $@
	@$(AS) $(OBJASFLAGS) $< -o $@

$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	@echo -e "\t" AS $(OBJASFLAGS) $< -o $@
	@$(AS) $(OBJASFLAGS) $< -o $@
