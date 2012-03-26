#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

extern void showHostentInfo( struct hostent* shHp );

int main ( int argc, char **argv )
{
    char   **pptr, *ptr = "www.baidu.com";
    char   str[64];
    struct hostent *hptr;

    if( 2 == argc )
       ptr = argv[1];
	
    if ( ( hptr = gethostbyname ( ptr ) ) == NULL )
    {
        printf ( "gethostbyname error for host: %s: %d",
                 ptr,  h_errno );
    }
	
    showHostentInfo( hptr );
   
    exit ( 0 );
}

