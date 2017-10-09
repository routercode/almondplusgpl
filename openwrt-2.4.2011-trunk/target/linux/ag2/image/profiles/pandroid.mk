# Copy Android FS
# Copy OpenWRT FS, excluding the /init script and /etc/*
# Copy OpenWRT /etc/* to /system/etc/* (android boot makes a symlink)
# Copy OpenWRT /init to /init.owrt
#
# FLASH_NAME *Must* match the name given to the flash
# in your flash driver(s) ( <linux>/drivers/mtd/maps/<xyz>_cfi.c, etc ).
#
FLASH_NAME	= cs752x_nor_flash

ifeq ($(strip $(CONFIG_CORTINA_FPGA)),y)
  FLASH_BASE	= 0xe8000000
  RAM_BASE	= 0xe0000000
else
  FLASH_BASE	= 0xe0000000
  RAM_BASE	= 0x00000000
endif

FLASH_SIZE	= 11264	# 11M in Kb
SECTOR_SIZE	= 128

KERNEL_FILE	  	= openwrt-ag2-$(SUBTARGET)-uImage
KERNEL_RAM_COPY_OFFSET	= 0x02100000  # flash-to-ram copy target
KERNEL_RAM_IMAGE_OFFSET = 0x00008000  # bootm extract target

BASIC_BOOTARGS	= console=ttyS0,115200

UBOOT_FILE	=     bootloader.bin

# MUST MATCH u-boot COMPILE TIME OPTION: CFG_ENV_SIZE
UBOOT_ENV_SIZE  =     0x20000

UBOOT0_SECTOR	=     0
UBOOT0_SIZE	=     256

UBOOT0_ENV_SECTOR =   2 
UBOOT0_ENV_SIZE =     128

UBOOT1_SECTOR	=     3 
UBOOT1_SIZE	=     256

UBOOT1_ENV_SECTOR =   5          
UBOOT1_ENV_SIZE =     128

SB0_SECTOR	=     6 
SB0_SIZE	=     128

SB1_SECTOR	=     7 
SB1_SIZE	=     128

#------------------------------------------

KERNEL0_SECTOR      = 8
KERNEL0_SIZE        = 5120  # 5M in Kb

#------------------------------------------

KERNEL1_SECTOR      = 48
KERNEL1_SIZE        = 5120  # 5M in Kb

#------------------------------------------

# HOW THIS WORKS:
#
# u-boot runs "sb_process" to decide which super block to use for
# booting (based in the valid,commit and active flags).  Once it
# picks a SB, it'll grab THE FIRST kernel it sees and THE FIRST
# rootfs it sees as the images to boot from.  These two entries
# (the first two) should be marked "--dont_export".  The second
# two entries, because they are not marked "--dont_export" will
# end up in the mtdparts partition table that is exported to
# the kernel.  The kernel can THUS write them, and will during
# a system upgrade ... so if you're booting from 0th images,
# you want to expose the 1th images for upgrade.  And if you're
# booting from the 1th images, you want to expose the 0th images
# for upgrade.
#

define Image/Flash
	# create the flash
	$(call create_flash_common)
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank0/uImage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank1/uImage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE))
	# create the superblocks
	$(call create_sb_common)
	# sb0
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel_0,kernel,uimage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank0/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel_standby,kernel,uimage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank1/uImage)
	# sb1
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel_1,kernel,uimage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank1/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel_standby,kernel,uimage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank0/uImage)
	# write super blocks to flash
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/sb0.bin,$(SB0_SECTOR),$(SB0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/sb1.bin,$(SB1_SECTOR),$(SB1_SIZE))
endef

define Image/SWDL
	# Just the kernel
	cs.mkimage -image $(BIN_DIR)/kernel-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage
endef

# The $(TMP_DIR)/android_fs symlink was created by the
# android package, so we have a good way of finding the
# build results.
#
ANDROID_FS:=$(TMP_DIR)/android_fs/out/target/product/$(CONFIG_ANDROID_PRODUCT)

