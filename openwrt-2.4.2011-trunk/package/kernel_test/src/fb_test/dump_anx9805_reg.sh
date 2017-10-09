#!/bin/sh
#
#  dump_anx9805_reg.sh
#
# Copyright (c) Cortina-Systems Limited 2011.  All rights reserved.
#
# Author: Joe Hsu <joe.hsu@cortina-systems.com>
#
# sh script for dumping registers of and9805 hdmi transmitter

reg_limit=256

dp_addr=38
system_addr=39
hdmi_addr=3d
dev_addrs="$dp_addr $system_addr $hdmi_addr"

for dev_addr in $dev_addrs; do
    regoff=0
    while(true)
    do
        if [ x"$reg_limit" = x"$regoff" ]; then
            break
        fi
        reghex=`printf "%x" $regoff`
        i2c_test -a $dev_addr -s 1 -o $reghex -r
        let regoff+=1
    done
done

