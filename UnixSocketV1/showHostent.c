#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/***********struct hostent definition**********
struct hostent{
	char* h_name; /host name/
	char** h_aliases; /host alias list/
	int h_addrtype; /AF_INET AF_INET6/
	int h_length; / 4 or 16 /
	char** h_addr_list; / IP address list/
	};
#define h_addr h_addr_list[0] /first address in list/
**************************************/

void showHostentInfo( struct hostent* shHp )
{
   char** pptr = NULL;
   char str[64];
   memset( str, 0, sizeof(str) );

   if( NULL == shHp )
   {
      printf( "hostent structure is NULL\n" );
      return;
   }
   
/*1. show official name*/
   printf ( "official host name: %s\n", shHp->h_name );

/*2. show alias name*/
   for ( pptr = shHp->h_aliases; *pptr != NULL; pptr++ )
        printf ( "\thost alias : %s\n", *pptr );

/*3. show address type and IP address*/
    switch ( shHp->h_addrtype )
    {
        case AF_INET:
            pptr = shHp->h_addr_list;
            for ( ; *pptr != NULL; pptr++ )
                printf ( "IP address: %s\n",
                         inet_ntop ( shHp->h_addrtype, *pptr, str, sizeof ( str ) ) );
            break;

        default:
            printf ( "unknown address type\n" );
            break;
    }
}