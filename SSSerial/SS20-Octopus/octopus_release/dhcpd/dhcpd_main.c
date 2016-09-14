#include <stdio.h>
#include <stdlib.h>
#include "tcm_msg.h"
#include "tcm_mem.h"
#include "tcm_eid.h"
#include "tcm_log.h"
#include "tcm_tms.h"
void *   msgHandle = NULL;
const tcmEntityId myId = ID_DHCPD;


tcmRet startRecvbySelect()
{
    tcmRet ret;
    tcmMsgHeader *msg = NULL;
    SINT32 commFd;
    SINT32 n, maxFd;
    fd_set readFdsMaster, readFds;

    struct timeval tv;

    /* set up all the fd stuff for select */
    FD_ZERO ( &readFdsMaster );

    tcmMsg_getEventHandle ( msgHandle, &commFd );
    FD_SET ( commFd, &readFdsMaster );

    maxFd = commFd;

    while ( 1 )
    {
        readFds = readFdsMaster;

        /* Set the default polling time is 1s for checking all the messages
        or event */
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        n = select ( maxFd + 1, &readFds, NULL, NULL, &tv );

        if ( n < 0 )
        {
            /* interrupted by signal or something, continue */
            continue;
        }

        if ( 0 == n )
        {
            /*receives data timeout*/
            continue;
        }

        if ( FD_ISSET ( commFd, &readFds ) )
        {
            //tcmLog_error( "There is data coming" );
            if ( ( ret = tcmMsg_receive ( msgHandle, &msg ) ) != TCMRET_SUCCESS )
            {
                if ( ret == TCMRET_DISCONNECTED )
                {
                    tcmLog_debug ( "detected exit of sched, zwctrl will "
                                   "also exit" );
                    return TCMRET_FAILURE;
                }
                else
                {
                    struct timespec sleep_ts = {0, 100*NSECS_IN_MSEC};

                    /* try to keep running after a 100ms pause */
                    tcmLog_error ( "error during tcmMsg_receive, ret=%d", ret );
                    nanosleep ( &sleep_ts, NULL );
                    continue;
                }
            }

            if( msg->type == TCM_MSG_REFRESH_LOG_LEVEL )
            {
                printf("Receive msg refresh log level!\n");
            }
            else
            {
                tcmLog_notice ( "Ignore this type %d message", msg->type );
                TCMMEM_FREE_BUF_AND_NULL_PTR ( msg );
            }
        }
    }
    return TCMRET_SUCCESS;
}

#if 0
UINT32 util_sendMsg(tcmEntityId src, tcmEntityId dst, tcmMsgType msgType, UINT32 wordData, void* msgData, UINT32 msgDataLen)
{
   UINT32 ret;
   tcmMsgHeader *msg;
   char *data;
   void *msgBuf;

   /* for msg with user data */
   if (msgData != NULL && msgDataLen != 0)
   {
      msgBuf = tcmMem_alloc(sizeof(tcmMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   } 
   else
   {
      msgBuf = tcmMem_alloc(sizeof(tcmMsgHeader), ALLOC_ZEROIZE);
   }
   
   msg = (tcmMsgHeader *)msgBuf;
   
   /* initialize some common fields */
   msg->src = src;
   msg->dst = dst;
   msg->flags_request = 1;
   msg->type = msgType;   
   msg->wordData = wordData;

   if (msgData != NULL && msgDataLen != 0)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)msgData, msgDataLen);
   }      
   
   tcmLog_debug("send msg id[0X%X] to smd, (wordData=%d) msg=%s from %d to %d", (UINT32) msgType, wordData, msgData, src, dst);

   ret = tcmMsg_sendAndGetReply(msgHandle, msg);

   TCMMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return ret;
}
#endif

int main()
{
    tcmRet ret;
    if ( ( ret = tcmMsg_init ( myId, &msgHandle ) ) != TCMRET_SUCCESS )
    {
        //tcmLog_cleanup();
        printf( "Fail to init the msg handle: %d", ret );
        return -1;
    }

       
   //ret = util_sendMsg(ID_LAN, ID_SCHED, TCM_MSG_START_APP, ID_DHCPD, NULL, 0);
   //if( TCMRET_SUCCESS != ret )
   //{
   //    printf("Failed to start app dhcpddd!");
   //}
   startRecvbySelect();
    return 0;
}
