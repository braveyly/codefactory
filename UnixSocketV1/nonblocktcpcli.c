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
#include <fcntl.h>

#define SERV_PORT 7838
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
    int val = 0;
    int ret ;

    memset( sendbuf, 0, sizeof(sendbuf) );
    memset( recvbuf, 0, sizeof(recvbuf) );
	
    strcpy ( sendbuf, "Hello World!" );
    clifd = socket ( AF_INET, SOCK_STREAM, 0 );

    if ( -1 == clifd )
    {
        printf ( "Create client socket error,reason is %s\n", strerror ( errno ) );
        return -1;
    }

  /***********set client socket to non-block**********/
    val = fcntl(clifd,F_GETFL,0);  
    fcntl(clifd,F_SETFL,val | O_NONBLOCK);
/**********************************************/

    bzero ( &serveraddr, sizeof ( serveraddr ) );
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_port        = htons ( SERV_PORT );
    serveraddr.sin_addr.s_addr = inet_addr ( "127.0.0.1" );

    do
    {
       ret  = connect ( clifd, ( SA * ) ( &serveraddr ), sizeof ( struct sockaddr ) );
	if( ret < 0 && errno != EINPROGRESS )
	{
	       printf( "connect error for no non-blocking reason %s\n", strerror( errno ) );
		exit(-1);
	}
	
	if( ret == 0 )
	{
	    printf( "connect succeed\n" );
	    break;
	}
    }while( 1 );


    sleep ( 5 );
	
SR:
    if ( -1 == ( sendlen = send ( clifd, sendbuf, strlen ( sendbuf ) + 1, 0 ) ) )
    {
       if (errno == EWOULDBLOCK)
      	{
      	   printf( "send error for non-blocking reason %s", strerror( errno ) );
      	}
	 else
	 {
      	   printf( "send error for no non-blocking reason %s", strerror( errno ) );			    
	 }
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
         if (errno == EWOULDBLOCK)
      	{
      	   printf( "receive error for non-blocking reason %s", strerror( errno ) );
      	}
	 else
	 {
      	   printf( "receive error for no non-blocking reason %s", strerror( errno ) );			    
	 }
        return -1;
    }
    else
    {
        printf ( "Client receive data successfully, data is %s Length is %d\n", recvbuf, recvlen );
    }
    sleep ( 30 );
    goto SR;

    while ( 1 )
    {
        sleep ( 5 );
    }
    return 0;
}
