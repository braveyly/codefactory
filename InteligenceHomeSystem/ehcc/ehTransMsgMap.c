#include <tcm.h>
#include <tcm_mem.h>
#include "ehTransMsgMap.h"
#include "ehMsgTypeDef.h"


pthread_mutex_t gehomeTransMsgMapQueueMutex = PTHREAD_MUTEX_INITIALIZER;

void showEhTransMsg ( EhTransMsgMap* shTransMsg )
{
    if ( shTransMsg == NULL )
    {
        tcmLog_notice ( "This trans msg is NULL" );
        return;
    }

    tcmLog_debug ( "This trans msg source type is %d", shTransMsg->srcMsg );
    tcmLog_debug ( "This trans msg map type is %d", shTransMsg->mapMsg );
    tcmLog_debug ( "This trans msg socket fd  is %d", shTransMsg->connfd );
}

void showEhTransMsgQueue ( EhTransMsgMap* shTransMsgQueue )
{
    EhTransMsgMap* transMsgTmp = shTransMsgQueue;
    int num = 0;
    while ( transMsgTmp != NULL )
    {
        tcmLog_debug ( "This is the %dth trans msg", num + 1 );
        showEhTransMsg ( transMsgTmp );
        transMsgTmp = transMsgTmp->next;
    }
}

EhTransMsgMap* createEhTransMsgMap ( HOMEMSGTYPE crtSrcMsg, 
                                     HOMEMSGTYPE crtMapMsg, int crtConnFd, 
                                     SSL* crtSsl,
                                      transMsgMapActFunc crtActFun )
{
    if ( !checkHomeMsgType ( crtSrcMsg ) && !checkHomeMsgType ( crtMapMsg ) )
    {
        tcmLog_error ( "Msg type is error\n" );
        return NULL;
    }

    if ( crtActFun == NULL )
    {
        tcmLog_error ( "TransMsg act function cannot be NULL\n" );
        return NULL;
    }

#ifdef SSL_CONNECT
    if ( crtConnFd < 1 || NULL == crtSsl )
    {
        tcmLog_error ( "SSL Socket error" );
        return NULL;
    }
#else
    if ( crtConnFd < 1 )
    {
        tcmLog_error ( "TCP Socket error" );
        return NULL;
    }
#endif

    char* buf =  tcmMem_alloc ( sizeof ( EhTransMsgMap ), ALLOC_ZEROIZE );
    EhTransMsgMap *msgTmp = ( EhTransMsgMap * ) buf;

    msgTmp->srcMsg = crtSrcMsg;
    msgTmp->mapMsg = crtMapMsg;
    msgTmp->connfd = crtConnFd;
    msgTmp->ssl = crtSsl;
    msgTmp->actFun = crtActFun;
    return msgTmp;
}

int putEhTransMsgToQueue ( EhTransMsgMap** transMsgQueue, 
                           EhTransMsgMap* transMsg )
{
    if ( transMsg == NULL )
        return -1;

    if ( *transMsgQueue == NULL )
    {
        *transMsgQueue = transMsg;
    }
    else
    {
        EhTransMsgMap *msgQueueTmp = *transMsgQueue;

        while ( NULL != msgQueueTmp->next )
            msgQueueTmp = msgQueueTmp->next;

        msgQueueTmp->next = transMsg;
    }
    return 0;
}

tcmRet installTransMsgMap ( HOMEMSGTYPE istSrcMsg, 
                            HOMEMSGTYPE istMapMsg, 
                            int istConnFd, SSL* istSsl, 
                            transMsgMapActFunc istActFun )
{
    EhTransMsgMap* msgTmp;
    msgTmp = createEhTransMsgMap ( istSrcMsg, istMapMsg, 
                                   istConnFd, istSsl, 
                                   istActFun );

    if ( NULL == msgTmp )
    {
        tcmLog_error ( "create ehtransMsgMap error" );
        return TCMRET_FAILURE;
    }

    if ( -1 == putEhTransMsgToQueue ( &gehTransMsgMapQueue, msgTmp ) )
    {
        tcmLog_error ( "put ehtransMsgMap to queue error" );
        return TCMRET_FAILURE;
    }

    return TCMRET_SUCCESS;
}

EhTransMsgMap* getTransMsgbySrcType ( EhTransMsgMap** transMsgQueue,  
                                      HOMEMSGTYPE gtSrcMsg )
{
    if ( !checkHomeMsgType ( gtSrcMsg ) )
    {
        tcmLog_error ( "Msg type is error\n" );
        return NULL;
    }

    EhTransMsgMap* msgQueueQ;
    msgQueueQ = *transMsgQueue;

    while ( msgQueueQ != NULL )
    {
        if ( msgQueueQ->srcMsg == gtSrcMsg )
        {
            tcmLog_notice ( "Find the msg with the type %d in the queue\n",
                            msgQueueQ->srcMsg );
            return msgQueueQ;
            break;
        }
        msgQueueQ = msgQueueQ -> next;
    }

    return NULL;
}

