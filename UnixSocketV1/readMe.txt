<1>simpleUdp
#./splUdpSrv
#./splUdpCli 172.16.0.201 8889
<2>bcast Chapter 18:broadcast
limited broadcast address
#./bcastsrv
#./bcastCli 255.255.255.255 8889
subnet broadcast address(72.16.0.201 255.255.224.0)
#./bcastSrv
#./bcastCli 172.16.31.255 8889
<3>mcast Chapter 19:multicast
172.16.0.201
#./mcastsrv
#./mcastcli
172.16.0.202
#./mcastcli
<4>Name VS Address Chapter 9:basic name and address convert
#./gethostbyname
#./gethostbyaddr
#./uname
#./getservice
#./gethostname
<5>IP address convert Chapter 3:basic socket programm
#./addrconvert
<6>simpleTcp
#./splTcpSrv
#./splTcpCli
<7>select
#./selecttcpsrv
#./selecttcpcli

