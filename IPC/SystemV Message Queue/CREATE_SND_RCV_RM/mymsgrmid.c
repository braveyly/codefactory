#include	<sys/msg.h>
#include <fcntl.h> // O_CREATE
#include <sys/types.h> // pid_t
#include	<sys/stat.h> 

int
main(int argc, char **argv)
{
	int		mqid;

	mqid = msgget(ftok("test3", 0), 0);
	msgctl(mqid, IPC_RMID, 0);

	exit(0);
}
