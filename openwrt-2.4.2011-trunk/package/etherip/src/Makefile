ifneq ($(KERNELRELEASE),)
  obj-m := etherip.o etherip6.o
else
  KERNELDIR ?= /lib/modules/$(shell uname -r)/build
  MODULEDIR ?= /lib/modules/$(shell uname -r)/
  PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(MAKE) ethiptunnel
	$(MAKE) ethip6tunnel

ethiptunnel: ethiptunnel.c
	$(CC) -Wall -O3 -o ethiptunnel ethiptunnel.c

ethip6tunnel: ethip6tunnel.c
	$(CC) -Wall -O3 -o ethip6tunnel ethip6tunnel.c

install:
	cp etherip.ko $(MODULEDIR)/kernel/net/ipv4/
	mkdir -p $(HOME)/bin
	cp ethiptunnel $(HOME)/bin/
	depmod -a
	cp etherip6.ko $(MODULEDIR)/kernel/net/ipv6/
	mkdir -p $(HOME)/bin
	cp ethip6tunnel $(HOME)/bin/
	depmod -a

clean:
	@rm -f etherip6.ko etherip6.mod.c etherip6.mod.o etherip6.o
	@rm -f etherip.ko etherip.mod.c etherip.mod.o etherip.o
	@rm -f ethiptunnel ethip6tunnel Module.symvers
	@find -type f -name "\.*"|xargs rm -f
	@rm -rf .tmp_versions
	@rm -f modules.order

.PHONY: clean

endif
