#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define   MSG_FILE "msg_srv"
#define   BUFFER 255
#define   PERM S_IRUSR|S_IWUSR
/*file permission*/
struct msgtype {
    long mtype;
    char buffer[BUFFER+1];
}; 

int main(int argc, char **argv)
{
    printf( "IPC_NOWAIT %d \t  IPC_NOERROR %d\n", IPC_NOWAIT, MSG_NOERROR );/*IPC_NOWAIT MSG_NOERROR are big int*/
    struct msgtype msg;
    key_t key;
    int msgid;
   
    if(argc != 2)
    {
        fprintf(stderr,"Usage:%s string\n", argv[0]);
        exit(1);
    }
   
    if((key=ftok(MSG_FILE,'a'))==-1)
    {
        fprintf(stderr,"Creat Key Error:%s\n", strerror(errno));
        exit(1);
    }
   
    if((msgid=msgget(key, PERM))==-1)
    {
        fprintf(stderr,"Creat Message  Error:%s\n", strerror(errno));
        exit(1);
    }
   
    msg.mtype = 1;
    strncpy(msg.buffer, argv[1], BUFFER);
    msgsnd(msgid, &msg, sizeof(struct msgtype), 0); 
    memset(&msg, '\0', sizeof(struct msgtype));
    msgrcv(msgid, &msg, sizeof(struct msgtype), 2, 0);
    fprintf(stderr, "Client receive:%s\n", msg.buffer);
    exit(0);
}   
