BOARDNAME:=ANDROID

define Target/Description
	Use this profile to compile open source software to overlay
	into an existing, already built Android file system.
endef

# Override the default packages here!!
#
DEFAULT_PACKAGES:=base-files libc libgcc \
	busybox dropbear mtd uci ucitrigger opkg udevtrigger hotplug2 \
	dnsmasq iptables ppp firewall udev \
	ip igmpproxy miniupnpd \
	rp-pppoe-client rp-pppoe-relay rp-pppoe-sniff \
	zile

# This macro effectively overrides BuildKernel, so we
# gain control over the various steps involving the
# kernel ... prepare, config, make and install.  It 
# is assumed that the Android kernel has already been
# configured and built, so we do not want to disturb
# that.  THE WAY THIS MACRO CURRENT STANDS is that
# any openwrt config that affects the kernel, like
# package/kernel/modules/*.mk will not happen!!
#
#
ANDROID_VERSION:=2.6.35
OWRT_PATCH_DIR:=patches-$(ANDROID_VERSION)
define BuildTarget
  $(STAMP_PREPARED):
	-rm -rf $(KERNEL_BUILD_DIR)
	-mkdir -p $(KERNEL_BUILD_DIR)
	$(Kernel/Prepare)
	touch $$@

  $(LINUX_DIR)/.opatched: $(STAMP_PREPARED)
	( cd $(LINUX_DIR); \
	  for pfile in `ls $(GENERIC_PLATFORM_DIR)/$(OWRT_PATCH_DIR)/*netfilter*`; do \
	    patch -p1 < $$$$pfile; \
	  done )
	touch $$@

  $(STAMP_CONFIGURED): $(LINUX_DIR)/.opatched $(TOPDIR)/.config $(SUBTARGET)/config-default $(SUBTARGET)/config-g2 $(LINUX_DIR)/.config
	$(SCRIPT_DIR)/kconfig.pl '+' $(SUBTARGET)/config-default $(SUBTARGET)/config-g2 > $(LINUX_DIR)/.config
	touch $$@

  mostlyclean:

  define BuildKernel
  endef

  download:
  prepare: $(STAMP_CONFIGURED)
  compile: $(STAMP_CONFIGURED)
	$(MAKE) -C image compile TARGET_BUILD=

  oldconfig menuconfig nconfig: $(STAMP_CONFIGURED) FORCE
	[ -e "$(LINUX_CONFIG)" ] || touch "$(LINUX_CONFIG)"
	$(_SINGLE)$(MAKE) -C $(LINUX_DIR) $(KERNEL_MAKEOPTS) $$@
	chmod +w $(SUBTARGET)/config-g2
	$(SCRIPT_DIR)/kconfig.pl '>' $(SUBTARGET)/config-default $(LINUX_DIR)/.config > $(SUBTARGET)/config-g2

  install:
	+$(MAKE) -C image compile install TARGET_BUILD=

  clean: FORCE
	rm -rf $(KERNEL_BUILD_DIR)

  image-prereq:
	@+$(NO_TRACE_MAKE) -s -C image prereq TARGET_BUILD=

  prereq: image-prereq

endef
