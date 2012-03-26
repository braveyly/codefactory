#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> //O_RDONLY
#include <errno.h>
#define MAX_LENGTH 10
int
main(int argc, char **argv)
{
int		readfd, writefd;
pid_t	childpid;
int w_length;
int r_length;
char buffer[MAX_LENGTH];
//mkfifo("/tmp/fifo.1", (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
if ((mkfifo("/tmp/fifo.1", (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))<0) && (errno == EEXIST))
		printf("can't create %s", "/tmp/fifo.1");
//if noexist create fifo.1
//if exist   return error=EEXIST

if((childpid=fork())==0)  //child process  server
   {
readfd = open("/tmp/fifo.1", O_RDONLY, 0);
r_length=read(readfd,buffer,MAX_LENGTH);
printf("r_length===%d\n",r_length);
printf("buffer===%s\n",buffer);
printf("buffer===%d\n",strlen(buffer));
//exit(0);
close(readfd);
 }
else
{
writefd=open("/tmp/fifo.1", O_WRONLY, 0);
w_length=write(writefd,"abcdef",11);
printf("w_length===%d\n",w_length);
waitpid(childpid, NULL, 0);
close(writefd);
unlink("/tmp/fifo.1");
}
}

