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
struct msgtype {
    long mtype;
    char buffer[BUFFER+1];
};

int main(int argc, char* argv[] )
{
    struct msgtype msg;
    key_t key;
    int msgid;
   if( argc != 2 )
   {
       printf( "./msgsrcv type\ntype is the message type that you want to receive\n" );
	   exit(0);
   }
    if((key=ftok(MSG_FILE,'a'))==-1)
    {
        fprintf(stderr,"Creat Key Error:%s\n", strerror(errno));
        exit(1);
    }

    if((msgid=msgget(key, PERM|IPC_CREAT|IPC_EXCL))==-1)
    {
        if( (msgid = msgget(key, PERM)) != -1 ) /*msg queue has been existed*/
        { 
            fprintf(stderr, "Creat Message Error:%s\n", strerror(errno));
        }
        else /*real error*/
        {
            fprintf(stderr, "Creat Message Error:%s\n", strerror(errno));
            exit(1);
        }
    }
    printf("msqid = %d\n", msgid);

        msgrcv(msgid, &msg, sizeof(struct msgtype), atoi(argv[1]), IPC_NOWAIT );/*0 is default flag, and IPC_NOWAIT MSG_NOERROR*/
        fprintf(stderr,"Server Receive:%s\n", msg.buffer);
        
    exit(0);
} 
/***************************Key*****************************
1.The same type msgs will be received by their sending order.
2.As you want, you can receive any type msg even though these msgs are sended later.
For example, four A type msgs(A1,A2,A3,A4) are sent first,then 2 B type msgs(B1,B2) are sent, 
but you CAN receive B1 first, then A1. and A1 is  received before A2 Affirmatively.
3.IPC_NOWAIT test
*/
