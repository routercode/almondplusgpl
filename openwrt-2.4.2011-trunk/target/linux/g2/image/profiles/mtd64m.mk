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
SECTOR_SIZE	= 128

KERNEL_FILE	  	= openwrt-g2-$(SUBTARGET)-uImage
KERNEL_RAM_COPY_OFFSET	= 0x02100000  # flash-to-ram copy target
KERNEL_RAM_IMAGE_OFFSET = 0x00008000  # bootm extract target

ROOTFS_FILE	  	= openwrt-g2-$(SUBTARGET)-rootfs-squashfs.img
ROOTFS_RAM_COPY_OFFSET	= 0  	      # 0 - means dont copy to ram
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

ROOTFS_LOG_SECTOR  = 504
ROOTFS_LOG_SIZE    = 1024  # 1M in Kb

include mtd-common.mk
