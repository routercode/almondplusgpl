#!/bin/sh
./ne_cfg -m fe -t AN_BNG_MAC -s -c add -b 0

./ne_cfg -m fe -t AN_BNG_MAC -f mac.0 -v 0x01
./ne_cfg -m fe -t AN_BNG_MAC -f mac.1 -v 0x02
./ne_cfg -m fe -t AN_BNG_MAC -f mac.2 -v 0x03
./ne_cfg -m fe -t AN_BNG_MAC -f mac.3 -v 0x04
./ne_cfg -m fe -t AN_BNG_MAC -f mac.4 -v 0x05
./ne_cfg -m fe -t AN_BNG_MAC -f mac.5 -v 0x06
./ne_cfg -m fe -t AN_BNG_MAC -f sa_da -v 0x07
./ne_cfg -m fe -t AN_BNG_MAC -f pspid -v 0x08
./ne_cfg -m fe -t AN_BNG_MAC -f pspid_mask -v 0x09
./ne_cfg -m fe -t AN_BNG_MAC -f valid -v 0x0a

./ne_cfg -m fe -t AN_BNG_MAC -e
