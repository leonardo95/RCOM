/etc/init.d/networking restart 
ifconfig eth0 up
ifconfig eth0 172.16.20.1/24
route add deafult gw 172.16.20.254 
route -n
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts