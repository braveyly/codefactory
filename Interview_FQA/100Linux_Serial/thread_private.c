#include <stdio.h>
#include <string.h>
#include <pthread.h>

pthread_key_t key;

void print()
{
  printf("thread2 %d returns %d\n",pthread_self(),pthread_getspecific(key));
}

void * thread2(void *arg)
{
  int tsd = 5;
  printf("thread2 %d is running\n",pthread_self());
  pthread_setspecific(key,(void *)tsd);
  print();
}

void * thread1(void *arg)
{
  int tsd = 1;
  pthread_t thid2;
  
  printf("thread1 %d is running\n",pthread_self());
  pthread_setspecific(key,(void *)tsd);
  pthread_create(&thid2,NULL,thread2,NULL);
  sleep(2);
  printf("thread1 %d return %d\n",pthread_self(),pthread_getspecific(key));
}

int main(void)
{
  pthread_t thid1;
  printf("main thread begins running\n");
  pthread_key_create(&key,NULL);
  pthread_create(&thid1,NULL,thread1,NULL);
  printf("main thread %d return %d\n",pthread_self(),pthread_getspecific(key));  
sleep(5);
  
  pthread_key_delete(key);
  printf("main thread exit\n");
  return 0;
}
/*
main thread begins running
main thread -1661896960 return 0
thread1 -1670666480 is running
thread2 -1679059184 is running
thread2 -1679059184 returns 5
thread1 -1670666480 return 1
main thread exit
*/
/*
first thread 1 set key to be 1
second thread 2 set key to be 5
third thread 2 get key that is 5
fourth thread 1 get key that is 1
the second step doesnot affect the fourth step.
As a result,every thread has its own global variable which is used among all thread functions
*/ 