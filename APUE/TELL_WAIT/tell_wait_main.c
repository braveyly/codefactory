#include <unistd.h>
#include <signal.h>
#include "tell_wait.h"

int main()
{
  pid_t pid = 0;
  TELL_WAIT();
  if ( (pid = fork()) < 0 )
  {
    printf( "fork failed!\n" );
    return -1;
  }
  else if ( 0 == pid )
  {
    WAIT_PARENT();
    printf( "This is child process1!\n" );
    TELL_PARENT( getppid() );
    WAIT_PARENT();
    printf( "This is child process2!\n" );
    exit(0);
  }
  printf( "This is parent process1!\n" ); //make sure that the parent process does something before child process
  TELL_CHILD(pid);
  WAIT_CHILD();
  printf( "This is parent process2!\n" );
  TELL_CHILD(pid);
  return 0;
}