define Image/mkfs/squashfs
	@mkdir -p $(TARGET_DIR)/overlay
	if [ ! -d "$(ANDROID_FS)" ]; then \
	  echo; \
	  echo "$(ANDROID_FS) not found!" ;\
	  echo "You must define CONFIG_ANDROID_PLATFORM and CONFIG_ANDROID_PRODUCT env vars" ;\
	  echo; \
	  exit 1; \
	fi
	rm -rf $(BIN_DIR)/android-rootfs
	mkdir -p $(BIN_DIR)/android-rootfs
	cp -Rfp $(ANDROID_FS)/root/* $(BIN_DIR)/android-rootfs
	cp -Rfp $(ANDROID_FS)/system $(BIN_DIR)/android-rootfs
	cp -Rfp $(ANDROID_FS)/data   $(BIN_DIR)/android-rootfs
	mv $(BIN_DIR)/android-rootfs/init $(BIN_DIR)/android-rootfs/init.android
	rm $(BIN_DIR)/android-rootfs/sbin/ueventd
	ln -s /init.android $(BIN_DIR)/android-rootfs/sbin/ueventd
	$(TAR) --exclude etc -zcf - -C $(TARGET_DIR) . | $(TAR) -zxf - -C $(BIN_DIR)/android-rootfs 
	$(TAR) -zcf - -C $(TARGET_DIR)/etc/ . | $(TAR) -zxf - -C $(BIN_DIR)/android-rootfs/system/etc
	ln -s /system/etc $(BIN_DIR)/android-rootfs/etc
	$(STAGING_DIR_HOST)/bin/mksquashfs4 $(BIN_DIR)/android-rootfs $(KDIR)/root.squashfs -nopad -noappend -root-owned -comp lzma -processors 1
	$(call Image/Build,squashfs)
endef

define Image/mkfs/ext4
	@mkdir -p $(TARGET_DIR)/overlay
	if [ ! -d "$(ANDROID_FS)" ]; then \
	  echo; \
	  echo "$(ANDROID_FS) not found!" ;\
	  echo "You must define CONFIG_ANDROID_PLATFORM and CONFIG_ANDROID_PRODUCT env vars" ;\
	  echo; \
	  exit 1; \
	fi
	rm -rf $(BIN_DIR)/android-rootfs
	mkdir -p $(BIN_DIR)/android-rootfs
	cp -Rfp $(ANDROID_FS)/root/* $(BIN_DIR)/android-rootfs
	cp -Rfp $(ANDROID_FS)/system $(BIN_DIR)/android-rootfs
	cp -Rfp $(ANDROID_FS)/data   $(BIN_DIR)/android-rootfs
	mv $(BIN_DIR)/android-rootfs/init $(BIN_DIR)/android-rootfs/init.android
	mv $(BIN_DIR)/android-rootfs/init.rc $(BIN_DIR)/android-rootfs/init.rc.default
	rm $(BIN_DIR)/android-rootfs/sbin/ueventd
	ln -s /init.android $(BIN_DIR)/android-rootfs/sbin/ueventd
	$(TAR) --exclude etc -zcf - -C $(TARGET_DIR) . | $(TAR) -zxf - -C $(BIN_DIR)/android-rootfs 
	$(TAR) -zcf - -C $(TARGET_DIR)/etc/ . | $(TAR) -zxf - -C $(BIN_DIR)/android-rootfs/system/etc
	ln -s /system/etc $(BIN_DIR)/android-rootfs/etc
	$(STAGING_DIR_HOST)/bin/genext2fs -U -b $(E2SIZE) -N $(CONFIG_TARGET_ROOTFS_MAXINODE) -d $(BIN_DIR)/android-rootfs/ $(KDIR)/root.ext4
	$(STAGING_DIR_HOST)/bin/tune2fs -O extents,uninit_bg,dir_index $(KDIR)/root.ext4
	$(STAGING_DIR_HOST)/bin/e2fsck -fy $(KDIR)/root.ext4
	$(call Image/Build,ext4)
endef

