#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

extern void showHostentInfo( struct hostent* shHp );

int main ( int argc, char** argv )
{
    const char *ipstr = "202.103.24.68";
    struct in_addr ip;
    struct hostent *hp;

    if ( argc == 2 )
        ipstr = argv[1];

    if ( !inet_aton ( ipstr, &ip ) )
        printf ( "can't parse IP address %s", ipstr );

    if ( ( hp = gethostbyaddr ( ( const char * ) & ip, sizeof(ip), AF_INET ) ) == NULL )
        printf ( "no name associated with %s\n", ipstr );

    showHostentInfo(hp);
    return 0;
}
