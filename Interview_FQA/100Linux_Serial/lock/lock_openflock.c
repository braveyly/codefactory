#include <stdio.h>
#include <sys/file.h>
int main()
{
   int fd = open( "./1.txt", O_WRONLY );
   if( fd < 0 )
   {
       printf( "Open file 1.txt failed!\n" );
   }
   else
   {
       printf( "Open file 1.txt succeed!\n" );
   }
   int ret = flock( fd, LOCK_EX );
   if( -1 == ret )
   {
      printf( "Flock file 1.txt failed!\n" );
   }
   else
   {
      printf( "Flock file 1.txt succeed!\n" );
   }
   while(1)
   {
      printf( "~~\n" );
      sleep(1);
   }
   return 0;
}
