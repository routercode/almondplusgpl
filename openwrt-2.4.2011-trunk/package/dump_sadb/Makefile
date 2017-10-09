#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=dump_sadb
PKG_VERSION:=0.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/dump_sadb
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Cortina G2 dump IPSec v4 tunnels
	URL:=http://www.cortina-systems.com/
endef

define Package/dump_sadb/Description
	dump_sadb - Dump IPSec v4 tunnels SA information
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
    CC="$(TARGET_CROSS)gcc"
endef

define Package/dump_sadb/install
	$(INSTALL_DIR) $(1)/dump_sadb/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dump_sadb $(1)/dump_sadb/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/r1.txt $(1)/dump_sadb/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/r2.txt $(1)/dump_sadb/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cfg_pe_fab.sh $(1)/dump_sadb/
endef

$(eval $(call BuildPackage,dump_sadb))

