#!/bin/sh
./ne_cfg -m fe -t VoQ_Policer -s -c add -b 0

./ne_cfg -m fe -t VoQ_Policer -f voq_base -v 0x01
./ne_cfg -m fe -t VoQ_Policer -f pol_base -v 0x02
./ne_cfg -m fe -t VoQ_Policer -f cpu_pid -v 0x03
./ne_cfg -m fe -t VoQ_Policer -f ldpid -v 0x04
./ne_cfg -m fe -t VoQ_Policer -f pppoe_session_id -v 0x0506
./ne_cfg -m fe -t VoQ_Policer -f cos_nop -v 0x07
./ne_cfg -m fe -t VoQ_Policer -f parity -v 0x08

./ne_cfg -m fe -t VoQ_Policer -e
