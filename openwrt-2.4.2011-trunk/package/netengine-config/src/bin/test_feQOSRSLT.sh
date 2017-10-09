#!/bin/sh
./ne_cfg -m fe -t QOSRSLT -s -c add -b 0

./ne_cfg -m fe -t QOSRSLT -f wred_cos -v 0x01
./ne_cfg -m fe -t QOSRSLT -f voq_cos -v 0x02
./ne_cfg -m fe -t QOSRSLT -f pol_cos -v 0x03
./ne_cfg -m fe -t QOSRSLT -f premark -v 0x04
./ne_cfg -m fe -t QOSRSLT -f change_dscp_en -v 0x05
./ne_cfg -m fe -t QOSRSLT -f dscp -v 0x06
./ne_cfg -m fe -t QOSRSLT -f dscp_markdown_en -v 0x07
./ne_cfg -m fe -t QOSRSLT -f marked_down_dscp -v 0x08
./ne_cfg -m fe -t QOSRSLT -f ecn_en -v 0x09
./ne_cfg -m fe -t QOSRSLT -f top_802_1p -v 0x0a
./ne_cfg -m fe -t QOSRSLT -f marked_down_top_802_1p -v 0x0b
./ne_cfg -m fe -t QOSRSLT -f top_8021p_markdown_en -v 0x0c
./ne_cfg -m fe -t QOSRSLT -f top_dei -v 0x0d
./ne_cfg -m fe -t QOSRSLT -f marked_down_top_dei -v 0x0e
./ne_cfg -m fe -t QOSRSLT -f inner_802_1p -v 0x0f
./ne_cfg -m fe -t QOSRSLT -f marked_down_inner_802_1p -v 0x10
./ne_cfg -m fe -t QOSRSLT -f inner_8021p_markdown_en -v 0x11
./ne_cfg -m fe -t QOSRSLT -f inner_dei -v 0x12
./ne_cfg -m fe -t QOSRSLT -f marked_Down_inner_dei -v 0x13
./ne_cfg -m fe -t QOSRSLT -f change_8021p_1_en -v 0x14
./ne_cfg -m fe -t QOSRSLT -f change_dei_1_en -v 0x15
./ne_cfg -m fe -t QOSRSLT -f change_8021p_2_en -v 0x16
./ne_cfg -m fe -t QOSRSLT -f change_dei_2_en -v 0x17
./ne_cfg -m fe -t QOSRSLT -f parity -v 0x18

./ne_cfg -m fe -t QOSRSLT -e
