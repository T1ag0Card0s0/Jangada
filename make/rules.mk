$(BUILD_DIR)/%.c.o: $(RTOS_PATH)/%.c 
	mkdir -p $(dir $@)
	@echo [CC] $(subst $(BUILD_DIR)/,,$@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: $(RTOS_PATH)/%.S 
	mkdir -p $(dir $@)
	@echo [AS] $(subst $(BUILD_DIR)/,,$@)
	$(AS) $(ASFLAGS) -c $< -o $@

format:
	@echo "Running clang-format on sources..."
	@for src in $(SRCS); do \
		if [ -f $$src ]; then \
			clang-format -i -style=file $$src && echo "Formatted $$src"; \
		fi; \
	done
	@echo "clang-format done."

build-dirs:
	for dir in $(RTOS_DIRS); do \
		$(MAKE) -C $$dir all; \
	done

install-dirs:
	for dir in $(RTOS_DIRS); do \
		$(MAKE) -C $$dir install; \
	done

clean-dirs:
	for dir in $(RTOS_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done