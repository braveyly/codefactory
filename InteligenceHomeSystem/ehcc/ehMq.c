#include <tcm_log.h>
#include <tcm_mem.h>
#include "ehMq.h"

uint32_t g_seqNum = 0;
pthread_mutex_t g_homeMsgQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_homeIdpTokenMutex = PTHREAD_MUTEX_INITIALIZER;

homeMsgIntToStr gMsgIDStrMap[MAX_MSG_NUM]={
{CMD_IDP_TO_MFCB_REGISTER,                      "CMD_IDP_TO_MFCB_REGISTER"},
{CMD_IDP_TO_MFCB_OPEN_DOOR,                     "CMD_IDP_TO_MFCB_OPEN_DOOR"},
{CMD_IDP_TO_MFCB_QUERY_DOOR_STATUS,             "CMD_IDP_TO_MFCB_QUERY_DOOR_STATUS"},
{CMD_IDP_TO_MFCB_QUERY_CARD_INFO,               "CMD_IDP_TO_MFCB_QUERY_CARD_INFO"},
{CMD_IDP_TO_MFCB_ADD_CRD_OPERATION,             "CMD_IDP_TO_MFCB_ADD_CRD_OPERATION"},
{CMD_IDP_TO_MFCB_EDIT_SINGLE_CRD,               "CMD_IDP_TO_MFCB_EDIT_SINGLE_CRD"},
{CMD_IDP_TO_MFCB_UPDATE_ALL_CRD,                "CMD_IDP_TO_MFCB_UPDATE_ALL_CRD"},
{CMD_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS,     "CMD_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS"},
{CMD_IDP_TO_MFCB_SET_SECURITY_MODE,             "CMD_IDP_TO_MFCB_SET_SECURITY_MODE"},
{CMD_IDP_TO_MFCB_QUERY_DETECT_POINT,            "CMD_IDP_TO_MFCB_QUERY_DETECT_POINT"},
{CMD_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT,       "CMD_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT"},
{CMD_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE,      "CMD_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE"},
{CMD_IDP_TO_MFCB_GET_CURRENT_SECURITY_MODE,     "CMD_IDP_TO_MFCB_GET_CURRENT_SECURITY_MODE"},
{CMD_IDP_TO_MFCB_CLEAR_ALARM,                   "CMD_IDP_TO_MFCB_CLEAR_ALARM"},
{CMD_IDP_TO_MFCB_QUERY_ALARM_HISTORY,           "CMD_IDP_TO_MFCB_QUERY_ALARM_HISTORY"},
{CMD_IDP_TO_MFCB_KEEP_ALIVE,                    "CMD_IDP_TO_MFCB_KEEP_ALIVE"},
{CMD_IDP_TO_MFCB_TOKEN_OPERATION,               "CMD_IDP_TO_MFCB_TOKEN_OPERATION"},
{CMD_IDP_TO_MFCB_QUERY_CAM,                     "CMD_IDP_TO_MFCB_QUERY_CAM"},
{CMD_IDP_TO_MFCB_ADD_CAM,                       "CMD_IDP_TO_MFCB_ADD_CAM"},
{CMD_IDP_TO_MFCB_EDIT_SINGLE_CAM,               "CMD_IDP_TO_MFCB_EDIT_SINGLE_CAM"},
{CMD_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE,   "CMD_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE"},
{CMD_IDP_TO_MFCB_SYNCHRONIZE_DEVICE,            "CMD_IDP_TO_MFCB_SYNCHRONIZE_DEVICE"},
{CMD_IDP_TO_MFCB_QUERY_DO,                      "CMD_IDP_TO_MFCB_QUERY_DO"},
{CMD_IDP_TO_MFCB_RESET_DEFAULT,                 "CMD_IDP_TO_MFCB_RESET_DEFAULT"},
{CMD_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD,      "CMD_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD"},
{CMD_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD,       "CMD_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD"},
{CMD_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO,      "CMD_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO"},
{CMD_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO,       "CMD_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO"},
{CMD_IDP_TO_MFCB_CLEAR_ALARM_SOUND,             "CMD_IDP_TO_MFCB_CLEAR_ALARM_SOUND"},
{CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO,            "CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO"},
{CMD_ODP_TO_MFCB_REGISTER,                      "CMD_ODP_TO_MFCB_REGISTER"},
{CMD_ODP_TO_MFCB_KEEP_ALIVE,                    "CMD_ODP_TO_MFCB_KEEP_ALIVE"},
{CMD_ODP_TO_MFCB_TOKEN_OPERATION,               "CMD_ODP_TO_MFCB_TOKEN_OPERATION"},
{CMD_ODP_TO_MFCB_REPORT_CARD,                   "CMD_ODP_TO_MFCB_REPORT_CARD"},
{CMD_MFCB_TO_ODP_OPEN_DOOR,                     "CMD_MFCB_TO_ODP_OPEN_DOOR"},
{INVALID_MSG_TYPE_ID,                           "INVALID_MSG_TYPE_ID"},
};

