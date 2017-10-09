#!/bin/sh
echo "===Begin TS test==="
date
for i in `seq 1 200`; do
    ts_test
done
date
echo "===End TS test==="
