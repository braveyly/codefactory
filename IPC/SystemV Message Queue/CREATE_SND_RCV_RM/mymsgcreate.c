#include	<sys/msg.h>
#include <fcntl.h> // O_CREATE
#include <sys/types.h> // pid_t
#include	<sys/stat.h> 


int
main(int argc, char **argv)
{
	int		c, oflag, mqid;

	oflag = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH | IPC_CREAT;
	if((mqid = msgget(ftok("test3", 0), oflag))<0)
            printf("create error!\n");
       printf("mqid===%d\n",mqid);
                  printf("ftokid==%d\n",ftok("test4", 0));
	exit(0);
}
