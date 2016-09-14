#include <stdio.h>
#include <stdlib.h>
#include "tcm_msg.h"
#include "tcm_mem.h"
#include "tcm_eid.h"
#include "tcm_log.h"

void *   msgHandle = NULL;
const tcmEntityId myId = ID_LAN;


UINT32 util_sendMsg(tcmEntityId src, tcmEntityId dst, tcmMsgType msgType, UINT32 wordData, void* msgData, UINT32 msgDataLen, int request)
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
   msg->flags_request = request;
   msg->type = msgType;   
   msg->wordData = wordData;

   if (msgData != NULL && msgDataLen != 0)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)msgData, msgDataLen);
   }      
   
   tcmLog_debug("send msg id[0X%X] to smd, (wordData=%d) msg=%s from %d to %d", (UINT32) msgType, wordData, msgData, src, dst);

   ret = tcmMsg_send(msgHandle, msg);

   TCMMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return ret;
}

int main()
{
    tcmRet ret;
    printf("Enter main\n");
    if ( ( ret = tcmMsg_init ( myId, &msgHandle ) ) != TCMRET_SUCCESS )
    {
        //tcmLog_cleanup();
        printf( "Fail to init the msg handle: %d", ret );
        return -1;
    }
    
     ret = util_sendMsg(ID_LAN, ID_SCHED, TCM_MSG_START_APP, ID_DHCPD, NULL, 0, 0);
   if( TCMRET_SUCCESS != ret )
   {
       printf("Failed to start app dhcpddd!");
   }
   printf("before sleep\n");    
   sleep(5);
   printf("before send msg:TCM_MSG_REFRESH_LOG_LEVEL\n");
   ret = util_sendMsg(ID_LAN, ID_DHCPD, TCM_MSG_REFRESH_LOG_LEVEL, ID_DHCPD, NULL, 0, 0);
   if( TCMRET_SUCCESS != ret )
   {
       printf("Failed to send TCM_MSG_REFRESH_LOG_LEVEL\n");
   }
   printf("after send msg:TCM_MSG_REFRESH_LOG_LEVEL\n");
    return 0;
}
