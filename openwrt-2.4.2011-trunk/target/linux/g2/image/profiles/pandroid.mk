# Copy Android FS
# Copy OpenWRT FS, excluding the /init script and /etc/*
# Copy OpenWRT /etc/* to /system/etc/* (android boot makes a symlink)
# Copy OpenWRT /init to /init.owrt
#
define Image/mkfs/targz
	if [ -z "$(ANDROID_FS)" ]; then \
	  echo; \
	  echo You must define ANDROID_FS env var; \
	  echo; \
	  exit 1; \
	fi
	rm -rf $(BIN_DIR)/android-rootfs
	mkdir -p $(BIN_DIR)/android-rootfs
	$(TAR) -zcf - -C $(ANDROID_FS)/ . | $(TAR) -zxf - -C $(BIN_DIR)/android-rootfs 
	$(TAR) --exclude etc --exclude init -zcf - -C $(TARGET_DIR) . | $(TAR) -zxf - -C $(BIN_DIR)/android-rootfs 
	$(TAR) -zcf - -C $(TARGET_DIR)/etc/ . | $(TAR) -zxf - -C $(BIN_DIR)/android-rootfs/system/etc
	cp -f  $(TARGET_DIR)/init $(BIN_DIR)/android-rootfs/init.owrt
	cp -f  $(TARGET_DIR)/sbin/init $(BIN_DIR)/android-rootfs/sbin/init
	$(TAR) -zcf $(BIN_DIR)/$(IMG_PREFIX)-rootfs.tar.gz --numeric-owner --owner=0 --group=0 -C $(BIN_DIR)/android-rootfs/ .
endef
