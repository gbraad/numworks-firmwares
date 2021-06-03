HANDY_TARGETS += bootloader

# stack protector
SFLAGS += -fstack-protector-strong

bootloader_src = $(ion_device_bootloader_src) $(liba_bootloader_src) $(kandinsky_minimal_src)
bootloader_obj = $(call flavored_object_for,$(bootloader_src),usbxip leavereset)
$(BUILD_DIR)/bootloader.$(EXE): $(bootloader_obj)

$(BUILD_DIR)/bootloader.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/bootloader
$(BUILD_DIR)/bootloader.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/bootloader/bootloader_flash.ld
$(BUILD_DIR)/bootloader.$(EXE): LDDEPS += $(LDSCRIPT) ion/src/$(PLATFORM)/$(MODEL)/shared/config_flash.ld ion/src/$(PLATFORM)/shared/shared_config_flash.ld
$(BUILD_DIR)/bootloader.dfu: $(BUILD_DIR)/bootloader.elf