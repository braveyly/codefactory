#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#define MAX_LENGTH 10
int main()
{
int mypipe[2];
pid_t childpid;
int w_length;
int r_length;
char buffer[MAX_LENGTH];
int status;
pipe(mypipe);

if((childpid=fork())==0)  //child process
   {
      close(mypipe[1]);
               r_length=read(mypipe[0],buffer,MAX_LENGTH);
                printf("r_length===%d\n",r_length);
                                          printf("buffer===%s\n",buffer);
                                   printf("buffer===%d\n",strlen(buffer));
   }
 else
  {
close(mypipe[0]);
w_length=write(mypipe[1],"abcdef",11);
printf("w_length===%d\n",w_length);
waitpid(childpid,NULL,0);
   }
   
}
