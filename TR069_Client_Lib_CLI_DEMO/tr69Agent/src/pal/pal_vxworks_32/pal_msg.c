#include "pal_msg.h"


PAL_MSG_T pal_create_msg (PAL_INT32_T key )
{
    return msgQCreate ( MAX_MSG_NUM, MAX_MSG_LEN, MSG_Q_FIFO );
}

PAL_INT32_T pal_send_msg ( PAL_MSG_T id, PAL_UINT32_T eventType, char *buf, PAL_INT32_T bufLen, PAL_INT32_T flags )
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

    return msgQSend ( id, &msg, bufLen, NO_WAIT, MSG_PRI_NORMAL );


}

PAL_INT32_T pal_recv_msg ( PAL_MSG_T id, PAL_UINT32_T *eventType, PAL_INT8_T *buf, PAL_INT32_T bufLen, PAL_INT32_T flags )
{


    PAL_SIZE_T nread;

    struct msgbuf
    {
        long mtype;
        char data[MAX_MSG_LEN];
    } msg;

    nread = msgQReceive (id, &msg, sizeof (msg.data),WAIT_FOREVER);

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

   // return msgQReceive ( id, ( PAL_INT8_T * ) buf, bufLen, WAIT_FOREVER );
}

PAL_INT32_T pal_destroy_msg ( PAL_MSG_T msg )
{
    return msgQDelete ( msg );
}


