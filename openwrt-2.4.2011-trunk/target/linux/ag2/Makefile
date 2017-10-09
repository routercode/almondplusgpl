include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ag2
BOARDNAME:=Cortina Goldengate Android Platform

LINUX_VERSION:=2.6.36
INITRAMFS_EXTRA_FILES = $(CURDIR)/image/initramfs-base-files.txt

SUBTARGETS:=eng ref
FEATURES:=usb usbgadget display gpio audio

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=uImage
KERNEL_MAKEOPTS += V=1

define Target/Description
  This target combines the enterprise networking features of the Cortina
  GoldenGate SOC with Android.  We are currently using Gingerbread (2.3).
endef

define BuildJumpTable
	./bin/genCortinaJt.sh > $(LINUX_DIR)/arch/arm/mach-goldengate/include/mach/hw_accel.h
	./bin/genCortinaJtInit.sh > $(LINUX_DIR)/arch/arm/mach-goldengate/cs_jt_init.c
endef

ifeq ($(strip $(CONFIG_EXTERNAL_KERNEL_TREE)),"")
define Kernel/Prepare/Default
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	( cd $(LINUX_DIR); patch -p1 < $(CURDIR)/arm-patches/100-arm.patch )
	$(Kernel/Patch)
	touch $(LINUX_DIR)/.quilt_used
	$(call BuildJumpTable)
endef
endif

# Some adjustments after our base-files have been added to the rootfs.
# Some packages change our files, which are read-only because of CVS,
# so we need to change the permissions for these downstream packages
# to work.
#
define Package/base-files/install-target
	chmod +w $(1)/etc/passwd
endef

$(eval $(call BuildTarget))
