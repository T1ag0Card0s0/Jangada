LINKER_SCRIPT := aarch64/qemu_virt/linker.ld

all:
	mkdir -p $(BSPS_BUILD_DIR)/$(dir $(LINKER_SCRIPT))
	cp $(RTOS_PATH)/bsps/$(LINKER_SCRIPT) $(BSPS_BUILD_DIR)/$(LINKER_SCRIPT)

install: all

clean:
	rm -f $(BSPS_BUILD_DIR)/$(LINKER_SCRIPT)

.PHONY: all install clean