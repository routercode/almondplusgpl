#
# Cortina Systems g2_diags application
#
include $(TOPDIR)/rules.mk

PKG_NAME:=g2_diags
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/target.mk

define Package/g2_diags
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=G2 Diagnostic Utilities
endef

# To "extract", just copy the source.  Use CCP to enable incremental
# compiles.
#
define Build/Prepare
	$(CCP) ./src/* $(PKG_BUILD_DIR)
endef

# No configure
#
define Build/Configure
endef

# These MAKE_FLAGS will be added to the compile line by openwrt
#
MAKE_FLAGS += CROSS_COMPILE=$(TARGET_CROSS) \
	IDIR_INCS="-I$(LINUX_DIR)/arch/arm/mach-goldengate/include/mach"

define Package/g2_diags/install
	$(INSTALL_DIR) $(1)/bin 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/g2_diags $(1)/bin/g2_diags
endef

$(eval $(call BuildPackage,g2_diags))
