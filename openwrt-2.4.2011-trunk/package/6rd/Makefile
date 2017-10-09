include $(TOPDIR)/rules.mk

PKG_NAME:=6rd
PKG_VERSION:=1.0
PKG_RELEASE:=0

include $(INCLUDE_DIR)/package.mk

define Package/6rd
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+kmod-ipv6
  TITLE:=IPv6 6rd
  PKGARCH:=all
endef

define Package/6rd/description
IPv6 script to setup 6rd ipv6 connectivity
endef

define Package/6rd/conffiles
/etc/config/6rd
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/6rd/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/6rd.conf $(1)/etc/config/6rd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/6rd.init $(1)/etc/init.d/6rd
endef

$(eval $(call BuildPackage,6rd))
