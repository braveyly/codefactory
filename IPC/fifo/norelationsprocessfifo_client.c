#include <sys/types.h>
#include <sys/stat.h>  //mkfifo
#include <fcntl.h> //O_RDONLY
int main(int argc, char **argv)
{
	int	writefd;
   int w_length;
       
printf("befor open fifo\n");
sleep(5);
writefd=open("/tmp/fifo.1", O_WRONLY, 0);
printf("fifo have open and befor write\n");
 
sleep(5);
w_length=write(writefd,"abcdef",11);
printf("have write\n");

sleep(5);
close(writefd);
unlink("/tmp/fifo.1");
}
