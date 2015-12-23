/etc/init.d/networking restart 
ifconfig eth0 up
ifconfig eth0 172.16.20.1/24
route add deafult gw 172.16.20.254 
arp -d 172.16.20.254
