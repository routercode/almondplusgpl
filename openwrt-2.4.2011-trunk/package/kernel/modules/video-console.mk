VC_MENU:=G2 LCD Support

define KernelPackage/video-console
  SUBMENU:=$(VC_MENU)
  TITLE:=Console on LCD Panel
  FILES:=$(LINUX_DIR)/drivers/video/fb.ko \
	$(LINUX_DIR)/drivers/video/console/fbcon.ko \
	$(LINUX_DIR)/drivers/video/console/bitblit.ko \
	$(LINUX_DIR)/drivers/video/console/font.ko \
	$(LINUX_DIR)/drivers/video/console/softcursor.ko \
	$(LINUX_DIR)/drivers/video/console/tileblit.ko \
	$(LINUX_DIR)/drivers/video/console/fbcon_rotate.ko \
	$(LINUX_DIR)/drivers/video/console/fbcon_cw.ko \
	$(LINUX_DIR)/drivers/video/console/fbcon_ud.ko \
	$(LINUX_DIR)/drivers/video/console/fbcon_ccw.ko

  KCONFIG:=\
	CONFIG_FB_BIG_ENDIAN=n \
	CONFIG_FB_BOTH_ENDIAN=y \
	CONFIG_FB_FOREIGN_ENDIAN=y \
	CONFIG_FB_LITTLE_ENDIAN=n \
	CONFIG_FB_MODE_HELPERS=y \
	CONFIG_FB_TILEBLITTING=y \
	CONFIG_FIRMWARE_EDID=y \
	CONFIG_FONTS=y \
	CONFIG_FONT_10x18=n \
	CONFIG_FONT_6x11=n \
	CONFIG_FONT_7x14=n \
	CONFIG_FONT_ACORN_8x8=n \
	CONFIG_FONT_MINI_4x6=n \
	CONFIG_FONT_PEARL_8x8=n \
	CONFIG_FONT_SUN12x22=n \
	CONFIG_FONT_SUN8x16=n \
	CONFIG_FONT_8x8=y \
	CONFIG_FONT_8x16=y \
	CONFIG_FRAMEBUFFER_CONSOLE_DETECT_PRIMARY=y \
	CONFIG_FRAMEBUFFER_CONSOLE_ROTATION=y \
	CONFIG_VT_HW_CONSOLE_BINDING=y \
	CONFIG_FB \
	CONFIG_FRAMEBUFFER_CONSOLE \
	CONFIG_LOGO=y \
	CONFIG_LOGO_LINUX_CLUT224=y
    AUTOLOAD:=$(call AutoLoad,95,\
	fb \
	fbcon \
	bitblit \
	font \
	softcursor \
	tileblit \
	fbcon_rotate \
	fbcon_cw \
	fbcon_ud \
	fbcon_ccw \
	)
endef

define KernelPackage/video-console/description
  Display the console on the attached LCD panel.
endef

$(eval $(call KernelPackage,video-console))


