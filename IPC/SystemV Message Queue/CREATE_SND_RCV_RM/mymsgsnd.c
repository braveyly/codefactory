#include	<sys/msg.h>
#include <fcntl.h> // O_CREATE
#include <sys/types.h> // pid_t
#include	<sys/stat.h> 

int
main(int argc, char **argv)
{
	struct msgbuf	*ptr;
	int	mqid;   //S_IWUSR
	if((mqid = msgget(ftok("test3", 0), 0))<0)
            printf("create error!\n");
	ptr = calloc(sizeof(long)+7, sizeof(char));
	ptr->mtype = 100;
   strcpy(ptr->mtext,"abcdefg");
	msgsnd(mqid, ptr,7, 0);

	exit(0);
}
