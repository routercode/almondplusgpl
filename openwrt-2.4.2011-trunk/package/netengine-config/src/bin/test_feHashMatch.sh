#!/bin/sh
./ne_cfg -m fe -t Hash_Match -s -c add -b 0

./ne_cfg -m fe -t Hash_Match -f crc32_0 -v 0x01020304
./ne_cfg -m fe -t Hash_Match -f crc32_1 -v 0x05060708
./ne_cfg -m fe -t Hash_Match -f result_index0 -v 0x090a
./ne_cfg -m fe -t Hash_Match -f result_index1 -v 0x0b0c
./ne_cfg -m fe -t Hash_Match -f mask_ptr0 -v 0x0d
./ne_cfg -m fe -t Hash_Match -f mask_ptr1 -v 0x0e

./ne_cfg -m fe -t Hash_Match -e