int checkHomeMsgType ( HOMEMSGTYPE msgType )
{
    return 1;
}

homeMsgCtrlBlock* createHomeMsgBlock ( uint16_t msgProVer, char* msgReserve,
                        uint16_t msgEventType, uint16_t msgValueLen,
                        void* msgValue, int msgConnfd, SSL* msgSsl )
{
    if ( !checkHomeMsgType ( msgEventType ) )
    {
        tcmLog_error ( "Msg type is error\n" );
        return NULL;
    }

    char* buf =  tcmMem_alloc ( sizeof ( homeMsgCtrlBlock ) + msgValueLen,
                                ALLOC_ZEROIZE );
    homeMsgCtrlBlock *msgTmp = ( homeMsgCtrlBlock * ) buf;
    UPDATE_SEQ_NUMBER ( g_seqNum );
    msgTmp->sequenceID = g_seqNum;
    gettimeofday ( & ( msgTmp->tmPoint ), NULL );
    msgTmp->msg.protocolVersion = msgProVer;
    msgTmp->msg.payloadLen = 4 + msgValueLen;
    if ( NULL != msgReserve )
    {
        strcpy ( msgTmp->msg.reserve, msgReserve );
    }
    
    msgTmp->msg.eventType = msgEventType;
    if ( NULL != msgValue )
    {
        memcpy ( (void*)(msgTmp + 1), msgValue, msgValueLen );
        msgTmp->msg.valueLen = msgValueLen;
    }
    else
    {
        msgTmp->msg.valueLen = 0;
    }
    
    msgTmp->connfd = msgConnfd;
    msgTmp->ssl = msgSsl;
    return msgTmp;

}

homeMsgCtrlBlock* cloneEhMsg ( homeMsgCtrlBlock* ehMsg )
{
    if ( ehMsg == NULL )
        return NULL;

    if ( ehMsg->msg.protocolVersion != HOME_MSG_PROTOCOL_VERSION )
    {
        tcmLog_error ( "Msg protocol version is error\n" );
        return NULL;
    }

    if ( !checkHomeMsgType ( ehMsg->msg.eventType ) )
    {
        tcmLog_error ( "Msg type is error\n" );
        return NULL;
    }

    char* buf =  tcmMem_alloc ( sizeof ( homeMsgCtrlBlock ) + ehMsg->msg.valueLen,
                                ALLOC_ZEROIZE );
    homeMsgCtrlBlock *msgTmp = ( homeMsgCtrlBlock * ) buf;
    msgTmp->sequenceID = ehMsg->sequenceID;
    msgTmp->msg.protocolVersion = ehMsg->msg.protocolVersion;
    msgTmp->msg.payloadLen = 4 + ehMsg->msg.valueLen;
    if ( NULL != ehMsg->msg.reserve )
    {
        strcpy ( msgTmp->msg.reserve, ehMsg->msg.reserve );
    }
    msgTmp->msg.eventType = ehMsg->msg.eventType;
    msgTmp->msg.valueLen = ehMsg->msg.valueLen;
    if ( 0 != ehMsg->msg.valueLen )
    {
        memcpy ( msgTmp + 1, ( uint8_t * ) ( ehMsg + 1 ), ehMsg->msg.valueLen );
    }
    msgTmp->connfd = ehMsg->connfd;
    msgTmp->ssl = ehMsg->ssl;
    return msgTmp;
}

