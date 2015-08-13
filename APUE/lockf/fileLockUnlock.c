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

   lock_result = lockf( fd, F_LOCK, 0 );
   if( 0 == lock_result )
   {
      printf("Succeed to lock file\n");
   } 
   else
   { 
      printf("Failed to lock file\n");
   }
   sleep(10);
   
   lock_result = lockf( fd, F_ULOCK, 0 );
   if( 0 == lock_result )
   {
      printf( "Succeed to unlock file\n" );
   }
   else
   {
      printf( "Failed to unlock file\n" );
   }
   sleep(100);
}
