NETWORK_DEVICES_MENU:=Network Support
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=etherip
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ethiptunnel
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=IPv4 etherip tool
endef

define Package/ethiptunnel/description
	This package contains IPv4 etherip utility
endef

define Package/ethip6tunnel
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=IPv6 etherip tool
endef

define Package/ethip6tunnel/description
	This package contains IPv6 etherip utility
endef

define KernelPackage/etherip
	SUBMENU:=$(NETWORK_DEVICES_MENU)
	TITLE:=IPv4 etherip drivers
	FILES:=$(PKG_BUILD_DIR)/etherip.$(LINUX_KMOD_SUFFIX)
	AUTOLOAD:=$(call AutoLoad,20,etherip)
endef

define KernelPackage/etherip/description
	This package contains IPv4 etherip drivers
endef

define KernelPackage/etherip6
	SUBMENU:=$(NETWORK_DEVICES_MENU)
	TITLE:=IPv6 etherip drivers
	FILES:=$(PKG_BUILD_DIR)/etherip6.$(LINUX_KMOD_SUFFIX)
	AUTOLOAD:=$(call AutoLoad,20,etherip6)
endef

define KernelPackage/etherip6/description
	This package contains IPv6 etherip drivers
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="-I$(TARGET_CFLAGS)" \
		ethiptunnel
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="-I$(TARGET_CFLAGS)" \
		ethip6tunnel
endef
       
define Package/ethiptunnel/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ethiptunnel $(1)/usr/bin/
endef

define Package/ethip6tunnel/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ethip6tunnel $(1)/usr/bin/
endef
$(eval $(call KernelPackage,etherip))
$(eval $(call BuildPackage,ethiptunnel))
$(eval $(call KernelPackage,etherip6))
$(eval $(call BuildPackage,ethip6tunnel))
