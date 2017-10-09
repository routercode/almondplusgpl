# Common configuration variables for
# building flash images and software
# download images for G2
#

# FLASH_NAME *Must* match the name given to the flash
# in your flash driver(s) ( <linux>/drivers/mtd/maps/<xyz>_cfi.c, etc ).
#
FLASH_NAME	= cs752x_nand_flash

ifeq ($(strip $(CONFIG_CORTINA_FPGA)),y)
  FLASH_BASE	= 0xe8000000
  RAM_BASE	= 0xe0000000
else
  FLASH_BASE	= 0xe0000000
  RAM_BASE	= 0x00000000
endif

FLASH_SIZE	= 131072  # 128M in Kb
SECTOR_SIZE	= $(call cs_div, ${CONFIG_NAND_FLASH_BLOCK_SIZE}, 1024)
PAGE_SIZE	= 2

KERNEL_FILE	  	= openwrt-g2-$(SUBTARGET)-uImage
KERNEL_RAM_COPY_OFFSET	= 0x02100000  # flash-to-ram copy target
KERNEL_RAM_IMAGE_OFFSET = 0x00008000  # bootm extract target

#ROOTFS_FILE	  	= openwrt-g2-$(SUBTARGET)-rootfs-jffs2-128k.img
ROOTFS_FILE	  	= openwrt-g2-$(SUBTARGET)-rootfs-squashfs.img
ROOTFS_RAM_COPY_OFFSET	= 0           # flash-to-ram copy target
ROOTFS_RAM_IMAGE_OFFSET = 0x00800000  # bootm extract target

BASIC_BOOTARGS	= console=ttyS0,115200

UBOOT_FILE	=     bootloader.bin

# MUST MATCH u-boot COMPILE TIME OPTION: CFG_ENV_SIZE
UBOOT_ENV_SIZE  = $(call cs_dec2hex, ${CONFIG_G2_UBOOT_ENV_SIZE})

#The unit of size is KB
#The XXX_SIZE is the actual patition size + reserved size
NAND_INFO_ADDR=0x00000000
NAND_INFO_SIZE=$(call cs_div, ${CONFIG_NAND_FLASH_BLOCK_SIZE}, 1024)

BOOTLOADER0_ADDR=$(call cs_dec2hex, ${CONFIG_NAND_FLASH_BLOCK_SIZE})
BOOTLOADER0_SIZE=$(call cs_minus, 1024, $(NAND_INFO_SIZE))

UBOOT_ENV0_ADDR=0x00100000
UBOOT_ENV0_SIZE=1024

BOOTLOADER1_ADDR=0x00200000
BOOTLOADER1_SIZE=1024

UBOOT_ENV1_ADDR=0x00300000
UBOOT_ENV1_SIZE=1024

SB0_ADDR=0x00400000
SB0_SIZE=1024

SB1_ADDR=0x00500000
SB1_SIZE=1024

KERNEL0_ADDR=0x00600000 
KERNEL0_SIZE=6144

ROOTFS0_ADDR=0x00C00000 
ROOTFS_TRUE_SIZE=46080
ROOTFS0_SIZE=47104

KERNEL1_ADDR=0x03A00000 
KERNEL1_SIZE=6144

ROOTFS1_ADDR=0x04000000 
ROOTFS1_SIZE=47104

ROOTFS_DATA_ADDR=0x6E00000
ROOTFS_DATA_TRUE_SIZE=15360
ROOTFS_DATA_SIZE=16384

LOG_ADDR=0x07E00000 
LOG_SIZE=2048


JFFS2_PAD := --pad=$(call cs_minus,$(call cs_times,$(ROOTFS_TRUE_SIZE),1024),4)
JFFS2_MORE_OPTS := --no-cleanmarkers

#-----------------------------------------

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