int delEhTransMsgFromQueue ( EhTransMsgMap** transMsgQueue, 
                             EhTransMsgMap* transMsg )
{
    if ( transMsgQueue == NULL || transMsg == NULL )
    {
        tcmLog_error ( "transmsg cannot be NULL" );
        return TCMRET_FAILURE;
    }

    EhTransMsgMap *msgQueueQ, *msgQueueP;
    msgQueueQ = *transMsgQueue;
    msgQueueP = msgQueueQ;
    if ( msgQueueQ == transMsg )
    {
        tcmLog_notice ( "Find the transmsg at the first position in the queue\n" );
        *transMsgQueue = msgQueueQ ->next;
        if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
        {
            tcmLog_error ( "Delete transmsg error \n" );
            return TCMRET_FAILURE;
        }
        return TCMRET_SUCCESS;
    }


    while ( msgQueueQ != NULL )
    {
        if ( msgQueueQ  == transMsg )
        {
            msgQueueP-> next = msgQueueQ -> next;
            if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
            {
                tcmLog_error ( "Delete transmsg error\n" );
                return TCMRET_FAILURE;
            }
            tcmLog_notice ( "Find the transmsg at the non-first position\n" );
            return TCMRET_SUCCESS;
            break;
        }
        msgQueueP = msgQueueQ;
        msgQueueQ = msgQueueQ -> next;
    }

    return TCMRET_SUCCESS;

}

tcmRet doTransMsgAct ( HOMEMSGTYPE gtSrcMsg, void* valueLen, void* value )
{
    if ( !checkHomeMsgType ( gtSrcMsg ) )
    {
        tcmLog_error ( "Trans Source Msg type is error\n" );
        return TCMRET_FAILURE;
    }

    EhTransMsgMap* transMsgTmp = getTransMsgbySrcType ( &gehTransMsgMapQueue,
                                                        gtSrcMsg );

    if ( transMsgTmp == NULL )
    {
        tcmLog_error ( "There is no transmsg srctype %d in queue", gtSrcMsg  );
        return TCMRET_FAILURE;
    }

    int ret = transMsgTmp->actFun ( valueLen, value, transMsgTmp );
    tcmLog_debug ( "Before delete trans msg" );
    showEhTransMsgQueue ( gehTransMsgMapQueue );
    delEhTransMsgFromQueue ( &gehTransMsgMapQueue, transMsgTmp );
    tcmLog_debug ( "After delete trans msg" );
    showEhTransMsgQueue ( gehTransMsgMapQueue );
    if ( 0 == ret )
    {
        tcmLog_notice ( "transmsg srctype %d act succeed", gtSrcMsg );
        return TCMRET_SUCCESS;
    }
    else
    {
        tcmLog_error ( "transmsg srctype %d act failed ,reason is %d", ret );
        return TCMRET_FAILURE;
    }
}

int deleteEhTransMsgByFd ( EhTransMsgMap** msgQueue, int fd )
{
    tcmLog_debug ( "Start to delete trans msg queue which msg fd is %d\n", fd );
    if ( msgQueue == NULL )
    {
        tcmLog_debug ( "Trans msg queue is NULL\n" );
        return -1;
    }

    if ( fd <= 0 )
    {
        tcmLog_error ( "Trans msg fd is error\n" );
        return -1;
    }

    EhTransMsgMap *msgQueueQ, *msgQueueP;
    int sum = 0;

    msgQueueQ = *msgQueue;
    msgQueueP = msgQueueQ;


    if (  NULL != msgQueueQ )
    {
        while ( msgQueueQ->connfd == fd )
        {
            tcmLog_notice ( "Find the fd at the first position in the trans msg queue for the %dth time\n", ++sum );
            *msgQueue = msgQueueQ -> next;
            if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
            {
                tcmLog_error ( "Delete trans msg error which fd is %d\n", 
                               msgQueueQ->connfd );
                return -1;
            }
            msgQueueQ = *msgQueue;
            msgQueueP = msgQueueQ;
            ++sum;
            if ( NULL == msgQueueQ )
            {
                break;
            }
        }
    }

    while ( msgQueueQ != NULL )
    {
        if ( msgQueueQ->connfd == fd )
        {
            msgQueueP-> next = msgQueueQ -> next;
            if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
            {
                tcmLog_error ( "Delete msg error\n" );
                return -1;
            }
            tcmLog_notice ( "Find the fd msg at the non-first position for the %dth time\n", ++sum );
        }
        msgQueueP = msgQueueQ;
        msgQueueQ = msgQueueQ -> next;
    }

    return sum;
}