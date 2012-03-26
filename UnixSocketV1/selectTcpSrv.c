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
    int rv = 0;
    int     listenfd = 0, connfd = 0;
    socklen_t   sinsize = 0;
    struct sockaddr_in cliaddr, servaddr;
    char buf[MAX_BUF_LEN];
    int buflen = MAX_BUF_LEN;
    int maxfd = 0;
    int tmpmaxfd =0;
    int actualfd =0;
    struct timeval tm;
    fd_set         readFds;
    fd_set      tmpreadFds;
    int recvlen = 0;
    int sendlen = 0;

    memset( buf, 0, sizeof( buf ) );
	
    listenfd = socket ( AF_INET, SOCK_STREAM, 0 );

    if ( -1 == listenfd )
    {
        printf ( "Create socket error,reason is %s\n", strerror ( errno ) );
        return -1;
    }

    bzero ( &servaddr, sizeof ( servaddr ) );
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl ( INADDR_ANY );
    servaddr.sin_port        = htons (  SERV_PORT );

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
	
    FD_ZERO ( &readFds );
    FD_SET ( listenfd, &readFds );
    maxfd = listenfd;
    while ( 1 )
    {
        tm.tv_sec = 10;
        tm.tv_usec = 0;/*select will less tm*/
        printf ( "Start select,waiting for connect/data coming! tm.tv_sec %d maxfd %d fd_set %d\n", ( int ) tm.tv_sec, maxfd, readFds );

        tmpreadFds = readFds;/*It is important.must use tmp variable.because select may change readFds*/
        tmpmaxfd = maxfd; /*It is important.maxfd in this position must match with select.*/

        rv = select ( tmpmaxfd + 1, &tmpreadFds, NULL, NULL, &tm );
        if ( rv < 0 )
        {
            printf ( "Select error, the reason is %s\n", strerror ( errno ) );
            return -1;
        }
        else if ( rv == 0 )
        {
            printf ( "Select timeout\n" );
            continue;
        }

        for ( actualfd = 0 ; actualfd < tmpmaxfd + 1; actualfd++ )
        {
            if ( !FD_ISSET ( actualfd, &tmpreadFds ) ) /*fd-set match with select*/
            {
                continue;
            }

            printf ( "actualfd %d listenfd %d maxfd %d\n", actualfd, listenfd, maxfd );
            if ( actualfd == listenfd )
            {
                sinsize = sizeof ( struct sockaddr_in );
                if ( -1 == ( connfd = accept ( listenfd, ( SA * ) & cliaddr, &sinsize ) ) )
                {
                    printf ( "Accept connection socket error, reason is %s\n", strerror ( errno ) );
                    return -1;
                }
                else
                {
                    printf ( "Listen socket accept a connection socket!\n" );
                }
                FD_SET ( connfd, &readFds );
                maxfd = ( maxfd > connfd ? maxfd : connfd );/*Change maxfd value.This result in puzzle for FD_ISSET*/
            }

            if ( actualfd != listenfd )
            {
                printf ( "Connection socket receive data\n" );/*This receive and send can also be called after accept*/
                if ( -1 == ( recvlen = recv ( connfd, buf, buflen, 0 ) ) )
                {
                    printf ( "TConnction socket recv data error ,reason is %s", strerror ( errno ) );
                    continue;
                }
                else
                {
                    printf ( "TServer receive data:%s. Length is %d", buf, recvlen );
                }

                if ( -1 == ( sendlen = send ( connfd, buf, strlen ( buf ) + 1, 0 ) ) )
                {
                    printf ( "TSend data error, reason is %s\n", strerror ( errno ) );
                    continue;
                }
                else
                {
                    printf ( "TServer send data successfully, Length is %d\n", sendlen );
                }
            }
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
/**********************************************
two match
1.fd_set in select match fd_set in FD_ISSET
  It can be changed in select
2.maxfd  in select match maxfd in for
  It can be changed in listenfd
************************************************/
