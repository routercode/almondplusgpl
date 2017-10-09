#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
##include $(TOPDIR)/include/kernel.mk

PKG_NAME:=button_utils
PKG_VERSION:=1.0.0
PKG_RELEASE:=0

include $(INCLUDE_DIR)/package.mk

define Package/button_utils
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Cortina G2 button utilities 
  URL:=http://www.cortina-systems.com/
endef

define Package/button_utils/Description
  mxl241sf - Cortina Systems Golden Gate button utilities
endef

MAKE_FLAGS += KERNDIR=$(LINUX_DIR)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

##TARGET_CFLAGS += -I$(LINUX_DIR)/include -I$(LINUX_DIR)/arch/arm/mach-goldengate/include

define Build/Compile
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	LIBS="$(TARGET_LDFLAGS)"
endef

define Package/button_utils/install
	$(INSTALL_DIR) $(1)/sbin 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/input_button $(1)/sbin/
endef

$(eval $(call BuildPackage,button_utils))

