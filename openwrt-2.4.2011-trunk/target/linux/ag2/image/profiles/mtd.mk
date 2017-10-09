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

KERNEL_FILE	  	= openwrt-ag2-$(SUBTARGET)-uImage
KERNEL_RAM_COPY_OFFSET	= 0x02100000  # flash-to-ram copy target
KERNEL_RAM_IMAGE_OFFSET = 0x00008000  # bootm extract target

ROOTFS_FILE	  	= openwrt-ag2-$(SUBTARGET)-rootfs-squashfs.img
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

#------------------------------------------

KERNEL0_SECTOR      = 8
KERNEL0_SIZE        = 5120  # 5M in Kb

ROOTFS0_SECTOR      = 48
ROOTFS0_SIZE        = 18432 # 18M in Kb

#------------------------------------------

KERNEL1_SECTOR      = 192
KERNEL1_SIZE        = 5120  # 5M in Kb

ROOTFS1_SECTOR      = 232
ROOTFS1_SIZE        = 18432 # 18M in Kb

#------------------------------------------

ROOTFS_DATA_SECTOR  = 376
ROOTFS_DATA_SIZE    = 16384  # 16M in Kb

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
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg,$(ROOTFS0_SECTOR),$(ROOTFS0_SIZE))
	$(call cs_fwrite_sector,$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.uimg,$(ROOTFS1_SECTOR),$(ROOTFS1_SIZE))
	# create the superblocks
	$(call create_sb_common)
	# sb0
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel_0,kernel,uimage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank0/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,rootfs_0,rootfs,uimage,$(ROOTFS0_SECTOR),$(ROOTFS0_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel_standby,kernel,uimage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank1/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,rootfs_standby,rootfs,uimage,$(ROOTFS1_SECTOR),$(ROOTFS1_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.uimg)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb0.bin,kernel+rootfs,data,other,$(KERNEL1_SECTOR),$(call cs_plus,$(KERNEL1_SIZE),$(ROOTFS1_SIZE)),,)
	# sb1
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel_1,kernel,uimage,$(KERNEL1_SECTOR),$(KERNEL1_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank1/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,rootfs_1,rootfs,uimage,$(ROOTFS1_SECTOR),$(ROOTFS1_SIZE),--dont_export,$(BIN_DIR)/scratch/jffs-root/bank1/rootfs.uimg)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel_standby,kernel,uimage,$(KERNEL0_SECTOR),$(KERNEL0_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank0/uImage)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,rootfs_standby,rootfs,uimage,$(ROOTFS0_SECTOR),$(ROOTFS0_SIZE),,$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg)
	$(call cs_sbwrite_mtd,$(BIN_DIR)/scratch/sb1.bin,kernel+rootfs,data,other,$(KERNEL0_SECTOR),$(call cs_plus,$(KERNEL0_SIZE),$(ROOTFS0_SIZE)),,)
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

	# Just the root file system
	cs.mkimage -image $(BIN_DIR)/rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg

	# Both the kernel and root file system
	cs.mkimage -image $(BIN_DIR)/kernel-rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel \
	  -destination mtd:kernel_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	  -block -name rootfs \
	  -destination mtd:rootfs_standby \
	  -payload $(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg

	# The kernel+rootfs combined image
	$(RM) -f $(BIN_DIR)/scratch/k+r.bin
	dd if=$(BIN_DIR)/scratch/jffs-root/bank0/uImage \
	   of=$(BIN_DIR)/scratch/k+r.bin \
	   bs=1K seek=0 \
	   count=$(KERNEL0_SIZE) conv=notrunc
	dd if=$(BIN_DIR)/scratch/jffs-root/bank0/rootfs.uimg \
	   of=$(BIN_DIR)/scratch/k+r.bin \
	   bs=1K seek=$(call cs_div,$(call sector_offset,$(call cs_minus,$(ROOTFS0_SECTOR),$(KERNEL0_SECTOR))),1024) \
	   count=$(ROOTFS0_SIZE) conv=notrunc
	cs.mkimage -image $(BIN_DIR)/kernel+rootfs-upgrade.img \
	  -create -buildid $(BUILDID) \
	  -block -name kernel+rootfs \
	  -destination mtd:kernel+rootfs \
	  -payload $(BIN_DIR)/scratch/k+r.bin
endef
