#ifndef _EH_TRANS_MSG_MAP_
#define _EH_TRANS_MSG_MAP_

#include "ehMsgTypeDef.h"
#include "ehDeviceDef.h"

/* valueLen value ehTransMsgMap */
typedef int ( *transMsgMapActFunc ) ( void*, void*, void* );  

typedef struct EHTRANSMSGMAP
{
   HOMEMSGTYPE srcMsg;
   HOMEMSGTYPE mapMsg;
   int          connfd;
   SSL*         ssl;
   transMsgMapActFunc  actFun;
   struct EHTRANSMSGMAP* next;
}EhTransMsgMap;

EhTransMsgMap *gehTransMsgMapQueue;
extern pthread_mutex_t gehomeTransMsgMapQueueMutex;
tcmRet installTransMsgMap( HOMEMSGTYPE istSrcMsg, HOMEMSGTYPE istMapMsg, 
                           int istConnFd, SSL* istSsl, 
                           transMsgMapActFunc istActFun );
tcmRet doTransMsgAct( HOMEMSGTYPE gtSrcMsg, void* valueLen, void* value );
void showEhTransMsg ( EhTransMsgMap* shTransMsg );
void showEhTransMsgQueue ( EhTransMsgMap* shTransMsgQueue );
int deleteEhTransMsgByFd( EhTransMsgMap** transMsgQueue, int delConnFd );

EhTransMsgMap* createEhTransMsgMap( HOMEMSGTYPE istSrcMsg, 
                                    HOMEMSGTYPE istMapMsg, int istConnFd, 
                                    SSL* istSsl, transMsgMapActFunc istActFun );
int putEhTransMsgToQueue ( EhTransMsgMap** transMsgQueue, EhTransMsgMap* transMsg );

EhTransMsgMap* getTransMsgbySrcType ( EhTransMsgMap** transMsgQueue,  
                                      HOMEMSGTYPE gtSrcMsg );
int delEhTransMsgFromQueue( EhTransMsgMap** transMsgQueue, 
                            EhTransMsgMap* transMsg );
#endif
