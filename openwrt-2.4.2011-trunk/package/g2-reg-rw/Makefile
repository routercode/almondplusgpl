#
# Cortina Systems g2-reg-rw application
#
include $(TOPDIR)/rules.mk

PKG_NAME:=g2-reg-rw
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/g2-reg-rw
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Command line utility for reading/writing G2 device registers
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
MAKE_FLAGS += CROSS_COMPILE=$(TARGET_CROSS)

define Package/g2-reg-rw/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/g2-reg-rw $(1)/bin/g2-reg-rw
endef

$(eval $(call BuildPackage,g2-reg-rw))