tcmRet freeEhMsg ( homeMsgCtrlBlock* msg )
{
    if ( NULL == msg )
    {
        tcmLog_notice ( "Msg is NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    tcmMem_free ( msg );
    return TCMRET_SUCCESS;

}
tcmRet freeEhMsgQueue ( homeMsgCtrlBlock* msgQueue )
{
    if ( NULL == msgQueue )
    {
        tcmLog_debug ( "Msg queue is NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    while ( NULL != msgQueue )
    {
        if ( TCMRET_SUCCESS != freeEhMsg ( msgQueue ) )
        {
            return TCMRET_FAILURE;
        }
        msgQueue = msgQueue->next;
    }

    return TCMRET_SUCCESS;
}

tcmRet putEhMsgToQueue ( homeMsgCtrlBlock** msgQueue, homeMsgCtrlBlock* msg )
{
    if ( msg == NULL )
        return TCMRET_INVALID_PARAMETER;

    if ( *msgQueue == NULL )
    {
        *msgQueue = msg;
    }
    else
    {
        homeMsgCtrlBlock *msgQueueTmp = *msgQueue;

        while ( NULL != msgQueueTmp->next )
            msgQueueTmp = msgQueueTmp->next;

        msgQueueTmp->next = msg;
    }
    return TCMRET_SUCCESS;
}

int deleteEhMsgFromQueueByType ( homeMsgCtrlBlock** msgQueue, HOMEMSGTYPE msgType )
{
    tcmLog_debug ( "Start to delete msg which msg type is %d\n", msgType );
    if ( msgQueue == NULL )
    {
        tcmLog_debug ( "Msg queue is NULL\n" );
        return -1;
    }

    if ( !checkHomeMsgType ( msgType ) )
    {
        tcmLog_error ( "Msg type is error\n" );
        return -1;
    }


    homeMsgCtrlBlock *msgQueueQ, *msgQueueP;
    int sum = 0;

    msgQueueQ = *msgQueue;
    msgQueueP = msgQueueQ;

    if ( NULL != msgQueueQ )
    {
        while ( msgQueueQ->msg.eventType == msgType )
        {
            tcmLog_notice ( "Find the type at the first position in the queue "
                            "for the %dth time\n", ++sum );
            *msgQueue = msgQueueQ -> next;
            if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
            {
                tcmLog_error ( "Delete msg error which type is %d\n",
                               msgQueueQ->msg.eventType );
                return -1;
            }
            msgQueueQ = *msgQueue;
            msgQueueP = msgQueueQ;
            if ( NULL == msgQueueQ )
            {
                break;
            }
            ++sum;
        }
    }

    while ( msgQueueQ != NULL )
    {
        if ( msgQueueQ->msg.eventType == msgType )
        {
            msgQueueP-> next = msgQueueQ -> next;
            if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
            {
                tcmLog_error ( "Delete msg error\n" );
                return -1;
            }
            tcmLog_notice ( "Find the type msg at the non-first position for "
                            "the %dth time\n", ++sum );
        }
        msgQueueP = msgQueueQ;
        msgQueueQ = msgQueueQ -> next;
    }

    return sum;
}



int deleteEhMsgFromQueueByFd ( homeMsgCtrlBlock** msgQueue, int fd )
{
    tcmLog_debug ( "Start to delete msg which msg fd is %d\n", fd );
    if ( msgQueue == NULL )
    {
        tcmLog_debug ( "Msg queue is NULL\n" );
        return -1;
    }

    if ( fd <= 0 )
    {
        tcmLog_error ( "Msg fd is error\n" );
        return -1;
    }

    homeMsgCtrlBlock *msgQueueQ, *msgQueueP;
    int sum = 0;

    msgQueueQ = *msgQueue;
    msgQueueP = msgQueueQ;


    if (  NULL != msgQueueQ )
    {
        while ( msgQueueQ->connfd == fd )
        {
            tcmLog_notice ( "Find the fd at the first position in the queue "
                            "for the %dth time\n", ++sum );
            *msgQueue = msgQueueQ -> next;
            if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
            {
                tcmLog_error ( "Delete msg error which fd is %d\n",
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
            tcmLog_notice ( "Find the fd msg at the non-first position "
                            "for the %dth time\n", ++sum );
        }
        msgQueueP = msgQueueQ;
        msgQueueQ = msgQueueQ -> next;
    }

    return sum;
}

tcmRet deleteEhMsgFromQueueBySeqID ( homeMsgCtrlBlock** msgQueue, uint32_t msgSeqID )
{
    if ( msgQueue == NULL )
    {
        tcmLog_debug ( "Msg queue is NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    if ( msgSeqID > UINT_MAX )
    {
        tcmLog_error ( "Msg sequence ID invalid" );
        return TCMRET_INVALID_PARAMETER;
    }

    homeMsgCtrlBlock *msgQueueQ, *msgQueueP;
    msgQueueQ = *msgQueue;
    msgQueueP = msgQueueQ;
    if ( msgQueueQ->sequenceID == msgSeqID )
    {
        *msgQueue = msgQueueQ ->next;
        if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
        {
            tcmLog_error ( "Delete msg error which sequence ID is %d",
                           msgQueueQ->sequenceID );
            return TCMRET_FAILURE;
        }
        return TCMRET_SUCCESS;
    }


    while ( msgQueueQ != NULL )
    {
        if ( msgQueueQ->sequenceID == msgSeqID )
        {
            msgQueueP-> next = msgQueueQ -> next;
            if ( TCMRET_SUCCESS != freeDevice ( msgQueueQ ) )
            {
                tcmLog_error ( "Delete msg error" );
                return TCMRET_FAILURE;
            }
            tcmLog_notice ( "Find the sequence ID msg at the non-first position" );
            return TCMRET_SUCCESS;
            break;
        }
        msgQueueP = msgQueueQ;
        msgQueueQ = msgQueueQ -> next;
    }

    tcmLog_notice ( "Donot find the sequence ID %d message", msgSeqID );
    return TCMRET_SUCCESS;
}

/*upMsg must be double pointer*/
tcmRet updateEhMsg ( homeMsgCtrlBlock* upMsg, char* upMsgReserve,
                     uint16_t upMsgValueLen,
                     void* upMsgValue )
{
    if ( NULL == upMsg )
    {
        tcmLog_notice ( "Msg queue can not be NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    if ( NOT_CHANGE_RESERVE != upMsgReserve )
    {
        tcmLog_debug ( "Update message reserve" );
        memset ( upMsg->msg.reserve, 0, OFF_SET_EVENT_TYPE - OFF_SET_RESERVE );
        memcpy ( upMsg->msg.reserve, upMsgReserve, 16 );
    }

    if ( NOT_CHANGE_VALUE != upMsgValue )
    {
        tcmLog_debug ( "Update message value" );
        memset ( upMsg + 1, 0, upMsg->msg.valueLen );
        memcpy ( upMsg + 1, upMsgValue, upMsgValueLen );
    }

    if ( NOT_CHANGE_VALUELEN != upMsgValueLen )
    {
        tcmLog_debug ( "Update message value length" );
        upMsg->msg.valueLen = upMsgValueLen;
    }

    return TCMRET_SUCCESS;
}

homeMsgCtrlBlock* searchEhMsgbyType ( homeMsgCtrlBlock* msgQueue, HOMEMSGTYPE msgType )
{
    if ( msgQueue == NULL )
    {
        tcmLog_debug ( "Msg queue is NULL\n" );
        return NULL;
    }

    if ( !checkHomeMsgType ( msgType ) )
    {
        tcmLog_error ( "Msg type is error\n" );
        return NULL;
    }


    homeMsgCtrlBlock *msgQueueQ;
    msgQueueQ = msgQueue;

    while ( msgQueueQ != NULL )
    {
        if ( msgQueueQ->msg.eventType == msgType )
        {
            tcmLog_notice ( "Find the msg with the type %d in the queue",
                            msgQueueQ->msg.eventType );
            return msgQueueQ;
            break;
        }
        msgQueueQ = msgQueueQ -> next;
    }

    return NULL;
}

tcmRet showEhMsgValue ( HOMEMSGTYPE msgType )
{
    return TCMRET_SUCCESS;
}

tcmRet showEhMsg ( homeMsgCtrlBlock* msg )
{
    if ( NULL == msg )
        return TCMRET_INVALID_PARAMETER;
    tcmLog_debug ( "The sequence message is %d", msg->sequenceID );
    tcmLog_debug ( "The message creating time is %s",
                   convertTimevalToChar ( msg->tmPoint ) );
    tcmLog_debug ( "The protocol version is %d",
                   msg->msg.protocolVersion );
    tcmLog_debug ( "The payload length is %d", msg->msg.payloadLen );
    if ( msg->msg.reserve == NULL )
    {
        tcmLog_notice ( "The reserve field is NULL" );
    }
    else
    {
        tcmLog_notice ( "The reserve field is %s", msg->msg.reserve );
    }
    tcmLog_debug ( "The type is %d", msg->msg.eventType );
    tcmLog_debug ( "The value length is %d", msg->msg.valueLen );
    if ( msg->msg.valueLen != 0 )
    {
        tcmLog_debug ( "The value is %s",
                       ( char * ) ( msg + 1 ) );
        if ( TCMRET_SUCCESS != showEhMsgValue ( msg->msg.eventType ) )
        {
            tcmLog_error ( "Show the value of this msg failed" );
            return TCMRET_FAILURE;
        }
    }

    return TCMRET_SUCCESS;
}
tcmRet showEhMsgQueue ( homeMsgCtrlBlock* msgQueue )
{
    if ( msgQueue == NULL )
    {
        tcmLog_debug ( "Msg queue is NULL\n" );
        return TCMRET_INVALID_PARAMETER;
    }

    homeMsgCtrlBlock *msgQueueQ;
    msgQueueQ = msgQueue;
    int num = 0;
    while ( msgQueueQ != NULL )
    {
        tcmLog_debug ( "\noutput the %dth msg", ++num );
        if ( TCMRET_SUCCESS != showEhMsg ( msgQueueQ ) )
        {
            return TCMRET_FAILURE;
        }

        msgQueueQ = msgQueueQ->next;
    }
    return TCMRET_SUCCESS;
}

void initHomeMsg ( uint8_t** buf, uint16_t* totalLen )
{
    memset ( *buf, 0, OFF_SET_VALUE_LEN );
    *totalLen = OFF_SET_VALUE;
}

void putMsgProtocolVersion ( uint8_t** buf,  uint16_t protocolVer )
{
    memcpy ( *buf, &protocolVer,
             OFF_SET_PAYLOAD_LEN - OFF_SET_PROTOCOL_VERSION );
}


void putMsgPayloadLen ( uint8_t** buf, uint16_t payloadLen )
{
    memcpy ( *buf + OFF_SET_PAYLOAD_LEN , &payloadLen,
             OFF_SET_RESERVE - OFF_SET_PAYLOAD_LEN );
}

void putMsgReserve ( uint8_t** buf,  uint8_t* reserve )
{
}

void putMsgEventType ( uint8_t** buf, uint16_t eventType )
{
    memcpy ( *buf + OFF_SET_EVENT_TYPE, &eventType,
             OFF_SET_VALUE_LEN - OFF_SET_EVENT_TYPE );
}

void putMsgValueLen ( uint8_t** buf, uint16_t valueLen )
{
    memcpy ( *buf + OFF_SET_VALUE_LEN, &valueLen,
             OFF_SET_VALUE - OFF_SET_VALUE_LEN );
}

void putMsgValue ( uint8_t** buf, uint16_t* totalLen, uint8_t* value,
                   uint16_t valueLen )
{
    if ( valueLen != 0 )
    {
        memcpy ( *buf + OFF_SET_VALUE, value, valueLen );
        *totalLen += valueLen;
    }
    putMsgPayloadLen ( buf, OFF_SET_VALUE - OFF_SET_EVENT_TYPE + valueLen );
    putMsgValueLen ( buf, valueLen );
}


void getMsgProtocolVersion ( uint8_t* buf,  uint16_t* protocolVer )
{
    memcpy ( protocolVer, buf, OFF_SET_PAYLOAD_LEN - OFF_SET_PROTOCOL_VERSION );
}


void getMsgPayloadLen ( uint8_t* buf, uint16_t* payloadLen )
{
    memcpy ( payloadLen, buf + OFF_SET_PAYLOAD_LEN ,
             OFF_SET_RESERVE - OFF_SET_PAYLOAD_LEN );
}

void getMsgReserve ( uint8_t* buf,  uint8_t** reserve )
{

    if ( 0 != * ( buf + OFF_SET_RESERVE ) )
        memcpy ( *reserve, buf + OFF_SET_RESERVE,
                 OFF_SET_EVENT_TYPE - OFF_SET_RESERVE );
    else
        *reserve = NULL;
}

void getMsgEventType ( uint8_t* buf, uint16_t* eventType )
{
    memcpy ( eventType, buf + OFF_SET_EVENT_TYPE,
             OFF_SET_VALUE_LEN - OFF_SET_EVENT_TYPE );
}

void getMsgValueLen ( uint8_t* buf, uint16_t* valueLen )
{
    memcpy ( valueLen, buf + OFF_SET_VALUE_LEN ,
             OFF_SET_VALUE - OFF_SET_VALUE_LEN );
}

void getMsgValue ( uint8_t* buf, uint8_t** value, uint16_t* valueLen )
{
    getMsgValueLen ( buf, valueLen );
    memcpy ( *value, buf + OFF_SET_VALUE, *valueLen );
}

void getMsgNumber ( uint8_t* buf, uint8_t* number )
{
    *number = * ( buf + OFF_SET_RESERVE + 3 );
}

tcmRet checkHomeMsgBuf ( char* buf, int bufLen )
{
    return TCMRET_SUCCESS;
}

tcmRet createHomeMsgBuf ( OUT char** buf, 
                              OUT int* bufLen,
                              IN uint16_t msgProVer, 
                              IN char* msgReserve,
                              IN uint16_t msgEventType, 
                              IN uint16_t msgValueLen,
                              IN uint8_t* msgValue )
{
    if ( msgProVer != HOME_MSG_PROTOCOL_VERSION )
    {
        tcmLog_error ( "Msg protocol version is error\n" );
        return TCMRET_FAILURE;
    }

    if ( !checkHomeMsgType ( msgEventType ) )
    {
        tcmLog_error ( "Msg type is error\n" );
        return TCMRET_FAILURE;
    }

    *buf =  tcmMem_alloc ( sizeof ( homeMessage ) + msgValueLen,
                           ALLOC_ZEROIZE );

    if ( *buf == NULL )
    {
        return TCMRET_RESOURCE_EXCEEDED;
    }
    
    initHomeMsg ( buf, bufLen );
    putMsgProtocolVersion ( buf, msgProVer );
    putMsgReserve ( buf, msgReserve );
    putMsgEventType ( buf, msgEventType );
    putMsgValue ( buf, bufLen, msgValue, msgValueLen );
    return TCMRET_SUCCESS;
}

tcmRet analyzeHomeMsgBuf ( IN char* buf, IN int bufLen,
                               OUT uint16_t* msgProVer,
                               OUT uint16_t payloadLen,
                               OUT char** msgReserve,
                               OUT uint16_t* msgEventType,
                               OUT uint16_t* msgValueLen ,
                               OUT uint8_t** msgValue )
{
    if ( TCMRET_SUCCESS != checkHomeMsgBuf ( buf, bufLen ) )
    {
        tcmLog_error ( "Protocol msg buf error format\n" );
        return TCMRET_FAILURE;
    }

    getMsgProtocolVersion ( buf, msgProVer );
    getMsgPayloadLen ( buf, payloadLen );
    getMsgReserve ( buf, msgReserve );
    getMsgEventType ( buf, msgEventType );
    getMsgValueLen ( buf, msgValueLen );
    getMsgValue ( buf, msgValue, msgValueLen );

    return TCMRET_SUCCESS;

}

tcmRet convertHomeMsgToCtrlBlock ( IN void* buf, IN int fd, IN SSL* ssl,
                                 OUT homeMsgCtrlBlock** msgCB )
{
    if ( buf == NULL )
    {
        tcmLog_notice ( "Protocol message can not be NULL\n" );
        return TCMRET_INVALID_PARAMETER;
    }
    homeMessage* msg = ( homeMessage* ) buf;
    homeMsgCtrlBlock* msgTmp = createHomeMsgBlock ( 
                                     msg->protocolVersion,
                                     msg->reserve,
                                     msg->eventType,
                                     msg->valueLen,
                                     ( void * ) ( msg + 1 ),
                                     fd, ssl );

    if ( msgTmp == NULL )
    {
        tcmLog_error ( "Create msg failed\n" );
        return TCMRET_INTERNAL_ERROR;
    }

    *msgCB = msgTmp;
    return TCMRET_SUCCESS;
}

tcmRet showHomeMsg ( homeMessage* msg )
{
    if ( msg == NULL )
    {
        tcmLog_notice ( "Msg can not be NULL\n" );
        return TCMRET_INVALID_PARAMETER;
    }

    tcmLog_debug ( "Now show one protocol message\n " );
    tcmLog_debug ( "Protocol message version is %d\n",
                   msg->protocolVersion );
    tcmLog_debug ( "Protocol message payload length is %d\n",
                   msg->payloadLen );
    if ( 0 == strlen ( msg->reserve ) )
    {
        tcmLog_debug ( "Protocol message reserve is NULL\n" );
    }
    else
    {
        tcmLog_debug ( "Protocol message reserve is %s\n",
                       msg->reserve );
    }
    tcmLog_debug ( "Protocol message type is %d\n", msg->eventType );
    if ( 0 != msg->valueLen )
    {
        tcmLog_debug ( "Protocol message value length is %d\n",
                       msg->valueLen );
        tcmLog_debug ( "Protocol message value  is %s\n",
                       ( char * ) ( msg + 1 ) );
    }

    return TCMRET_SUCCESS;
}

tcmRet checkHomeMsg ( homeMessage* msg )
{
    if ( msg == NULL )
    {
        return TCMRET_FAILURE;
    }

    if ( msg->protocolVersion != HOME_MSG_PROTOCOL_VERSION )
    {
        return TCMRET_FAILURE;
    }

    if ( !checkHomeMsgType ( msg->eventType ) )
    {
        return TCMRET_FAILURE;
    }

    if ( msg->payloadLen != ( msg->valueLen + 4 ) )
    {
        return TCMRET_FAILURE;
    }

    return TCMRET_SUCCESS;
}

char* getMsgStr( int msgID )
{   
   printf( "strlen is %d", sizeof( gMsgIDStrMap )/ sizeof( homeMsgIntToStr) );  
   int i = 0;   
   for( i = 0; i < MAX_MSG_NUM; i++ )   
   {      
      if( msgID == gMsgIDStrMap[i].msgID )      
      {         
         return gMsgIDStrMap[i].msgStr;      
      }   
   }

   for( i = MAX_MSG_NUM -1; i > -1; i-- )   
   {      
      if( INVALID_MSG_TYPE_ID == gMsgIDStrMap[i].msgID  )      
      {         
         return gMsgIDStrMap[i].msgStr;      
      }   
   }
}
