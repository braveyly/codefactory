#include <sys/types.h>
#include <sys/stat.h>  //mkfifo
#include <errno.h>  // errno
#include <fcntl.h> //O_RDONLY
int main(int argc, char **argv)
{
int readfd;
int r_length;
char buffer[10];

if ((mkfifo("/tmp/fifo.1", (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))<0) && (errno == EEXIST))
		printf("can't create %s", "/tmp/fifo.1");
printf("befor open fifo\n");
readfd = open("/tmp/fifo.1", O_RDONLY, 0);
printf("have open and befor read\n");
r_length=read(readfd,buffer,10);
printf("have read and r_length=%d;strlen(buffer)=%d;buffer=%s;\n",r_length,strlen(buffer),buffer);
close(readfd);
}
