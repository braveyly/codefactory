#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/*******struct definition*********
struct servent{
	char* s_name; /official name/
	char** s_aliases; /alias list/
	int port; /port number, network-byte order/
	char* s_proto; /protocol to use/
	};
*****************************/

/*********************
Definition:struct servent *getservbyname(const char *name, const char *proto);
Description:
**********************/

/**********************
Definition:struct servent *getservbyport(int port, const char *proto);
Decription:
***********************/

void showServent( struct servent* svt )
{
   char** aliasList;
   if( NULL == svt )
   	return;
   
   printf( "service name is %s\n", svt->s_name );
   for( aliasList = svt->s_aliases; *aliasList != NULL; aliasList++ )
   	printf( "alias name is %s\n", *aliasList );
   printf( "service port is %d\n", ntohs(svt->s_port) );
   printf( "service protocol is %s\n", svt->s_proto );
   
}

int main()
{
    struct servent* svt;

    svt = getservbyname( "domain", "udp" );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyname error for domain and udp\n" );	

    svt = getservbyname( "ftp", "tcp" );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyname error for ftp and tcp\n" );

    svt = getservbyname( "ftp", NULL );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyname error for ftp and null\n" );

    svt = getservbyname( "ftp", "udp" );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyname error for ftp and udp\n" );


    svt = getservbyport( htons(53), "udp" );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyport error for 53 and udp\n" );

    svt = getservbyport( htons(21), "tcp" );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyport error for 21 and tcp\n" );

    svt = getservbyport( htons(21), NULL );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyport error for 21 and NULL\n" );

    svt = getservbyport( htons(21), "udp" );
    if( NULL != svt )
	showServent( svt );
    else
	printf( "getservbyport error for 21 and udp\n" );	
}