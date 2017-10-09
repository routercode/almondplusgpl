#
# Cortina Internet Video Demo Program openwrt integration Makefile
#

include $(TOPDIR)/rules.mk

PKG_NAME:=internet-video-demo
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/

include $(INCLUDE_DIR)/package.mk

define Package/internet-video-demo
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Cortina System Internet Video Demo Utility
  URL:=http://www.cortina-systems.com
  DEPENDS:=+live555 +ffmpeg
endef

define Package/internet-video-demo/description
  This is the Internet Video Demo source package for Cortins Systems GoldenGate
endef

define Build/Prepare
	$(CCP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	CC="$(TARGET_CC)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILIER=$(TARGET_CROSS)
endef

define Package/internet-video-demo/install
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_DIR) $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/inetvideodemo.conf $(1)/etc/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/inetvideodemo $(1)/sbin/
endef

$(eval $(call BuildPackage,internet-video-demo))

