include $(INCLUDE_DIR)/package.mk

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(CP) ./src/* $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(Build/Patch)
endef

define Build/Configure
endef

LUA_TARGET:=source
ifneq ($(CONFIG_PACKAGE_luci-lib-core_compile),)
  LUA_TARGET:=compile
endif

ifneq ($(CONFIG_PACKAGE_luci-lib-core_stripped),)
  LUA_TARGET:=strip
endif

ifneq ($(CONFIG_PACKAGE_luci-lib-core_srcdiet),)
  LUA_TARGET:=diet
endif

define Build/Compile
	( cd $(LUCI_HOME) && \
	  ./build/i18n-po2lua.pl $(PKG_BUILD_DIR)/$(PKG_NAME)/po host/lua-po )
	$(MAKE) -C $(LUCI_HOME) \
		LUCI_HOME=$(LUCI_HOME) \
		HOST=$(LUCI_HOME)/host \
		MODULES=$(PKG_BUILD_DIR)/$(PKG_NAME) \
		LUA_TARGET=$(LUA_TARGET) \
		LUA_SHLIBS="-llua -lm -ldl -lcrypt" \
		CFLAGS="$(TARGET_CFLAGS) $(LUCI_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib" \
		NIXIO_TLS="$(NIXIO_TLS)" OS="Linux" modulebuild
endef

define Package/$(PKG_NAME)/install
	$(CP) -a $(PKG_BUILD_DIR)/$(PKG_NAME)/dist/* $(1)/ -R
	$(CP) -a $(PKG_BUILD_DIR)/$(PKG_NAME)/ipkg/* $(1)/CONTROL/ 2>/dev/null || true
endef

