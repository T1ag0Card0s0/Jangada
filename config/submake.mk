SUBDIR := $(patsubst $(RTOS_PATH)/%,%,$(CURDIR))
ifeq ($(SUBDIR),$(CURDIR))
    SUBDIR := $(patsubst $(PROJECT_DIR)/%,%,$(CURDIR))
endif

SUBDIR := $(if $(filter /%,$(SUBDIR)),$(subst /,,$(wordlist 2,$(words $(SUBDIR)),$(subst /, ,$(SUBDIR)))),$(SUBDIR))
OBJS_WITH_PATH := $(addprefix $(BUILD_DIR)/$(SUBDIR)/,$(OBJS))

.PHONY: all
all: $(OBJS_WITH_PATH) build-subdirs

.PHONY: clean
clean: clean-subdirs
	@echo CLEAN $(OBJS_WITH_PATH)
	@rm -f $(OBJS_WITH_PATH)

include $(MAKE_INCLUDE)

$(BUILD_DIR)/$(SUBDIR)/%.c.o: $(CURDIR)/%.c
	@mkdir -p $(dir $@)
	@echo -e "\t" CC $(OBJCFLAGS) $< -o $@
	@$(CC) $(OBJCFLAGS) $< -o $@

$(BUILD_DIR)/$(SUBDIR)/%.cpp.o: $(CURDIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -e "\t" CXX $(OBJCXXFLAGS) $< -o $@
	@$(CXX) $(OBJCXXFLAGS) $< -o $@

$(BUILD_DIR)/$(SUBDIR)/%.S.o: $(CURDIR)/%.S
	@mkdir -p $(dir $@)
	@echo -e "\t" AS $(OBJASFLAGS) $< -o $@
	@$(AS) $(OBJASFLAGS) $< -o $@

$(BUILD_DIR)/$(SUBDIR)/%.s.o: $(CURDIR)/%.s
	@mkdir -p $(dir $@)
	@echo -e "\t" AS $(OBJASFLAGS) $< -o $@
	@$(AS) $(OBJASFLAGS) $< -o $@
