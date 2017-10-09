!/bin/sh
./ne_cfg -m fe -t Hash_Hash -s -c add -b 0

./ne_cfg -m fe -t Hash_Hash -f mac_da -v 01:02:03:04:05:06
./ne_cfg -m fe -t Hash_Hash -f mac_sa -v 07:08:09:0a:0b:0c
./ne_cfg -m fe -t Hash_Hash -f eth_type -v 0x0d0e
./ne_cfg -m fe -t Hash_Hash -f llc_type_enc -v 0x0f
./ne_cfg -m fe -t Hash_Hash -f ip_frag -v 0x10

./ne_cfg -m fe -t Hash_Hash -f tpid_enc_1 -v 0x11
./ne_cfg -m fe -t Hash_Hash -f _8021p_1 -v 0x12
./ne_cfg -m fe -t Hash_Hash -f dei_1 -v 0x13
./ne_cfg -m fe -t Hash_Hash -f vid_1 -v 0x1415
./ne_cfg -m fe -t Hash_Hash -f revd_135 -v 0x16

./ne_cfg -m fe -t Hash_Hash -f tpid_enc_2 -v 0x17
./ne_cfg -m fe -t Hash_Hash -f _8021p_2 -v 0x18
./ne_cfg -m fe -t Hash_Hash -f dei_2 -v 0x19
./ne_cfg -m fe -t Hash_Hash -f vid_2 -v 0x1a1b
./ne_cfg -m fe -t Hash_Hash -f da -v 192.168.1.2
./ne_cfg -m fe -t Hash_Hash -f sa -v 192.168.0.1
./ne_cfg -m fe -t Hash_Hash -f ip_prot -v 0x24
./ne_cfg -m fe -t Hash_Hash -f dscp -v 0x25
./ne_cfg -m fe -t Hash_Hash -f ecn -v 0x26
./ne_cfg -m fe -t Hash_Hash -f ip_frag -v 0x27
./ne_cfg -m fe -t Hash_Hash -f revd_428 -v 0x28
./ne_cfg -m fe -t Hash_Hash -f revd_430_429 -v 0x29

./ne_cfg -m fe -t Hash_Hash -f ipv6_flow_label -v 0x2a2b2c2d
./ne_cfg -m fe -t Hash_Hash -f ip_version -v 0x2e
./ne_cfg -m fe -t Hash_Hash -f ip_valid -v 0x2f
./ne_cfg -m fe -t Hash_Hash -f l4_dp -v 0x3031
./ne_cfg -m fe -t Hash_Hash -f l4_sp -v 0x3233
./ne_cfg -m fe -t Hash_Hash -f tcp_ctrl_flags -v 0x34
./ne_cfg -m fe -t Hash_Hash -f tcp_ecn_flags -v 0x35
./ne_cfg -m fe -t Hash_Hash -f l4_valid -v 0x36
./ne_cfg -m fe -t Hash_Hash -f sdbid -v 0x37
./ne_cfg -m fe -t Hash_Hash -f lspid -v 0x38
./ne_cfg -m fe -t Hash_Hash -f fwdtype -v 0x39
./ne_cfg -m fe -t Hash_Hash -f pppoe_session_id_valid -v 0x3a
./ne_cfg -m fe -t Hash_Hash -f pppoe_session_id -v 0x3b3c
./ne_cfg -m fe -t Hash_Hash -f mask_ptr_0_7 -v 0x3d
./ne_cfg -m fe -t Hash_Hash -f mcgid -v 0x3e3f
./ne_cfg -m fe -t Hash_Hash -f mc_idx -v 0x40
./ne_cfg -m fe -t Hash_Hash -f da_an_mac_sel -v 0x41
./ne_cfg -m fe -t Hash_Hash -f da_an_mac_hit -v 0x42
./ne_cfg -m fe -t Hash_Hash -f sa_bng_mac_sel -v 0x43
./ne_cfg -m fe -t Hash_Hash -f sa_bng_mac_hit -v 0x44
./ne_cfg -m fe -t Hash_Hash -f orig_lspid -v 0x45
./ne_cfg -m fe -t Hash_Hash -f recirc_idx -v 0x46474849
./ne_cfg -m fe -t Hash_Hash -f l7_field -v 0x4a4b
./ne_cfg -m fe -t Hash_Hash -f l7_field_valid -v 0x4c
./ne_cfg -m fe -t Hash_Hash -f hdr_a_flags_crcerr -v 0x4d
./ne_cfg -m fe -t Hash_Hash -f l3_csum_err -v 0x4e
./ne_cfg -m fe -t Hash_Hash -f l4_csum_err -v 0x4f
./ne_cfg -m fe -t Hash_Hash -f not_hdr_a_flags_stsvld -v 0x50
./ne_cfg -m fe -t Hash_Hash -f hash_fid -v 0x51
./ne_cfg -m fe -t Hash_Hash -f mc_da -v 0x52
./ne_cfg -m fe -t Hash_Hash -f bc_da -v 0x53
./ne_cfg -m fe -t Hash_Hash -f spi_vld -v 0x54
./ne_cfg -m fe -t Hash_Hash -f spi_idx -v 0x55565758
./ne_cfg -m fe -t Hash_Hash -f ipv6_ndp -v 0x59
./ne_cfg -m fe -t Hash_Hash -f ipv6_hbh -v 0x5a
./ne_cfg -m fe -t Hash_Hash -f ipv6_rh -v 0x5b
./ne_cfg -m fe -t Hash_Hash -f ipv6_doh -v 0x5c

./ne_cfg -m fe -t Hash_Hash -e
