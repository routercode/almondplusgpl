#!/bin/sh
./ne_cfg -m fe -t FVLAN -s -c add -b 0

./ne_cfg -m fe -t FVLAN -f first_vlan_cmd -v 0x10
./ne_cfg -m fe -t FVLAN -f first_vid -v 0x1112
./ne_cfg -m fe -t FVLAN -f first_tpid_enc -v 0x13
./ne_cfg -m fe -t FVLAN -f second_vlan_cmd -v 0x14
./ne_cfg -m fe -t FVLAN -f second_vid -v 0x1516
./ne_cfg -m fe -t FVLAN -f second_tpid_enc -v 0x17
./ne_cfg -m fe -t FVLAN -f parity -v 0x18

./ne_cfg -m fe -t FVLAN -e
