include $(TOPDIR)/rules.mk

G2ARCH ?= arm
ARCH:=$(G2ARCH)
BOARD:=g2
BOARDNAME:=Cortina Goldengate Platform
DEVICE_TYPE:=G2
DEFAULT_PACKAGES.G2:=dnsmasq

ifeq ($(CONFIG_LINUX_2_6_36),y)
  LINUX_VERSION:=2.6.36
endif

ifeq ($(CONFIG_LINUX_3_3_8),y)
  LINUX_VERSION:=3.3.8
endif

ifeq ($(CONFIG_LINUX_3_4_11),y)
  LINUX_VERSION:=3.4.11
endif


ifeq ($(LINUX_VERSION),)
  LINUX_VERSION:=2.6.36
endif

INITRAMFS_EXTRA_FILES = $(CURDIR)/image/initramfs-base-files.txt

SUBTARGETS:=custom_board fpga bhr pon eng engs ref refb refs refq wan ms 
FEATURES:=usb usbgadget display gpio audio

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=uImage
KERNEL_MAKEOPTS += V=1

define Target/Description
	Build firmware images for Goldengate platform
endef


# Some adjustments after our base-files have been added to the rootfs.
# Some packages change our files, which are read-only because of CVS,
# so we need to change the permissions for these downstream packages
# to work.
#
define Package/base-files/install-target
	chmod +w $(1)/etc/passwd
endef

$(eval $(call BuildTarget))
