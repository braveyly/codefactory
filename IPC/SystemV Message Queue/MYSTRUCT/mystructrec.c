#include "mymesg.h"
#define	MAXMSG	(8192 + sizeof(long))

int
main(int argc, char **argv)
{
	int	mqid,n;
   struct mymesg	*buff;
	if((mqid = msgget(ftok("test3", 0), S_IRUSR))<0)
            printf("create error!\n");
	buff = malloc(MAXMSG);
   buff->mesg_type=100;
	n =msgrcv(mqid,&(buff->mesg_type), MAXMSG,buff->mesg_type, 0);
       buff->mesg_len=n;
	printf("read %d bytes, type = %ld,mtext=%s\n", buff->mesg_len, buff->mesg_type,buff->mesg_data);

	exit(0);
}
