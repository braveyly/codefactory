
#include <stdlib.h>
#include <stdio.h>
#include <conflib.h>
#include <tcm_msg.h>

/* Extern API declaration */
tcmRet oalMsg_init(tcmEntityId eid, void **msgHandle);
void   oalMsg_cleanup(void **msgHandle);
tcmRet oalMsg_send(int fd, const tcmMsgHandle *buf);
tcmRet oalMsg_receive(int fd, tcmMsgHandle **buf, unsigned int *timeout);
tcmRet oalMsg_getEventHandle(const tcmMsgHandle *msgHandle, void *eventHandle);

void tcmMsg_putBack(void *msgHandle, tcmMsgHeader **buf)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;
   tcmMsgHeader *prevMsg;

   (*buf)->next = NULL;

   /* put the new message at the end of the putBackQueue */
   if (handle->putBackQueue == NULL)
   {
      handle->putBackQueue = (*buf);
   }
   else
   {
      prevMsg = handle->putBackQueue;
      while (prevMsg->next != NULL)
      {
         prevMsg = prevMsg->next;
      }

      prevMsg->next = (*buf);
   }

   /* we've taken ownership of this msg, so null out caller's pointer */
   *buf = NULL;

   return;
}


void tcmMsg_requeuePutBacks(void *msgHandle)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;
   tcmMsgHeader *msg;

   while ((msg = handle->putBackQueue) != NULL)
   {
      handle->putBackQueue = msg->next;
      msg->next = NULL;

      msg->flags_requeue = 1;

      oalMsg_send(handle->commFd, msg);
   }

   return;
}

tcmRet tcmMsg_init(tcmEntityId eid, void **msgHandle)
{
   return oalMsg_init(eid, msgHandle);
}

void tcmMsg_cleanup(void **msgHandle)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) *msgHandle;
   tcmMsgHeader *msg;

   /* free any queued up messages */
   while ((msg = handle->putBackQueue) != NULL)
   {
      handle->putBackQueue = msg->next;
      if(msg != NULL)
      {
         free(msg);
         msg = NULL;
      }
   }

   oalMsg_cleanup(msgHandle);
}

tcmRet tcmMsg_send(void *msgHandle, const tcmMsgHeader *buf)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;

   return oalMsg_send(handle->commFd, buf);
}

tcmRet tcmMsg_sendBySocket(int fd, const tcmMsgHeader *buf)
{
   return oalMsg_send(fd, buf);
}

tcmRet tcmMsg_sendReply(void *msgHandle, const tcmMsgHeader *msg, tcmRet retCode)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;
   tcmMsgHeader replyMsg = EMPTY_MSG_HEADER;

   replyMsg.dst = msg->src;
   replyMsg.src = msg->dst;
   replyMsg.type = msg->type;

   replyMsg.flags_request = 0;
   replyMsg.flags_response = 1;
   /* do we want to copy any other flags? */

   replyMsg.wordData = retCode;

   return oalMsg_send(handle->commFd, &replyMsg);
}


static tcmRet sendAndGetReply(void *msgHandle, const tcmMsgHeader *buf, unsigned int *timeout)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;
   tcmMsgType sentType;
   tcmMsgHeader *replyMsg=NULL;
   char doReceive = true;
   tcmRet ret;

   /* remember what msg type we sent out. */
   sentType = buf->type;

   ret = oalMsg_send(handle->commFd, buf);
   if (ret != TCMRET_SUCCESS)
   {
      return ret;
   }

   while (doReceive)
   {
      ret = oalMsg_receive(handle->commFd, &replyMsg, timeout);
      if (ret != TCMRET_SUCCESS)
      {
         if ((timeout == NULL) ||
             ((timeout != NULL) && (ret != TCMRET_TIMED_OUT)))
         {
            printf("error during get of reply, ret=%d", ret);
         }
         free(replyMsg);
         doReceive = false;
      }
      else
      {
         if (replyMsg->type == sentType)
         {
            ret = replyMsg->wordData;
            doReceive = false;
            if(replyMsg != NULL)
            {
               free(replyMsg);
               replyMsg = NULL;
            }
         }
         else
         {
            /* we got a mesage, but it was not the reply we were expecting.
             * Could be an event msg.  Push it back on the put-back queue and
             * keep trying to get the message we really want.
             */
            tcmMsg_putBack(msgHandle, &replyMsg);
            replyMsg = NULL;

            /*
             * I don't know how much time was used to get this response message
             * we weren't expecting, but approximate some elapsed time by
             * reducing the timeout by 1/3.  This ensures that we will
             * eventually timeout if we don't get the response msg we are expecting.
             */
            if (timeout != NULL)
            {
               (*timeout) = (*timeout) / 3;
            }
         }
      }
   }

   tcmMsg_requeuePutBacks(msgHandle);

   return ret;
}

