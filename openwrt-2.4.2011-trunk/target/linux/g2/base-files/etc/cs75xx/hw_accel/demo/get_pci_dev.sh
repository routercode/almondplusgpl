#!/bin/sh
#

## return value
##  PCI#0  PCI#1
##  0 - Atheros 11AC 
##  1 - Atheros 11n AR9380, AR9580
##  0 - Atheros 11AC 2.0
##  3 - RT3593
 
i=0
pci0=-1
pci1=-1

for s in `/bin/busybox lspci | awk '{print $1 $4 }'` ; do

  #echo $s
  substr=`expr substr $s 1 2`
  k=`expr substr $s 8 9`
  #echo $substr
  #echo $k
  
  temp=-1  
 
    if [ '168c:0030' = $k ]; then
       #echo "$k Atheros 11n AR9380"
       temp=1
    else if [ '168c:abcd' = $k ]; then
       #echo "$k Atheros 11ac"
       temp=0
    else if [ '168c:003c' = $k ]; then
       #echo "$k Atheros 11ac 2.0"
       temp=0
    else if [ '168c:0033' = $k ]; then
       #echo "$k Atheros 11n AR9580"
       temp=1
    else if [ '1814:3593' = $k ]; then
       #echo "$k RT3593"
       temp=3

    fi

    fi 
    fi   
    fi 
    fi
  
  if [ $temp -ne -1 ]; then
    if [ '01' = $substr ]; then
      pci0=$(($temp))    
    else
      pci1=$(($temp))
    fi
  fi
  
done

echo $pci0 $pci1
