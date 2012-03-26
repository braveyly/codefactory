#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>

int gahGetPeerMacbyIp ( char *ipaddr, char* buf, char* localethname )
{
    int     sockfd;
    unsigned char *ptr;
    struct arpreq arpreq;
    struct sockaddr_in *sin;
    struct sockaddr_storage ss;
    char addr[INET_ADDRSTRLEN+1];

    memset ( addr, 0, INET_ADDRSTRLEN + 1 );
    memset ( &ss, 0, sizeof ( ss ) );

    sockfd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( sockfd == -1 )
    {
        printf ( "socket error" );
        return -1;
    }
    sin = ( struct sockaddr_in * ) & ss;
    sin->sin_family = AF_INET;
    if ( inet_pton ( AF_INET, ipaddr, & ( sin->sin_addr ) ) <= 0 )
    {
        printf (  "inet_pton error" );
        return -1;
    }
    sin = ( struct sockaddr_in * ) & arpreq.arp_pa;
    memcpy ( sin, &ss, sizeof ( struct sockaddr_in ) );
    strcpy ( arpreq.arp_dev, localethname );
    arpreq.arp_ha.sa_family = AF_UNSPEC;
    if ( ioctl ( sockfd, SIOCGARP, &arpreq ) < 0 )
    {
        printf ( "ioctl SIOCGARP: " );
        return -1;
    }
    ptr = ( unsigned char * ) arpreq.arp_ha.sa_data;
    sprintf ( buf, "%02x:%02x:%02x:%02x:%02x:%02x", *ptr, * ( ptr + 1 ), * ( ptr + 2 ), * ( ptr + 3 ), * ( ptr + 4 ), * ( ptr + 5 ) );
    return 0;
}

int main()
{
    char ipaddr[100];
    memset ( ipaddr, 0, sizeof ( ipaddr ) );
    strcpy ( ipaddr, "172.16.13.89" );
    char localethname[100];
    memset ( localethname, 0, sizeof ( localethname ) );
    strcpy ( localethname, "eth0" );
    char mac[100];
    memset( mac, 0, sizeof(mac));
    gahGetPeerMacbyIp(ipaddr, mac, localethname);
    printf("ip %s 's mac is %s",ipaddr, mac);
}
