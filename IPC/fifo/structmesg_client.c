#include "structmesg.h"
int main(int argc, char **argv)
{
int writefd;
struct mymesg mesg;
writefd = open("/tmp/fifo.1", O_WRONLY, 0);
mesg.mesg_len=10;
mesg.mesg_type=1;

strcpy(mesg.mesg_data,"abcdefghij");

write(writefd,&mesg,MESGHDRSIZE+mesg.mesg_len);
close(writefd);
unlink("/tmp/fifo.1");
}
