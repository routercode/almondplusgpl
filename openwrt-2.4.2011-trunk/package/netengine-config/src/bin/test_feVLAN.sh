#!/bin/sh
./ne_cfg -m fe -t VLAN -s -c add -b 0

./ne_cfg -m fe -t VLAN -f vlan_member -v 0x10
./ne_cfg -m fe -t VLAN -f vlan_egress_untagged -v 0x11
./ne_cfg -m fe -t VLAN -f vlan_fid -v 0x12
./ne_cfg -m fe -t VLAN -f vlan_first_vlan_cmd -v 0x13
./ne_cfg -m fe -t VLAN -f vlan_first_vid -v 0x1415
./ne_cfg -m fe -t VLAN -f vlan_first_tpid_enc -v 0x16
./ne_cfg -m fe -t VLAN -f vlan_second_vlan_cmd -v 0x17
./ne_cfg -m fe -t VLAN -f vlan_second_vid -v 0x1819
./ne_cfg -m fe -t VLAN -f vlan_second_tpid_enc -v 0x1a
./ne_cfg -m fe -t VLAN -f vlan_mcgid -v 0x1b1c
./ne_cfg -m fe -t VLAN -f parity -v 0x1d

./ne_cfg -m fe -t VLAN -e
