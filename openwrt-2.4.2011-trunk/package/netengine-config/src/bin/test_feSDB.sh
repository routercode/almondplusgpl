#!/bin/sh
./ne_cfg -m fe -t SDB -s -c add -b 0

./ne_cfg -m fe -t SDB -f sdb_tuple.0.priority -v 0x01
./ne_cfg -m fe -t SDB -f sdb_tuple.0.enable -v 0x02
./ne_cfg -m fe -t SDB -f sdb_tuple.1.priority -v 0x03
./ne_cfg -m fe -t SDB -f sdb_tuple.1.enable -v 0x04
./ne_cfg -m fe -t SDB -f sdb_tuple.2.priority -v 0x05
./ne_cfg -m fe -t SDB -f sdb_tuple.2.enable -v 0x06
./ne_cfg -m fe -t SDB -f sdb_tuple.3.priority -v 0x07
./ne_cfg -m fe -t SDB -f sdb_tuple.3.enable -v 0x08
./ne_cfg -m fe -t SDB -f sdb_tuple.4.priority -v 0x09
./ne_cfg -m fe -t SDB -f sdb_tuple.4.enable -v 0x0a
./ne_cfg -m fe -t SDB -f sdb_tuple.5.priority -v 0x0b
./ne_cfg -m fe -t SDB -f sdb_tuple.5.enable -v 0x0c
./ne_cfg -m fe -t SDB -f sdb_tuple.6.priority -v 0x0d
./ne_cfg -m fe -t SDB -f sdb_tuple.6.enable -v 0x0e
./ne_cfg -m fe -t SDB -f sdb_tuple.7.priority -v 0x0f
./ne_cfg -m fe -t SDB -f sdb_tuple.7.enable -v 0x10

./ne_cfg -m fe -t SDB -f sdb_lpm4.0.start_ptr -v 0x12
./ne_cfg -m fe -t SDB -f sdb_lpm4.0.end_ptr -v 0x13
./ne_cfg -m fe -t SDB -f sdb_lpm4.0.lpm_ptr_en -v 0x15
./ne_cfg -m fe -t SDB -f sdb_lpm4.1.start_ptr -v 0x17
./ne_cfg -m fe -t SDB -f sdb_lpm4.1.end_ptr -v 0x18
./ne_cfg -m fe -t SDB -f sdb_lpm4.1.lpm_ptr_en -v 0x1a
./ne_cfg -m fe -t SDB -f sdb_lpm6.0.start_ptr -v 0x1c
./ne_cfg -m fe -t SDB -f sdb_lpm6.0.end_ptr -v 0x1d
./ne_cfg -m fe -t SDB -f sdb_lpm6.0.lpm_ptr_en -v 0x1f
./ne_cfg -m fe -t SDB -f sdb_lpm6.1.start_ptr -v 0x21
./ne_cfg -m fe -t SDB -f sdb_lpm6.1.end_ptr -v 0x22
./ne_cfg -m fe -t SDB -f sdb_lpm6.1.lpm_ptr_en -v 0x24

./ne_cfg -m fe -t SDB -f lpm_en -v 0x25
    
./ne_cfg -m fe -t SDB -f misc.uu_flowidx -v 0x2627
./ne_cfg -m fe -t SDB -f misc.bc_flowidx -v 0x2829
./ne_cfg -m fe -t SDB -f misc.um_flow.flowidx -v 0x2a2b
./ne_cfg -m fe -t SDB -f misc.rsvd_202 -v 0x2c
    
./ne_cfg -m fe -t SDB -f misc.ttl_hop_limit_zero_discard_en -v 0x2f
./ne_cfg -m fe -t SDB -f misc.key_rule -v 0x30
./ne_cfg -m fe -t SDB -f misc.drop -v 0x31
./ne_cfg -m fe -t SDB -f misc.egr_vln_ingr_mbrshp_en -v 0x32

./ne_cfg -m fe -t SDB -f pvid.pvid -v 0x3334
./ne_cfg -m fe -t SDB -f pvid.pvid_tpid_enc -v 0x35
./ne_cfg -m fe -t SDB -f pvid.pvid_en -v 0x36

./ne_cfg -m fe -t SDB -f misc.use_egrlen_pkttype_policer -v 0x37
./ne_cfg -m fe -t SDB -f misc.use_egrlen_src_policer -v 0x38
./ne_cfg -m fe -t SDB -f misc.use_egrlen_flow_policer -v 0x39

./ne_cfg -m fe -t SDB -f vlan.vlan_ingr_membership_en -v 0x3a
./ne_cfg -m fe -t SDB -f vlan.vlan_egr_membership_en -v 0x3b
./ne_cfg -m fe -t SDB -f vlan.vlan_egr_untag_chk_en -v 0x3c

./ne_cfg -m fe -t SDB -f misc.acl_dsbl -v 0x3d
./ne_cfg -m fe -t SDB -f misc.hash_sts_update_ctrl -v 0x47
./ne_cfg -m fe -t SDB -f parity -v 0x3e

./ne_cfg -m fe -t SDB -f sdb_tuple.0.mask_ptr -v 0x3f
./ne_cfg -m fe -t SDB -f sdb_tuple.1.mask_ptr -v 0x40
./ne_cfg -m fe -t SDB -f sdb_tuple.2.mask_ptr -v 0x41
./ne_cfg -m fe -t SDB -f sdb_tuple.3.mask_ptr -v 0x42
./ne_cfg -m fe -t SDB -f sdb_tuple.4.mask_ptr -v 0x43
./ne_cfg -m fe -t SDB -f sdb_tuple.5.mask_ptr -v 0x44
./ne_cfg -m fe -t SDB -f sdb_tuple.6.mask_ptr -v 0x45
./ne_cfg -m fe -t SDB -f sdb_tuple.7.mask_ptr -v 0x46

./ne_cfg -m fe -t SDB -e
