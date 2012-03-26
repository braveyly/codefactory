#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/utsname.h>
/*****************
Definition:int uname(struct utsname *buf);
Description:
*******************/
void showUtsname( struct utsname* utsnm )
{
   printf( "sysname is %s\n", utsnm->sysname );
   printf( "nodename is %s\n", utsnm->nodename );
   printf( "release is %s\n", utsnm->release );
   printf( "version is %s\n", utsnm->version );
   printf( "machine is %s\n", utsnm->machine );
}
int main()
{
   struct hostent *hp;
   struct utsname myname;

   if( 0 != uname(&myname) )
   {
       printf( "uname error\n" );
	   return;
   }

   showUtsname( &myname );

   if( ( hp = gethostbyname( myname.nodename ) ) == NULL)
   {
       printf( "gethostbyname error for %s\n", myname.nodename );
	 return;
   }

   showHostentInfo(hp);

}