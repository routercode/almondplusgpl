#!/bin/sh
./ne_cfg -m fe -t ETYPE -s -c add -b 0

./ne_cfg -m fe -t ETYPE -f ether_type -v 0x1011
./ne_cfg -m fe -t ETYPE -f valid -v 0x12

./ne_cfg -m fe -t ETYPE -e
