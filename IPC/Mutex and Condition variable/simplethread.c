#include <stdio.h>
#include <pthread.h>
void thread(void* arg)
{
  int i;
  printf("arg==%d",*((int *)arg));
  for(i=0;i<3;i++)
  printf("This is a pthread.\n");
  sleep(10);
}

int main(void)
{
  pthread_t id;
  int i,ret;
  int a=9;
  ret=pthread_create(&id,NULL,(void *) thread,&a);
  if(ret!=0){
          printf ("Create pthread error!\n");
          exit (1);
            }
  for(i=0;i<3;i++)
  printf("This is the main process.\n");
  pthread_join(id,NULL);
  
  printf("========");
  return (0);
}

