#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#define SERV_PORT 7838
#define SA struct sockaddr
#define LISTENQ 20
#define MAX_BUF_LEN 1000
int main ( int argc, char* argv[] )
{
    int     listenfd = 0, connfd = 0;
    socklen_t   sinsize = 0;
    struct sockaddr_in cliaddr, servaddr;
    char buf[MAX_BUF_LEN];
    int buflen = MAX_BUF_LEN;

    memset( buf, 0, sizeof(buf) );
    memset ( &servaddr, 0, sizeof ( servaddr ) );
    memset( &cliaddr, 0, sizeof( cliaddr ) );

    listenfd = socket ( AF_INET, SOCK_STREAM, 0 );

    if ( -1 == listenfd )
    {
        printf ( "Create socket error,reason is %s\n", strerror ( errno ) );
        return -1;
    }

    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl ( INADDR_ANY );
    servaddr.sin_port        = htons ( SERV_PORT );

    if ( -1 == bind ( listenfd, ( SA * ) ( &servaddr ), sizeof ( SA ) ) )
    {
        printf ( "Bind listen socket error, reason is %s\n", strerror ( errno ) );
        return -1;
    }

    if ( -1 == listen ( listenfd, LISTENQ ) )
    {
        printf ( "Listen connection error, reason is %s\n", strerror ( errno ) );
        return -1;
    }

    sinsize = sizeof ( struct sockaddr_in );
    if ( -1 == ( connfd = accept ( listenfd, ( SA * ) & cliaddr, &sinsize ) ) )
    {
        printf ( "Accept connection socket error, reason is %s\n", strerror ( errno ) );
        return -1;
    }

    while ( 1 )
    {
        int recvlen;
        int sendlen;
        if ( 1 > ( recvlen = recv ( connfd, buf, buflen, 0 ) ) )
        {
            printf ( "Connction socket recv data error ,reason is %s", strerror ( errno ) );
            continue;
        }
        else
        {
            printf ( "Server receive data:%s. Length is %d", buf, recvlen );
        }

        if ( -1 == ( sendlen = send ( connfd, buf, strlen ( buf ) + 1, 0 ) ) )
        {
            printf ( "Send data error, reason is %s\n", strerror ( errno ) );
            continue;
        }
        else
        {
            printf ( "Server send data successfully, Length is %d\n", sendlen );
        }
    }

    if ( -1 == close ( connfd ) )
    {
        printf ( "Close connection socket error, reason is %s\n", strerror ( errno ) );
        return -1;
    }

    if ( -1 == close ( listenfd ) )
    {
        printf ( "Close listen socket error, reason is %s\n", strerror ( errno ) );
        return -1;
    }

    return 0;
}
