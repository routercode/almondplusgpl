include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=g2-buildid
PKG_VERSION:=1.0
PKG_RELEASE:=0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/g2-buildid
  SUBMENU:=G2 Misc
  TITLE:=Cortina build id information in /proc/csinfo
  FILES:=$(PKG_BUILD_DIR)/g2-buildid.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,g2-buildid)
endef

define KernelPackage/g2-buildid/description
  This kernel module will supply build id and version information
  in /proc/csinfo.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

BS_BUILDID ?= g2-$(shell date "+%s")
BS_BUILDTAG = $(shell cat $(TOPDIR)/../version)
ifeq ($(BS_BUILDTAG),)
  BS_BUILDTAG=unknown
endif
EXTRA_CFLAGS:=\
	-DBS_BUILDID=$(BS_BUILDID) \
	-DBS_BUILDTAG=$(BS_BUILDTAG)

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG) \
	V=1

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,g2-buildid))
