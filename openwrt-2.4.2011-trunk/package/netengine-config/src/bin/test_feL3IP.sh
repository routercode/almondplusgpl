#!/bin/sh
./ne_cfg -m fe -t L3_IP -s -c add -b 0

./ne_cfg -m fe -t L3_IP -f ip_addr.0 -v 0x01020304
./ne_cfg -m fe -t L3_IP -f ip_addr.1 -v 0x05060708
./ne_cfg -m fe -t L3_IP -f ip_addr.2 -v 0x090a0b0c
./ne_cfg -m fe -t L3_IP -f ip_addr.3 -v 0x0d0e0f10
./ne_cfg -m fe -t L3_IP -f count.0 -v 0x1112
./ne_cfg -m fe -t L3_IP -f count.1 -v 0x1314
./ne_cfg -m fe -t L3_IP -f count.2 -v 0x1516
./ne_cfg -m fe -t L3_IP -f count.3 -v 0x1718

./ne_cfg -m fe -t L3_IP -e
