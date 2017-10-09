#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=rboot
PKG_VERSION:=0.99
PKG_RELEASE:=1

# This URL won't exist at a customer site, but the
# tarball will have been delivered pre-cached in owrt/dl.
#
PKG_SOURCE_URL = http://cs-tarball-server:9090/mirror
PKG_SOURCE     = $(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

# Have to over-write tar command because the
# structure of rboot-0.99.tar.gz does not have
# a rboot-0.99/ hierarchy.
#
TAR_CMD=$(HOST_TAR) -C $(1) $(TAR_OPTIONS)

define Package/rboot
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Cortina G2 RCPU boot utility
	URL:=http://www.cortina-systems.com/
endef

define Package/rboot/Description
	rboot - Cortina Systems Golden Gate Re-Circulation CPU boot utilities
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CROSS)gcc"
endef

define Package/rboot/install
	$(INSTALL_DIR) $(1)/rboot/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rboot $(1)/rboot/
endef

$(eval $(call BuildPackage,rboot))

