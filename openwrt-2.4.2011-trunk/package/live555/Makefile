#
# Cortina version of live555 openwrt integration Makefile
#

include $(TOPDIR)/rules.mk

PKG_NAME:=live555
PKG_VERSION:=20110520
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/

include $(INCLUDE_DIR)/package.mk

define Package/live555
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=LIVE555 Streaming Media for Cortins Systems GoldenGate
  URL:=http://www.cortina-systems.com
endef

define Package/live555/description
  This is the LIVE555 source package for Cortins Systems GoldenGate
endef

define Build/Prepare
	$(CCP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS)
endef

define Package/live555/install
    $(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mediaServer/live555MediaServer $(1)/usr/bin/
endef

$(eval $(call BuildPackage,live555))
