#!/bin/sh
for i in `ls $*`; do
  patch -p1 < $*/$i
done
