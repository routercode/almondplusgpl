#!/bin/sh
./ne_cfg -m fe -t Classifier -s -c add -b 0

./ne_cfg -m fe -t Classifier -f sdb_idx -v 0x01
./ne_cfg -m fe -t Classifier -f rule_priority -v 0x02
./ne_cfg -m fe -t Classifier -f entry_valid -v 0x03
./ne_cfg -m fe -t Classifier -f parity -v 0x04
    
./ne_cfg -m fe -t Classifier -f port.lspid -v 0x05
./ne_cfg -m fe -t Classifier -f port.hdr_a_orig_lspid -v 0x06
./ne_cfg -m fe -t Classifier -f port.fwd_type -v 0x07
./ne_cfg -m fe -t Classifier -f port.hdr_a_flags_crcerr -v 0x08
./ne_cfg -m fe -t Classifier -f port.l3_csum_err -v 0x09
./ne_cfg -m fe -t Classifier -f port.l4_csum_err -v 0x0a
./ne_cfg -m fe -t Classifier -f port.not_hdr_a_flags_stsvld -v 0x0b
./ne_cfg -m fe -t Classifier -f port.lspid_mask -v 0x0c
./ne_cfg -m fe -t Classifier -f port.hdr_a_orig_lspid_mask -v 0x0b
./ne_cfg -m fe -t Classifier -f port.fwd_type_mask -v 0x0e
./ne_cfg -m fe -t Classifier -f port.hdr_a_flags_crcerr_mask -v 0x0f
./ne_cfg -m fe -t Classifier -f port.l3_csum_err_mask -v 0x10
./ne_cfg -m fe -t Classifier -f port.l4_csum_err_mask -v 0x11
./ne_cfg -m fe -t Classifier -f port.not_hdr_a_flags_stsvld_mask -v 0x12
./ne_cfg -m fe -t Classifier -f port.mcgid -v 0xfffe
./ne_cfg -m fe -t Classifier -f port.mcgid_mask -v 0xfdfc

./ne_cfg -m fe -t Classifier -f l2.tpid_enc_1 -v 0x13
./ne_cfg -m fe -t Classifier -f l2.vid_1 -v 0x1414
./ne_cfg -m fe -t Classifier -f l2._8021p_1 -v 0x15
./ne_cfg -m fe -t Classifier -f l2.tpid_enc_2 -v 0x16
./ne_cfg -m fe -t Classifier -f l2.vid_2 -v 0x1717
./ne_cfg -m fe -t Classifier -f l2._8021p_2 -v 0x18
./ne_cfg -m fe -t Classifier -f l2.tpid_enc_1_msb_mask -v 0x19
./ne_cfg -m fe -t Classifier -f l2.tpid_enc_1_lsb_mask -v 0x1a
./ne_cfg -m fe -t Classifier -f l2.vid_1_mask -v 0x1b
./ne_cfg -m fe -t Classifier -f l2._8021p_1_mask -v 0x1c
./ne_cfg -m fe -t Classifier -f l2.tpid_enc_2_msb_mask -v 0x1d
./ne_cfg -m fe -t Classifier -f l2.tpid_enc_2_lsb_mask -v 0x1e
./ne_cfg -m fe -t Classifier -f l2.vid_2_mask -v 0x1f
./ne_cfg -m fe -t Classifier -f l2._8021p_2_mask -v 0x20
./ne_cfg -m fe -t Classifier -f l2.da_an_mac_sel -v 0x21
./ne_cfg -m fe -t Classifier -f l2.da_an_mac_hit -v 0x22
./ne_cfg -m fe -t Classifier -f l2.sa_bng_mac_sel -v 0x23
./ne_cfg -m fe -t Classifier -f l2.sa_bng_mac_hit -v 0x24
./ne_cfg -m fe -t Classifier -f l2.da_an_mac_sel_mask -v 0x25
./ne_cfg -m fe -t Classifier -f l2.da_an_mac_hit_mask -v 0x26
./ne_cfg -m fe -t Classifier -f l2.sa_bng_mac_sel_mask -v 0x27
./ne_cfg -m fe -t Classifier -f l2.sa_bng_mac_hit_mask -v 0x28
./ne_cfg -m fe -t Classifier -f l2.ethertype_enc -v 0x29
./ne_cfg -m fe -t Classifier -f l2.ethertype_enc_mask -v 0x2a
./ne_cfg -m fe -t Classifier -f l2.da -v 01:23:45:67:89:ab
./ne_cfg -m fe -t Classifier -f l2.sa -v 01:23:45:67:89:ab
./ne_cfg -m fe -t Classifier -f l2.da_mask -v 0x2d
./ne_cfg -m fe -t Classifier -f l2.sa_mask -v 0x2e
./ne_cfg -m fe -t Classifier -f l2.mcast_da -v 0x2f
./ne_cfg -m fe -t Classifier -f l2.bcast_da -v 0x30
./ne_cfg -m fe -t Classifier -f l2.mcast_da_mask -v 0x31
./ne_cfg -m fe -t Classifier -f l2.bcast_da_mask -v 0x32
./ne_cfg -m fe -t Classifier -f l2.len_encoded -v 0x33
./ne_cfg -m fe -t Classifier -f l2.len_encoded_mask -v 0x34

./ne_cfg -m fe -t Classifier -f l3.dscp -v 0x35
./ne_cfg -m fe -t Classifier -f l3.ecn -v 0x36
./ne_cfg -m fe -t Classifier -f l3.ip_prot -v 0x37
./ne_cfg -m fe -t Classifier -f l3.ip_da -v 192.168.0.1
./ne_cfg -m fe -t Classifier -f l3.ip_sa -v 192.168.1.2
./ne_cfg -m fe -t Classifier -f l3.ip_valid -v 0x3a
./ne_cfg -m fe -t Classifier -f l3.ip_ver -v 0x3b
./ne_cfg -m fe -t Classifier -f l3.ip_frag -v 0x3c
./ne_cfg -m fe -t Classifier -f l3.dscp_mask -v 0x3d
./ne_cfg -m fe -t Classifier -f l3.ecn_mask -v 0x3e
./ne_cfg -m fe -t Classifier -f ip_prot_mask -v 0x3f
./ne_cfg -m fe -t Classifier -f l3.ip_sa_mask -v 0x40
./ne_cfg -m fe -t Classifier -f l3.ip_da_mask -v 0x41
./ne_cfg -m fe -t Classifier -f l3.ip_valid_mask -v 0x42
./ne_cfg -m fe -t Classifier -f l3.ip_ver_mask -v 0x43
./ne_cfg -m fe -t Classifier -f l3.ip_frag_mask -v 0x44
./ne_cfg -m fe -t Classifier -f l3.spi -v 0x45
./ne_cfg -m fe -t Classifier -f l3.spi_valid -v 0x46
./ne_cfg -m fe -t Classifier -f l3.spi_mask -v 0x47
./ne_cfg -m fe -t Classifier -f l3.spi_valid_mask -v 0x48

./ne_cfg -m fe -t Classifier -f l4.l4_sp -v 0x49
./ne_cfg -m fe -t Classifier -f l4.l4_dp -v 0x4a
./ne_cfg -m fe -t Classifier -f l4.l4_valid -v 0x4b
./ne_cfg -m fe -t Classifier -f l4.l4_port_mask -v 0x4c
./ne_cfg -m fe -t Classifier -f l4.l4_valid_mask -v 0x4d

./ne_cfg -m fe -t Classifier -e

./ne_cfg -m fe -t Classifier -s -c get 0 100 -b 0 
./ne_cfg -m fe -t Classifier -e
