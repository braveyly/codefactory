#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[])
{
   int fd;
   int lock_result;
   
   fd = open( argv[1],O_RDWR);
   if( fd < 0 )
   {
      printf( "Open file failed!\n" );
      return 1;
   }

   lock_result = lockf( fd, F_TEST, 0 );
   if( 0 == lock_result )
   {
      printf("No lock for file\n");
   } 
   else
   { 
      printf("Lock for file\n");
   }

}
