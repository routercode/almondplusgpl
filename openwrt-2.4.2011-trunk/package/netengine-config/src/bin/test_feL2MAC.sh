#!/bin/sh
./ne_cfg -m fe -t L2_MAC -s -c add -b 0

./ne_cfg -m fe -t L2_MAC -f mac_sa.0 -v 0x01
./ne_cfg -m fe -t L2_MAC -f mac_sa.1 -v 0x02
./ne_cfg -m fe -t L2_MAC -f mac_sa.2 -v 0x03
./ne_cfg -m fe -t L2_MAC -f mac_sa.3 -v 0x04
./ne_cfg -m fe -t L2_MAC -f mac_sa.4 -v 0x05
./ne_cfg -m fe -t L2_MAC -f mac_sa.5 -v 0x06
./ne_cfg -m fe -t L2_MAC -f sa_count -v 0x0708
./ne_cfg -m fe -t L2_MAC -f mac_da.0 -v 0x09
./ne_cfg -m fe -t L2_MAC -f mac_da.1 -v 0x0a
./ne_cfg -m fe -t L2_MAC -f mac_da.2 -v 0x0b
./ne_cfg -m fe -t L2_MAC -f mac_da.3 -v 0x0c
./ne_cfg -m fe -t L2_MAC -f mac_da.4 -v 0x0d
./ne_cfg -m fe -t L2_MAC -f mac_da.5 -v 0x0e
./ne_cfg -m fe -t L2_MAC -f da_count -v 0x0f10

./ne_cfg -m fe -t L2_MAC -e
