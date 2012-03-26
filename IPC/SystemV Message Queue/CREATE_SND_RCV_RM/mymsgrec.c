#include	<sys/msg.h>
#include <fcntl.h> // O_CREATE
#include <sys/types.h> // pid_t
#include	<sys/stat.h> 

#define	MAXMSG	(8192 + sizeof(long))

int
main(int argc, char **argv)
{
	int	mqid,n;
   struct msgbuf	*buff;//S_IWUSR
	if((mqid = msgget(ftok("test3", 0), 0))<0)
            printf("create error!\n");
	buff = malloc(MAXMSG);

	n =msgrcv(mqid, buff, MAXMSG, 100, 0);
	printf("read %d bytes, type = %ld,mtext=%s\n", n, buff->mtype,buff->mtext);

	exit(0);
}
