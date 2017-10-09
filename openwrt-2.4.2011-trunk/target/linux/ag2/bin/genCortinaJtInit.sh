#!/bin/bash
#
# Cortina Systems
#

echo '/*'
echo ' * Cortina Systems '
echo ' * '
echo ' * This is auto generated. Please do not make modification to this file'
echo ' * For any changes, please update it to ',$0 
echo ' */'
echo 'struct cs_kernel_hw_accel_jt hw_jt = {'
grep -R ' k_jt_' --include '*.h' *  | sed 's/(.*/ =  cs_hw_accel_nop,/' | sed 's/^.*k_jt_/\t ./' | sed 's/k_jt_//'
echo }\;
