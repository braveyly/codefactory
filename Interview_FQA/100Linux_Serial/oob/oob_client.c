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

#define SERV_PORT 7841
#define SA struct sockaddr
#define MAX_BUF_LEN 1000
int main ( int argc, char* argv[] )
{
    int  clifd = 0;
    struct sockaddr_in serveraddr;
    char sendbuf[MAX_BUF_LEN];
    int buflen = MAX_BUF_LEN;
    char recvbuf[MAX_BUF_LEN];
    int recvlen = 0;
    int sendlen = 0;

    memset( sendbuf, 0, sizeof(sendbuf) );
    memset( recvbuf, 0, sizeof(recvbuf) );
    strcpy ( sendbuf, "Hello World!" );
	
    clifd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( -1 == clifd )
    {
        printf ( "Create client socket error,reason is %s\n", strerror ( errno ) );
        return -1;
    }

    memset ( &serveraddr, 0, sizeof ( serveraddr ) );
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_port        = htons ( SERV_PORT  );
    serveraddr.sin_addr.s_addr = inet_addr ( "127.0.0.1" );
	
    if ( -1 == connect ( clifd, ( SA * ) ( &serveraddr ), sizeof ( struct sockaddr ) ) )
    {
        printf ( "Client connect Server error, reason is %s\n", strerror ( errno ) );
        return -1;
    }

    sleep ( 2 );
    if ( -1 == ( sendlen = send ( clifd, sendbuf, strlen ( sendbuf ) + 1, MSG_OOB ) ) )  // client modification 1: the last charactor of sendbuf is the urgent data
    {
        printf ( "Client send data error, reason is %s\n", strerror ( errno ) );
        return -1;
    }
    else
    {
        printf ( "Client send successfully, data is %s Length is %d\n", sendbuf, sendlen );
    }

    if ( -1 == ( recvlen = recv ( clifd, recvbuf, buflen, 0 ) ) )
    {
        printf ( "Client recv data error, reason is %s\n", strerror ( errno ) );
        return -1;
    }
    else
    {
        printf ( "Client receive data successfully, data is %s Length is %d\n", recvbuf, recvlen );
    }

    while ( 1 )
    {
        sleep ( 5 );
    }
    return 0;
}