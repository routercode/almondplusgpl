#
# Cortina Systems anx9805 application
#
include $(TOPDIR)/rules.mk

PKG_NAME:=anx9805
PKG_VERSION:=1.1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/anx9805
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=G2 Anx9805 HDMI Utilities
endef

# To "extract", just copy the source.  Use CCP to enable incremental
# compiles.
#
define Build/Prepare
	$(CCP) ./ANX9805-1.1.0/* $(PKG_BUILD_DIR)
endef

# No configure
#
define Build/Configure
endef

# These MAKE_FLAGS will be added to the compile line by openwrt
#
MAKE_FLAGS += CROSS_COMPILE=$(TARGET_CROSS)

define Package/anx9805/install
	$(INSTALL_DIR) $(1)/bin 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/anx9805 $(1)/bin/anx9805
endef

$(eval $(call BuildPackage,anx9805))
