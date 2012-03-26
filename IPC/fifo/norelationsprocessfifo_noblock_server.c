#include <sys/types.h>
#include <sys/stat.h>  //mkfifo
#include <errno.h>  // errno
#include <fcntl.h> //O_RDONLY
int main(int argc, char **argv)
{
int readfd;
int r_length;
char buffer[10];

if ((mkfifo("/tmp/fifo.1", (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))<0) && (errno != EEXIST))
printf("can't create %s", "/tmp/fifo.1");

printf("befor open fifo\n");
readfd = open("/tmp/fifo.1", O_RDONLY|O_NONBLOCK, 0);
printf("have open and befor read\n");

int p=0;
while(1)
{
sleep(20);
r_length=read(readfd,buffer,10);
printf("%d\n",p++);
   if(r_length>0)
   {
     printf("have read and r_length=%d;strlen(buffer)=%d;buffer=%s;\n",r_length,strlen(buffer),buffer);
     sleep(3);
   }
}
close(readfd);
}
