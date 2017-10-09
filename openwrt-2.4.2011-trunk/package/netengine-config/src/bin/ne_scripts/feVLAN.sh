#!/bin/sh
if [ $# -ne 2 ]; then
    echo "Syntax: $0 start end"
    echo "Example: $0 1 5"	
    exit
fi
ne_cfg -m fe -t VLAN -s -c get $1 $2 -b 0
ne_cfg -m fe -t VLAN -e
