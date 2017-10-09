#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=dynamic_suspend
PKG_VERSION:=0.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/dynamic_suspend
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Cortina G2 dynamically suspend
	URL:=http://www.cortina-systems.com/
endef

define Package/dynamic_suspend/Description
	dynamic_suspend - Dynamically suspend system
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -af ./src/* $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	CC="$(TARGET_CROSS)gcc"
endef

define Package/dynamic_suspend/install
	$(INSTALL_DIR) $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dynamic_suspend $(1)/sbin/dynamic_suspend
endef

$(eval $(call BuildPackage,dynamic_suspend))