#include <unistd.h> 
#include "tell_wait.h"

static int pfd1[2], pfd2[2];

void TELL_WAIT(void)
{
  if(pipe(pfd1) < 0 || pipe(pfd2) < 0)
  {
    printf("Failed to pipe!\n");
  }
}

void TELL_PARENT(pid_t pid)
{
  if(1 != write(pfd2[1],"c",1))
  {
    printf("write pfd2 error!\n");
  }
}

void WAIT_PARENT(void)
{
  char c;
  if(1 != read(pfd1[0], &c,1))
  {
    printf("read pfd1 error!\n");
  }
  if(c != 'p')
  {
    printf("WAIT_PARENT: incorrect data!\n");
  }
}

void TELL_CHILD(pid_t pid)
{
  if(1 != write(pfd1[1],"p",1))
  {
    printf("write pfd1 error!\n");
  }
}

void WAIT_CHILD(void)
{
  char c;
  if(1 != read(pfd2[0], &c, 1))
  {
    printf("read pfd2 error!\n");
  }

  if('c' != c)
  {
    printf("WAIT_CHILD:incorrect data!\n");
  }
}