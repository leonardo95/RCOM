ifconfig eth0 up
ifconfig eth0 172.16.21.1/24
route add -net 172.16.20.0/24 gw 172.16.21.253

route add default gw 172.16.21.254

echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects 
echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects
route del –net 172.16.20.0 gw 172.16.21.253 netmask 255.255.255.0 dev eth0
traceroute 172.16.20.1
route add –net 172.16.20.0 gw 172.16.21.253 netmask 255.255.255.0 dev eth0
traceroute 172.16.20.1
echo 1 > /proc/sys/net/ipv4/conf/eth0/accept_redirects 
echo 1 > /proc/sys/net/ipv4/conf/all/accept_redirects
