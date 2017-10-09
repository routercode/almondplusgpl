define Profile/minimal
  NAME:=minimal
  PACKAGES:=
endef

define Profile/minimal/Description
	Absolute minimal set of packages to implement a
	basic OpenWRT -style RG on Android.
endef

$(eval $(call Profile,minimal))
