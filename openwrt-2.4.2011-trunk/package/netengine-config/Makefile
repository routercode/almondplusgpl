#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=netengine_config
PKG_VERSION:=0.99
PKG_RELEASE:=1
##PKG_MD5SUM:=89273f67a6d8067cbbecefaa13747153

##PKG_SOURCE_URL:=ftp://ftp.samba.org/pub/samba/old-versions/
##PKG_SOURCE:=netengine_config-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/netengine_config/

include $(INCLUDE_DIR)/package.mk

define Package/netengine_config
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Cortina G2 net engine configure utilities
	URL:=http://www.cortina-systems.com/
endef

define Package/netengine_config/Description
	netengine_config - Cortina Systems Golden Gate net engine configure utilities
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) KERNEL_INC1=$(LINUX_DIR)/include \
        KERNEL_INC2=$(LINUX_DIR)/arch/arm/mach-goldengate/include/mach \
        -C $(PKG_BUILD_DIR)
endef

define Package/netengine_config/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/sbin/
	$(INSTALL_DIR) $(1)/sbin/cs75xx/ne_scripts/
	$(INSTALL_DIR) $(1)/sbin/cs75xx/pppoe/
	$(INSTALL_DIR) $(1)/sbin/cs75xx/bhr/
	$(INSTALL_DIR) $(1)/sbin/cs75xx/iplip/
	$(INSTALL_BIN) ./files/neSendCmd.init $(1)/etc/init.d/neSendCmd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/ne_cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/*.sh $(1)/sbin/cs75xx/ne_scripts/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/ne_scripts/*.sh $(1)/sbin/cs75xx/ne_scripts/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/sumitomo/*.sh $(1)/sbin/cs75xx/pppoe/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/sumitomo/*.network $(1)/sbin/cs75xx/pppoe/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/sumitomo/*.radvd $(1)/sbin/cs75xx/pppoe/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/bhr/*.sh $(1)/sbin/cs75xx/bhr/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/iplip/*.sh $(1)/sbin/cs75xx/iplip/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/neSendCmd $(1)/sbin/
endef

$(eval $(call BuildPackage,netengine_config))

