include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=xl2tpd
PKG_VERSION:=1.3.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.openswan.org/xl2tpd/
PKG_MD5SUM:=cf61576fef5c2d6c68279a408ec1f0d5

include $(INCLUDE_DIR)/package.mk

define Package/xl2tpd
	SECTION:=net
	CATEGORY:=Network
	TITLE:=L2TP daemon
	URL:=ftp://ftp.openswan.org/xl2tpd/
	DEPENDS:=+ppp
endef

define Package/xl2tpd/description
	This package contains a L2TP (Layer Two Tunneling Protocol) daemon
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		xl2tpd xl2tpd-control
endef
       
define Package/xl2tpd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xl2tpd $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xl2tpd-control $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./files/options.xl2tpd $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/xl2tpd
	$(INSTALL_DATA) ./files/xl2tpd.conf $(1)/etc/xl2tpd/
	$(INSTALL_DATA) ./files/l2tp-secrets $(1)/etc/xl2tpd/
	$(INSTALL_DATA) ./files/start_xl2tpd.sh $(1)/etc/xl2tpd/
endef

$(eval $(call BuildPackage,xl2tpd))
