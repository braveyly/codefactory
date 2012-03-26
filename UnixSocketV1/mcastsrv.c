#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MCAST_PORT 8888
#define MCAST_ADDR "224.0.0.88"
#define MCAST_DATA "BROADCAST TEST DATA"
#define MCAST_INTERVAL 5
int main ( int argc, char*argv )
{
/* 1st step: create one udp socket*/
    int s;
    struct sockaddr_in mcast_addr;
    s = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( s == -1 )
    {
        printf ( "socket() error\n" );
        return -1;
    }

/* 2nd step: send data to multicast IP address and port*/	
    memset ( &mcast_addr, 0, sizeof ( mcast_addr ) );
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr ( MCAST_ADDR );
    mcast_addr.sin_port = htons ( MCAST_PORT );

    while ( 1 )
    {
        int n = sendto (  s,
                          MCAST_DATA,
                          sizeof ( MCAST_DATA ),
                          0,
                          ( struct sockaddr* ) & mcast_addr,
                          sizeof ( mcast_addr )  ) ;
        if ( n < 0 )
        {
            perror ( "sendto() error\n" );
            return -2;
        }

        sleep ( MCAST_INTERVAL );
    }

    return 0;
}