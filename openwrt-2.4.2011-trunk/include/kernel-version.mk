# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),2.6.30.10)
  LINUX_KERNEL_MD5SUM:=eb6be465f914275967a5602cb33662f5
endif
ifeq ($(LINUX_VERSION),2.6.31.14)
  LINUX_KERNEL_MD5SUM:=3e7feb224197d8e174a90dd3759979fd
endif
ifeq ($(LINUX_VERSION),2.6.32.27)
  LINUX_KERNEL_MD5SUM:=c8df8bed01a3b7e4ce13563e74181d71
endif
ifeq ($(LINUX_VERSION),2.6.34.7)
  LINUX_KERNEL_MD5SUM:=8964e26120e84844998a673464a980ea
endif
ifeq ($(LINUX_VERSION),2.6.35.9)
  LINUX_KERNEL_MD5SUM:=18d339e9229560e73c4249dffdc3fd90
endif
ifeq ($(LINUX_VERSION),2.6.36.2)
  LINUX_KERNEL_MD5SUM:=d465f8ba05bf1b7530c596f1cca658e7
endif
ifeq ($(LINUX_VERSION),2.6.37)
  LINUX_KERNEL_MD5SUM:=c8ee37b4fdccdb651e0603d35350b434
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))

