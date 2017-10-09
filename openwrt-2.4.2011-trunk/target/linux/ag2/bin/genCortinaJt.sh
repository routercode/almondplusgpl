#!/bin/bash
#
# Cortina Systems
#

echo '/*'
echo ' * Cortina Systems '
echo ' * '
echo ' * This is auto generated. Please do not make modification to this file'
echo ' * For any changes, please update it to , ' $0
echo ' */'
echo
echo '#ifndef CS_KERNEL_HW_ACCEL_JT'
echo '#define CS_KERNEL_HW_ACCEL_JT 1'
echo
echo '#include <linux/skbuff.h>'
echo '#include <linux/mroute.h>'
echo '#include <linux/mroute6.h>'
echo '#ifdef CONFIG_NETFILTER'
echo '#include <linux/netfilter_ipv4.h>'
echo '#include <linux/netfilter/nf_conntrack_common.h>'
echo '#include <net/netfilter/nf_conntrack.h>'
echo '#endif'
echo
echo 'void cs_hw_accel_nop(void);'
echo
echo 'struct cs_kernel_hw_accel_jt {'
grep -R ' k_jt_' --include '*.h' *  | sed 's/(/) (/' | sed 's/^.*:/\t/' | sed 's/k_jt_/(*/'
echo }\;
echo '#endif'
