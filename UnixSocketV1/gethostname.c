#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/******************
Definition:int gethostname(char *name, size_t len);
Description:
********************/
#define MAX_HOST_NAME_LEN 100
int main()
{
   char hostname[MAX_HOST_NAME_LEN];
   int ret = 0;

   memset( hostname, 0, sizeof(hostname) );

   ret = gethostname( hostname, sizeof( hostname ) );

   if( 0 == ret )
   {
      printf( "hostname is %s\n", hostname );
   }
   else
   {
      printf( "gethostname error\n" );
   }
}