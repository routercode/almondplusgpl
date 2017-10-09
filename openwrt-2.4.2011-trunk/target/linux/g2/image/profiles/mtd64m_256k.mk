# Common configuration variables for
# building flash images and software
# download images for G2
#

# FLASH_NAME *Must* match the name given to the flash
# in your flash driver(s) ( <linux>/drivers/mtd/maps/<xyz>_cfi.c, etc ).
#
FLASH_NAME	= $(strip $(CONFIG_CS_FLASH_NAME))

ifeq ($(strip $(CONFIG_CORTINA_FPGA)),y)
  FLASH_BASE	= 0xe8000000
  RAM_BASE	= 0xe0000000
else
  FLASH_BASE	= 0xe0000000
  RAM_BASE	= 0x00000000
endif

FLASH_SIZE	= 65536		# 64M in Kb
SECTOR_SIZE	= 256

KERNEL_FILE	  	= openwrt-g2-$(SUBTARGET)-uImage
KERNEL_RAM_COPY_OFFSET	= 0x02100000  # flash-to-ram copy target
KERNEL_RAM_IMAGE_OFFSET = 0x00008000  # bootm extract target

ROOTFS_FILE	  	= openwrt-g2-$(SUBTARGET)-rootfs-squashfs.img
ROOTFS_RAM_COPY_OFFSET	= 0  	      # 0 - means dont copy to ram
ROOTFS_RAM_IMAGE_OFFSET = 0x00800000  # bootm extract target

BASIC_BOOTARGS	= console=ttyS0,115200

UBOOT_FILE	=     bootloader.bin

# MUST MATCH u-boot COMPILE TIME OPTION: CFG_ENV_SIZE
UBOOT_ENV_SIZE  =     0x40000

UBOOT0_SECTOR	=     0
UBOOT0_SIZE	=     256

UBOOT0_ENV_SECTOR =   1
UBOOT0_ENV_SIZE =     256

UBOOT1_SECTOR	=     2
UBOOT1_SIZE	=     256

UBOOT1_ENV_SECTOR =   3
UBOOT1_ENV_SIZE =     256

SB0_SECTOR	=     4
SB0_SIZE	=     256

SB1_SECTOR	=     5
SB1_SIZE	=     256

#------------------------------------------

KERNEL0_SECTOR      = 6
KERNEL0_SIZE        = 5120  # 5M in Kb

ROOTFS0_SECTOR      = 26
ROOTFS0_SIZE        = 18432 # 18M in Kb

#------------------------------------------

KERNEL1_SECTOR      = 98
KERNEL1_SIZE        = 5120  # 5M in Kb

ROOTFS1_SECTOR      = 118
ROOTFS1_SIZE        = 18432 # 18M in Kb

#------------------------------------------

ROOTFS_DATA_SECTOR  = 190
ROOTFS_DATA_SIZE    = 16384  # 16M in Kb

#------------------------------------------

include mtd-common.mk
