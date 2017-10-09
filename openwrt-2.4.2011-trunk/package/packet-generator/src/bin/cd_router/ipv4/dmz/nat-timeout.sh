#!/bin/sh
TIME_PATH=/proc/sys/net/ipv4/netfilter
echo 180 > $TIME_PATH/ip_conntrack_tcp_timeout_established
echo 120 > $TIME_PATH/ip_conntrack_tcp_timeout_fin_wait
echo 120 > $TIME_PATH/ip_conntrack_tcp_timeout_close
echo 120 > $TIME_PATH/ip_conntrack_tcp_timeout_syn_sent
echo 120 > $TIME_PATH/ip_conntrack_tcp_timeout_time_wait
echo 60  > $TIME_PATH/ip_conntrack_udp_timeout
echo 30  > $TIME_PATH/ip_conntrack_icmp_timeout
echo "######################"
echo "##run SW nat-timeout##"
echo "######################"
echo "tcp_timeout_established" 
cat $TIME_PATH/ip_conntrack_tcp_timeout_established       
cat $TIME_PATH/ip_conntrack_tcp_timeout_fin_wait
echo "tcp_timeout_close"          
cat $TIME_PATH/ip_conntrack_tcp_timeout_close  
echo "tcp_timeout_syn_sent"     
cat $TIME_PATH/ip_conntrack_tcp_timeout_syn_sent 
echo "tcp_timeout_time_wait"      
cat $TIME_PATH/ip_conntrack_tcp_timeout_time_wait
echo "udp_timeout"       
cat $TIME_PATH/ip_conntrack_udp_timeout 
echo "icmp_timeout"
cat $TIME_PATH/ip_conntrack_icmp_timeout
