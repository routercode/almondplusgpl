#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=packet_generator
PKG_VERSION:=0.99
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/packet_generator/

include $(INCLUDE_DIR)/package.mk

define Package/packet_generator
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Cortina G2 packet generator utilities
	URL:=http://www.cortina-systems.com/
endef

define Package/packet_generator/Description
	packet_generator - Cortina Systems GoldenGate packet generator utilities
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

define Package/packet_generator/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/sbin/cs75xx/pktgen_des/
	$(INSTALL_DIR) $(1)/sbin/cs75xx/ne_scripts/
	$(INSTALL_DIR) $(1)/sbin/cs75xx/cd_router/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/pktgen $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/*.des $(1)/sbin/cs75xx/pktgen_des/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/ne_scripts/*.sh $(1)/sbin/cs75xx/ne_scripts/
	find $(PKG_BUILD_DIR)/bin/cd_router -type f -exec $(INSTALL_BIN)  '{}'  $(1)/sbin/cs75xx/cd_router/ \;
	
endef

$(eval $(call BuildPackage,packet_generator))

