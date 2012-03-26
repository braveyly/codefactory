#include <errno.h>
#include "structmesg.h"
int main(int argc, char **argv)
{
int readfd;
size_t	len;
	ssize_t	n;
struct mymesg mesg;
if ((mkfifo("/tmp/fifo.1", (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))<0) && (errno == EEXIST))
		printf("can't create %s", "/tmp/fifo.1");

readfd = open("/tmp/fifo.1", O_RDONLY, 0);

if ( (n = read(readfd, &mesg, MESGHDRSIZE)) == 0)
{
printf("end of file!\n");
		return(0);		/* end of file */
}
	else if (n != MESGHDRSIZE)
{
		printf("message header: expected %d, got %d", MESGHDRSIZE, n);
return(0);
}

	if ( (len = mesg.mesg_len) > 0)
		if ( (n = read(readfd, mesg.mesg_data, len)) != len)
{
			printf("message data: expected %d, got %d", len, n);
	return(0);
}
printf("mesg_len=%d\tmesg_type=%d\tmesg_data=%s\n",mesg.mesg_len,mesg.mesg_type,mesg.mesg_data);
return 0;
}
