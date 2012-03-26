#include "mymesg.h"
int main(int argc, char **argv)
{
	struct mymesg	*ptr;
	int	mqid;
	if((mqid = msgget(ftok("test3", 0), S_IWUSR))<0)
            printf("create error!\n");
	ptr = calloc(2*sizeof(long)+7, sizeof(char));
	ptr->mesg_type = 100;
   ptr->mesg_len  = 7;
   strcpy(ptr->mesg_data,"abcdefg");
        msgsnd(mqid, &(ptr->mesg_type),ptr->mesg_len, 0);

	exit(0);
}
