#!/bin/sh
./ne_cfg -m fe -t FWDRSLT -s -c add -b 0

./ne_cfg -m fe -t FWDRSLT -f l2.mac_sa_replace_en -v 0x01
./ne_cfg -m fe -t FWDRSLT -f l2.mac_da_replace_en -v 0x02
./ne_cfg -m fe -t FWDRSLT -f l2.l2_index -v 0x0304
./ne_cfg -m fe -t FWDRSLT -f l2.mcgid -v 0x0506
./ne_cfg -m fe -t FWDRSLT -f l2.mcgid_valid -v 0x07
./ne_cfg -m fe -t FWDRSLT -f l2.flow_vlan_op_en -v 0x08
./ne_cfg -m fe -t FWDRSLT -f l2.flow_vlan_index -v 0x090a
./ne_cfg -m fe -t FWDRSLT -f l2.pppoe_encap_en -v 0x0b
./ne_cfg -m fe -t FWDRSLT -f l2.pppoe_decap_en -v 0x0c

./ne_cfg -m fe -t FWDRSLT -f l3.ip_sa_replace_en -v 0x0d
./ne_cfg -m fe -t FWDRSLT -f l3.ip_da_replace_en -v 0x0e
./ne_cfg -m fe -t FWDRSLT -f l3.ip_sa_index -v 0x0f10
./ne_cfg -m fe -t FWDRSLT -f l3.ip_da_index -v 0x1112
./ne_cfg -m fe -t FWDRSLT -f l3.decr_ttl_hoplimit -v 0x13
    
./ne_cfg -m fe -t FWDRSLT -f l4.sp_replace_en -v 0x14
./ne_cfg -m fe -t FWDRSLT -f l4.dp_replace_en -v 0x15
./ne_cfg -m fe -t FWDRSLT -f l4.sp -v 0x1617
./ne_cfg -m fe -t FWDRSLT -f l4.dp -v 0x1819

./ne_cfg -m fe -t FWDRSLT -f dest.pol_policy -v 0x1a
./ne_cfg -m fe -t FWDRSLT -f dest.voq_policy -v 0x1b
./ne_cfg -m fe -t FWDRSLT -f dest.voq_pol_table_index -v 0x1c1d
    
./ne_cfg -m fe -t FWDRSLT -f act.fwd_type_valid -v 0x1e
./ne_cfg -m fe -t FWDRSLT -f act.fwd_type -v 0x1f
./ne_cfg -m fe -t FWDRSLT -f act.drop -v 0x20
    
./ne_cfg -m fe -t FWDRSLT -f acl_dsbl -v 0x21
./ne_cfg -m fe -t FWDRSLT -f parity -v 0x22

./ne_cfg -m fe -t FWDRSLT -e
