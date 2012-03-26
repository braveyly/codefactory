#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MCAST_PORT 8888
#define MCAST_ADDR "224.0.0.88"
#define MCAST_INTERVAL 5
#define BUFF_SIZE 256
int main ( int argc, char*argv[] )
{
/*1st step:create one  udp socket*/
    int s;
    struct sockaddr_in local_addr;
    int err = -1;

    s = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( s == -1 )
    {
        printf ( "socket() error\n" );
        return -1;
    }

/*2nd step:bind this socket on the multiple cast port*/
    memset ( &local_addr, 0, sizeof ( local_addr ) );
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl ( INADDR_ANY );
    local_addr.sin_port = htons ( MCAST_PORT );

    err = bind ( s, ( struct sockaddr* ) & local_addr, sizeof ( local_addr ) ) ;
    if ( err < 0 )
    {
        perror ( "bind() error\n" );
        return -2;
    }

/*3rd step:set socket multicast loop parameter*/
    int loop = 1;
    err = setsockopt ( s, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof ( loop ) );
    if ( err < 0 )
    {
        perror ( "setsockopt():IP_MULTICAST_LOOP error\n" );
        return -3;
    }

/*4th step:add this socket into MCAST_ADDR multiple group*/
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr ( MCAST_ADDR );
    mreq.imr_interface.s_addr = htonl ( INADDR_ANY );

    err = setsockopt ( s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof ( mreq ) );
    if ( err < 0 )
    {
        perror ( "setsockopt():IP_ADD_MEMBERSHIP" );
        return -4;
    }

/*5th step: normal receive steps*/
    int times = 0;
    int addr_len = 0;
    char buff[BUFF_SIZE];
    int n = 0;

    for ( times = 0;times < 5;times++ )
    {
        addr_len = sizeof ( local_addr );
        memset ( buff, 0, BUFF_SIZE );

        n = recvfrom ( s, buff, BUFF_SIZE, 0, ( struct sockaddr* ) & local_addr, &addr_len );
        if ( n == -1 )
        {
            perror ( "recvfrom() error\n" );
        }

        printf ( "Recv %dst message from server:%s\n", times, buff );
        sleep ( MCAST_INTERVAL );
    }

/*6th step: leave the multicast group*/
    err = setsockopt ( s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof
                       ( mreq ) );

    close ( s );
    return 0;
}