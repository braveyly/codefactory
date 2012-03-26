all:simpleUdp simpleTcp broadcast multicast addrconvert getnameoraddr uname getservice \
    gethostname select poll epoll noblock ioctl rawsocket packetsocket
simpleUdp:splUdpCli.c splUdpSrv.c
	gcc splUdpCli.c -o ./bin/simpleUdp/splUdpCli
	gcc splUdpSrv.c -o ./bin/simpleUdp/splUdpSrv

simpleTcp:splTcpSrv.c splTcpCli.c
	gcc splTcpSrv.c -o ./bin/simpleTcp/splTcpSrv
	gcc splTcpCli.c -o ./bin/simpleTcp/splTcpCli

broadcast:bcastCli.c splUdpSrv.c
	gcc bcastCli.c -o ./bin/bcast/bcastCli
	gcc splUdpSrv.c -o ./bin/bcast/bcastSrv

multicast: mcastsrv.c mcastcli.c
	gcc mcastsrv.c -o ./bin/mcast/mcastsrv
	gcc mcastcli.c -o ./bin/mcast/mcastcli

addrconvert:addrconvert.c
	gcc addrconvert.c -o ./bin/addrconvert/addrconvert

getnameoraddr: gethostbyaddr.c gethostbyname.c showHostent.c
	gcc gethostbyaddr.c showHostent.c -o ./bin/getnameoraddr/gethostbyaddr
	gcc gethostbyname.c showHostent.c -o ./bin/getnameoraddr/gethostbyname

uname:uname.c showHostent.c
	gcc uname.c showHostent.c -o ./bin/getnameoraddr/uname

getservice:getservice.c
	gcc getservice.c -o ./bin/getnameoraddr/getservice

gethostname:gethostname.c
	gcc gethostname.c -o ./bin/getnameoraddr/gethostname

select:selectTcpSrv.c selectTcpCli.c
	gcc selectTcpSrv.c -o ./bin/select/selecttcpsrv
	gcc selectTcpCli.c -o ./bin/select/selecttcpcli

poll:pollTcpSrv.c
	gcc pollTcpSrv.c -o ./bin/poll/polltcpsrv

epoll:epollTcpSrv.c
	gcc epollTcpSrv.c -o ./bin/epoll/epolltcpsrv

noblock:nonblocktcpsrv.c nonblocktcpcli.c
	gcc nonblocktcpsrv.c  -o ./bin/noblock/nonblocktcpsrv
	gcc nonblocktcpcli.c -o ./bin/noblock/nonblocktcpcli

ioctl:ioctl_IFCONF_IFFLAGS_IFADDR_IFHWADDR.c ioctl_SIOCGARP.c ioctl_SIOCGIFCONF.c  
    gcc ioctl_IFCONF_IFFLAGS_IFADDR_IFHWADDR.c -o ./bin/ioctl/ioctlmany    
    gcc ioctl_SIOCGARP.c -o ./bin/ioctl/ioctlarp
    gcc ioctl_SIOCGIFCONF.c -o ./bin/ioctl/ioctlifconf
    
rawsocket:myping.c
    gcc myping.c -o ./bin/ping/myping

packetsocket:arp.c
    gcc arp.c -o ./bin/arp/arp

clean:
	rm -rf *.o
	rm -rf ./bin/simpleUdp/*
	rm -rf ./bin/simpleTcp/*
	rm -rf ./bin/bcast/*
	rm -rf ./bin/mcast/*
	rm -rf ./bin/getnameoraddr/*
	rm -rf ./bin/addrconvert/*
	rm -rf ./bin/poll/*
	rm -rf ./bin/select/*
	rm -rf ./bin/epoll/*
	rm -rf ./bin/nonblock/*
	rm -rf ./bin/ping/*
    rm -rf ./bin/arp/*
