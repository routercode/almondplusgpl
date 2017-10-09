#
# Proprietary Information of Cortina Technologies
# Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.
#
# file      : Makefile
# Author    : Axl Lee <axl.lee@cortina-systems.com>
#
NECFG_ROOT=.

#BUILD_OPT = DO_APP=1

all:
	echo "K1=${K1}"
	make -C ./ne_cfg ${BUILD_OPT}
	make -C ./neSendCmd ${BUILD_OPT}

clean:
	rm -f tags
	rm -f *.o *.core core *.bak *~
	make -C ./bin clean
	make -C ./ne_cfg clean
	make -C ./neSendCmd clean

	
