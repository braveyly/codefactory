#include <stdio.h>
#include <unistd.h>
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
   int ret = write ( fd, "11100110",8 );
   if( -1 == ret )
   {
      printf( "Write file 1.txt failed!\n" );
   }
   else
   {
      printf( "Write file 1.txt %d bytes succeed!\n", ret );
   }
   while(1)
   {
      printf( "~~\n" );
      sleep(1);
   }
   return 0;
}
