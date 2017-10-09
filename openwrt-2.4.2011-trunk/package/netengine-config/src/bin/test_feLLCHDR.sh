#!/bin/sh
./ne_cfg -m fe -t LLC_HDR -s -c add -b 0

./ne_cfg -m fe -t LLC_HDR -f llc_hdr -v 0x101112
./ne_cfg -m fe -t LLC_HDR -f valid -v 0x13

./ne_cfg -m fe -t LLC_HDR -e
