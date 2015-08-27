#ifndef __TCM_MSG_H__
#define __TCM_MSG_H__

#include <tcm.h>
#include <conflib.h>

/* Message type definition */
typedef enum
{
   TCM_MSG_INIT          = 0x10000000,
   TCM_MSG_APP_LAUNCHED  = 0x10000001,
   TCM_MSG_SYSTEM_BOOT   = 0x10000002,
   TCM_MSG_DELAYED_MSG   = 0x10000003,
   TCM_MSG_REGISTER_EVENT_INTEREST = 0x10000004,
   TCM_MSG_UNREGISTER_EVENT_INTEREST = 0x10000005,
   TCM_MSG_BOUNCED       = 0x10000006,
   TCM_MSG_START_APP     = 0x10000007,
   TCM_MSG_STOP_APP      = 0x10000008,
   TCM_MSG_RESTART_APP   = 0x10000009,
   TCM_MSG_IS_APP_RUNNING= 0x1000000A,
} tcmMsgType;

typedef struct tcm_msg_header
{
   tcmMsgType type;
   tcmEntityId src;
   tcmEntityId dst;

   union {
      UINT16 all;
      struct {
         UINT16 event:1;    /* Event Message type */
         UINT16 request:1;  /* Request Message type */
         UINT16 response:1; /* Response Message type */
         UINT16 requeue:1;  /* Send back msg to sender */
         UINT16 bounceIfNotRunning:1; /* Do not launch the app to receive message if not running */
         UINT16 unused:11;  /* Future use */
      } bits;
   } flags;

   UINT16 seqNum; /* Message Sequence Number */

   struct tcm_msg_header *next;
   UINT32 wordData;
   UINT32 dataLength;
} tcmMsgHeader;

#define flags_event flags.bits.event
#define flags_request flags.bits.request
#define flags_response flags.bits.response
#define flags_requeue flags.bits.requeue
#define flags_bounceIfNotRunning flags.bits.bounceIfNotRunning

typedef struct
{
   tcmEntityId  eid;        /**< Entity id of the owner of this handle. */
   int          commFd;     /**< communications fd */
   char         standalone; /**< are we running without smd, for unittests */
   tcmMsgHeader *putBackQueue;  /**< Messages pushed back into the handle. */
} tcmMsgHandle;

#define EMPTY_MSG_HEADER {0, 0, 0, {0}, 0, 0, 0, 0}


tcmRet tcmMsg_init(tcmEntityId eid, void **msgHandle);

void tcmMsg_cleanup(void **msgHandle);


tcmRet tcmMsg_send(void *msgHandle, const tcmMsgHeader *buf);


tcmRet tcmMsg_sendBySocket(int fd, const tcmMsgHeader *buf);


tcmRet tcmMsg_sendReply(void *msgHandle, const tcmMsgHeader *msg, tcmRet retCode);


tcmRet tcmMsg_sendAndGetReply(void *msgHandle, const tcmMsgHeader *buf);


tcmRet tcmMsg_sendAndGetReplyWithTimeout(void *msgHandle, const tcmMsgHeader *buf, unsigned int timeoutMilliSeconds);


tcmRet tcmMsg_sendAndGetReplyBuf(void *msgHandle, const tcmMsgHeader *buf, tcmMsgHeader **replyBuf);


tcmRet tcmMsg_sendAndGetReplyBufWithTimeout(void *msgHandle, const tcmMsgHeader *buf, tcmMsgHeader **replyBuf, unsigned int timeoutMilliSeconds);


tcmRet tcmMsg_receive(void *msgHandle, tcmMsgHeader **buf);


tcmRet tcmMsg_receiveBySocket(int fd, tcmMsgHeader **buf, unsigned int *timeout);


tcmRet tcmMsg_receiveWithTimeout(void *msgHandle, tcmMsgHeader **buf, unsigned int timeoutMilliSeconds);


void tcmMsg_putBack(void *msgHandle, tcmMsgHeader **buf);


void tcmMsg_requeuePutBacks(void *msgHandle);


tcmMsgHeader *tcmMsg_duplicate(const tcmMsgHeader *buf);

#endif

