#include "pal_msg.h"

PAL_MSG_T pal_create_msg()
{
    MSGQUEUEOPTIONS options;
    PAL_MSG_T h;
    
    options.dwMaxMessages = 0; /* no restriction */
    options.bReadAccess = TRUE;
    options.cbMaxMessage = MAX_PAL_RECV_MSG_LEN;
    options.dwFlags = MSGQUEUE_NOPRECOMMIT;
    
    return CreateMsgQueue(NULL, &options);
}

PAL_INT32_T pal_send_msg(PAL_MSG_T id, char *buf, PAL_INT32_T buf_len
                        , PAL_INT32_T flags )
{
    BOOL retval;
    
    retval = WriteMsgQueue(id, buf, buf_len, INFINITE, MSGQUEUE_MSGALERT);
    
    return (retval == TRUE) ? 0 : -1;
}

PAL_INT32_T pal_recv_msg(PAL_MSG_T id, char *buf, PAL_INT32_T buf_len
                        , PAL_INT32_T flags )                        
{
    DWORD nbytes, _flags;
    BOOL retval;
    
    retval = ReadMsgQueue(id, buf, buf_len, &nbytes, INFINITE, &_flags);
    
    return (retval == TRUE) ? 0 : -1;
}

PAL_INT32_T pal_destroy_msg (PAL_MSG_T id)
{
    return (CloseMsgQueue(id) == TRUE) ? 0 : -1;
}
