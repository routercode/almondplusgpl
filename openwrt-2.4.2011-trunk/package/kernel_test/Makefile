#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(TOPDIR)/include/kernel.mk

PKG_NAME:=kernel_test
PKG_VERSION:=0.99
PKG_RELEASE:=1
##PKG_MD5SUM:=89273f67a6d8067cbbecefaa13747153

##PKG_SOURCE_URL:=ftp://ftp.samba.org/pub/samba/old-versions/
##PKG_SOURCE:=kernel_test-$(PKG_VERSION).tar.gz
##PKG_BUILD_DIR:=$(BUILD_DIR)/kernel_test/

include $(INCLUDE_DIR)/package.mk

define Package/kernel_test
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Cortina G2 testing utilities
	URL:=http://www.cortina-systems.com/
	DEPENDS:=+libevent
endef

define Package/kernel_test/Description
	kernel_test - Cortina Systems Golden Gate kernel test utilities
endef

MAKE_FLAGS += KERNDIR=$(LINUX_DIR)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

TARGET_CFLAGS += -I$(LINUX_DIR)/include -I$(LINUX_DIR)/arch/arm/mach-goldengate/include -I$(LINUX_DIR)/drivers/net/rtl83xx

define Build/Compile
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/reg_rw \
                $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR)/fb_test \
	$(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR)/sd_test \
	$(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR)/ts_test \
	$(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)"

	 CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/rtc_test \
        $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/wdt_test \
        $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/ssp_test \
        $(TARGET_CONFIGURE_OPTS) LINUX_DIR="$(LINUX_DIR)" \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/spdif_test \
        $(TARGET_CONFIGURE_OPTS) LINUX_DIR="$(LINUX_DIR)" \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/spi_test \
        $(TARGET_CONFIGURE_OPTS) LINUX_DIR="$(LINUX_DIR)" \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/i2c_test \
        $(TARGET_CONFIGURE_OPTS) LINUX_DIR="$(LINUX_DIR)" \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/ir_test \
        $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/pwr_test \
        $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/mtd_test \
        $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"


	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/usb_test \
        $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/crypto_test \
        $(TARGET_CONFIGURE_OPTS) LINUX_DIR="$(LINUX_DIR)" \
                LIBS="$(TARGET_LDFLAGS)"

	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PKG_BUILD_DIR)/crypto_alg_if modules \
                CC="$(TARGET_CC)" \
                LD="$(TARGET_CROSS)ld" \
		PKG_BUILD_DIR="$(PKG_BUILD_DIR)" \
                ARCH="$(LINUX_KARCH)"

	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
        $(MAKE) -C $(PKG_BUILD_DIR)/pkt_fwd_ctl \
                $(TARGET_CONFIGURE_OPTS) \
                LIBS="$(TARGET_LDFLAGS)"

endef

define Package/kernel_test/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR)  $(1)/etc

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/reg_rw/reg_rw $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_hybridtest.sh $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/test2part.sh $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/ddtest.sh $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_endurance.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_sequence.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_enduranceram.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_vfat.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_secondendur.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_ramfs.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_format.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_dd_blksz.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/sd_dd_test.cfg $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/gensequfile $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sd_test/getimeofday $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fb_test/fbtest $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fb_test/drawbox.sh $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fb_test/lcd_test.sh $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fb_test/dump_anx9805_reg.sh $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ts_test/ts_test $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ts_test/ratetest $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ts_test/ts_config $(1)/sbin/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ts_test/ts.cfg $(1)/etc/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtc_test/rtc-test $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wdt_test/watchdog-test $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ssp_test/dac-test $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ssp_test/zphone-test $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ir_test/ir-test $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pwr_test/pwr-test $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/spdif_test/spdif-test $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/spi_test/spi_test $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/i2c_test/i2c_test $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtd_test/mtd_debug $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtd_test/2kdata.bin $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtd_test/testran.bin $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usb_test/mousetest $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usb_test/usb_gpio_1 $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usb_test/usbled $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/crypto_test/crypto-hash-test $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/crypto_test/crypto-cipher-test $(1)/sbin/
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/crypto_alg_if/*.ko $(1)/lib/modules/$(LINUX_VERSION)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pkt_fwd_ctl/pkt_fwd_ctl $(1)/sbin/

endef

$(eval $(call BuildPackage,kernel_test))