static tcmRet sendAndGetReplyBuf(void *msgHandle, const tcmMsgHeader *buf, tcmMsgHeader **replyBuf, unsigned int *timeout)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;
   tcmMsgType sentType;
   tcmMsgHeader *replyMsg=NULL;
   char doReceive=true;
   tcmRet ret;

   /* remember what msg type we sent out. */
   sentType = buf->type;

   ret = oalMsg_send(handle->commFd, buf);
   if (ret != TCMRET_SUCCESS)
   {
      return ret;
   }

   while (doReceive)
   {
      ret = oalMsg_receive(handle->commFd, &replyMsg, timeout);
      if (ret != TCMRET_SUCCESS)
      {
         if ((timeout == NULL) ||
             ((timeout != NULL) && (ret != TCMRET_TIMED_OUT)))
         {
            printf("error during get of reply, ret=%d", ret);
         }
         free(replyMsg);
         doReceive = false;
      }
      else
      {
         if (replyMsg->type == sentType)
         {
            memcpy((*replyBuf), replyMsg, (sizeof(tcmMsgHeader) + replyMsg->dataLength));
            doReceive = false;
            if(replyBuf)
            {
               free(replyBuf);
               replyBuf = NULL;
            }
         }
         else
         {
            /* we got a mesage, but it was not the reply we were expecting.
             * Could be an event msg.  Push it back on the put-back queue and
             * keep trying to get the message we really want.
             */
            tcmMsg_putBack(msgHandle, &replyMsg);
            replyMsg = NULL;

            /*
             * I don't know how much time was used to get this response message
             * we weren't expecting, but approximate some elapsed time by
             * reducing the timeout by 1/3.  This ensures that we will
             * eventually timeout if we don't get the response msg we are expecting.
             */
            if (timeout != NULL)
            {
               (*timeout) = (*timeout) / 3;
            }
         }
      }
   }

   tcmMsg_requeuePutBacks(msgHandle);

   return ret;
}


tcmRet tcmMsg_sendAndGetReply(void *msgHandle, const tcmMsgHeader *buf)
{
   return (sendAndGetReply(msgHandle, buf, NULL));
}


tcmRet tcmMsg_sendAndGetReplyWithTimeout(void *msgHandle,
                                         const tcmMsgHeader *buf,
                                         unsigned int timeoutMilliSeconds)
{
   unsigned int timeout = timeoutMilliSeconds;

   return (sendAndGetReply(msgHandle, buf, &timeout));
}

tcmRet tcmMsg_sendAndGetReplyBuf(void *msgHandle, const tcmMsgHeader *buf, tcmMsgHeader **replyBuf)
{
   return (sendAndGetReplyBuf(msgHandle, buf, replyBuf, NULL));
}

tcmRet tcmMsg_sendAndGetReplyBufWithTimeout(void *msgHandle, const tcmMsgHeader *buf, tcmMsgHeader **replyBuf, unsigned int timeoutMilliSeconds)
{
   unsigned int timeout = timeoutMilliSeconds;

   return (sendAndGetReplyBuf(msgHandle, buf, replyBuf, &timeout));
}

tcmRet tcmMsg_receive(void *msgHandle, tcmMsgHeader **buf)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;

   return oalMsg_receive(handle->commFd, buf, NULL);
}


tcmRet tcmMsg_receiveBySocket(int fd, tcmMsgHeader **buf, unsigned int *timeout)
{
   return oalMsg_receive(fd, buf, timeout);
}


tcmRet tcmMsg_receiveWithTimeout(void *msgHandle, tcmMsgHeader **buf, unsigned int timeoutMilliSeconds)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) msgHandle;
   unsigned int timeout = timeoutMilliSeconds;

   return oalMsg_receive(handle->commFd, buf, &timeout);
}


tcmRet tcmMsg_getEventHandle(const void *msgHandle, void *eventHandle)
{
   return (oalMsg_getEventHandle((tcmMsgHandle *) msgHandle, eventHandle));
}


tcmEntityId tcmMsg_getHandleEid(const void *msgHandle)
{
   const tcmMsgHandle *handle = (const tcmMsgHandle *) msgHandle;

   return (handle == NULL ? 0 : handle->eid);
}


tcmMsgHeader *tcmMsg_duplicate(const tcmMsgHeader *msg)
{
   unsigned int totalLen;
   void *newMsg;

   totalLen = sizeof(tcmMsgHeader) + msg->dataLength;
   newMsg = malloc(totalLen);
   if (newMsg != NULL)
   {
      memcpy(newMsg, msg, totalLen);
   }

   return newMsg;
}







