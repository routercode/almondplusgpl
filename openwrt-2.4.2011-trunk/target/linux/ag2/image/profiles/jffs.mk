# Common configuration variables for
# building flash images and software
# download images for G2
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

FLASH_SIZE	= 65536		# 64M in Kb
SECTOR_SIZE	= 128

KERNEL_FILE	  	= openwrt-g2-$(SUBTARGET)-uImage
KERNEL_RAM_COPY_OFFSET	= 0x02100000  # flash-to-ram copy target
KERNEL_RAM_IMAGE_OFFSET = 0x00008000  # bootm extract target

ROOTFS_FILE	  	= openwrt-g2-$(SUBTARGET)-rootfs-squashfs.img
ROOTFS_RAM_COPY_OFFSET	= 0x02800000  # flash-to-ram copy target
ROOTFS_RAM_IMAGE_OFFSET = 0x00800000  # bootm extract target

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

ROOTFS_DATA_SECTOR =  8
ROOTFS_DATA_SIZE   =  16384	# 16M in Kb

# The kernel(s) and rootfs(s) can be stored in
# a JFFS2 file system, or can be stored in 
# flash partitions.  

# For JFFS Storage ...
JFFS_SECTOR     = 136
JFFS_SIZE	= 40960	# 40M in Kb

define Image/Flash
	# create the flash
	$(call create_flash_common)
	# Create a JFFS2 file system out of the banks structure
	mkfs.jffs2 --pad --little-endian --squash -v -m none \
	  --eraseblock=$(call cs_dec2hex,$(call cs_times,$(SECTOR_SIZE),1024)) \
	  --root=$(BIN_DIR)/scratch/jffs-root -o $(BIN_DIR)/scratch/jffs-root.jffs2

	# Write the JFFS2 file system image into the flash
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root.jffs2,$(JFFS_SECTOR),$(JFFS_SIZE))
	# create the superblocks
	$(call create_sb_common)
	# sb0 
	$(call cs_sbwrite_file,$(BIN_DIR)/scratch/sb0.bin,bank0,uImage,kernel,uimage,,$(BIN_DIR)/scratch/jffs-root/bank0/uImage)
	$(call cs_sbwrite_file,$(BIN_DIR)/scratch/sb0.bin,bank0,rootfs.uimg,rootfs,uimage,,$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,mgmt,data,jffs2,$(JFFS_SECTOR),$(JFFS_SIZE),--mountpoint=/mnt/mgmt --mountopts=noatime,)
	# sb1	
	$(call cs_sbwrite_file,$(BIN_DIR)/scratch/sb1.bin,bank1,uImage,kernel,uimage,,$(BIN_DIR)/scratch/jffs-root/bank1/uImage)
	$(call cs_sbwrite_file,$(BIN_DIR)/scratch/sb1.bin,bank1,rootfs.uimg,rootfs,uimage,,$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.uimg)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,mgmt,data,jffs2,$(JFFS_SECTOR),$(JFFS_SIZE),--mountpoint=/mnt/mgmt --mountopts=noatime,)
	# write superblocks into flash
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/sb0.bin,$(SB0_SECTOR),$(SB0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/sb1.bin,$(SB1_SECTOR),$(SB1_SIZE))
endef

define Image/SWDL
	# Just the kernel
	cs.mkimage -image $(BIN_DIR)/kernel-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel \
	  -destination uImage \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage

	# Just the root file system
	cs.mkimage -image $(BIN_DIR)/rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name rootfs \
	  -destination rootfs.uimg \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg

	# Both the kernel and root file system
	cs.mkimage -image $(BIN_DIR)/kernel-rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel \
	  -destination uImage \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	  -block -name rootfs \
	  -destination rootfs.uimg \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg
endef
