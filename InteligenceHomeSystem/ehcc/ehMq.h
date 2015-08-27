#ifndef _EH_MQ_H_
#define _EH_MQ_H_
#include <tcm_log.h>
#include "ehMsgTypeDef.h"
#include "ehDeviceDef.h"


extern pthread_mutex_t g_homeMsgQueueMutex;
extern pthread_mutex_t g_homeIdpTokenMutex;
homeMsgCtrlBlock* gehomeMsgQueue;
homeMsgCtrlBlock* gehomeWriteMsgQueue;
extern uint32_t g_seqNum;
#define UPDATE_SEQ_NUMBER(num) {if(num == UINT_MAX) num=0;else ++num;}
int checkHomeMsgType ( HOMEMSGTYPE msgType );
homeMsgCtrlBlock* createHomeMsgBlock ( uint16_t msgProVer, char* msgReserve, 
                        uint16_t msgEventType, uint16_t msgValueLen, 
                        void* msgValue, int msgConnfd, SSL* msgSsl );
homeMsgCtrlBlock* cloneEhMsg( homeMsgCtrlBlock* ehMsg );
tcmRet freeEhMsg ( homeMsgCtrlBlock* msg );
tcmRet freeEhMsgQueue ( homeMsgCtrlBlock* msgQueue );
tcmRet putEhMsgToQueue ( homeMsgCtrlBlock** msgQueue, homeMsgCtrlBlock* msg );
int deleteEhMsgFromQueueByType ( homeMsgCtrlBlock** msgQueue, HOMEMSGTYPE msgType );
int deleteEhMsgFromQueueByFd ( homeMsgCtrlBlock** msgQueue, int fd );
tcmRet deleteEhMsgFromQueueBySeqID ( homeMsgCtrlBlock** msgQueue, uint32_t msgSeqID );
tcmRet updateEhMsg ( homeMsgCtrlBlock* msgQueue, char* msgReserve, uint16_t msgValueLen,
                     void* msgValue );
homeMsgCtrlBlock* searchEhMsgbyType ( homeMsgCtrlBlock* msgQueue, HOMEMSGTYPE msgType );
tcmRet showEhMsgValue ( HOMEMSGTYPE msgType );
tcmRet showEhMsg ( homeMsgCtrlBlock* msg );
tcmRet showEhMsgQueue ( homeMsgCtrlBlock* msgQueue );
tcmRet checkHomeMsg( homeMessage* ehMsg );

typedef enum
{
    OFF_SET_PROTOCOL_VERSION = 0x00,
    OFF_SET_PAYLOAD_LEN   = 0x02,
    OFF_SET_RESERVE    = 0x04,
    OFF_SET_EVENT_TYPE   = 0x14,
    OFF_SET_VALUE_LEN   = 0x16,
    OFF_SET_VALUE    = 0x18,
}PROTOCOL_DOMAIN_OFFSET;

tcmRet checkHomeMsgBuf ( char* buf, int bufLen );
tcmRet createHomeMsgBuf ( OUT char** buf, OUT int* bufLen, IN uint16_t msgProVer, IN char* msgReserve, IN uint16_t msgEventType, IN uint16_t msgValueLen, IN uint8_t* msgValue );
tcmRet analyzeHomeMsgBuf ( IN char* buf, IN int bufLen, OUT uint16_t* msgProVer, OUT uint16_t payloadLen, OUT char** msgReserve, OUT uint16_t* msgEventType, OUT uint16_t* msgValueLen , OUT uint8_t** msgValue );
tcmRet analyzeHomeMsgBufToMsg ( IN char* buf, IN int bufLen, OUT homeMsgCtrlBlock** msg );
tcmRet analyzeHomeMsgBufToHomeMsg ( IN char* buf, IN int bufLen, OUT homeMessage** msg );
tcmRet convertHomeMsgToCtrlBlock ( IN void* buf, IN int fd, IN SSL* ssl, OUT homeMsgCtrlBlock** msg );
tcmRet showHomeMsg ( homeMessage* msg );
char* getMsgStr( int msgID );
#endif