# Create SB0 first for test
define create_superblock_bin
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --create  --valid --active 
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=uboot-env0	--what=data --type=data     --size=$(UBOOT_ENV0_SIZE)K --offset=$(UBOOT_ENV0_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=uboot-env1	--what=data --type=data     --size=$(UBOOT_ENV1_SIZE)K --offset=$(UBOOT_ENV1_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=sb0		--what=data --type=data     --size=$(SB0_SIZE)K --offset=$(SB0_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=sb1		--what=data --type=data     --size=$(SB1_SIZE)K --offset=$(SB1_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=kernel		--what=kernel --type=uimage --size=$(KERNEL0_SIZE)K --offset=$(KERNEL0_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/uImage --dont_export
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=rootfs		--what=rootfs --type=uimage --size=$(ROOTFS0_SIZE)K --offset=$(ROOTFS0_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=kernel_standby  --what=kernel --type=uimage --size=$(KERNEL1_SIZE)K --offset=$(KERNEL1_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/uImage
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=rootfs_standby  --what=rootfs --type=uimage --size=$(ROOTFS1_SIZE)K --offset=$(ROOTFS1_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=rootfs_data	--what=data --type=data     --size=$(ROOTFS_DATA_SIZE)K --offset=$(ROOTFS_DATA_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=log		--what=data --type=data     --size=$(LOG_SIZE)K --offset=$(LOG_ADDR) --mountpoint=/log
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=uboot0		--what=data --type=data     --size=$(BOOTLOADER0_SIZE)K --offset=$(BOOTLOADER0_ADDR) --path=$(BIN_DIR)/bootloader.bin
	cs.mksb --sb=$(BIN_DIR)/scratch/sb0.bin --mtdpart -name=uboot1		--what=data --type=data     --size=$(BOOTLOADER1_SIZE)K --offset=$(BOOTLOADER1_ADDR) --path=$(BIN_DIR)/bootloader.bin

	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --create  --valid 
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=uboot-env0	--what=data --type=data     --size=$(UBOOT_ENV0_SIZE)K --offset=$(UBOOT_ENV0_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=uboot-env1	--what=data --type=data     --size=$(UBOOT_ENV1_SIZE)K --offset=$(UBOOT_ENV1_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=sb0		--what=data --type=data     --size=$(SB0_SIZE)K --offset=$(SB0_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=sb1		--what=data --type=data     --size=$(SB1_SIZE)K --offset=$(SB1_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=kernel		--what=kernel --type=uimage --size=$(KERNEL1_SIZE)K --offset=$(KERNEL1_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/uImage  --dont_export
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=rootfs		--what=rootfs --type=uimage --size=$(ROOTFS1_SIZE)K --offset=$(ROOTFS1_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=kernel_standby  --what=kernel --type=uimage --size=$(KERNEL0_SIZE)K --offset=$(KERNEL0_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/uImage
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=rootfs_standby  --what=rootfs --type=uimage --size=$(ROOTFS0_SIZE)K --offset=$(ROOTFS0_ADDR) --path=$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=rootfs_data	--what=data --type=data     --size=$(ROOTFS_DATA_SIZE)K --offset=$(ROOTFS_DATA_ADDR)
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=log		--what=data --type=data     --size=$(LOG_SIZE)K --offset=$(LOG_ADDR) --mountpoint=/log
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=uboot0		--what=data --type=data     --size=$(BOOTLOADER0_SIZE)K --offset=$(BOOTLOADER0_ADDR) --path=$(BIN_DIR)/bootloader.bin
	cs.mksb --sb=$(BIN_DIR)/scratch/sb1.bin --mtdpart -name=uboot1		--what=data --type=data     --size=$(BOOTLOADER1_SIZE)K --offset=$(BOOTLOADER1_ADDR) --path=$(BIN_DIR)/bootloader.bin
endef


define create_nand_info_bin
	ECC_ALGORITHM=$(strip 
		$(if $(CONFIG_KERNEL_CS752X_NAND_ECC_HW_HAMMING_256), 0, \
		$(if $(CONFIG_KERNEL_CS752X_NAND_ECC_HW_HAMMING_512), 1, \
		$(if $(CONFIG_KERNEL_CS752X_NAND_ECC_HW_BCH_8_512), 2, \
	        $(if $(CONFIG_KERNEL_CS752X_NAND_ECC_HW_BCH_12_512), 3, 4))))) ;\
	bash gen_nand_info  $$$${ECC_ALGORITHM}   \
		${CONFIG_NAND_FLASH_OOB_SIZE} ${CONFIG_NAND_FLASH_BLOCK_SIZE} \
		$(BIN_DIR)/nand-info.bin
endef

define mk_uboot_nand_env
	mkdir -p $(BIN_DIR)/scratch
	perl expand-tmpl.pl $(1) < uboot-env-nand.tmpl > $(BIN_DIR)/scratch/uboot-env.sh
endef

# Use the 'uboot-flashbin-setenv' utility to write the
# u-boot environment to the image.
# Do it twice; for u-boot0 and u-boot1

define create_uboot_env_bin
	$(call mk_uboot_nand_env,\
	  flash_name=$(FLASH_NAME) \
	  basic_args=$(BASIC_BOOTARGS) \
	  ethaddr1=$(CONFIG_CS_UBOOT_ETHADDR1) \
	  ethaddr2=$(CONFIG_CS_UBOOT_ETHADDR2) \
	  ethaddr=$(CONFIG_CS_UBOOT_ETHADDR0) \
	  wifiaddr0=$(CONFIG_CS_UBOOT_WIFIADDR0) \
	  wifiaddr1=$(CONFIG_CS_UBOOT_WIFIADDR1) \
	  ipaddr=$(CONFIG_CS_UBOOT_IPADDR) \
	  netmask=$(CONFIG_CS_UBOOT_NETMASK) \
	  serverip=$(CONFIG_CS_UBOOT_SERVERIP) \
	  gatewayip=$(CONFIG_CS_UBOOT_GATEWAYIP) \
          mtd_args=$(CONFIG_CS_UBOOT_MTDPARTS_ARGS) \
	  rootfs_args=$(CONFIG_CS_UBOOT_ROOTFS_ARGS) \
	  more_args=$(CONFIG_CS_UBOOT_MORE_ARGS) \
	  kernel_ram_cp=$(call cs_ram_addr,$(call cs_hex2dec,$(KERNEL_RAM_COPY_OFFSET))) \
	  rootfs_ram_cp=- \
	  sb0_addr=$(call cs_flash_addr, $(call cs_hex2dec, $(SB0_ADDR))) \
	  sb1_addr=$(call cs_flash_addr, $(call cs_hex2dec, $(SB1_ADDR))) \
	  qm_init_buff=0 \
	  ni_napi_budget=16 \
	  qm_acp_enable=0 \
	  ni_rx_noncache=0 \
	)
	dd if=/dev/zero of=$(BIN_DIR)/uboot-env.bin bs=1 count=$(call cs_hex2dec, $(UBOOT_ENV_SIZE))
	uboot-flashbin-setenv -binfile $(BIN_DIR)/uboot-env.bin \
	  -envoffset 0 \
	  -envsize $(UBOOT_ENV_SIZE) \
	  -setenv $(BIN_DIR)/scratch/uboot-env.sh
endef

define create_flash_bin
	$(RM) $(BIN_DIR)/flash.bin
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/nand-info.bin  seek=$(call cs_div, $(call cs_hex2dec, $(NAND_INFO_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/bootloader.bin seek=$(call cs_div, $(call cs_hex2dec, $(BOOTLOADER0_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/uboot-env.bin seek=$(call cs_div, $(call cs_hex2dec, $(UBOOT_ENV0_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/bootloader.bin seek=$(call cs_div, $(call cs_hex2dec, $(BOOTLOADER1_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/uboot-env.bin seek=$(call cs_div, $(call cs_hex2dec, $(UBOOT_ENV1_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/scratch/sb0.bin seek=$(call cs_div, $(call cs_hex2dec, $(SB0_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/scratch/sb1.bin seek=$(call cs_div, $(call cs_hex2dec, $(SB1_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/scratch/jffs-root/bank0/uImage seek=$(call cs_div, $(call cs_hex2dec, $(KERNEL0_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img seek=$(call cs_div, $(call cs_hex2dec, $(ROOTFS0_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/scratch/jffs-root/bank0/uImage seek=$(call cs_div, $(call cs_hex2dec, $(KERNEL1_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img seek=$(call cs_div, $(call cs_hex2dec, $(ROOTFS1_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=$(BIN_DIR)/scratch/rootfs_data.jffs2 seek=$(call cs_div, $(call cs_hex2dec, $(ROOTFS_DATA_ADDR)), 1024) conv=notrunc bs=1K
	dd of=$(BIN_DIR)/flash.bin if=/dev/zero seek=$(call cs_div, $(FLASH_SIZE), 1024) count=0 bs=1M 
endef

define calculate_padding_size
	$(call cs_times, $(call cs_plus, $(call cs_div, $(call cs_minus, $(1), 1), $(2)), 1), $(2))
endef

define create_rootfs_data
	  mkdir -p $(BIN_DIR)/scratch/overlay/etc
	  echo "/dev/mtd0 0x0 $(UBOOT_ENV_SIZE) $(call cs_dec2hex,$(call cs_times,$(SECTOR_SIZE),1024))" > \
	    $(BIN_DIR)/scratch/overlay/etc/fw_env.config ;
	  mkfs.jffs2 --little-endian --squash -v -m none \
	    --eraseblock=$(call cs_dec2hex,$(call cs_times,$(SECTOR_SIZE),1024)) \
	    --pagesize=$(call cs_dec2hex,$(call cs_times,$(PAGE_SIZE),1024)) \
	    --root=$(BIN_DIR)/scratch/overlay -o $(BIN_DIR)/scratch/rootfs_data.tmp ;
	  mkfs.jffs2 \
	    --pad=$(strip $(call calculate_padding_size, $(shell stat -c%s $(BIN_DIR)/scratch/rootfs_data.tmp), $(call cs_times,$(PAGE_SIZE),1024))) \
	    --little-endian --squash -v -m none \
	    --eraseblock=$(call cs_dec2hex,$(call cs_times,$(SECTOR_SIZE),1024)) \
	    --pagesize=$(call cs_dec2hex,$(call cs_times,$(PAGE_SIZE),1024)) \
	    --root=$(BIN_DIR)/scratch/overlay -o $(BIN_DIR)/scratch/rootfs_data.jffs2 ;
	  $(RM) -f $(BIN_DIR)/scratch/rootfs_data.tmp
endef


define Image/Flash

	# Create a scratch area
	mkdir -p $(BIN_DIR)/scratch/jffs-root/bank0
	mkdir -p $(BIN_DIR)/scratch/jffs-root/bank1

	# Copy the kernel uImage into the two banks
	cp $(BIN_DIR)/$(KERNEL_FILE) $(BIN_DIR)/scratch/jffs-root/bank0/uImage
	cp $(BIN_DIR)/$(KERNEL_FILE) $(BIN_DIR)/scratch/jffs-root/bank1/uImage

	# Make the u-image for the rootfs, store in bank0
	if [ ! -z "$(ROOTFS_FILE)" ]; then \
	  cp $(BIN_DIR)/$(ROOTFS_FILE) $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img; \
	  mkimage -A arm -T ramdisk -C none -n 'Root Filesystem' \
		-d $(BIN_DIR)/$(ROOTFS_FILE) -O linux \
		-a $(call cs_ram_addr,$(call cs_hex2dec,$(ROOTFS_RAM_IMAGE_OFFSET))) \
		-e $(call cs_ram_addr,$(call cs_hex2dec,$(ROOTFS_RAM_IMAGE_OFFSET))) \
		$(BIN_DIR)/scratch/rootfs.uimg ;\
	fi

	# Make the u-image for the rootfs, store in bank1
	if [ ! -z "$(ROOTFS_FILE)" ]; then \
	  cp $(BIN_DIR)/$(ROOTFS_FILE) $(BIN_DIR)/scratch/jffs-root/bank1/rootfs.img; \
	fi
	# create the nand_info
	$(call create_nand_info_bin)
	# create the uboot-env
	$(call create_uboot_env_bin)
	# create the superblocks
	$(call create_superblock_bin)
endef

define uboot-http-dl-img
	python uboot-dl-gen  -n $(1) -a $(call cs_flash_addr, $(call cs_hex2dec,$2)) -s $(call cs_times, $(call cs_hex2dec, $3), 1024)  -i $4 -m 0 -r 0 -o $(BIN_DIR)/uboot-http-dl/$5
endef

define Image/SWDL/uboot
	rm -rf $(BIN_DIR)/uboot-http-dl
	mkdir $(BIN_DIR)/uboot-http-dl

	# bootloader
	$(call uboot-http-dl-img, "bootloader", $(BOOTLOADER0_ADDR), $(BOOTLOADER0_SIZE), $(BIN_DIR)/bootloader.bin,loader.bin)
	$(call uboot-http-dl-img, "uboot-env", $(UBOOT_ENV0_ADDR), $(UBOOT_ENV0_SIZE), $(BIN_DIR)/uboot-env.bin,loader.bin)

	# rootfs+kenrel
	$(call uboot-http-dl-img, "sb0", $(SB0_ADDR), $(SB0_SIZE), $(BIN_DIR)/scratch/sb0.bin,kernel-rootfs.bin)
	$(call uboot-http-dl-img, "sb1", $(SB1_ADDR), $(SB1_SIZE), $(BIN_DIR)/scratch/sb1.bin,kernel-rootfs.bin)
	$(call uboot-http-dl-img, "kernel", $(KERNEL0_ADDR), $(KERNEL0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/uImage,kernel-rootfs.bin)
	$(call uboot-http-dl-img, "rootfs", $(ROOTFS0_ADDR), $(ROOTFS0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img,kernel-rootfs.bin)

	# all  
	$(call uboot-http-dl-img, "bootloader", $(BOOTLOADER0_ADDR), $(BOOTLOADER0_SIZE), $(BIN_DIR)/bootloader.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "uboot-env", $(UBOOT_ENV0_ADDR), $(UBOOT_ENV0_SIZE), $(BIN_DIR)/uboot-env.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "sb0", $(SB0_ADDR), $(SB0_SIZE), $(BIN_DIR)/scratch/sb0.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "sb1", $(SB1_ADDR), $(SB1_SIZE), $(BIN_DIR)/scratch/sb1.bin,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "kernel", $(KERNEL0_ADDR), $(KERNEL0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/uImage,loader-kernel-rootfs.bin)
	$(call uboot-http-dl-img, "rootfs", $(ROOTFS0_ADDR), $(ROOTFS0_SIZE), $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img,loader-kernel-rootfs.bin)
endef

define Image/SWDL
	# Just the kernel
	cs.mkimage -image $(BIN_DIR)/kernel-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -tool-version $(TOOL_VERSION) -sw-version $(SW_VERSION) \
	  -chip-bitmap $(CHIP_ID) -board-bitmap $(BOARD_ID) \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage

	# Just the root file system
	cs.mkimage -image $(BIN_DIR)/rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -tool-version $(TOOL_VERSION) -sw-version $(SW_VERSION) \
	  -chip-bitmap $(CHIP_ID) -board-bitmap $(BOARD_ID) \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img

	# Both the kernel and root file system
	cs.mkimage -image $(BIN_DIR)/kernel-rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -tool-version $(TOOL_VERSION) -sw-version $(SW_VERSION) \
	  -chip-bitmap $(CHIP_ID) -board-bitmap $(BOARD_ID) \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img

	# Just uboot
	cs.mkimage -image $(BIN_DIR)/uboot-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -tool-version $(TOOL_VERSION) -sw-version $(SW_VERSION) \
	  -chip-bitmap $(CHIP_ID) -board-bitmap $(BOARD_ID) \
	  -block -name uboot0 \
	  -destination mtd:uboot0 \
	  -payload $(BIN_DIR)/bootloader.bin \
	  -block -name uboot-env0 \
	  -destination mtd:uboot-env0 \
	  -payload $(BIN_DIR)/uboot-env.bin \
	  -block -name uboot1 \
	  -destination mtd:uboot1 \
	  -payload $(BIN_DIR)/bootloader.bin \
	  -block -name uboot-env1 \
	  -destination mtd:uboot-env1 \
	  -payload $(BIN_DIR)/uboot-env.bin

	# Just uboot env
	cs.mkimage -image $(BIN_DIR)/uboot-env-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -tool-version $(TOOL_VERSION) -sw-version $(SW_VERSION) \
	  -chip-bitmap $(CHIP_ID) -board-bitmap $(BOARD_ID) \
	  -block -name uboot-env0 \
	  -destination mtd:uboot-env0 \
	  -payload $(BIN_DIR)/uboot-env.bin \
	  -block -name uboot-env1 \
	  -destination mtd:uboot-env1 \
	  -payload $(BIN_DIR)/uboot-env.bin

	# Kernel, rootfs and uboot
	cs.mkimage -image $(BIN_DIR)/uboot-kernel-rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -tool-version $(TOOL_VERSION) -sw-version $(SW_VERSION) \
	  -chip-bitmap $(CHIP_ID) -board-bitmap $(BOARD_ID) \
	  -block -name uboot0 \
	  -destination mtd:uboot0 \
	  -payload $(BIN_DIR)/bootloader.bin \
	  -block -name uboot-env0 \
	  -destination mtd:uboot-env0 \
	  -payload $(BIN_DIR)/uboot-env.bin \
	  -block -name uboot1 \
	  -destination mtd:uboot1 \
	  -payload $(BIN_DIR)/bootloader.bin \
	  -block -name uboot-env1 \
	  -destination mtd:uboot-env1 \
	  -payload $(BIN_DIR)/uboot-env.bin \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img

	# Special reflash image.  This image contains everything.  It will be
	# large.  It is equiv. to a factory re-flash.  Upgrading with this
	# image may require special aruments to cs.extract/cs.swdl that are
	# not available in the webgui.
	cs.mkimage -image $(BIN_DIR)/reflash.img \
	  -create -buildid $(BUILDID) \
	  -tool-version $(TOOL_VERSION) -sw-version $(SW_VERSION) \
	  -chip-bitmap $(CHIP_ID) -board-bitmap $(BOARD_ID) \
	  -block -name sb0 \
	  -destination /tmp/sb0.bin \
	  -payload $(BIN_DIR)/scratch/sb0.bin \
	  -block -name sb1 \
	  -destination /tmp/sb1.bin \
	  -payload $(BIN_DIR)/scratch/sb1.bin \
	  -block -name uboot0 \
	  -destination mtd:uboot0 \
	  -payload $(BIN_DIR)/bootloader.bin \
	  -block -name uboot-env0 \
	  -destination mtd:uboot-env0 \
	  -payload $(BIN_DIR)/uboot-env.bin \
	  -block -name uboot1 \
	  -destination mtd:uboot1 \
	  -payload $(BIN_DIR)/bootloader.bin \
	  -block -name uboot-env1 \
	  -destination mtd:uboot-env1 \
	  -payload $(BIN_DIR)/uboot-env.bin \
	  -block -name kernel \
	  -destination mtd:kernel \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	  -block -name rootfs \
	  -destination mtd:rootfs \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.img

	rm -f $(BIN_DIR)/openwrt-*
	$(call Image/SWDL/uboot)
endef
