#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define get_flash_profile
$(shell grep ^CONFIG_GB_FLASH_PROFILE $(TOPDIR)/.config | sed 's/=y//g' | sed 's/^CONFIG_GB_FLASH_PROFILE_//g' | tr '[A-Z]' '[a-z]')
endef

include flash-functions.mk
-include profiles/$(call get_flash_profile).mk

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
endef

define Image/BuildKernel
	mkdir -p $(BIN_DIR)
	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
	cp $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage
endef

define Image/Build
	# Combine POST and U-boot into a single image called
	# bootloader.bin
	$(STAGING_DIR_HOST)/bin/gen_post_uboot_bin -p $(BIN_DIR)/post.bin -u $(BIN_DIR)/u-boot.bin -o $(BIN_DIR)/bootloader.bin
	$(call Image/Build/$(1),$(1))
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
  ifeq ($(1),ext2)
	gzip -f9 $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
  endif
ifeq ($(CONFIG_GB_FLASH_CONSTRUCTION),y)
	# Create the flash
	$(call Image/Flash)
	# Create software upgrade images
	$(call Image/SWDL)
endif
endef

define Image/Build/jffs2-128k
	( \
		dd if=$(KDIR)/uImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).img
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img
endef

define Image/Build/ext2
        cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-ext2.img
endef

$(eval $(call BuildImage))
