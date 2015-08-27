#include "pal_msg.h"

#define IPC_SHAREKEY 200899811111L

PAL_MSG_T pal_create_msg (PAL_INT32_T key )
{
    PAL_INT32_T mqid;

    char bigbuf[sizeof(struct msqid_ds)*2]; 
    struct msqid_ds *buff = (struct msqid_ds *) bigbuf; 

    mqid = msgget (IPC_SHAREKEY + key, IPC_CREAT | S_IRWXU);

    msgctl(mqid, IPC_STAT, buff);
    buff->msg_qbytes = MAX_MSG_LEN;
    msgctl(mqid, IPC_SET, buff);	

    return mqid;
}

PAL_INT32_T pal_send_msg ( PAL_MSG_T id, PAL_UINT32_T eventType, char *buf, PAL_SIZE_T bufLen, PAL_INT32_T flags )
{
    struct msgbuf
    {
        long mtype;
        char data[MAX_MSG_LEN];
    }msg;

    msg.mtype = eventType;

    if (bufLen  > MAX_MSG_LEN)
		bufLen = MAX_MSG_LEN;

    memcpy ( (void *)msg.data, (const void *)buf,  bufLen );

    return msgsnd ( id, &msg, bufLen, MSG_NOERROR | IPC_NOWAIT );
}

PAL_INT32_T pal_recv_msg ( PAL_MSG_T id, PAL_UINT32_T *eventType, char *buf, PAL_SIZE_T bufLen, PAL_INT32_T flags )
{
    PAL_SIZE_T nread;

    struct msgbuf
    {
        long mtype;
        char data[MAX_MSG_LEN];
    } msg;

    nread = msgrcv (id, &msg, sizeof (msg.data), 0L, MSG_NOERROR);

    if (nread > bufLen)
    {
        nread = bufLen;
    }

    if (nread >= 0)
    {
        memcpy (buf, msg.data, nread);
	 *eventType = msg.mtype;
    }

    return nread;

}

PAL_INT32_T pal_destroy_msg ( PAL_MSG_T msg )
{
    return msgctl ( msg, IPC_RMID, 0 );
}
