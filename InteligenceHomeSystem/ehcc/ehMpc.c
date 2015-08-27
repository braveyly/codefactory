#include <conflib.h>
#include <tcm_msg.h>
#include <tcm_util.h>
#include <configact.h>
#include "ehDevice.h"
#include "ehMq.h"
#include "ehMpc.h"
#include "ehTimeout.h"
#include "ehIndexDevice.h"
#include "ehDeviceDef.h"
#include "ehTransMsgMap.h"
#include "writedb.h"
#include "ehProto.h"
#include "ehDebug.h"

#define PAGING_GROUP_VEDIO   1
#define PAGING_GROUP_PICTURE 2
#define PAGING_GROUP_NONE    0

extern int gTokenStatus;
extern int gTokenOwern;

extern void* msgHandle;

typedef struct EHOMEDP
{
    uint8_t dpID;
    uint8_t dpName[MAXPARA_DPNAMELEN + 1];
    uint16_t dpDelayTime;
    uint8_t dpSetStatus;
    uint8_t dpVisualStatus;
    uint8_t dpAttribute;
    uint8_t dpDelayTimeSwitch;
}EhomeDP;

typedef struct EHOMESM
{
    uint8_t smID;
    uint8_t smRing;
    EhomeDP smDP[MAXTABLE_DETACTPOINT];
}EhomeSM;

typedef enum DPATTRIBUTETYPE
{
    DP_ATTRIBUTE_TYPE_SETTING_STATUS = 0x01,
    DP_ATTRIBUTE_TYPE_DP_NAME        = 0x02,
    DP_ATTRIBUTE_TYPE_DP_ATTRIBUTE   = 0x03,
    DP_ATTRIBUTE_TYPE_DELAY_TIME     = 0x04,
}dpAttributeType;

typedef enum SETSINGLEDPRESULT
{
    DP_SET_SINGLE_RESULT_SUCCEED    = 0x01,
    DP_SET_SINGLE_RESULT_FAILED     = 0x02,
}setSingleDpResult;


extern tcmRet processIDPToMFCBGetAuthInfo ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue, OUT uint16_t* prValueLen );

extern tcmRet processIDPToMFCBSynchronizeDeviceCmd ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue, OUT uint16_t* prValueLen );

/* ==================== Message Lib Part (Start) ==================== */
tcmRet sendEhomeMsg ( homeMsgCtrlBlock* sdMsg )
{
    char* buf;
    int bufLen;
    int ret = TCMRET_FAILURE;
    if ( (ret = createHomeMsgBuf ( &buf, &bufLen, sdMsg->msg.protocolVersion,
            sdMsg->msg.reserve, sdMsg->msg.eventType, sdMsg->msg.valueLen,
            ( char * ) ( sdMsg + 1 ) )) != TCMRET_SUCCESS)
    {
        tcmLog_error ( "+++ Create write buf failed +++" );

        if ( NULL != buf )
        {
            tcmMem_free ( buf );
        }
        
        return ret;
    }
    cvtHtoNEndian ( &buf );

#ifdef SSL_CONNECT
    ret = ehSslWrite ( sdMsg->ssl, ( homeMessage * ) buf );
#else
    ret = ehWrite ( sdMsg->connfd, ( homeMessage * ) buf );
#endif
    if ( TCMRET_SUCCESS != ret )
    {

        if ( ret == TCMRET_DISCONNECTED )
        {
            tcmLog_notice ( "+++ Socket fd %d disconnected +++", sdMsg->connfd );
        }
        else
        {
            tcmLog_error ( "+++ Write buffer failed +++" );
        }
    }

    tcmMem_free ( buf );
    return ret;
}

tcmRet ehSendMsgToDevice( int connfd, SSL* ssl, uint16_t msgEventType, 
                                   uint16_t msgValueLen, void* msgValue )
{
    int ret = TCMRET_INTERNAL_ERROR;
    homeMsgCtrlBlock* msgTmp = createHomeMsgBlock ( HOME_MSG_PROTOCOL_VERSION, 
                               NULL, msgEventType, msgValueLen, msgValue,
                               connfd, ssl );

    if ( NULL != msgTmp )
    {
        ret = sendEhomeMsg ( msgTmp );
        tcmMem_free ( msgTmp );
    }

    return ret;
}

tcmRet ehSendMsgToAllDevice( int deviceType, uint16_t msgEventType, 
                                       uint16_t msgValueLen, void* msgValue )
{
    if ( msgValue == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }
    
    int i = 0;
    DeviceInfo *deviceTmp = NULL;

    moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    for ( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
        deviceTmp = getDeviceInfoByDeviceIndex( i + 1 );

        if ( deviceTmp == NULL )
        {
            continue;
        }

        if ( deviceTmp ->status == DEVICE_STATUS_CONNECTED 
            && ( tcm_itob(deviceTmp->type) & deviceType ) )
        {
            ehSendMsgToDevice(deviceTmp->connfd, deviceTmp->ssl, msgEventType, msgValueLen, msgValue);
        }

        tcmMem_free(deviceTmp);
    }
    moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);

    return TCMRET_SUCCESS;
}
/* ==================== Message Lib Part (End) ==================== */



/* ==================== Protocol Encode or Decode Part (Start) ==================== */
tcmRet ehEncodeSimpleReply( IN uint8_t* msgValue, IN uint16_t msgLen,
                            IN uint8_t* replyValue, IN uint16_t replyLen,
                            OUT uint8_t* prValue, OUT uint16_t* prValueLen)
{
    if ( replyValue == NULL || prValue == NULL || prValueLen == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    *prValueLen = 0;

    if ( msgValue != NULL && msgLen > 0 )
    {
        memcpy ( prValue + *prValueLen, msgValue, msgLen );
        *prValueLen += msgLen;
    }

    memcpy ( prValue + *prValueLen, replyValue, replyLen );
    *prValueLen += replyLen;

    return TCMRET_SUCCESS;
}

tcmRet ehEncodeAppendOneCard( IN int index, OUT uint8_t* prValue, 
                             OUT uint16_t* prValueLen)
{
    if ( prValue == NULL || prValueLen == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    int i = 0;
    
    char tmp[TAB_PARA_CARD_END -1][MAX_VALUE_LEN + 1];
    char* pTmp[TAB_PARA_CARD_END -1];

    for ( i = 0; i < TAB_PARA_CARD_END -1; i++ )
    {
        pTmp[i] = tmp[i];
        memset(tmp[i], 0, sizeof(tmp[i]));
    }

    if ( capi_read_table_row(ID_CARD, index, pTmp) )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }
    
    * ( prValue + *prValueLen ) = ( uint8_t ) strlen ( pTmp[TAB_PARA_CARD_ID - 1] );
    *prValueLen += 1;

    memcpy ( prValue + *prValueLen, pTmp[TAB_PARA_CARD_ID - 1],
             strlen ( pTmp[TAB_PARA_CARD_ID - 1] ) );
    *prValueLen += ( uint8_t ) strlen ( pTmp[TAB_PARA_CARD_ID - 1] );

    * ( prValue + *prValueLen ) = ( uint8_t ) strlen ( pTmp[TAB_PARA_CARD_NAME - 1] );
    *prValueLen += 1;

    memcpy ( prValue + *prValueLen, pTmp[TAB_PARA_CARD_NAME - 1],
             strlen ( pTmp[TAB_PARA_CARD_NAME - 1] ) );
    *prValueLen += ( uint8_t ) strlen ( pTmp[TAB_PARA_CARD_NAME - 1] );  

    * ( prValue + *prValueLen ) = (uint8_t)actCard_Type_StringToInt(pTmp[TAB_PARA_CARD_TYPE - 1]);
    *prValueLen += 1;

    * ( prValue + *prValueLen ) = (uint8_t)actCard_Status_StringToInt(pTmp[TAB_PARA_CARD_STATUS - 1]);
    *prValueLen += 1;

    return TCMRET_SUCCESS;
}

void ehEncodeParseDetectPoint(EhomeDP* pDP, int numDP, int mode_ID)
{
    int i = 0;
    uint16_t delayTime;
    char value[MAX_VALUE_LEN + 1];
    char tmp[TAB_PARA_MODE_END - 1][MAX_VALUE_LEN + 1];
    char* pTmp[TAB_PARA_MODE_END -1];

    /* For reading tables */
    for ( i = 0; i < TAB_PARA_MODE_END -1; i++ )
    {
        pTmp[i] = tmp[i];
        memset(tmp[i], 0, sizeof(tmp[i]));
    }

    /* Read the delay time */
    if ( capi_read_parameter ( ID_SECURITYMODE, PARA_SECURITYMODE_DELAYTIME,
                               value ) )
    {
        tcmLog_error ( "Read delay time wrong" );
        delayTime = 0;
    }
    else
    {
        delayTime = ( uint16_t ) atoi ( value );
        delayTime = htons ( delayTime );
    }

    for ( i = 0; i < numDP; i++ )
    {
        if ( capi_read_table_row(mode_ID, i + 1, pTmp) )
        {
            tcmLog_notice("Can not get the mode id: %d, row: %d", mode_ID, i + 1);
            continue;
        }

        if ( strcasecmp ( pTmp[TAB_PARA_MODE_ACTIVE_STATUSE - 1], "enable" ) == 0 )
        {
            (pDP + i)->dpSetStatus = PPI_DP_STATUS_ENABLE;
        }
        else
        {
            (pDP + i)->dpSetStatus = PPI_DP_STATUS_DISABLE;
        }

        if ( strcasecmp ( pTmp[TAB_PARA_MODE_ATTRIBUTE - 1], "enable" ) == 0 )
        {
            (pDP + i)->dpAttribute = PPI_DP_ATTRIBUTE_OPENFOREVER;
        }
        else
        {
            (pDP + i)->dpAttribute = PPI_DP_ATTRIBUTE_USER_SET;
        }

        if ( strcasecmp ( pTmp[TAB_PARA_MODE_ACTIVE_DELAY - 1], "enable" ) == 0 )
        {
            (pDP + i)->dpDelayTimeSwitch = PPI_DP_DELAYTIME_ENABLE;
        }
        else
        {
            (pDP + i)->dpDelayTimeSwitch = PPI_DP_DELAYTIME_DISABLE;
        }

        if ( capi_read_table_parameter ( ID_DETACTPOINT, i+1,
                                         TAB_PARA_DETECTPOINT_NAME, value ))
        {
            tcmLog_error("Can not get the detect point name");
            memset(value, 0, sizeof(value));
        }
        strcpy ( (pDP + i)->dpName, value );

        if ( capi_read_table_parameter ( ID_DETACTPOINT, i+1,
                                         TAB_PARA_DETECTPOINT_ALARMSTATUS, value ))
        {
            tcmLog_error("Can not get the detect point alarm status");
        }

        if ( strcasecmp ( value, "normal" ) == 0 )
        {
            (pDP + i)->dpVisualStatus = PPI_DP_NORMAL;
        }
        else
        {
            (pDP + i)->dpVisualStatus = PPI_DP_ALERT;
        }

        (pDP + i)->dpID  = ( uint8_t ) ( i + 1 );
        (pDP + i)->dpDelayTime = delayTime;
    }
}

tcmRet ehEncodeFlashTime( IN const char* flashTime, OUT uint8_t* prValue, 
                          OUT uint16_t* prValueLen)
{
    if ( flashTime == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    char tmpStr[BUFLEN_8];
    
    if ( strlen(flashTime) == 0 )
    {
        * ( prValue + *prValueLen ) = 0;
        *prValueLen += 1;
        * ( prValue + *prValueLen ) = 0;
        *prValueLen += 1;
    }
    else
    {
        memset(tmpStr, 0, sizeof(tmpStr));
        memcpy ( tmpStr, flashTime, 2 );
        * ( prValue + *prValueLen ) = atoi ( tmpStr );
        *prValueLen += 1;

        memset(tmpStr, 0, sizeof(tmpStr));
        memcpy ( tmpStr, flashTime + 3, 2 );
        * ( prValue + *prValueLen ) = atoi ( tmpStr );
        *prValueLen += 1;
    }
    
    return TCMRET_SUCCESS;
}

tcmRet ehEncodeOneTypeDeviceInfo(IN deviceStruct* ds, IN int dNum,
    IN uint8_t dType, OUT uint8_t* prValue, OUT uint16_t* prValueLen)
{
    if ( ds == NULL || prValue == NULL || prValueLen == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    uint8_t devNameLen, devNumberLen;

    if ( dNum >= 0 )
    {
        * ( prValue + *prValueLen ) = dType;
        *prValueLen += 1;

        * ( prValue + *prValueLen ) = dNum;
        *prValueLen += 1;

        int j;

        for ( j = 0; j < dNum; j++ )
        {
            * ( prValue + *prValueLen ) = (ds+j)->devID;
            *prValueLen += 1;

            devNameLen = strlen ((ds+j)->devName );
            devNumberLen = strlen ( (ds+j)->devNumber );

            * ( prValue + *prValueLen ) = devNameLen;
            *prValueLen += 1;

            memcpy ( prValue + *prValueLen, (ds+j)->devName, devNameLen );
            *prValueLen += devNameLen;

            * ( prValue + *prValueLen ) = devNumberLen;
            *prValueLen += 1;

            memcpy ( prValue + *prValueLen, (ds+j)->devNumber, devNumberLen );
            *prValueLen += devNumberLen;

            * ( prValue + *prValueLen ) = (ds+j)->devSubType;
            *prValueLen += 1;
        }
    }

    return TCMRET_SUCCESS;
}

tcmRet sendSynDeviceEvt()
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;
    processIDPToMFCBSynchronizeDeviceCmd( NULL, value, &valueLen );

    int dType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    return ehSendMsgToAllDevice( dType, EVT_MFCB_TO_IDP_SYN_DEVICE, 
                         valueLen, value);
}

/* ==================== Protocol Encode or Decode Part (End) ==================== */


void releaseTokenByFD(int fd)
{
    if ( gTokenOwern == fd )
    {
        gTokenStatus = PPI_TOKEN_STATUS_FREE;
        gTokenOwern  = 0;
    }
}

int getRingTypeIDBySM( int smID )
{
    char smMediaTypeStr[MAX_VALUE_LEN+1];
    int ringType;
    int smAppID = actSecurityMode_getModeAppIDByModeIndex(smID);

    if( -1 == capi_read_parameter( smAppID, PARA_EACHMODE_RINGMODE, 
                                   smMediaTypeStr ) )
    {
        return TCMRET_FAILURE;
    }

    if( 0 == strcasecmp( smMediaTypeStr, SECURITYMODE_RINGGROUP_IDP ) )
    {
       ringType = PPO_MEDIA_TYPE_VEDIO;
    }
    else if( 0 == strcasecmp( smMediaTypeStr, SECURITYMODE_RINGGROUP_SMP ) )
    {
       ringType = PPO_MEDIA_TYPE_PICTURE; 
    }
    else
    {
       ringType = PPO_MEDIA_TYPE_NONE;   
    }

    return ringType;
}

tcmRet convertRingTypeIDToStr( int ringID, char* ringStr )
{
   int ret = TCMRET_SUCCESS;
   if( ringID == PPO_MEDIA_TYPE_VEDIO )
   {
      strcpy( ringStr, SECURITYMODE_RINGGROUP_IDP );
   }
   else if( ringID == PPO_MEDIA_TYPE_PICTURE )
   {
      strcpy( ringStr, SECURITYMODE_RINGGROUP_SMP );
   }
   else if( ringID == PPO_MEDIA_TYPE_NONE )
   {
      strcpy( ringStr, SECURITYMODE_RINGGROUP_NONE );
   }
   else
   {
      ret = TCMRET_FAILURE;
   }

   return ret;
}

/* ==================== Handle Message Part (Start) ==================== */
#ifdef AUTO_CALL_TEST
void getPageNumByAccount( IN char* gtAccount, OUT char* gtPage, OUT uint8_t* pageNumLen )
{
   char IDPGroupNumbers[MAX_VALUE_LEN+1];
   char SMPGroupNumbers[MAX_VALUE_LEN+1];
   char* result;
   
   memset( IDPGroupNumbers, 0, sizeof( IDPGroupNumbers ) );
   memset( SMPGroupNumbers, 0, sizeof( SMPGroupNumbers ) );
   
   if( capi_read_table_parameter( ID_PROXY, 1, TAB_PARA_PHONE_NUMBERS, 
                                        IDPGroupNumbers ) )
      return;

   if( capi_read_table_parameter( ID_PROXY, 2, TAB_PARA_PHONE_NUMBERS, 
                                        SMPGroupNumbers ) )
      return;   

   result = strstr( IDPGroupNumbers, gtAccount );
   if( NULL != result )
   {
      strcpy( gtPage, "400" );
      *pageNumLen = 3;
      return;
   }

   result = strstr( SMPGroupNumbers, gtAccount );
   if( NULL != result )
   {
      strcpy( gtPage, "401" );
      *pageNumLen = 3;
      return;
   }

   *pageNumLen = 0;
   
}


//tcmRet sendMsgByAccount( )
tcmRet sendAutoCallTest( CallTest* sdCall, int act )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t curValueLen = 0;

    uint8_t actTmp = ( uint8_t ) act;
    uint8_t callerNumLen = ( uint8_t ) strlen( sdCall->caller );
    uint8_t calleeNumLen = ( uint8_t ) strlen( sdCall->callee );
    uint16_t intervalTime = htons( ( uint16_t ) sdCall->intervalTime );
    uint16_t duringTime = htons( ( uint16_t ) sdCall->duringTime );
    uint8_t stopper = ( 0 == strcasecmp( sdCall->stopper, "caller" ) ? 1:2 );
    uint8_t pageNum[MAX_VALUE_LEN+1];
    uint8_t pageNumLen = 0;

    tcmLog_debug( "actTmp is %d", actTmp );
    tcmLog_debug( "callerNumLen is %d", callerNumLen );
    tcmLog_debug( "calleeNumLen is %d", calleeNumLen );
    tcmLog_debug( "intervalTime is %d", intervalTime );
    tcmLog_debug( "duringTime is %d", duringTime );
    tcmLog_debug( "stopper is %d", stopper );
    memset( pageNum, 0, sizeof(pageNum) );
    getPageNumByAccount( sdCall->callee, pageNum, &pageNumLen );
    tcmLog_error( "pageNum is %s", pageNum );
    tcmLog_error( "pageNumLen is %d", pageNumLen );
     
    *( value + curValueLen ) = actTmp;
    curValueLen += 1;

    *( value + curValueLen ) = callerNumLen;
    curValueLen += 1;    

    memcpy( value + curValueLen, sdCall->caller, callerNumLen );
    curValueLen += callerNumLen;     
    
    *( value + curValueLen ) = calleeNumLen;
    curValueLen += 1;    

    memcpy( value + curValueLen, sdCall->callee, calleeNumLen );
    curValueLen += calleeNumLen; 

    memcpy( value + curValueLen, &intervalTime, 2 );
    curValueLen += 2;    

    *( value + curValueLen ) = stopper;
    curValueLen += 1; 

    memcpy( value + curValueLen, &duringTime, 2 );
    curValueLen += 2;  

    *( value + curValueLen ) = pageNumLen;
    curValueLen += 1; 

    if( 0 != pageNumLen )
    {
       memcpy( value + curValueLen, pageNum, pageNumLen );
       curValueLen += pageNumLen;
    }

    if( 2 == actTmp )
    {
       int dType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP) | tcm_itob(DEVICE_ODP);
       return ehSendMsgToAllDevice(dType, EVT_MFCB_AUTO_CALL_TEST, 
                             curValueLen, value);
    }
    
    int callerIndex = getDeviceIndexByNumber( sdCall->caller );
    int calleeIndex = getDeviceIndexByNumber( sdCall->callee );
    showIndexDeviceArray();
    tcmLog_error( "caller fd %d", g_devArray[callerIndex-1].connfd );
    tcmLog_error( "callee fd %d", g_devArray[calleeIndex-1].connfd );
    if( INVALID_DEVICE_INDEX != callerIndex)
    ehSendMsgToDevice( g_devArray[callerIndex-1].connfd, 
                       g_devArray[callerIndex-1].ssl, EVT_MFCB_AUTO_CALL_TEST,
                       curValueLen, value );
    
    if( INVALID_DEVICE_INDEX != callerIndex)
    ehSendMsgToDevice( g_devArray[calleeIndex-1].connfd, 
                       g_devArray[calleeIndex-1].ssl, EVT_MFCB_AUTO_CALL_TEST,
                       curValueLen, value );
       
}
#endif

/*deal with EVT_MFCB_TO_IDP_ALARM_DETECT*/
tcmRet sendAlarmStatus ( uint8_t smID, uint8_t dpID, uint8_t visualStatus )
{
    uint8_t value[3];
    value[0] = smID;
    value[1] = dpID;
    value[2] = visualStatus;

    int dType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    return ehSendMsgToAllDevice(dType, EVT_MFCB_TO_IDP_ALARM_DETECT, 
                             sizeof(value), value);
}

/*deal with EVT_ODP_TO_MFCB_NEW_CARD*/
tcmRet sendNewCard ( int newIndex )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t curValueLen = 0;

    * ( value + curValueLen ) = 1; //card num
    curValueLen += 1;

    if ( ehEncodeAppendOneCard( newIndex, value, &curValueLen ) != TCMRET_SUCCESS )
    {
        tcmLog_error("Invalid new card index: %d", newIndex);
        return TCMRET_INVALID_ARGUMENTS;
    }

    int type = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    return ehSendMsgToAllDevice( type, EVT_MFCB_TO_IDP_NEW_CARD_INFO,
                                 curValueLen, value );
}


/*deal with EVT_MFCB_TO_ODP_FLASH_ENABLE_TIME*/
tcmRet sendNewFlashTimeToODP ()
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t curValueLen = 0;
    uint8_t startTime[MAX_VALUE_LEN+1];
    uint8_t endTime[MAX_VALUE_LEN+1];

    /* Read the front door */
    if ( capi_read_table_parameter ( ID_DOOR, 1, TAB_PARA_START_FLASH_TIME, startTime ) 
      || capi_read_table_parameter ( ID_DOOR, 1, TAB_PARA_END_FLASH_TIME, endTime ))
    {
        return TCMRET_FAILURE;
    }

    * ( value + curValueLen ) = PPO_DOOR_FRONT; //front outdoor
    curValueLen += 1;

    ehEncodeFlashTime(startTime, value, &curValueLen);
    ehEncodeFlashTime(endTime, value, &curValueLen);

    /* Read the front door */
    if ( capi_read_table_parameter ( ID_DOOR, 2, TAB_PARA_START_FLASH_TIME, startTime ) 
      || capi_read_table_parameter ( ID_DOOR, 2, TAB_PARA_END_FLASH_TIME, endTime ))
    {
        return TCMRET_FAILURE;
    }

    * ( value + curValueLen ) = PPO_DOOR_BACK; //back outdoor
    curValueLen += 1;

    ehEncodeFlashTime(startTime, value, &curValueLen);
    ehEncodeFlashTime(endTime, value, &curValueLen);

    return ehSendMsgToAllDevice( tcm_itob(DEVICE_ODP), EVT_MFCB_TO_ODP_FLASH_ENABLE_TIME, 
                                 curValueLen, value );
}

// TODO: switch security mode need to gear the mode, we need to send msg to zwave
tcmRet switchSecurityMode()
{
    uint8_t userMode[MAX_VALUE_LEN+1];
    if ( -1 == capi_read_parameter ( ID_SECURITYMODE,
                                     PARA_SECURITYMODE_USERMODE,
                                     userMode ) )
    {
        return TCMRET_FAILURE;
    }

    if ( strcasecmp ( userMode, SECURITYMODE_CLEAR ) == 0 )
    {
        capi_write_parameter ( ID_SECURITYMODE, PARA_SECURITYMODE_USERMODE,
                               SECURITYMODE_OUTDOOR );
    }
    else
    {
        capi_write_parameter ( ID_SECURITYMODE, PARA_SECURITYMODE_USERMODE,
                               SECURITYMODE_CLEAR );
    }
    
    /* Send msg to sdd, renew the delay time */
    actUtil_sendMsg(ID_SDD, FALSE, TCM_MSG_RENEW_DELAYTIME, 0, NULL, 0);

    capi_write_to_flash();
    return TCMRET_SUCCESS;
}

/*deal with EVT_MFCB_TO_ODP_FLASH_ENABLE_TIME*/
tcmRet sendSecurityModeToODP ()
{
    uint8_t value[MAX_PACKET_LEN];
    int curSecurityMode, smOrderId, pagingGroupType;
    uint8_t smFlag = PPO_SECURITY_MODE_CLOSE;
    uint16_t curValueLen = 0;
    char userMode[MAX_VALUE_LEN+1], ringMode[MAX_VALUE_LEN+1], pageGroupNum[MAX_VALUE_LEN+1];
    uint8_t smMediaTypeID;
       
    if ( capi_read_parameter ( ID_SECURITYMODE, PARA_SECURITYMODE_USERMODE, userMode ) )
    {
        return TCMRET_CONFLIB_READ_ERROR;
    }

    tcmLog_notice("Current user mode is %s", userMode);

    curSecurityMode = actSecurityMode_getModeAppIDByModeString(userMode);
    smOrderId = actSecurityMode_getModeIndexByModeString(userMode);

    if ( curSecurityMode == ID_MODE_CLEAR )
    {
        smFlag = PPO_SECURITY_MODE_CLOSE;
    }
    else
    {
        smFlag = PPO_SECURITY_MODE_OPEN;
    }

    if( capi_read_parameter( curSecurityMode, PARA_EACHMODE_RINGMODE, ringMode ) )
    {
        return TCMRET_CONFLIB_READ_ERROR;
    }

    tcmLog_notice("Current ring mode is %s", ringMode);

    if( 0 == strcasecmp( ringMode, SECURITYMODE_RINGGROUP_IDP ) )
    {
       smMediaTypeID = PPO_MEDIA_TYPE_VEDIO;
       pagingGroupType = PAGING_GROUP_VEDIO;
    }
    else if( 0 == strcasecmp( ringMode, SECURITYMODE_RINGGROUP_SMP ) )
    {
       smMediaTypeID = PPO_MEDIA_TYPE_PICTURE; 
       pagingGroupType = PAGING_GROUP_PICTURE;
    }
    else
    {
       smMediaTypeID = PPO_MEDIA_TYPE_NONE;   
       pagingGroupType = PAGING_GROUP_NONE;
    }

    if( PAGING_GROUP_NONE != pagingGroupType )
    {
        if( -1 == capi_read_table_parameter( ID_PROXY, pagingGroupType, 
                                             TAB_PARA_PAGINGGROUP_NUM, 
                                             pageGroupNum ) )
        {
           return TCMRET_FAILURE;
        }

        tcmLog_notice("Current group number: %s", pageGroupNum);
    }

    tcmLog_notice("security mode flag 0x%x, mode id 0x%x, media type 0x%x, group type 0x%x", 
      smFlag, smOrderId, smMediaTypeID, pagingGroupType);
    
    * ( value + curValueLen ) = smFlag; //security mode status
    curValueLen += 1;

    * ( value + curValueLen ) = smOrderId; // current security mode
    curValueLen += 1;

    * ( value + curValueLen ) = smMediaTypeID; //media type
    curValueLen += 1;

    if( PAGING_GROUP_NONE == pagingGroupType )
    {
       * ( value + curValueLen ) = 0x00;  //paging group num length
       curValueLen += 1;
    }
    else
    {
       * ( value + curValueLen ) = (uint8_t) strlen( pageGroupNum );  //paging group num length
       curValueLen += 1;   

       memcpy( (void*)(value + curValueLen), pageGroupNum, strlen(pageGroupNum) );
       curValueLen += (uint8_t) strlen(pageGroupNum);
    }
    
    return ehSendMsgToAllDevice( tcm_itob(DEVICE_ODP), EVT_MFCB_TO_ODP_SECURITY_MODE, 
                                 curValueLen, value);
}

/*deal with EVT_MFCB_TO_IDP_SYNCHRONIZATION_TIME*/
int sendSynTime ( void* arg1, void* arg2 )
{
    char tmp[BUFLEN_64];
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen = 0;
    time_t synTime;
    time ( &synTime );
    struct tm* synTime_p = gmtime ( &synTime );

    memset(tmp, 0, sizeof(tmp));
    sprintf ( tmp, "%04u.%02u.%02u %02d:%02d:%02d", 
        synTime_p->tm_year + 1900, synTime_p->tm_mon + 1, synTime_p->tm_mday,
        synTime_p->tm_hour, synTime_p->tm_min, synTime_p->tm_sec );

    int tmpLen = strlen(tmp);
    *( value + valueLen ) = tmpLen;
    valueLen += 1;
    memcpy(value + valueLen, tmp, tmpLen);
    valueLen += tmpLen;

    int devType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_ODP) | tcm_itob(DEVICE_SMP);
    return ehSendMsgToAllDevice(devType, EVT_MFCB_TO_IDP_SYNCHRONIZATION_TIME, 
                                valueLen, value);
}

tcmRet sendAuthInfoToIDP()
{
    uint8_t value[MAX_PACKET_LEN+1];
    uint16_t valueLen;
    tcmRet ret = TCMRET_SUCCESS;
    
    if( TCMRET_SUCCESS != 
        ( ret = processIDPToMFCBGetAuthInfo( NULL, value, &valueLen ) ) )
    {
       tcmLog_error( "Get auth info wrong" );
       return TCMRET_FAILURE;
    }

    int dType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    return ehSendMsgToAllDevice(dType, 
                                EVT_MFCB_TO_IDP_CHANGE_AUTHENTICATION_INFO, 
                                valueLen, value);
}


/*deal with CMD_MFCB_TO_ODP_OPENDOORTIME*/
tcmRet sendNewOpenDoorTimeToODP ()
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t curValueLen = 0;
    uint8_t frontDoorOpenTime[MAX_VALUE_LEN+1];
    uint8_t backDoorOpenTime[MAX_VALUE_LEN+1];
    uint8_t frontDoorRingTime[MAX_VALUE_LEN+1];
    uint8_t backDoorRingTime[MAX_VALUE_LEN+1];
    uint16_t frontDoorOpenTimeInt = 0;
    uint16_t backDoorOpenTimeInt = 0;
    uint16_t frontDoorOpenTimeIntTmp = 0;
    uint16_t backDoorOpenTimeIntTmp = 0;    
    uint8_t frontDoorRingTimeInt = 0;
    uint8_t backDoorRingTimeInt = 0;

    memset( frontDoorOpenTime, 0, sizeof(frontDoorOpenTime) );
    memset( backDoorOpenTime, 0, sizeof(backDoorOpenTime) );
    memset( frontDoorRingTime, 0, sizeof(frontDoorRingTime) );
    memset( backDoorRingTime, 0, sizeof(backDoorRingTime) );
    /* Read the front/back ODP open door time */
    if ( capi_read_table_parameter ( ID_DOOR, 1, TAB_PARA_KEEP_OPEN_TIME, 
                                     frontDoorOpenTime ) 
      || capi_read_table_parameter ( ID_DOOR, 2, TAB_PARA_KEEP_OPEN_TIME, 
                                     backDoorOpenTime ) 
      || capi_read_table_parameter ( ID_DOOR, 1, TAB_PARA_RING_TIME, 
                                     frontDoorRingTime )
      || capi_read_table_parameter ( ID_DOOR, 2, TAB_PARA_RING_TIME, 
                                     backDoorRingTime ) )
    {
        return TCMRET_FAILURE;
    }

    frontDoorOpenTimeInt = (uint16_t)atoi( frontDoorOpenTime );
    backDoorOpenTimeInt = (uint16_t)atoi( backDoorOpenTime );
    frontDoorRingTimeInt = (uint8_t)atoi( frontDoorRingTime );
    backDoorRingTimeInt = (uint8_t)atoi( backDoorRingTime );
    
    * ( value + curValueLen ) = PPO_DOOR_FRONT; //front outdoor
    curValueLen += 1;

    frontDoorOpenTimeIntTmp = htons ( frontDoorOpenTimeInt );
    memcpy( value + curValueLen , &frontDoorOpenTimeIntTmp, 2 );
    curValueLen += 2;

    * ( value + curValueLen ) = frontDoorRingTimeInt; //temporary steady
    curValueLen += 1;

    * ( value + curValueLen ) = PPO_DOOR_BACK; //front outdoor
    curValueLen += 1;

    backDoorOpenTimeIntTmp = htons( backDoorOpenTimeInt );
    memcpy( value + curValueLen, &backDoorOpenTimeIntTmp, 2 );
    curValueLen += 2;

    * ( value + curValueLen ) = backDoorRingTimeInt; //temporary steady
    curValueLen += 1;
    
    return ehSendMsgToAllDevice( tcm_itob(DEVICE_ODP), CMD_MFCB_TO_ODP_OPENDOORTIME, 
                                 curValueLen, value );
}

tcmRet sendRegisterNoDeviceACK( int connfd, SSL* ssl )
{
   char value[MAX_PACKET_LEN];
   uint16_t valueLen = 0;

   memset( value, 0, MAX_PACKET_LEN );
   value[0] = PPI_PAIR_FAILED;
   value[1] = PPI_PAIR_REASON_CANNOT_FIND;
   valueLen = 2;

   return ehSendMsgToDevice( connfd, ssl, ACK_IDP_TO_MFCB_REGISTER, valueLen, 
                             value );
}

tcmRet sendOpenDoorAckToIdp ( uint16_t* valueLen, uint8_t* value,
                           EhTransMsgMap* transMsg )
{
    if ( valueLen == NULL || value == NULL || transMsg == NULL )
    {
        return -1;
    }
    
    return ehSendMsgToDevice( transMsg->connfd, transMsg->ssl, transMsg->mapMsg, 
                              *valueLen, value);
}
void showKeepAliveInfoArray()
{
    int i;
    tcmLog_debug ( "Show all keep alive devices.");
    for ( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
           tcmLog_debug ( "Array index %d: connfd %d, flag %d", i ,
                           g_keepAliveInfo[i].connfd,
                           g_keepAliveInfo[i].flag);
    }
}

int keepAliveSync ( void* arg1, void* arg2 )
{
    int i = 0;

    for ( i = 0 ; i < MAXTABLE_DEVICES; i++ )
    {
        if(g_keepAliveInfo[i].connfd > 0)
        {
            g_keepAliveInfo[i].flag += KEEP_ALIVE_POLLING_TIME;
        }
        if(g_keepAliveInfo[i].flag > KEEP_ALIVE_CHECKING_TIME)
        {
            tcmLog_debug ( "keepAliveSync:No heartbeat for %d times from %d. Close it and"
                               "update DISCONNECTED status",
                                KEEP_ALIVE_CHECKING_TIME/KEEP_ALIVE_POLLING_TIME,
                                g_keepAliveInfo[i].connfd );
            pthread_mutex_lock ( &g_homeIdpTokenMutex );
            releaseTokenByFD ( g_keepAliveInfo[i].connfd );
            pthread_mutex_unlock ( &g_homeIdpTokenMutex );

            moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
            int devIndex = getDeviceIndexByFd(g_keepAliveInfo[i].connfd);
            tcmLog_debug( "devIndex is %d", devIndex );
            DeviceInfo* ehDeviceTmp = getDeviceInfoByDeviceIndex( devIndex );

            showKeepAliveInfoArray();
            /*deal with data without device match case*/
            if ( ehDeviceTmp != NULL ) /*this fd has match device*/
            {
                tcmLog_debug ( "Device mac[%s] ip[%s] num[%s] fd[%d] type[%d]",
                               ehDeviceTmp->mac, ehDeviceTmp->ip,
                               ehDeviceTmp->userName, g_keepAliveInfo[i].connfd,
                               ehDeviceTmp->type);
                //close SSL & socket
#ifdef SSL_CONNECT
                SSL_free ( g_devArray[devIndex-1].ssl );
                FD_CLR ( g_keepAliveInfo[i].connfd, &readFdsMaster );
                close ( g_keepAliveInfo[i].connfd );

                g_devArray[devIndex - 1].connfd = ZERO_FD;
#else
                FD_CLR ( g_keepAliveInfo[i].connfd, &readFdsMaster );
                close ( g_keepAliveInfo[i].connfd );
                if ( g_keepAliveInfo[i].connfd == gehMaxFd )
                {
                    gehMaxFd = gehMaxFd - 1;
                }
                g_devArray[devIndex -1].connfd = ZERO_FD;
#endif
                    
                updateStatusByDeviceIndex(DEVICE_STATUS_DISCONNECTED, devIndex);
                gen_db_file();
                UpdatePagingConfig();
            }//if ( ehDeviceTmp != NULL )
            g_keepAliveInfo[i].connfd = -1;
            g_keepAliveInfo[i].flag = -1;
            showIndexDeviceArray();
            showKeepAliveInfoArray();
            moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        }

    }

    return 1;
}


/*deal with EVT_MFCB_TO_IDP_RESET_DEFAULT*/
tcmRet sendResetDefaultEvt ( int subType )
{
    uint8_t value = subType;
    int dType = tcm_itob(DEVICE_ODP) | tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);

    return ehSendMsgToAllDevice(dType, EVT_MFCB_TO_IDP_RESET_DEFAULT, 1,  &value);
}


tcmRet sendStartPairEvtToODP ( DeviceInfo* gehomeODPDevice  )
{
    return ehSendMsgToDevice( gehomeODPDevice->connfd, gehomeODPDevice->ssl, 
                              EVT_MFCB_TO_ODP_START_REGISTER, 0, NULL);
}

tcmRet processIDPToMFCBDoControlCmd ( IN homeMsgCtrlBlock* prMsg,
                                      OUT uint8_t* prValue,
                                      OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL || prValueLen == NULL )
    {
        return TCMRET_FAILURE;
    }

    uint8_t doId;
    uint8_t doSetType;
    uint8_t doSetStatus;
    char doSetStatusStr[10];
    char doSetName[MAX_VALUE_LEN];
    uint8_t doSetNameLen;
    uint16_t curValueLen = 0;

    uint8_t actResult = ACT_RESULT_FAILED;

    doId = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    doSetType = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    if ( doId > MAXTABLE_DOUT || doId <= 0 )
    {
        tcmLog_error ( "DO id error" );
        ehEncodeSimpleReply(( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen, 
            &actResult, 1, prValue, prValueLen);
        return TCMRET_FAILURE;
    }

    if ( doSetType == DOUT_SET_STATUS )
    {
        doSetStatus = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;

        if ( doSetStatus == DOUT_SET_STATUS_ON )
        {
            sprintf ( doSetStatusStr, "on" );
        }
        else
        {
            sprintf ( doSetStatusStr, "off" );
        }

        if ( TCMRET_SUCCESS == actDout_DOControl(doId, doSetStatusStr) )
        {
            actResult = ACT_RESULT_SUCCEED;
        }
    }
    else if ( doSetType == DOUT_SET_NAME )
    {
        doSetNameLen = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;

        memcpy ( doSetName, ( uint8_t * ) ( prMsg + 1 ) + curValueLen,
                 doSetNameLen );
        curValueLen += doSetNameLen;
        doSetName[doSetNameLen] = '\0';

        if ( !capi_write_table_parameter ( ID_DOUT, doId,  TAB_PARA_DOUT_NAME, 
                                           doSetName ) )
        {
            config_act_refresh_apps(ID_DOUT, APP_WRITE);
            actResult = ACT_RESULT_SUCCEED;
        }
    }
    else if ( doSetType == DOUT_SET_BOTH )
    {
        doSetStatus = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;

        if ( doSetStatus == DOUT_SET_STATUS_ON )
        {
            sprintf ( doSetStatusStr, "on" );
        }
        else
        {
            sprintf ( doSetStatusStr, "off" );
        }

        doSetNameLen = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;

        memcpy ( doSetName, ( uint8_t * ) ( prMsg + 1 ) + curValueLen,
                 doSetNameLen );
        curValueLen += doSetNameLen;
        doSetName[doSetNameLen] = '\0';

        if ( !capi_write_table_parameter ( ID_DOUT, doId, TAB_PARA_DOUT_NAME, doSetName ) 
            && TCMRET_SUCCESS == actDout_DOControl(doId, doSetStatusStr))
        {
            actResult = ACT_RESULT_SUCCEED;
        }
    }

    ehEncodeSimpleReply(( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen, 
        &actResult, 1, prValue, prValueLen);
    return (actResult == ACT_RESULT_SUCCEED)?(TCMRET_SUCCESS):(TCMRET_FAILURE);
}



tcmRet handleIDPToMFCBDoControlCmd ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBDoControlCmd ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_DO_CONTROL failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice( "Process CMD_IDP_TO_MFCB_DO_CONTROL OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, 
                              ACK_IDP_TO_MFCB_DO_CONTROL, valueLen, value );
}

tcmRet processIDPToMFCBQueryDoCmd ( IN homeMsgCtrlBlock* prMsg,
                                    OUT uint8_t* prValue,
                                    OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL )
    {
        return TCMRET_FAILURE;
    }

    int i = 0, iCurrNum = 0, nameLen = 0, j = 0;
    char value[MAX_VALUE_LEN + 1];
    uint8_t* tmp = NULL;
    
    char   app_result[TAB_PARA_DOUT_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_DOUT_END-1];

    for ( i = 0; i < TAB_PARA_DOUT_END - 1; i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }


    if ( capi_read_parameter(ID_SECURITYMODE, PARA_SECURITYMODE_CURRENT_DONUM, value ) )
    {
        tcmLog_error("Can not get the current DO number");
        return TCMRET_CONFLIB_READ_ERROR;
    }

    iCurrNum = atoi(value);

    uint16_t curValueLen = 0;

    /* Record the place to fill the total number later */
    tmp = prValue + curValueLen;
    curValueLen += 1;

    for ( i = 1; i < ( iCurrNum + 1 ) ; i++ )
    {
        if( capi_read_table_row ( ID_DOUT, i, p_result ) )
        {
            continue;
        }

        * ( prValue + curValueLen ) = i;
        curValueLen += 1;

        nameLen = strlen ( p_result[TAB_PARA_DOUT_NAME-1] );
        * ( prValue + curValueLen ) = nameLen;
        curValueLen += 1;

        memcpy ( prValue + curValueLen, p_result[TAB_PARA_DOUT_NAME-1], nameLen );
        curValueLen += nameLen;

        if ( strcasecmp ( p_result[TAB_PARA_DOUT_STATUE-1], "on" ) == 0 )
        {
            * ( prValue + curValueLen ) = DOUT_SET_STATUS_ON;
        }
        else
        {
            * ( prValue + curValueLen ) = DOUT_SET_STATUS_OFF;
        }
        curValueLen += 1;

        j++;
    }

    /* Fill the total number */
    *tmp = j;

    *prValueLen = curValueLen;

    return TCMRET_SUCCESS;
}

tcmRet handleIDPToMFCBQueryDoCmd ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryDoCmd ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_DO failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_DO OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, 
                              ACK_IDP_TO_MFCB_QUERY_DO, valueLen, value );
}




tcmRet processIDPToMFCBQueryCamCmd ( IN homeMsgCtrlBlock* prMsg,
                                     OUT uint8_t* prValue,
                                     OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint16_t curValueLen = 0;

    uint8_t ipCamNum = 0, camNameLen, camIPLen;
    uint8_t* tmpIpNum = NULL;
    int i;

    char   app_result[TAB_PARA_IPCAMERA_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_IPCAMERA_END-1];
    for ( i = 0; i < TAB_PARA_IPCAMERA_END - 1; i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }

    *prValue = PPI_CAM_TYPE_IP;
    curValueLen += 1;
    tmpIpNum =  prValue + curValueLen;
    curValueLen += 1;

    for ( i = 0; i < MAXTABLE_IPCAMERA; i++ )
    {
        if( capi_read_table_row ( ID_IPCAMERA, i + 1, p_result ) )
        {
            continue;
        }
        else
        {
            * ( prValue + curValueLen ) = i+1;
            curValueLen += 1;

            camNameLen = ( uint8_t ) strlen ( p_result[TAB_PARA_IPCAMERA_NAME-1] );
            * ( prValue + curValueLen ) = camNameLen;
            curValueLen += 1;

            memcpy ( prValue + curValueLen, p_result[TAB_PARA_IPCAMERA_NAME-1],
                     camNameLen );
            curValueLen += camNameLen;

            camIPLen = ( uint8_t ) strlen ( p_result[TAB_PARA_IPCAMERA_IP-1] );
            * ( prValue + curValueLen ) = camIPLen;
            curValueLen += 1;

            memcpy ( prValue + curValueLen, p_result[TAB_PARA_IPCAMERA_IP-1],
                     camIPLen );
            curValueLen += camIPLen;
        
            ipCamNum++;
        }
    }

    *tmpIpNum = ipCamNum;

    /*Add other type camera*/
    * ( prValue + curValueLen ) = PPI_CAM_TYPE_DVR;
    curValueLen += 1;

    * ( prValue + curValueLen ) = 0;
    curValueLen += 1;

    * ( prValue + curValueLen ) = PPI_CAM_TYPE_NVR;
    curValueLen += 1;

    * ( prValue + curValueLen ) = 0;
    curValueLen += 1;

    *prValueLen = curValueLen;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBQueryCamCmd ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS != processIDPToMFCBQueryCamCmd ( hdMsg, value, 
                                                          &valueLen ) )
    {
        tcmLog_error( "Process CMD_IDP_TO_MFCB_QUERY_CAM failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_CAM OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_QUERY_CAM, 
                              valueLen, value);
}

tcmRet processIDPToMFCBAddCamCmd ( IN homeMsgCtrlBlock* prMsg,
                                   OUT uint8_t* prValue,
                                   OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t* msgValue = ( uint8_t * ) ( prMsg + 1 );
    uint8_t actResult = ACT_RESULT_SUCCEED;
    uint16_t curValueLen = 0;
    uint8_t addCamType;
    uint8_t addCamNameLen, addCamIPLen;
    uint8_t addCamNameStr[MAX_VALUE_LEN+1], addCamIPStr[MAX_VALUE_LEN+1];
    memset ( addCamNameStr, 0, sizeof(addCamNameStr) );
    memset ( addCamIPStr, 0, sizeof(addCamIPStr) );

    addCamType = * ( msgValue + curValueLen );
    curValueLen += 1;

    addCamNameLen = * ( msgValue + curValueLen );
    curValueLen += 1;

    if ( addCamNameLen != 0 )
    {
        memcpy ( addCamNameStr, msgValue + curValueLen, addCamNameLen );
        addCamNameStr[addCamNameLen] = '\0';
    }
    curValueLen += addCamNameLen;

    addCamIPLen = * ( msgValue + curValueLen );
    curValueLen += 1;

    if ( addCamIPLen != 0 )
    {
        memcpy ( addCamIPStr, msgValue + curValueLen, addCamIPLen );
        addCamIPStr[addCamIPLen] = '\0';
    }
    curValueLen += addCamIPLen;
    
    /* Refresh act */
    char* pTmp[TAB_PARA_IPCAMERA_END -1];
    int indexNew = 0;

    pTmp[TAB_PARA_IPCAMERA_IP-1] = addCamIPStr;
    pTmp[TAB_PARA_IPCAMERA_PORT-1] = "80";
    pTmp[TAB_PARA_IPCAMERA_NAME-1] = addCamNameStr;
    pTmp[TAB_PARA_IPCAMERA_REDIRECTPORT-1] = "10000";
    pTmp[TAB_PARA_IPCAMERA_AUTO-1] = "false";

    if ( config_act_advCheck_tables(ID_IPCAMERA, pTmp) != TCMRET_SUCCESS )
    {
        tcmLog_error("Can not pass the advcheck of the IP camera");
        actResult = ACT_RESULT_FAILED;
        goto FINISH;
    }
    
    if ( capi_add_table_row ( ID_IPCAMERA, 0, pTmp, &indexNew ) )
    {
        tcmLog_error ( "Can not add ipcamera table" );
        actResult = ACT_RESULT_FAILED;
        goto FINISH;
    }

    if ( config_act_refresh_tables ( ID_IPCAMERA, RULE_ADD, 0, pTmp ) 
         != TCMRET_SUCCESS )
    {
        tcmLog_notice ( "Error to ipcamera config_act_refresh_tables" );
        actResult = ACT_RESULT_FAILED;
        goto FINISH;
    }

    config_act_refresh_apps ( ID_IPCAMERA, APP_WRITE );
    
FINISH:
    curValueLen = 0;
    * ( prValue ) =  * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    * ( prValue + curValueLen ) = ( uint8_t ) indexNew;
    curValueLen += 1;

    memcpy ( prValue + curValueLen, ( uint8_t * ) ( prMsg + 1 ) + 1,
             prMsg->msg.valueLen - 1 );
    curValueLen += prMsg->msg.valueLen - 1;

    * ( prValue + curValueLen ) = actResult;
    curValueLen += 1;

    *prValueLen = curValueLen;

    return TCMRET_SUCCESS ;

}


tcmRet handleIDPToMFCBAddCamCmd ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBAddCamCmd ( hdMsg, value, &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_ADD_CAM failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_ADD_CAM OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_ADD_CAM, 
                              valueLen, value);
}

tcmRet processIDPToMFCBEditSingleCam ( IN homeMsgCtrlBlock* prMsg,
                                       OUT uint8_t* prValue,
                                       OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t actResult = ACT_RESULT_FAILED;
    uint8_t camType, camId, editType;
    uint16_t curValueLen = 0;
    uint8_t editCamNameLen;
    uint8_t editCamNameStr[MAX_VALUE_LEN+1];
    uint8_t editCamIpLen;
    uint8_t editCamIpStr[MAX_VALUE_LEN+1];
    UBOOL8 isSaved = FALSE;
    
    camType = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    if ( camType == PPI_CAM_TYPE_DVR || camType == PPI_CAM_TYPE_NVR )
    {
        tcmLog_notice ( "Can not handle camera type is DVR or NVR" );
        return ehEncodeSimpleReply(( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen, 
                &actResult, 1, prValue, prValueLen);
    }

    camId = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    editType = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    if ( editType == PPI_EDIT_CAM_TYPE_NAME )
    {
        editCamNameLen = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;

        memcpy ( editCamNameStr, ( uint8_t* ) ( prMsg + 1 ) + curValueLen,
                 editCamNameLen );
        curValueLen += editCamNameLen ;
        editCamNameStr[editCamNameLen] = '\0';
        
        tcmLog_notice ( "Edit camera name to %s", editCamNameStr );

        if ( capi_write_table_parameter ( ID_IPCAMERA, camId, TAB_PARA_IPCAMERA_NAME,
                                                ( char * ) editCamNameStr ) )
        {
            tcmLog_error ( "Edit camera name error" );
            return ehEncodeSimpleReply(( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen, 
                    &actResult, 1, prValue, prValueLen);
        }

        isSaved = TRUE;
    }
    else if ( editType == PPI_EDIT_CAM_TYPE_IP )
    {
        editCamIpLen = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;

        memcpy ( editCamIpStr, ( uint8_t* ) ( prMsg + 1 ) + curValueLen,
                 editCamIpLen );
        curValueLen += editCamIpLen ;
        editCamIpStr[editCamIpLen] = '\0';
        
        tcmLog_notice ( "Edit camera ip to %s and camera id %d",
                        editCamIpStr, camId );

        if ( capi_write_table_parameter ( ID_IPCAMERA, camId, TAB_PARA_IPCAMERA_IP,
                                                ( char * ) editCamIpStr ) )
        {
            tcmLog_error ( "Edit camera IP error" );
            return ehEncodeSimpleReply(( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen, 
                    &actResult, 1, prValue, prValueLen);
        }

        isSaved = TRUE;

    }
    else if ( editType == PPI_EDIT_CAM_TYPE_DELETE )
    {
        config_act_refresh_tables ( ID_IPCAMERA, RULE_DELETE, camId, NULL );
        capi_delete_table_row ( ID_IPCAMERA, camId );
        actResult = ACT_RESULT_SUCCEED;
        isSaved = TRUE;
    }

    if ( isSaved )
    {
        config_act_refresh_apps(ID_IPCAMERA, APP_WRITE);
    }

    return ehEncodeSimpleReply(( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen, 
            &actResult, 1, prValue, prValueLen);
}


tcmRet handleIDPToMFCBEditSingleCam ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBEditSingleCam ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_EDIT_SINGLE_CAM failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_EDIT_SINGLE_CAM OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_EDIT_SINGLE_CAM, 
                              valueLen, value);
}

tcmRet processIDPToMFCBQueryDoorStatusCmd ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    int doorID;
    uint8_t doorStatus;
    char doorStatusStr[MAX_VALUE_LEN + 1];

    if ( prMsg->msg.valueLen != 1 )
    {
        tcmLog_error("Value length is %d, it should be 1", prMsg->msg.valueLen );
        return TCMRET_INVALID_ARGUMENTS;
    }

    doorID = * ( ( uint8_t * ) ( prMsg + 1 ) );

    if ( doorID != DEVICE_SUBTYPE_ODP_FRONT && doorID != DEVICE_SUBTYPE_ODP_BACK )
    {
        tcmLog_error ( "Door type error" );
        return TCMRET_FAILURE;
    }

    if ( capi_read_table_parameter ( ID_DOOR, doorID, TAB_PARA_DOOR_STATUS,
                                     doorStatusStr ) )
    {
        tcmLog_error ( "Read door status error" );
        return TCMRET_FAILURE;
    }

    if ( strcasecmp ( doorStatusStr, "opened" ) == 0 )
    {
        doorStatus = PPO_OUTDOOR_STATUS_OPEN;
    }
    else
    {
        doorStatus = PPO_OUTDOOR_STATUS_CLOSED;
    }

    return ehEncodeSimpleReply(( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen, 
            &doorStatus, 1, prValue, prValueLen);
}

tcmRet handleIDPToMFCBQueryDoorStatusCmd ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryDoorStatusCmd ( hdMsg,
            value, &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_DOOR_STATUS failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_DOOR_STATUS OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_QUERY_DOOR_STATUS, 
                              valueLen, value);
}

tcmRet processIDPToMFCBSynchronizeDeviceCmd ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prValue == NULL || prValueLen == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    *prValueLen = 0;

    moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    int i, ret;

    char   app_result[TAB_PARA_UNIT_INFO_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_UNIT_INFO_END-1];
    for ( i = 0; i < TAB_PARA_UNIT_INFO_END - 1; i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }

    deviceStruct allIDPDevice[MAXTABLE_DEVICES];
    deviceStruct allODPDevice[MAXTABLE_DEVICES];
    deviceStruct allSMPDevice[MAXTABLE_DEVICES];
    int idpCount = 0, odpCount = 0, smpCount = 0;

    memset((void*)allIDPDevice, 0, sizeof(deviceStruct)*MAXTABLE_DEVICES);  
    memset((void*)allODPDevice, 0, sizeof(deviceStruct)*MAXTABLE_DEVICES);  
    memset((void*)allSMPDevice, 0, sizeof(deviceStruct)*MAXTABLE_DEVICES); 

    for ( i = 1 ; i <= MAXTABLE_DEVICES; i++ )
    {
        ret = capi_read_table_row ( ID_UNIT_INFO, i, p_result );

        if ( ret == -1 ||
                strcasecmp ( p_result[TAB_PARA_UNIT_STATE-1],
                             DEVICE_STATUS_INITIAL_STRING ) == 0 ||
                strcasecmp ( p_result[TAB_PARA_UNIT_STATE-1],
                             DEVICE_STATUS_CONFIGURED_STRING ) == 0 ||
                strcasecmp ( p_result[TAB_PARA_UNIT_STATE-1],
                             DEVICE_STATUS_PAIRING_STRING ) == 0 )
        {
            continue;
        }

        if ( strcasecmp ( p_result[TAB_PARA_UNIT_TYPE-1], "idp" ) == 0 )
        {
            allIDPDevice[idpCount].devID = i;
            strcpy ( allIDPDevice[idpCount].devName, p_result[TAB_PARA_UNIT_NAME-1] );
            strcpy ( allIDPDevice[idpCount].devNumber, p_result[TAB_PARA_UNIT_ACCOUNT-1] );
            /* As IDP and SMP are 0,1,2, I need to match with them */
            allIDPDevice[idpCount].devSubType = actDevice_subType_getIDByString
                                         ( p_result[TAB_PARA_UNIT_SUB_TYPE-1] ) - 2;
            
            idpCount++;
        }

        if ( strcasecmp ( p_result[TAB_PARA_UNIT_TYPE-1], "odp" ) == 0 )
        {
            allODPDevice[odpCount].devID = i;
            strcpy ( allODPDevice[odpCount].devName, p_result[TAB_PARA_UNIT_NAME-1] );
            strcpy ( allODPDevice[odpCount].devNumber, p_result[TAB_PARA_UNIT_ACCOUNT-1] );
            allODPDevice[odpCount].devSubType = actDevice_subType_getIDByString
                                         ( p_result[TAB_PARA_UNIT_SUB_TYPE-1] );
            odpCount++;
        }

        if ( strcasecmp ( p_result[TAB_PARA_UNIT_TYPE-1], "smp" ) == 0 )
        {
            allSMPDevice[smpCount].devID = i;
            strcpy ( allSMPDevice[smpCount].devName, p_result[TAB_PARA_UNIT_NAME-1] );
            strcpy ( allSMPDevice[smpCount].devNumber, p_result[TAB_PARA_UNIT_ACCOUNT-1] );
            /* As IDP and SMP are 0,1,2, I need to match with them */
            allSMPDevice[smpCount].devSubType = actDevice_subType_getIDByString
                                         ( p_result[TAB_PARA_UNIT_SUB_TYPE-1] ) - 2;
            
            smpCount++;
        }
    }

    ehEncodeOneTypeDeviceInfo(allIDPDevice, idpCount, DEVICE_IDP, prValue, prValueLen);
    ehEncodeOneTypeDeviceInfo(allODPDevice, odpCount, DEVICE_ODP, prValue, prValueLen);
    ehEncodeOneTypeDeviceInfo(allSMPDevice, smpCount, DEVICE_SMP, prValue, prValueLen);

    moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBSynchronizeDeviceCmd ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBSynchronizeDeviceCmd ( hdMsg,
            value, &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_SYNCHRONIZE_DEVICE failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_SYNCHRONIZE_DEVICE OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_SYNCHRONIZE_DEVICE, 
                              valueLen, value);
}

tcmRet processIDPToMFCBOpenDoor ( IN homeMsgCtrlBlock* prMsg,
                                  OUT uint8_t* prValue,
                                  OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL )
    {
        return TCMRET_FAILURE;
    }

    pthread_mutex_lock ( &gehomeTransMsgMapQueueMutex );
    if ( TCMRET_SUCCESS != installTransMsgMap ( ACK_MFCB_TO_ODP_OPEN_DOOR,
            ACK_IDP_TO_MFCB_OPEN_DOOR, prMsg->connfd, prMsg->ssl,
            sendOpenDoorAckToIdp ) )
    {
        tcmLog_error ( "Install transmsgMap error" );
        pthread_mutex_unlock ( &gehomeTransMsgMapQueueMutex );
        return TCMRET_FAILURE;
    }
    pthread_mutex_unlock ( &gehomeTransMsgMapQueueMutex );

    uint8_t curValueLen = 0 ;

    *prValue = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;
    *prValueLen = curValueLen;

    if ( ( *prValue != DEVICE_SUBTYPE_ODP_FRONT ) && ( *prValue != DEVICE_SUBTYPE_ODP_BACK ) )
    {
        tcmLog_error( "Outdoor type %d is wrong", *prValue );
        return TCMRET_FAILURE;
    }
    else
    {
        return TCMRET_SUCCESS;
    }
}


tcmRet handleIDPToMFCBOpenDoor ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;
    uint8_t outdoorType;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBOpenDoor ( hdMsg, value, &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_OPEN_DOOR failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_OPEN_DOOR OK" );
    }

    outdoorType = *value;
    
    moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);

    DeviceInfo* deviceTmp = getOutdoorDeviceInfoByType ( outdoorType );

    if ( deviceTmp == NULL )
    {
        tcmLog_error ( "Cannot find outdoor type %d",  outdoorType );
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        return TCMRET_FAILURE;
    }

    char odpValue[2];
    uint16_t keepTimeInt;
    memset ( odpValue, 0, sizeof ( odpValue ) );
    keepTimeInt = getOutdoorKeepTimeBySocket ( FALSE, deviceTmp->connfd );
    memcpy ( odpValue, & ( keepTimeInt ), sizeof(odpValue) );

    ret = ehSendMsgToDevice( deviceTmp->connfd, deviceTmp->ssl, 
                             CMD_MFCB_TO_ODP_OPEN_DOOR, 
                             sizeof(odpValue), odpValue);
    
    tcmMem_free ( deviceTmp );
    moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    return ret;
}


tcmRet processMFCBToODPOpenDoorAck ( IN homeMsgCtrlBlock* prMsg,
                                     OUT uint8_t* prValue,
                                     OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL )
    {
        return TCMRET_FAILURE;
    }

    moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);

    uint8_t openDoorStatusTmp;
    uint8_t openDoorReasonTmp;
    int doorRowIndex;
    char doorStatusStr[10];
    DeviceInfo* deviceTmp = getDeviceInfoByFd ( prMsg->connfd );

    if ( deviceTmp == NULL )
    {
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        return TCMRET_FAILURE;
    }

    uint8_t curValueLen = 0 ;
    
    openDoorStatusTmp = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    openDoorReasonTmp = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    *prValueLen = 0;
    doorRowIndex  = ( int ) deviceTmp->subType;
    * ( prValue + *prValueLen ) = deviceTmp->subType;
    *prValueLen += 1;

    if ( openDoorStatusTmp == PPO_OUTDOOR_STATUS_OPEN )
    {
        * ( prValue + *prValueLen ) = PPO_OUTDOOR_STATUS_OPEN;
        *prValueLen += 1;
        strcpy ( doorStatusStr, "opened" );
    }
    else
    {
        * ( prValue + *prValueLen ) = PPO_OUTDOOR_STATUS_CLOSED;
        *prValueLen += 1;
        strcpy ( doorStatusStr, "closed" );
    }

    if ( capi_write_table_parameter ( ID_DOOR, doorRowIndex,TAB_PARA_DOOR_STATUS,
                                      doorStatusStr ) )
    {
        tcmLog_error ( "Update door status in db error" );
        tcmMem_free( deviceTmp );
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        return TCMRET_FAILURE;
    }

    capi_write_to_flash();
    tcmMem_free( deviceTmp );
    moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    return TCMRET_SUCCESS;
}


tcmRet handleMFCBToODPOpenDoorAck ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[300];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;

    if ( TCMRET_SUCCESS !=  processMFCBToODPOpenDoorAck ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process ACK_MFCB_TO_ODP_OPEN_DOOR failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process ACK_MFCB_TO_ODP_OPEN_DOOR OK" );
    }

    pthread_mutex_lock ( &gehomeTransMsgMapQueueMutex );
    if ( TCMRET_SUCCESS != doTransMsgAct ( ACK_MFCB_TO_ODP_OPEN_DOOR,
                                           &valueLen, value ) )
    {
        tcmLog_error ( " doTransMsgAct ACK_MFCB_TO_ODP_OPEN_DOOR error" );
        pthread_mutex_unlock ( &gehomeTransMsgMapQueueMutex );
        return TCMRET_FAILURE;
    }
    pthread_mutex_unlock ( &gehomeTransMsgMapQueueMutex );

    return ret;
}

tcmRet processIDPToMFCBQueryDetectPoint ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    EhomeSM sm[MAXTABLE_MODETYPE];

    uint8_t curSMID;

    int i, currDPNum;

    char value[MAX_VALUE_LEN + 1];

    if ( capi_read_parameter ( ID_SECURITYMODE, PARA_SECURITYMODE_USERMODE,
                               value ) )
    {
        tcmLog_error ( "Read current security mode wrong" );
        return TCMRET_FAILURE;
    }
    else
    {
        curSMID = (uint8_t)actSecurityMode_getModeIndexByModeString(value);
    }

    if ( capi_read_parameter ( ID_SECURITYMODE, PARA_SECURITYMODE_CURRENT_DINUM,
                               value ) )
    {
        tcmLog_error ( "Read current DI number wrong" );
        return TCMRET_FAILURE;
    }
    else
    {
        currDPNum = atoi(value);
    }

    for ( i = 0; i < SECURITYINDEX_END - 1; i++ )
    {
        sm[i].smID = (uint8_t)(i + 1);
        sm[i].smRing = (uint8_t) getRingTypeIDBySM( sm[i].smID );
        ehEncodeParseDetectPoint(&(sm[i].smDP), currDPNum, 
                          actSecurityMode_getModeAppIDByModeIndex(i + 1));
    }

    /*Packet value buf*/
    * ( prValue + 0 ) = curSMID;

    * ( prValue + 1 ) = currDPNum;

    *prValueLen = 2;

    int j;

    for ( j = 0; j < currDPNum; j++ )
    {
        * ( prValue + *prValueLen ) = sm[0].smDP[j].dpID;
        *prValueLen = *prValueLen + 1;

        * ( prValue + *prValueLen ) = strlen ( sm[0].smDP[j].dpName );
        *prValueLen = *prValueLen + 1;

        uint8_t nameLen = strlen ( sm[0].smDP[j].dpName );
        memcpy ( prValue + *prValueLen, sm[0].smDP[j].dpName, ( int ) nameLen );
        *prValueLen = *prValueLen + nameLen;

        memcpy ( prValue + *prValueLen, & ( sm[0].smDP[j].dpDelayTime ), 2 );
        *prValueLen = *prValueLen + 2;
    }

    for ( i = 0; i < MAXTABLE_MODETYPE; i++ )
    {
        * ( prValue + *prValueLen ) = sm[i].smID;
        *prValueLen += 1;

        for ( j = 0; j < currDPNum; j++ )
        {
            * ( prValue + *prValueLen ) = sm[i].smDP[j].dpID;
            *prValueLen += 1;

            //* ( prValue + *prValueLen ) = sm[i].smDP[j].dpSetStatus;
            if ( sm[i].smDP[j].dpAttribute == PPI_DP_ATTRIBUTE_OPENFOREVER )
                * ( prValue + *prValueLen ) = 0x03;
            else
                * ( prValue + *prValueLen ) = sm[i].smDP[j].dpSetStatus;
            *prValueLen += 1;

            * ( prValue + *prValueLen ) = sm[i].smDP[j].dpVisualStatus;
            *prValueLen += 1;

            * ( prValue + *prValueLen ) = sm[i].smDP[j].dpAttribute;
            *prValueLen += 1;

            * ( prValue + *prValueLen ) = sm[i].smDP[j].dpDelayTimeSwitch;
            *prValueLen += 1;
        }
        
        * ( prValue + *prValueLen ) = sm[i].smRing;
        *prValueLen += 1;        
    }

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBQueryDetectPoint ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryDetectPoint ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_DETECT_POINT failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_DETECT_POINT OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_MFCB_TO_IDP_QUERY_DETECT_POINT, 
                              valueLen, value);
}


tcmRet processIDPToMFCBPair ( homeMsgCtrlBlock* prMsg, uint8_t* failReason)
{
    tcmRet ret = TCMRET_FAILURE;
    
    if ( prMsg == NULL || failReason == NULL )
    {
        tcmLog_error ( "Message can not be NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    *failReason = PPI_PAIR_REASON_CANNOT_FIND;

#ifdef SSL_CONNECT
    if ( prMsg->connfd == 0 || prMsg->ssl == NULL )
    {
        tcmLog_error ( "Socket fd and ssl can not be NULL" );
        return TCMRET_INVALID_PARAMETER;
    }
#else
    if ( prMsg->connfd == 0  )
    {
        tcmLog_error ( "Socket fd can not be NULL" );
        return TCMRET_INVALID_PARAMETER;
    }
#endif

    moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    int devIndex = getDeviceIndexByFd( prMsg->connfd );
    DeviceInfo* deviceTmp = getDeviceInfoByDeviceIndex( devIndex );
    tcmLog_debug ( "prMsg->connfd is %d", prMsg->connfd );
    showIndexDeviceArray();
    
    if ( deviceTmp == NULL )
    {
        tcmLog_notice ( "The device that maps the socket fd does not exist, "
                       "its fd is %d",  prMsg->connfd );
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        return TCMRET_INTERNAL_ERROR;
    }

    if ( prMsg->msg.valueLen == 0 )
    {
        tcmMem_free(deviceTmp);
        tcmLog_notice ( "There are no account info in message" );
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        return TCMRET_FAILURE;
    }

    char username[MAX_VALUE_LEN + 1];
    char password[MAX_VALUE_LEN + 1];
    uint8_t usernameLen;
    uint8_t passwordLen;
    uint8_t idpType = 0;
    uint16_t currLen = 0;
    
    uint8_t* msgValue = ( uint8_t * ) ( prMsg + 1 );
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));

    usernameLen = *( msgValue + currLen );
    currLen += 1;
    memcpy(username, msgValue + currLen, usernameLen);
    currLen += usernameLen;

    passwordLen = *( msgValue + currLen );
    currLen += 1;
    memcpy(password, msgValue + currLen, passwordLen);
    currLen += passwordLen;

    idpType = *( msgValue + currLen );
    currLen += 1;
    
    tcmLog_notice( "IDP type is %d", idpType );
    if( idpType < DEVICE_SUBTYPE_IDP_7_INCH - 2 || 
        idpType > DEVICE_SUBTYPE_SMP - 2 )
    {
       tcmLog_notice( "IDP sub type is wrong, set it to default 7 inch" );
       idpType = DEVICE_SUBTYPE_IDP_7_INCH - 2;
    }
    
    if ( currLen != prMsg->msg.valueLen )
    {
        tcmLog_error ( "Fail in register packet length check" );
        tcmMem_free(deviceTmp);
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        return TCMRET_INTERNAL_ERROR;
    }

    tcmLog_notice ( "Account info in DB:  %s|%s", deviceTmp->userName, deviceTmp->passWord);
    tcmLog_notice ( "Account info in MSG: %s|%s", username, password);

    if ( strcmp ( username, deviceTmp->userName ) == 0 &&
            strcmp ( password, deviceTmp->passWord ) == 0 )
    {
        tcmLog_notice ( "The pair info is correct" );

        updateStatusByDeviceIndex(DEVICE_STATUS_CONNECTED, devIndex);
        updateSubTypeByDeviceIndex(idpType + 2, devIndex);

        gen_db_file();
        UpdatePagingConfig();
        *failReason = PPI_PAIR_REASON_SUCCESS;
        ret = TCMRET_SUCCESS;
    }
    else
    {
        tcmLog_notice ( "The pair info is not correct" );
        *failReason = PPI_PAIR_REASON_INVALID_ACCOUNT_INFO;
        ret = TCMRET_FAILURE;
    }

    tcmMem_free(deviceTmp);
    moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);

    return ret;
}

tcmRet handleIDPToMFCBPair ( homeMsgCtrlBlock* hdMsg )
{
    char value[3];
    uint16_t valueLen;
    uint8_t  failReason = PPI_PAIR_REASON_SUCCESS;
    int ret = TCMRET_SUCCESS;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBPair ( hdMsg, &failReason ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_REGISTER failed" );
        value[0] = PPI_PAIR_FAILED;
        value[1] = failReason;
        valueLen = 2;
        ret = TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_REGISTER OK" );
        value[0] = PPI_PAIR_SUCCESS;
        valueLen = 1;
    }

    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_REGISTER, 
                              valueLen, value);

    sendSynTime ( NULL, NULL );
    sendSynDeviceEvt();
    return ret;
}



tcmRet processIDPToMFCBQueryDetectPointStatus ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    int i, j = 0, currNum = 0;
    char result[MAX_VALUE_LEN + 1];
    int alertFlag = 0;

    if ( capi_read_parameter(ID_SECURITYMODE, PARA_SECURITYMODE_CURRENT_DINUM, result))
    {
        tcmLog_error("Fail to get the current DI number");
        return TCMRET_CONFLIB_READ_ERROR;
    }

    currNum = atoi(result);

    for ( i = 1; i <= currNum; i++ )
    {
        if ( capi_read_table_parameter ( ID_DETACTPOINT, i,
                                         TAB_PARA_DETECTPOINT_ALARMSTATUS,
                                         result ) )
        {
            continue;
        }

        if ( strcasecmp ( result, "alert" ) == 0 )
        {
            alertFlag = 1;
            break;
        }
    }

    if ( alertFlag == 1 )
    {
        *prValue = PPI_DP_ALERT;

        for ( i = 1; i <= currNum; i++ )
        {
            if ( capi_read_table_parameter ( ID_DETACTPOINT, i,
                                             TAB_PARA_DETECTPOINT_ALARMSTATUS,
                                             result ) )
            {
                continue;
            }

            if ( strcasecmp ( result, "alert" ) == 0 )
            {
                j++;
                * ( prValue + j ) = ( uint8_t ) i;
            }
        }
    }
    else
    {
        *prValue = PPI_DP_NORMAL;
    }

    *prValueLen = ( j + 1 );

    return TCMRET_SUCCESS;
}



tcmRet handleIDPToMFCBQueryDetectPointStatus ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryDetectPointStatus ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS, 
                              valueLen, value);
}



tcmRet processIDPToMFCBTokenOperation ( IN homeMsgCtrlBlock* prMsg,
                                        OUT uint8_t* prValue,
                                        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL )
    {
        return TCMRET_FAILURE;
    }

    uint8_t TOKENOPTYPE;
    uint8_t actResult = ACT_RESULT_FAILED;
    uint8_t curValueLen = 0;

    TOKENOPTYPE = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    if ( TOKENOPTYPE == PPI_OPERATE_TOKEN_REQUEST )
    {
        tcmLog_notice ( "IDP fd %d wants to request token", prMsg->connfd );
        if ( gTokenStatus == PPI_TOKEN_STATUS_FREE )
        {
            actResult = ACT_RESULT_SUCCEED;
            gTokenStatus = PPI_TOKEN_STATUS_BUSY;
            gTokenOwern = prMsg->connfd;
            tcmLog_notice ( "IDP fd %d get token", prMsg->connfd );
        }
        else if ( gTokenStatus == PPI_TOKEN_STATUS_BUSY )
        {
            actResult = ACT_RESULT_FAILED;
            tcmLog_notice ( "IDP fd %d cannot get token", prMsg->connfd );
        }
    }
    else if ( TOKENOPTYPE == PPI_OPERATE_TOKEN_RELEASE )
    {
        tcmLog_notice ( "IDP fd %d want to release token", prMsg->connfd );
        if ( gTokenStatus == PPI_TOKEN_STATUS_FREE )
        {
            actResult = ACT_RESULT_FAILED;
            tcmLog_notice ( "Token is free, no need to release by %d", prMsg->connfd );
        }
        else if ( gTokenStatus == PPI_TOKEN_STATUS_BUSY )
        {
            if ( gTokenOwern == prMsg->connfd )
            {
                actResult = ACT_RESULT_SUCCEED;
                gTokenStatus = PPI_TOKEN_STATUS_FREE;
                gTokenOwern = 0;
                tcmLog_debug ( "IDP fd %d release token", prMsg->connfd );
            }
            else
            {
                tcmLog_debug ( "Token has been requested by socket fd %d, "
                               "you %d cannot request it again",
                               gTokenOwern, prMsg->connfd );
                actResult = ACT_RESULT_FAILED;
            }
        }
    }
    else
    {
        actResult = ACT_RESULT_FAILED;
        tcmLog_debug ( "Token : error token operation type" );
    }

    curValueLen = 0;

    * ( prValue + curValueLen ) = TOKENOPTYPE;
    curValueLen += 1;

    * ( prValue + curValueLen ) = actResult;
    curValueLen += 1;

    *prValueLen = curValueLen;

    return ( ( actResult == ACT_RESULT_SUCCEED ) ?
             TCMRET_SUCCESS :              TCMRET_FAILURE );
}


tcmRet handleIDPToMFCBTokenOperation ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    pthread_mutex_lock ( &g_homeIdpTokenMutex );
    if ( TCMRET_SUCCESS !=  processIDPToMFCBTokenOperation ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_notice ( "Request token failed" );
    }
    else
    {
        tcmLog_notice ( "Request token OK" );
    }
    pthread_mutex_unlock ( &g_homeIdpTokenMutex );

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_TOKEN_OPERATION, 
                              valueLen, value);
}


/*deal with EVT_MFCB_TO_ODP_FLASH_ENABLE_TIME*/
tcmRet sendSecurityModeToIDP ()
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen = 0;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryDetectPoint ( NULL,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_DETECT_POINT failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_DETECT_POINT OK" );
    }

    int devType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    return ehSendMsgToAllDevice(devType, EVT_MFCB_TO_IDP_UPDATE_SECURITY_MODE, valueLen, value);
}



tcmRet processIDPToMFCBSetSecurityMode ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t securityMode = * ( ( uint8_t * ) ( prMsg + 1 ) ) ;

    char smName[BUFLEN_16];

    uint8_t result = 0x01;

    tcmLog_notice ( "The new security mode is %d", securityMode );

    actSecurityMode_getModeStringByModeIndex(securityMode, smName);

    if (  -1 == capi_write_parameter ( ID_SECURITYMODE,
                                       PARA_SECURITYMODE_USERMODE,
                                       smName ) )
    {
        tcmLog_error ( "set security mode,write DB error" );
        result = 0x02;
    }

    /* Send msg to sdd, renew the delay time */
    actUtil_sendMsg(ID_SDD, FALSE, TCM_MSG_RENEW_DELAYTIME, 0, NULL, 0);

    capi_write_to_flash();

    *prValue = * ( ( uint8_t * ) ( prMsg + 1 ) );
    * ( prValue + 1 ) = result;
    *prValueLen = 2;
    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBSetSecurityMode ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_VALUE_LEN];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBSetSecurityMode ( hdMsg, value,
                                                              &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_SET_SECURITY_MODE failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_SET_SECURITY_MODE OK" );
    }

    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_SET_SECURITY_MODE, 
                              valueLen, value);

    sendSecurityModeToODP();
    sendSecurityModeToIDP();
    return ret;
}


tcmRet processIDPToMFCBQueryCurrentSecurityMode ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    char smName[MAX_VALUE_LEN + 1];

    uint8_t curSmId;

    if ( -1 == capi_read_parameter ( ID_SECURITYMODE,
                                     PARA_SECURITYMODE_USERMODE,
                                     smName ) )
    {
        tcmLog_error ( "read current security mode error" );
        return TCMRET_FAILURE;
    }

    curSmId = actSecurityMode_getModeIndexByModeString(smName);

    *prValue = curSmId;

    *prValueLen = 1;

    return TCMRET_SUCCESS;
}



tcmRet handleIDPToMFCBQueryCurrentSecurityMode ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[3];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryCurrentSecurityMode ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE, 
                              valueLen, value);
}


tcmRet processIDPToMFCBSetSingleDetectPoint ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    /*Analyze Msg and Do set act Begin*/
    uint8_t actResult;

    actResult = DP_SET_SINGLE_RESULT_SUCCEED;

    uint8_t smID, dpID, type, curValueLen;

    curValueLen = 0;

    smID = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );

    curValueLen += 1;

    dpID = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );

    curValueLen += 1;

    type = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );

    curValueLen += 1;


    if ( type == DP_ATTRIBUTE_TYPE_SETTING_STATUS )
    {
        uint8_t setStatus = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;
        tcmLog_debug ( "Change setting status to %d",  setStatus );

        if ( setStatus == PPI_DP_STATUS_ENABLE )
        {
            if ( -1 == capi_write_table_parameter (
                        ( int ) smID,
                        ( int ) dpID,
                        TAB_PARA_MODE_ACTIVE_STATUSE,
                        "enable" ) )
            {
                tcmLog_error ( "set sm dp setstatus error" );
                actResult = DP_SET_SINGLE_RESULT_FAILED;
            }
        }

        if ( setStatus == PPI_DP_STATUS_DISABLE )
        {
            if ( -1 == capi_write_table_parameter (
                        ( int ) smID,
                        ( int ) dpID,
                        TAB_PARA_MODE_ACTIVE_STATUSE,
                        "disable" ) )
            {
                tcmLog_error ( "set sm dp setstatus error" );
                actResult = DP_SET_SINGLE_RESULT_FAILED;
            }
        }

    }
    else if ( type == DP_ATTRIBUTE_TYPE_DP_NAME )
    {
        tcmLog_debug ( "Change DP name" );
        uint8_t dpNameLen;
        uint8_t dpName[MAXPARA_DPNAMELEN];
        dpNameLen = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen ) ;
        curValueLen += 1;
        tcmLog_debug ( "DP Name length is %d", dpNameLen );

        memcpy ( dpName, ( uint8_t * ) ( prMsg + 1 ) + curValueLen, dpNameLen );
        curValueLen += dpNameLen;
        dpName[dpNameLen] = '\0';
        tcmLog_debug ( "DP new Name is %s", dpName );

        if ( -1 == capi_write_table_parameter ( ID_DETACTPOINT,
                                                ( int ) dpID,
                                                TAB_PARA_DETECTPOINT_NAME,
                                                dpName ) )
        {
            tcmLog_error ( "set dp name error" );
            actResult = DP_SET_SINGLE_RESULT_FAILED;
        }
    }
    else if ( type == DP_ATTRIBUTE_TYPE_DP_ATTRIBUTE )
    {
        uint8_t dpAttribute = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;
        tcmLog_debug ( "Change DP attribute to %d", dpAttribute );

        if ( dpAttribute == PPI_DP_ATTRIBUTE_OPENFOREVER )
        {
            if ( -1 == capi_write_table_parameter ( ( int ) smID,
                                                    ( int ) dpID,
                                                    TAB_PARA_MODE_ATTRIBUTE,
                                                    "enable" ) )
            {
                tcmLog_error ( "set sm dp setstatus error" );
                actResult = DP_SET_SINGLE_RESULT_FAILED;
            }
        }

        if ( dpAttribute == PPI_DP_ATTRIBUTE_USER_SET )
        {
            if ( -1 == capi_write_table_parameter ( ( int ) smID,
                                                    ( int ) dpID,
                                                    TAB_PARA_MODE_ATTRIBUTE,
                                                    "disable" ) )
            {
                tcmLog_error ( "set sm dp setstatus error" );
                actResult = DP_SET_SINGLE_RESULT_FAILED;
            }
        }
    }
    else if ( type == DP_ATTRIBUTE_TYPE_DELAY_TIME )
    {
        uint16_t delayTime, delayTimeTmp;
        memcpy ( &delayTimeTmp, ( uint8_t * ) ( prMsg + 1 ) + curValueLen, 2 );
        delayTime = ntohs ( delayTimeTmp );
        tcmLog_debug ( "Change DP delay time to %d", delayTime );
        char delayTimeStr[MAXPARA_DPNAMELEN];
        sprintf ( delayTimeStr, "%d",  delayTime );

        if ( -1 == capi_write_parameter ( ID_SECURITYMODE,
                                          PARA_SECURITYMODE_DELAYTIME,
                                          delayTimeStr ) )
        {
            tcmLog_error ( "set sm delay time error" );
            actResult = DP_SET_SINGLE_RESULT_FAILED;
        }
    }
    else
    {
        tcmLog_error ( "Change DP type error" );
    }

    /*Analyze Msg and Do set act End*/
    capi_write_to_flash();
    /*Packet value Buf Begin*/
    memcpy ( prValue, ( uint8_t * ) ( prMsg + 1 ), prMsg->msg.valueLen );

    * ( prValue + prMsg->msg.valueLen ) = actResult;

    *prValueLen = prMsg->msg.valueLen + 1;

    /*Packet value Buf End*/
    if ( actResult == DP_SET_SINGLE_RESULT_SUCCEED )
    {
        return TCMRET_SUCCESS;
    }
    else
    {
        return TCMRET_FAILURE;
    }
}


tcmRet handleIDPToMFCBSetSingleDetectPoint ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[300];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBSetSingleDetectPoint ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT, 
                              valueLen, value);
}

tcmRet processIDPToMFCBUpdateOneSecurityMode ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    /*Analyze Msg Packet and Do update act Begin*/
    uint8_t actResult;

    actResult = 0x01;

    EhomeSM sm;

    uint16_t msgCurValueLen = 0;

    sm.smID = * ( ( uint8_t * ) ( prMsg + 1 ) );

    msgCurValueLen += 1;

    int dpIndex = 0;

    while ( msgCurValueLen + 8 < prMsg->msg.valueLen )
    {
        tcmLog_debug ( "Receive the %dth DP info\n", dpIndex );
        sm.smDP[dpIndex].dpID = * ( ( uint8_t * ) ( prMsg + 1 )
                                    + msgCurValueLen );
        msgCurValueLen += 1;
        tcmLog_debug ( "DP id %d\n", sm.smDP[dpIndex].dpID );

        int dpNameLen = * ( ( uint8_t * ) ( prMsg + 1 )
                            + msgCurValueLen );
        msgCurValueLen += 1;
        tcmLog_debug ( "DP name length %d\n", dpNameLen );

        memcpy ( sm.smDP[dpIndex].dpName, ( uint8_t * ) ( prMsg + 1 )
                 + msgCurValueLen, dpNameLen );
        msgCurValueLen += dpNameLen;
        sm.smDP[dpIndex].dpName[dpNameLen] = '\0';
        tcmLog_debug ( "DP name is %s \n", sm.smDP[dpIndex].dpName );

        memcpy ( & ( sm.smDP[dpIndex].dpDelayTime ),
                 ( uint8_t * ) ( prMsg + 1 ) + msgCurValueLen, 2 );
        msgCurValueLen += 2;
        tcmLog_debug ( "DP delay time is %d\n", sm.smDP[dpIndex].dpDelayTime );

        sm.smDP[dpIndex].dpSetStatus = * ( ( uint8_t * ) ( prMsg + 1 ) +
                                           msgCurValueLen );
        msgCurValueLen += 1;
        tcmLog_debug ( "DP set status is %d\n", sm.smDP[dpIndex].dpSetStatus );

        sm.smDP[dpIndex].dpVisualStatus = * ( ( uint8_t * ) ( prMsg + 1 ) +
                                              msgCurValueLen );
        msgCurValueLen += 1;
        tcmLog_debug ( "DP visual status is %d\n",
                       sm.smDP[dpIndex].dpVisualStatus );

        sm.smDP[dpIndex].dpAttribute = * ( ( uint8_t * ) ( prMsg + 1 ) +
                                           msgCurValueLen );
        msgCurValueLen += 1;
        tcmLog_debug ( "DP attribute is %d\n", sm.smDP[dpIndex].dpAttribute );

        sm.smDP[dpIndex].dpDelayTimeSwitch = * ( ( uint8_t * ) ( prMsg + 1 ) +
                                             msgCurValueLen );
        msgCurValueLen += 1;
        tcmLog_debug ( "DP delaytimeswitch is %d\n",
                       sm.smDP[dpIndex].dpDelayTimeSwitch );

        dpIndex += 1;
    }
    
    sm.smRing = * ( ( uint8_t * ) ( prMsg + 1 ) + msgCurValueLen );
    tcmLog_debug( "Get ringmode number is %d", sm.smRing );

    msgCurValueLen += 1;
    /*Now we get this sm,Then will save it into db*/
    int appIndex = actSecurityMode_getModeAppIDByModeIndex(sm.smID);

    int i;

    char value[MAX_VALUE_LEN + 1];
    int currDPNum = 0;
    if ( capi_read_parameter ( ID_SECURITYMODE, PARA_SECURITYMODE_CURRENT_DINUM,
                               value ) )
    {
        tcmLog_debug ( "Read current DI number error" );
        return TCMRET_FAILURE;
    }
    else
    {
        currDPNum = atoi(value);
    }


    for ( i = 1; i < currDPNum + 1; i++ )
    {
        /*set dp name*/
        if ( -1 == capi_write_table_parameter ( ID_DETACTPOINT, i,
                                                TAB_PARA_DETECTPOINT_NAME,
                                                sm.smDP[i-1].dpName ) )
        {
            tcmLog_error ( "update one sm set dp name error" );
            continue;
        }


        /*set delay time*/
        char delayTimeStr[MAX_VALUE_LEN + 1];

        sprintf ( delayTimeStr, "%d", sm.smDP[i-1].dpDelayTime );

        if ( -1 == capi_write_parameter ( ID_SECURITYMODE,
                                          PARA_SECURITYMODE_DELAYTIME,
                                          delayTimeStr ) )
        {
            tcmLog_error ( "update one sm set delay time error" );
            continue;
        }

        //set settingstatus
        char settingStatusStr[MAX_VALUE_LEN + 1];

        if ( sm.smDP[i-1].dpSetStatus == PPI_DP_STATUS_ENABLE )
        {
            strcpy ( settingStatusStr, "enable" );
        }
        else
        {
            strcpy ( settingStatusStr, "disable" );
        }

        if ( -1 == capi_write_table_parameter ( appIndex, i,
                                                TAB_PARA_MODE_ACTIVE_STATUSE,
                                                settingStatusStr ) )
        {
            tcmLog_error ( "update one sm set setting status error" );
            continue;
        }

        /*set visualstatus*/

        char visualStatus[MAX_VALUE_LEN + 1];

        if ( sm.smDP[i-1].dpVisualStatus == PPI_DP_NORMAL )
        {
            strcpy ( visualStatus, "enable" );
        }
        else
        {
            strcpy ( visualStatus, "alert" );
        }

        if ( -1 == capi_write_table_parameter (
                    ID_DETACTPOINT, i,
                    TAB_PARA_DETECTPOINT_ALARMSTATUS,
                    visualStatus ) )
        {
            tcmLog_error ( "upadte one sm set visual status" );
            continue;
        }

        /*set attribute*/
        char attributeStr[MAX_VALUE_LEN + 1];

        if ( sm.smDP[i-1].dpAttribute == PPI_DP_ATTRIBUTE_OPENFOREVER )
        {
            tcmLog_debug ( "Set attribute to enable for the %d dp", i );
            strcpy ( attributeStr, "enable" );
        }
        else
        {
            tcmLog_debug ( "Set attribute to disable for the %d dp", i );
            strcpy ( attributeStr, "disable" );
        }

        if ( -1 == capi_write_table_parameter ( appIndex, i,
                                                TAB_PARA_MODE_ATTRIBUTE,
                                                attributeStr ) )
        {
            tcmLog_error ( "update one sm set attribute error" );
            continue;
        }

        /*set delaytimeswitch*/
        char delayTimeSwitchStr[MAX_VALUE_LEN + 1];

        if ( sm.smDP[i-1].dpDelayTimeSwitch == PPI_DP_DELAYTIME_ENABLE )
        {
            tcmLog_debug ( "Set delayTimeSwitch to enable for the %d dp", i );
            strcpy ( delayTimeSwitchStr, "enable" );
        }
        else
        {
            tcmLog_debug ( "Set delayTimeSwitch to disable for the %d dp", i );
            strcpy ( delayTimeSwitchStr, "disable" );
        }

        if ( -1 == capi_write_table_parameter ( appIndex, i,
                                                TAB_PARA_MODE_ACTIVE_DELAY,
                                                delayTimeSwitchStr ) )
        {
            tcmLog_error ( "update one sm set delayTimeSwitch error" );
            continue;
        }
    }

    char ringTypeStr[MAX_VALUE_LEN+1];
    if( -1 == convertRingTypeIDToStr( sm.smRing, ringTypeStr ) )
    {
       tcmLog_error( "ring type %d is wrong", sm.smRing );
       return TCMRET_FAILURE;
    }

    tcmLog_debug( "ringmode str is %s", ringTypeStr );
    if( -1 == capi_write_parameter( appIndex, PARA_EACHMODE_RINGMODE, ringTypeStr) )
    {
       tcmLog_error( "Update SM %d ringtype %s wrong ", appIndex, ringTypeStr );
       return TCMRET_FAILURE;
    }
    
    capi_write_to_flash();

    /*Analyze Msg Packet and Do update act End*/

    *prValue = sm.smID;
    * ( prValue + 1 ) =  actResult;
    *prValueLen = 2;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBUpdateOneSecurityMode ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;
    if ( TCMRET_SUCCESS !=  processIDPToMFCBUpdateOneSecurityMode ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE "
                       "failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE OK" );
    }
    
    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE, 
                              valueLen, value);

    sendSecurityModeToODP();
    sendSecurityModeToIDP();
    
    return ret;
}


/*MFCB how to clear alarm*/
tcmRet processIdpToMfcbClearAlarm ( IN homeMsgCtrlBlock* prMsg,
                                    OUT uint8_t* prValue,
                                    OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    /*Analyze Msg Packet and Do clear alarm act Begin*/
    uint8_t actResult;

    actResult = 0x01;

    uint8_t smID, dpID;

    if ( prMsg->msg.valueLen == 0x02 )
    {
        smID = * ( ( uint8_t * ) ( prMsg + 1 ) );
        dpID = * ( ( uint8_t * ) ( prMsg + 1 ) + 1 );
    }
    else
    {
        tcmLog_notice ( "MSG Packet is invalid" );
        return TCMRET_INTERNAL_ERROR;
    }

    /*Analyze Msg Packet and Do update act End*/

    *prValue = smID;

    * ( prValue + 1 ) =  dpID;

    * ( prValue + 2 ) =  actResult;

    *prValueLen = 3;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBClearAlarm ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIdpToMfcbClearAlarm ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_CLEAR_ALARM failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_CLEAR_ALARM OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_CLEAR_ALARM, 
                              valueLen, value);
}

tcmRet processODPToMFCBReportCardInfoCmd ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prCardID,
        OUT uint16_t* prCardIDLen)
{
    if ( prMsg == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    int curValueLen = 0;
    uint8_t cardIDLen;

    cardIDLen    =   * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    char reportCardID[MAXPARA_CARDIDLEN + 1];
    memset ( reportCardID , 0 , sizeof(reportCardID) );
    int cdK = 0;
    while ( cdK < cardIDLen )
    {
        tcmLog_debug ( "cdK %d = %d ",  cdK,
                       * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen + cdK ) );
        sprintf ( reportCardID + 2*cdK, "%0*x", 2,
                  * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen + cdK ) );
        cdK++;
    }

    tcmLog_debug ( "Now new card ID TMP is %s length is %d", reportCardID,
                   strlen ( reportCardID ) );

    curValueLen += cardIDLen;

    memcpy(prCardID, reportCardID, strlen(reportCardID));
    *prCardIDLen = strlen(reportCardID);
    return TCMRET_SUCCESS;
}

uint16_t getOutdoorKeepTime ( int outdoorType )
{
    char keepTimeStrTmp[MAX_VALUE_LEN];

    if ( 0 == capi_read_table_parameter ( ID_DOOR, outdoorType,
                                          TAB_PARA_KEEP_OPEN_TIME,
                                          keepTimeStrTmp ) )
    {
        return atoi ( keepTimeStrTmp ) ;
    }

    return -1;
}

uint16_t getOutdoorKeepTimeBySocket ( UBOOL8 needLock, int ktsocket )
{
    if ( needLock )
    {
        moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    }
    
    DeviceInfo* ODPDeviceTmp = getDeviceInfoByFd ( ktsocket );
    uint16_t keepTimeInt, keepTimeIntTmp;
    if ( ODPDeviceTmp == NULL )
    {
        tcmLog_notice ( "Cannot find the socket fd %d map device", ktsocket );
        if ( needLock )
        {
            moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        }
        return -1;
    }
    keepTimeIntTmp = getOutdoorKeepTime ( ODPDeviceTmp->subType );
    keepTimeInt = htons ( keepTimeIntTmp );
    if ( needLock )
    {
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    }
    return keepTimeInt;
}

/* donot know whether it is new card or not */
tcmRet handleODPToMFCBReportCardInfoCmd ( homeMsgCtrlBlock* hdMsg )
{
    int ret = TCMRET_SUCCESS;
    uint8_t value[MAXPARA_CARDIDLEN + 1], outValue[BUFLEN_16];
    char saveCardID[MAX_VALUE_LEN + 1], tmp[MAX_VALUE_LEN + 1], regStatus[MAX_VALUE_LEN + 1];
    int valueLen = 0, cardExistIndex = 0, msgAck = 0, outValueLen = 0, newIndex = 0;
    UBOOL8 needSwitchMode = FALSE;

    memset ( value, 0, sizeof(value) );

    if ( TCMRET_SUCCESS !=  processODPToMFCBReportCardInfoCmd ( hdMsg, value,
                &valueLen) )
    {
        tcmLog_error ( "Process CMD_ODP_TO_MFCB_REPORT_CARD failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_ODP_TO_MFCB_REPORT_CARD OK" );
    }

    int i = 0;
    for ( i = 0; i < MAXTABLE_CARD; i++ )
    {
        if ( !capi_read_table_parameter ( ID_CARD, i + 1, TAB_PARA_CARD_ID,
                                          saveCardID ) )
        {
            if ( strcasecmp ( saveCardID, value ) == 0 )
            {
                cardExistIndex = i + 1;
                break;
            }
        }
    }

    if ( capi_read_parameter(ID_CARD, PARA_CARD_REGISTER_STATUS, regStatus) )
    {
        return TCMRET_CONFLIB_READ_ERROR;
    }

    /*normal status for not adding card*/
    if ( strcasecmp(regStatus, "end") == 0 )
    {
        tcmLog_debug ( "Card Opearaiotn Now is normal status" );
        if ( cardExistIndex > 0 )
        {
            capi_read_table_parameter(ID_CARD, cardExistIndex, TAB_PARA_CARD_STATUS, tmp);
            if ( 0 == strcasecmp(tmp, STRING_CARD_STATUS_LOST) )
            {
                // TODO: record the alarm history for lost card

                /* Tell outdoor the card is not exit */
                msgAck = PPO_CARD_NOEXIST;
            }
            else
            {
                capi_read_table_parameter(ID_CARD, cardExistIndex, TAB_PARA_CARD_TYPE, tmp);
                
                /* Only the security card, switch the mode */
                if ( 0 == strcasecmp(tmp, STRING_CARD_TYPE_SECURITY ) )
                {
                    msgAck = PPO_CARD_SECURITY;
                    needSwitchMode = TRUE;
                }
                /* switch the mode and open door */
                else if ( 0 == strcasecmp(tmp, STRING_CARD_TYPE_ADMIN ) )
                {
                    msgAck = PPO_CARD_OPENDOOR;
                    needSwitchMode = TRUE;
                }
                /* Only the normal card, open the door */
                else
                {
                    msgAck = PPO_CARD_OPENDOOR;
                }
            }
        }
        else
        {
            /* Card does not exist */
            msgAck = PPO_CARD_NOEXIST;
        }
    }
    else if ( strcasecmp(regStatus, "start") == 0 )
    {
        /* Check timeout first */
        if ( 1 == isTimeout ( g_cardStartAddTime, gcardTimeout ) )
        {
            tcmLog_debug ( "Timeout for add card, enter the normal status" );
            capi_write_parameter(ID_CARD, PARA_CARD_REGISTER_STATUS, "end");
            return TCMRET_INTERNAL_ERROR;
        }

        /* If the new card exists in DB, return nothing */
        if ( cardExistIndex > 0 )
        {
            return TCMRET_FAILURE;
        }
        else
        {
            ret = actCard_addDefaultRule(value, &newIndex);

            if ( ret != TCMRET_SUCCESS )
            {
                return ret;
            }

            config_act_refresh_apps(ID_CARD, APP_WRITE);
            
            tcmLog_debug ( "Card Opearation is accepting new card" );
            msgAck = PPO_CARD_EDIT;
        }
    
    }
    else
    {
        tcmLog_error ( "Card Operation Invalid Flags" );
        return TCMRET_INTERNAL_ERROR;
    }

    uint16_t keepTimeInt;
    keepTimeInt = getOutdoorKeepTimeBySocket ( TRUE, hdMsg->connfd );
    memset(outValue, 0, sizeof(outValue));
    *(outValue + outValueLen) = msgAck;
    outValueLen += 1;
    memcpy ( outValue + outValueLen, & ( keepTimeInt ), 2 );
    outValueLen += 2;

    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_ODP_TO_MFCB_REPORT_CARD, 
                              outValueLen, outValue);

    /*send new card info to all idps*/
    if ( msgAck == PPO_CARD_EDIT )
    {
        if ( TCMRET_SUCCESS != sendNewCard ( newIndex ) )
        {
            tcmLog_error ( "Send new card info error" );
        }
        else
        {
            tcmLog_notice ( "Send new card info to all devices ok" );
        }
    }

    if ( needSwitchMode )
    {
        switchSecurityMode();
        sendSecurityModeToODP();
        sendSecurityModeToIDP();
    }
    
    return ret;
}

tcmRet processIDPToMFCBQueryCardInfo ( IN homeMsgCtrlBlock* prMsg,
                                       OUT uint8_t* prValue,
                                       OUT uint16_t* prValueLen )
{
    if ( prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }
    
    int cardNumber = 0, i = 0;
    int curValueLen = 0;

    /* leave the card number here, fill it later */
    uint8_t* tmpCardNum =  prValue;
    curValueLen += 1;

    for ( i = 1; i < ( MAXTABLE_CARD + 1 ) ; i++ )
    {
        if ( TCMRET_SUCCESS == ehEncodeAppendOneCard(i, prValue, &curValueLen) )
        {
            cardNumber++;
        }
    }

    *tmpCardNum = ( uint8_t )cardNumber;

    *prValueLen = curValueLen;

    return TCMRET_SUCCESS;
}



tcmRet handleIDPToMFCBQueryCardInfo ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryCardInfo ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_CARD_INFO failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_CARD_INFO OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_QUERY_CARD_INFO, 
                              valueLen, value);
}

int updateCardInfo( void* arg, void* arg2, int len )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen = 0;

    processIDPToMFCBQueryCardInfo(NULL, value, &valueLen);
    int devType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    return ehSendMsgToAllDevice(devType, EVT_MFCB_TO_IDP_UPDATE_CARD, valueLen, value);
}


tcmRet processIDPToMFCBAddCardOperation ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t actResult;

    uint8_t type;

    if ( prMsg->msg.valueLen == 0x01 )
    {
        type = * ( ( uint8_t * ) ( prMsg + 1 ) );

        if ( PPI_CARD_START_REGISTER == type )
        {
            tcmLog_debug ( "Set card operation: start adding card" );
            capi_write_parameter(ID_CARD, PARA_CARD_REGISTER_STATUS, "start");
            time ( &g_cardStartAddTime );
        }
        else if ( PPI_CARD_END_REGISTER == type )
        {
            tcmLog_debug ( "Set card operation: finish adding card" );
            capi_write_parameter(ID_CARD, PARA_CARD_REGISTER_STATUS, "end");
        }
        else
        {
            tcmLog_notice ( "Set card operation: add Card Type error" );
            return TCMRET_INTERNAL_ERROR;
        }
    }

    actResult = ACT_RESULT_SUCCEED;

    *prValue = type;

    * ( prValue + 1 ) = actResult;

    *prValueLen = 2;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBAddCardOperation ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBAddCardOperation ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_ADD_CRD_OPERATION failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_ADD_CRD_OPERATION OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_ADD_CRD_OPERATION, 
                              valueLen, value);
}



tcmRet processIDPToMFCBEditSingleCard ( IN homeMsgCtrlBlock* prMsg,
                                        OUT uint8_t* prValue,
                                        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t actResult = ACT_RESULT_FAILED;
    char cardId[MAXPARA_CARDIDLEN + 1], cardName[MAXPARA_CARDNAMELEN + 1],
        strCardType[MAX_VALUE_LEN + 1], strCardStatus[MAX_VALUE_LEN + 1],
        saveCardInfo[MAX_VALUE_LEN + 1];
    int cardIdLen, cardNameLen, existIndex = 0;
    uint8_t cardType, cardStatus;
    uint16_t curValueLen = 0;

    /* Card ID */
    cardIdLen = * ( ( uint8_t* ) ( prMsg + 1 ) );
    curValueLen += 1;
    memcpy ( cardId, ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen ), cardIdLen );
    curValueLen += cardIdLen;
    cardId[cardIdLen] = '\0';

    /* Card Name */
    cardNameLen = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;
    memcpy ( cardName, ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen ), cardNameLen );
    curValueLen += cardNameLen;
    cardName[cardNameLen] = '\0';

    /* Card Type */
    cardType = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    /* Card status */
    cardStatus = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    int i;

    for ( i = 0; i < MAXTABLE_CARD; i++ )
    {
        if ( !capi_read_table_parameter ( ID_CARD, i + 1,
                                          TAB_PARA_CARD_ID, saveCardInfo ) )
        {
            if ( strcasecmp ( saveCardInfo, cardId ) == 0 )
            {
                existIndex = i + 1;
                break;
            }
        }
    }
    
    if ( 0 == existIndex )
    {
        actResult = ACT_RESULT_FAILED;
        tcmLog_notice ( "Do not have this card %s length is %d in DB",
                        cardId, strlen ( cardId ) );
    }
    else
    {
        actCard_Type_IntToString(strCardType, cardType);
        actCard_Status_IntToString(strCardStatus, cardStatus);
        
        /* Write the table */
        if ( capi_write_table_parameter(ID_CARD, existIndex, TAB_PARA_CARD_NAME, cardName) ||
             capi_write_table_parameter(ID_CARD, existIndex, TAB_PARA_CARD_STATUS, strCardStatus) ||
             capi_write_table_parameter(ID_CARD, existIndex, TAB_PARA_CARD_TYPE, strCardType) )
        {
            actResult = ACT_RESULT_FAILED;
        }
        else
        {
            actResult = ACT_RESULT_SUCCEED;
            capi_write_to_flash();
        }
    }

    *prValueLen = 0;

    memcpy ( prValue + *prValueLen, ( uint8_t * ) ( prMsg + 1 ),
             prMsg->msg.valueLen );

    *prValueLen += ( uint16_t ) prMsg->msg.valueLen;

    * ( prValue + *prValueLen ) = actResult;

    *prValueLen += 1;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBEditSingleCard ( homeMsgCtrlBlock* hdMsg )
{
    char value[300];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBEditSingleCard ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_EDIT_SINGLE_CRD failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_EDIT_SINGLE_CRD OK" );
    }

    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_EDIT_SINGLE_CRD, 
                              valueLen, value);

    /* If IDP changes the card information, we need to notify all IDP and SMP */
    updateCardInfo(NULL, NULL, 0);

    return ret;
}

#define ALARM_HISTORY_QUERY_STRING_LENGTH 21
#define ALARM_HISTORY_QUERY_STRING_SEPRATION ' '
#define ALARM_HISTORY_QUERY_STRING_SEPRATION_POSITION 10
#define ALARM_HISTORY_QUERY_STRING_DOT '.'
#define ALARM_HISTORY_QUERY_STRING_YEAR_LENGTH 4
#define ALARM_HISTORY_QUERY_STRING_MONTH_LENGTH 2
#define ALARM_HISTORY_QUERY_STRING_DAY_LENGTH 2

int checkAlarmHistoryQueryDataFormat ( char* alarmQueryData )
{
    if ( strlen ( alarmQueryData ) !=  ALARM_HISTORY_QUERY_STRING_LENGTH )
    {
        tcmLog_notice ( "Alarm query data length is not %d which is %s, "
                        "and its length is %d",
                        ALARM_HISTORY_QUERY_STRING_LENGTH,
                        alarmQueryData, strlen ( alarmQueryData ) );
        return -1;
    }

    if ( alarmQueryData[ALARM_HISTORY_QUERY_STRING_SEPRATION_POSITION] !=
            ALARM_HISTORY_QUERY_STRING_SEPRATION )
    {
        tcmLog_notice ( "Alarm history query data NULL sepration is error" );
        return -1;
    }

    if ( ! ( ( alarmQueryData[4] == ALARM_HISTORY_QUERY_STRING_DOT ) &&
             ( alarmQueryData[7] == ALARM_HISTORY_QUERY_STRING_DOT ) &&
             ( alarmQueryData[15] == ALARM_HISTORY_QUERY_STRING_DOT ) &&
             ( alarmQueryData[18] == ALARM_HISTORY_QUERY_STRING_DOT ) ) )
    {
        tcmLog_notice ( "Alarm history query data dot sepration error" );
        return -1;
    }

    return 0;
}
//2010.12.21 2011.01.15
int separateAlarmQueryStartandEndTime ( char* alarmQueryData,
                                        time_t* startTime,
                                        time_t* endTime )
{
    struct tm alarmStartTm;
    struct tm alarmEndTm;
    char startYearStr[5], startMonthStr[3], startDayStr[3];
    char endYearStr[5], endMonthStr[3], endDayStr[3];

    int curValueLen = 0;
    memcpy ( startYearStr, alarmQueryData + curValueLen,
             ALARM_HISTORY_QUERY_STRING_YEAR_LENGTH );
    startYearStr[ALARM_HISTORY_QUERY_STRING_YEAR_LENGTH] = '\0';
    curValueLen += ALARM_HISTORY_QUERY_STRING_YEAR_LENGTH;//2010
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( startYearStr, 0, 9999 ) )
    {
        return -1;
    }

    curValueLen += 1; //.

    memcpy ( startMonthStr, alarmQueryData + curValueLen,
             ALARM_HISTORY_QUERY_STRING_MONTH_LENGTH );
    startMonthStr[ALARM_HISTORY_QUERY_STRING_MONTH_LENGTH] = '\0';
    curValueLen += ALARM_HISTORY_QUERY_STRING_MONTH_LENGTH; //12
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( startMonthStr, 0, 99 ) )
    {
        return -1;
    }

    curValueLen += 1; //.

    memcpy ( startDayStr, alarmQueryData + curValueLen,
             ALARM_HISTORY_QUERY_STRING_DAY_LENGTH );
    startDayStr[ALARM_HISTORY_QUERY_STRING_DAY_LENGTH] = '\0';
    curValueLen += ALARM_HISTORY_QUERY_STRING_DAY_LENGTH;
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( startDayStr, 0, 99 ) )
    {
        return -1;
    }

    curValueLen += 1; //' '

    memcpy ( endYearStr, alarmQueryData + curValueLen,
             ALARM_HISTORY_QUERY_STRING_YEAR_LENGTH );
    endYearStr[ALARM_HISTORY_QUERY_STRING_YEAR_LENGTH] = '\0';
    curValueLen += ALARM_HISTORY_QUERY_STRING_YEAR_LENGTH;//2011
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( endYearStr, 0, 9999 ) )
    {
        return -1;
    }

    curValueLen += 1; //'.'

    memcpy ( endMonthStr, alarmQueryData + curValueLen,
             ALARM_HISTORY_QUERY_STRING_MONTH_LENGTH );
    endMonthStr[ALARM_HISTORY_QUERY_STRING_MONTH_LENGTH] = '\0';
    curValueLen += ALARM_HISTORY_QUERY_STRING_MONTH_LENGTH;//01
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( endMonthStr, 0, 99 ) )
    {
        return -1;
    }

    curValueLen += 1; //'.'

    memcpy ( endDayStr, alarmQueryData + curValueLen,
             ALARM_HISTORY_QUERY_STRING_DAY_LENGTH );
    endDayStr[ALARM_HISTORY_QUERY_STRING_DAY_LENGTH] = '\0';
    curValueLen += ALARM_HISTORY_QUERY_STRING_DAY_LENGTH;//15
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( endDayStr, 0, 99 ) )
    {
        return -1;
    }

    alarmStartTm.tm_year = atoi ( startYearStr ) - 1900;
    alarmStartTm.tm_mon  = atoi ( startMonthStr ) - 1;
    alarmStartTm.tm_mday = atoi ( startDayStr );
    alarmStartTm.tm_hour = 0;
    alarmStartTm.tm_min  = 0;
    alarmStartTm.tm_sec  = 0;

    alarmEndTm.tm_year = atoi ( endYearStr ) - 1900;
    alarmEndTm.tm_mon  = atoi ( endMonthStr ) - 1;
    alarmEndTm.tm_mday = atoi ( endDayStr ) + 1;
    alarmEndTm.tm_hour = 0;
    alarmEndTm.tm_min  = 0;
    alarmEndTm.tm_sec  = 0;

    *startTime = mktime ( &alarmStartTm );
    *endTime   = mktime ( &alarmEndTm );
    return 0;
}

#define DB_ALARM_TIME_STRING_LENGTH 19
#define DB_ALARM_TIME_STRING_SEPARATION ' '
#define DB_ALARM_TIME_STRING_SEPARATION_LENGTH 10
int checkAlarmTimeFormatInDB ( char* alarmTime ) //2000-01-01 03:17:32
{
    if ( strlen ( alarmTime ) != DB_ALARM_TIME_STRING_LENGTH )
    {
        tcmLog_notice ( "alarm time length in db is not %d",
                        DB_ALARM_TIME_STRING_LENGTH );
        return -1;
    }

    if ( alarmTime[DB_ALARM_TIME_STRING_SEPARATION_LENGTH] !=
            DB_ALARM_TIME_STRING_SEPARATION )
    {
        tcmLog_notice ( "alarm time string separation is error" );
        return -1;
    }

    return 0;
}
//2000-01-02 03:17:32
int checkAlarmTimeInDB ( char* alarmTime, time_t alarmQueryStartTime,
                         time_t alarmQueryEndTime )
{
    if ( -1 == checkAlarmTimeFormatInDB ( alarmTime ) )
    {
        tcmLog_notice ( "alarm time format in db is error" );
        return -1;
    }
    tcmLog_notice ( " Alarm time format in db is right %s", alarmTime );
    struct tm alarmTm;
    time_t alarmTt;
    int curValueLen = 0;
    char alarmYear[5], alarmMonth[3], alarmDay[3], alarmHour[3], alarmMin[3],
    alarmSec[3];

    memcpy ( alarmYear, alarmTime + curValueLen, 4 );
    alarmYear[4] = '\0';
    curValueLen += 4;//2000
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( alarmYear, 0, 9999 ) )
    {
        return -1;
    }

    curValueLen += 1;//'-'

    memcpy ( alarmMonth, alarmTime + curValueLen, 2 );
    alarmMonth[2] = '\0';
    curValueLen += 2;//01
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( alarmMonth, 0, 99 ) )
    {
        return -1;
    }

    curValueLen += 1;//'-'

    memcpy ( alarmDay, alarmTime + curValueLen, 2 );
    alarmDay[2] = '\0';
    curValueLen += 2;//02
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( alarmDay, 0, 99 ) )
    {
        return -1;
    }

    curValueLen += 1;//' '

    memcpy ( alarmHour, alarmTime + curValueLen, 2 );
    alarmHour[2] = '\0';
    curValueLen += 2;//03
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( alarmHour, 0, 99 ) )
    {
        return -1;
    }

    curValueLen += 1;//':'

    memcpy ( alarmMin, alarmTime + curValueLen, 2 );
    alarmMin[2] = '\0';
    curValueLen += 2;//17
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( alarmMin, 0, 99 ) )
    {
        return -1;
    }

    curValueLen += 1;//':'

    memcpy ( alarmSec, alarmTime + curValueLen, 2 );
    alarmSec[2] = '\0';
    curValueLen += 2;//32
    if ( TCMRET_SUCCESS != actUtil_numberCheck ( alarmSec, 0, 99 ) )
    {
        return -1;
    }

    alarmTm.tm_year = atoi ( alarmYear ) - 1900;
    alarmTm.tm_mon  = atoi ( alarmMonth ) - 1;
    alarmTm.tm_mday = atoi ( alarmDay );
    alarmTm.tm_hour = atoi ( alarmHour );
    alarmTm.tm_min  = atoi ( alarmMin );
    alarmTm.tm_sec  = atoi ( alarmSec );

    alarmTt = mktime ( &alarmTm );

    if ( ( alarmTt < alarmQueryEndTime ) && ( alarmTt > alarmQueryStartTime ) )
    {
        tcmLog_notice ( " Return 0 alarmTt == %d  "
                        "alarmQueryStartTime == %d "
                        "alarmQueryEndTime == %d",
                        alarmTt,
                        alarmQueryStartTime,
                        alarmQueryEndTime );
        return 0;
    }
    else
    {
        tcmLog_notice ( " Return -1 alarmTt == %d  "
                        "alarmQueryStartTime == %d "
                        "alarmQueryEndTime == %d",
                        alarmTt,
                        alarmQueryStartTime,
                        alarmQueryEndTime );
        return -1;
    }

}

tcmRet processIDPToMFCBQueryAlarmHistory ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    if ( prMsg->msg.valueLen != ( ALARM_HISTORY_QUERY_STRING_LENGTH + 1 ) )
    {
        tcmLog_notice ( "Value length is not %d",
                        ALARM_HISTORY_QUERY_STRING_LENGTH + 1 );
        return TCMRET_INTERNAL_ERROR;
    }

    /***Analyze Msg Packet***/
    uint16_t curValueLen = 0;
    uint8_t queryDataLen = 0;
    uint8_t queryDataStr[100];
    uint16_t alarmHistoryLength = 0;
    uint16_t alarmHistoryLengthTmp = 0;

    queryDataLen = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    memcpy ( queryDataStr, ( char * ) ( prMsg + 1 ) + curValueLen,
             queryDataLen );
    queryDataStr[queryDataLen] = '\0';
    curValueLen += queryDataLen;

    memcpy ( prValue, ( uint8_t * ) ( prMsg + 1 ), queryDataLen + 1 );
    memcpy ( prValue + queryDataLen + 1,
             ( uint8_t * ) ( &alarmHistoryLength ), 2 );
    curValueLen += 2;

    if ( -1 == checkAlarmHistoryQueryDataFormat ( queryDataStr ) )
    {
        tcmLog_notice ( "Check alarm history query data format error" );
        return TCMRET_INTERNAL_ERROR;
    }


    time_t alarm_start_tt;
    time_t alarm_end_tt;
    if ( -1 == separateAlarmQueryStartandEndTime ( queryDataStr,
            &alarm_start_tt,
            &alarm_end_tt ) )
    {
        tcmLog_notice ( "Separate alarm query data error" );
        return TCMRET_FAILURE;
    }

    tcmLog_notice ( "queryDataStr:%s\t alarm_start_tt:%d\t alarm_end_tt:%d",
                    queryDataStr, alarm_start_tt, alarm_end_tt );

    int alarmEndRowIndex;
    int i;
    char alarmEndRowIndexStr[4];
    if ( -1 == capi_read_parameter ( ID_ALARMHISTORY,
                                     PARA_ALARM_HISTORY_NEXT_INDEX,
                                     alarmEndRowIndexStr ) )
    {
        tcmLog_notice ( "Read alarm history next index error" );
        return TCMRET_FAILURE;
    }

    if ( TCMRET_SUCCESS != actUtil_numberCheck ( alarmEndRowIndexStr, 0, 999 ) )
    {
        return -1;
    }

    *prValueLen = 0;
    alarmEndRowIndex = atoi ( alarmEndRowIndexStr );

    char   app_result[TAB_PARA_ALARM_HISTORY_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_ALARM_HISTORY_END-1];

    for ( i = 0; i < ( TAB_PARA_ALARM_HISTORY_END - 1 ); i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }

    for ( i = 0 ; i < alarmEndRowIndex ; i++ )
    {
        if ( -1 == capi_read_table_row ( ID_ALARMHISTORY, i + 1, p_result ) )
        {
            continue;
        }

        if ( -1 == checkAlarmTimeInDB ( p_result[TAB_PARA_ALARM_HISTORY_TIME-1],
                                        alarm_start_tt, alarm_end_tt ) )
        {
            continue;
        }

        sprintf ( prValue + curValueLen, "Node index %s ",
                  p_result[TAB_PARA_ALARM_HISTORY_NODE_INDEX-1] );
        curValueLen += 12 +
                       strlen ( p_result[TAB_PARA_ALARM_HISTORY_NODE_INDEX-1] );
        alarmHistoryLength += 12 +
                              strlen ( p_result[TAB_PARA_ALARM_HISTORY_NODE_INDEX-1] );

        sprintf ( prValue + curValueLen, "name %s ",
                  p_result[TAB_PARA_ALARM_HISTORY_NAME-1] );
        curValueLen += 6 + strlen ( p_result[TAB_PARA_ALARM_HISTORY_NAME-1] );
        alarmHistoryLength += 6 +
                              strlen ( p_result[TAB_PARA_ALARM_HISTORY_NAME-1] );

        sprintf ( prValue + curValueLen, "time %s \n",
                  p_result[TAB_PARA_ALARM_HISTORY_TIME-1] );
        curValueLen += 7 + strlen ( p_result[TAB_PARA_ALARM_HISTORY_TIME-1] );
        alarmHistoryLength += 7 +
                              strlen ( p_result[TAB_PARA_ALARM_HISTORY_TIME-1] );
    }

    alarmHistoryLengthTmp = htons ( alarmHistoryLength );
    alarmHistoryLength = alarmHistoryLengthTmp;
    memcpy ( prValue + queryDataLen + 1,
             ( uint8_t * ) ( &alarmHistoryLength ), 2 );
    *prValueLen = curValueLen;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBQueryAlarmHistory ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN*3];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryAlarmHistory ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_ALARM_HISTORY failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_ALARM_HISTORY OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_QUERY_ALARM_HISTORY, 
                              valueLen, value);
}


tcmRet checkODPTypeExist ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t doorID = * ( ( uint8_t * ) ( prMsg + 1 ) );
    tcmLog_debug( "start check odp type, get door id %d",doorID );
    char typeTmp[MAX_VALUE_LEN+1];
    char ODPTypeTmp[MAX_VALUE_LEN+1];
    char ODPStatus[MAX_VALUE_LEN+1];
    int existFlag = -1;
    
    int i;
    for( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
       memset( typeTmp, 0, MAX_VALUE_LEN+1 );
       memset( ODPTypeTmp, 0, MAX_VALUE_LEN+1 );
       memset( ODPStatus, 0, MAX_VALUE_LEN+1 );
       if( !capi_read_table_parameter( ID_UNIT_INFO, i+1, TAB_PARA_UNIT_TYPE, 
                                       typeTmp ) )
       if( !strcasecmp( typeTmp, "ODP" ) )
       if( !capi_read_table_parameter( ID_UNIT_INFO, i+1, TAB_PARA_UNIT_STATE, 
                                       ODPStatus ) )
       if( ( !strcasecmp( ODPStatus, "connected" ) ) || 
           ( !strcasecmp( ODPStatus, "disconnected" ) ) )
       if( !capi_read_table_parameter( ID_UNIT_INFO, i+1, 
                                       TAB_PARA_UNIT_SUB_TYPE, ODPTypeTmp ))
       {
          if( ( !strcasecmp( ODPTypeTmp, "front" ) ) && ( doorID == DEVICE_SUBTYPE_ODP_FRONT ) )
          {
             existFlag = 1;
             tcmLog_notice( "Find %s ODP status %s doorID %d", ODPTypeTmp, ODPStatus, doorID );
          }
          else if( ( !strcasecmp( ODPTypeTmp, "back" ) ) && 
                                                    ( doorID == DEVICE_SUBTYPE_ODP_BACK ) )
          {
             existFlag = 1;
             tcmLog_notice( "Find %s ODP status %s doorID %d", ODPTypeTmp, ODPStatus, doorID );
          }
       }
       
    }
    
    *prValueLen = 0;
    tcmLog_debug( "existFlag == %d", existFlag );
    return existFlag == 1 ? TCMRET_SUCCESS:TCMRET_FAILURE;
}


tcmRet processODPToMFCBQueryAccountInfo ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    uint8_t doorID = * ( ( uint8_t * ) ( prMsg + 1 ) );

    if ( DEVICE_SUBTYPE_ODP_FRONT == doorID || DEVICE_SUBTYPE_ODP_BACK == doorID )
    {
        int devIndex = getDeviceIndexByFd(prMsg->connfd);
        DeviceInfo* ODPDeviceTmp = getDeviceInfoByDeviceIndex(devIndex);
        if ( ODPDeviceTmp == NULL )
        {
            tcmLog_debug ( "Cannot find socket fd %d map device ",
                           prMsg->connfd );
            moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
            return TCMRET_FAILURE;
        }

        updateSubTypeByDeviceIndex(doorID, devIndex);
        
        int curValueLen = 0;

        uint8_t usernameLen, passwordLen;

        usernameLen = strlen ( ODPDeviceTmp->userName );

        passwordLen = strlen ( ODPDeviceTmp->passWord );

        * ( prValue + curValueLen ) = usernameLen;

        curValueLen += 1;

        memcpy ( prValue + curValueLen, ODPDeviceTmp->userName,
                 ( int ) usernameLen );

        curValueLen += usernameLen;

        * ( prValue + curValueLen ) = passwordLen;

        curValueLen += 1;

        memcpy ( prValue + curValueLen, ODPDeviceTmp->passWord,
                 ( int ) passwordLen );

        curValueLen += passwordLen;

        *prValueLen = curValueLen;
        tcmMem_free( ODPDeviceTmp );
    }//if ( SUBTYPE_ODP_FRONT == doorID || SUBTYPE_ODP_BACK == doorID )
    else
    {
        tcmLog_notice ( "This outdoor is not front or back door" );
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        /*make sure that only front or back door can access mfcb*/
        return TCMRET_FAILURE;
    }
    moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    return TCMRET_SUCCESS;
}



tcmRet handleODPToMFCBQueryAccountInfo ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  checkODPTypeExist( hdMsg, value, &valueLen ) )
    {
       /*ODP Type is not existed*/
       tcmLog_notice( "ODP Type is not existed,so process this msg" );
       if ( TCMRET_SUCCESS !=  processODPToMFCBQueryAccountInfo ( hdMsg, value,
               &valueLen ) )
       {
           tcmLog_error ( "Process CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO failed" );
           return TCMRET_FAILURE;
       }
       else
       {
           tcmLog_notice ( "Process CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO OK" );
       }
    }
    else
    {
       /*ODP Type is existed*/
        tcmLog_notice( "ODP Type is existed,so update its status to CONFIGURED"
                       "and then register again");
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_ODP_TO_MFCB_QUERY_ACCOUNT_INFO, 
                              valueLen, value);
}


tcmRet processODPToMFCBPair ( IN homeMsgCtrlBlock* prMsg,
                              OUT uint8_t* prValue,
                              OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }
    
    moduleMutexLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    char odpUsername[MAX_VALUE_LEN + 1];
    char odpPassword[MAX_VALUE_LEN + 1];
    uint16_t usernameLen, passwordLen, curValueLen;

    curValueLen = 0;
    usernameLen = * ( ( uint8_t * ) ( prMsg + 1 ) );
    curValueLen += 1;

    memcpy ( odpUsername, ( uint8_t * ) ( prMsg + 1 ) + curValueLen, 
             usernameLen );
    curValueLen += usernameLen;
    odpUsername[usernameLen] = '\0';

    passwordLen = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    memcpy ( odpPassword, ( uint8_t * ) ( prMsg + 1 ) + curValueLen,
             passwordLen );
    curValueLen += passwordLen;
    odpPassword[passwordLen] = '\0';

    /*now check odp  account*/
    int devIndex = getDeviceIndexByFd(prMsg->connfd);
    DeviceInfo* ODPDeviceTmp = getDeviceInfoByDeviceIndex(devIndex);
    if ( ODPDeviceTmp == NULL )
    {
        tcmLog_notice ( "Cannot find the socket fd %d map device",
                        prMsg->connfd );
        moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
        return TCMRET_FAILURE;
    }
    
    curValueLen = 0;

    if ( strcasecmp ( odpUsername, ODPDeviceTmp->userName ) != 0)
    {
        tcmLog_notice ( "ODP account don't match" );
        * ( prValue + curValueLen ) =  PPO_PAIR_FAILED;
        curValueLen += 1;
        * ( prValue + curValueLen ) = PPO_PAIR_REASON_INVALID_ACCOUNT;
        curValueLen += 1;
    }
    else if ( strcasecmp ( odpPassword, ODPDeviceTmp->passWord ) != 0 )
    {
        tcmLog_notice ( "ODP password don't match" );
        * ( prValue + curValueLen ) =  PPO_PAIR_FAILED;
        curValueLen += 1;
        * ( prValue + curValueLen ) = PPO_PAIR_REASON_INVALID_PASSWORD;
        curValueLen += 1;
    }
    else
    {
        tcmLog_notice ( "ODP account right,pass it" );
        * ( prValue + curValueLen ) = PPO_PAIR_SUCCESS;
        curValueLen += 1;
        * ( prValue + curValueLen ) = PPO_PAIR_REASON_SUCCESS;
        curValueLen += 1;

        updateStatusByDeviceIndex(DEVICE_STATUS_CONNECTED, devIndex);

        gen_db_file();
    }

    *prValueLen = curValueLen;
    tcmMem_free( ODPDeviceTmp );
    moduleMutexUnLock(MPC_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);

    return TCMRET_SUCCESS;
}



tcmRet handleODPToMFCBPair ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;

    if ( TCMRET_SUCCESS !=  processODPToMFCBPair ( hdMsg, value, &valueLen ) )
    {
        tcmLog_error ( "Process CMD_ODP_TO_MFCB_REGISTER failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_ODP_TO_MFCB_REGISTER OK" );
    }

    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_ODP_TO_MFCB_REGISTER, 
                              valueLen, value);

    sendSynTime ( NULL, NULL );

    sendNewFlashTimeToODP();

    sendSecurityModeToODP();

    sendNewOpenDoorTimeToODP();
    
    sendSynDeviceEvt();

    return ret;
}

void updateKeepAliveStatus(int connfd)
{
    int i = 0;
    //if the fd is store,update the flag status
    for ( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
        if(g_keepAliveInfo[i].connfd == connfd)
        {
            tcmLog_debug( "updateKeepAliveStatus: this socket fd %d has been stored,"
                          "set flag to 0", connfd);
            g_keepAliveInfo[i].flag = 0;
            return;
        }
    }
    //the fd is not store, init it
    for ( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
        if(g_keepAliveInfo[i].connfd == -1)
        {
            tcmLog_debug( "updateKeepAliveStatus: this socket fd %d has not been stored, "
                          "store it", connfd);
            g_keepAliveInfo[i].connfd = connfd;
            g_keepAliveInfo[i].flag = 0;
            return;
        }
    }
}

tcmRet handleKeepAliveCmd ( homeMsgCtrlBlock* hdMsg )
{
    updateKeepAliveStatus(hdMsg->connfd);

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_KEEP_ALIVE, 
                              0, NULL);
}


tcmRet processIDPToMFCBResetDefault ( IN homeMsgCtrlBlock* prMsg, OUT uint8_t* prValue,
                                      OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t actResult = ACT_RESULT_SUCCEED;

    *prValueLen = 0;

    * ( prValue + *prValueLen ) = actResult;

    *prValueLen += 1;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBResetDefault ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBResetDefault ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_RESET_DEFAULT failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_RESET_DEFAULT OK" );
    }

    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_RESET_DEFAULT, 
                              valueLen, value);

    return ret;
}

#define SECURITY_PASSWORD_WRONG 0x01
#define SECURITY_PASSWORD_RIGHT 0x00

tcmRet processIDPToMFCBChangeSecurityPassword ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t actResult = ACT_RESULT_SUCCEED;

    uint8_t resultReason = SECURITY_PASSWORD_RIGHT;

    uint8_t oldPassword[MAX_VALUE_LEN+1];

    uint8_t oldPasswordLen;

    uint8_t newPassword[MAX_VALUE_LEN+1];

    uint8_t newPasswordLen;

    uint8_t realPassword[MAX_VALUE_LEN+1];

    uint16_t curValueLen = 0;

    oldPasswordLen = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    if ( oldPasswordLen != 0 )
    {
        memcpy ( oldPassword, ( uint8_t* ) ( prMsg + 1 ) + curValueLen,
                 oldPasswordLen );
        oldPassword[oldPasswordLen] = '\0';
        curValueLen += oldPasswordLen;
    }
    else
    {
        memset ( oldPassword, 0, MAX_VALUE_LEN + 1 );
    }

    newPasswordLen = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    if ( newPasswordLen != 0 )
    {
        memcpy ( newPassword, ( uint8_t* ) ( prMsg + 1 ) + curValueLen,
                 newPasswordLen );
        newPassword[newPasswordLen] = '\0';
        curValueLen += newPasswordLen;
    }
    else
    {
        memset ( newPassword, 0, MAX_VALUE_LEN + 1 );
    }

    if ( -1 == capi_read_parameter ( ID_SECURITYMODE,
                                     PARA_SECURITYMODE_PASSWORD,
                                     realPassword ) )
    {
        tcmLog_error ( "read security mode password error" );
        return TCMRET_FAILURE;
    }

    if ( 0 != strcmp ( oldPassword, realPassword ) )
    {
        actResult = ACT_RESULT_FAILED;
        resultReason = SECURITY_PASSWORD_WRONG;
        goto FINISH;
    }

    if ( -1 == capi_write_parameter ( ID_SECURITYMODE,
                                      PARA_SECURITYMODE_PASSWORD,
                                      newPassword ) )
    {
        tcmLog_error ( "write security mode password error" );
        return TCMRET_FAILURE;
    }

FINISH:

    capi_write_to_flash();

    *prValueLen = 0;

    * ( prValue + *prValueLen ) = actResult;
    *prValueLen += 1;

    * ( prValue + *prValueLen ) = resultReason;
    *prValueLen += 1;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBChangeSecurityPassword ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBChangeSecurityPassword ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD, 
                              valueLen, value);
}


tcmRet processIDPToMFCBCheckSecurityPassword ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t actResult = ACT_RESULT_SUCCEED;

    uint8_t oldPassword[MAX_VALUE_LEN+1];

    uint8_t oldPasswordLen;

    uint8_t realPassword[MAX_VALUE_LEN+1];

    uint16_t curValueLen = 0;

    oldPasswordLen = * ( ( uint8_t* ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    if ( oldPasswordLen != 0 )
    {
        memcpy ( oldPassword, ( uint8_t* ) ( prMsg + 1 ) + curValueLen,
                 oldPasswordLen );
        oldPassword[oldPasswordLen] = '\0';
        curValueLen += oldPasswordLen;
    }
    else
    {
        memset ( oldPassword, 0, MAX_VALUE_LEN + 1 );
    }

    if ( -1 == capi_read_parameter ( ID_SECURITYMODE,
                                     PARA_SECURITYMODE_PASSWORD,
                                     realPassword ) )
    {
        tcmLog_error ( "read security mode password error" );
        return TCMRET_FAILURE;
    }

    if ( 0 != strcmp ( oldPassword, realPassword ) )
    {
        actResult = ACT_RESULT_FAILED;
        goto FINISH;
    }


FINISH:

    *prValueLen = 0;

    * ( prValue + *prValueLen ) = actResult;
    *prValueLen += 1;

    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBCheckSecurityPassword ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBCheckSecurityPassword ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD, 
                              valueLen, value);
}

tcmRet processIDPToMFCBQueryAlarmLinkageInfo ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint16_t curValueLen = 0;
    uint8_t alarmLinkageNum = 0;
    uint8_t alarmLinkageDIIdTmp;
    uint8_t alarmLinkageDOIdTmp;
    uint8_t alarmLinkageAction;

    * ( prValue + curValueLen ) = alarmLinkageNum;
    curValueLen += 1;

    int i;
    char   app_result[TAB_PARA_GEAR_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_GEAR_END-1];

    for ( i = 0; i < ( TAB_PARA_GEAR_END - 1 ); i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }
    
    for ( i = 1 ; i <= MAXTABLE_GEAR; i++ )
    {
        if ( capi_read_table_row ( ID_GEAR, i, p_result ) )
        {
            continue;
        }
        else
        {
            alarmLinkageDIIdTmp = atoi ( p_result[ TAB_PARA_GEAR_DP_ID - 1 ] );
            * ( prValue + curValueLen ) = alarmLinkageDIIdTmp;
            curValueLen += 1;

            alarmLinkageDOIdTmp = atoi ( p_result[ TAB_PARA_GEAR_DO_ID - 1 ] );
            * ( prValue + curValueLen ) = alarmLinkageDOIdTmp;
            curValueLen += 1;

            if ( strcasecmp ( p_result[ TAB_PARA_GEAR_ACTION - 1 ],
                              "off" ) == 0 )
            {
                alarmLinkageAction = DOUT_SET_STATUS_OFF;
            }
            else
            {
                alarmLinkageAction = DOUT_SET_STATUS_ON;
            }

            * ( prValue + curValueLen ) = alarmLinkageAction;
            curValueLen += 1;

            alarmLinkageNum += 1;
        }

    }

    * ( prValue + 0 ) = alarmLinkageNum;
    *prValueLen = curValueLen;

    return TCMRET_SUCCESS;
}



tcmRet handleIDPToMFCBQueryAlarmLinkageInfo ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBQueryAlarmLinkageInfo ( hdMsg,
            value,
            &valueLen )
       )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO, 
                              valueLen, value);
}



tcmRet processIDPToMFCBEditAlarmLinkageInfo ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    uint8_t actResult = ACT_RESULT_SUCCEED;
    tcmRet ret;
    uint16_t curValueLen = 0;
    uint8_t editType = 0;
    uint8_t editDIId;
    uint8_t editDOId;
    uint8_t editAction;
    uint8_t editDIIdStr[3];
    uint8_t editDOIdStr[3];
    uint8_t editActionStr[3];
    int i;

    char   app_result[TAB_PARA_GEAR_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_GEAR_END-1];

    for ( i = 0; i < ( TAB_PARA_GEAR_END - 1 ); i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }

    editType = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;

    editDIId = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;
    sprintf ( editDIIdStr, "%d",  editDIId );
    tcmLog_notice ( "editDIIdStr %s\n", editDIIdStr );

    editDOId = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
    curValueLen += 1;
    sprintf ( editDOIdStr, "%d", editDOId );
    tcmLog_notice ( "editDOIdStr %s\n", editDOIdStr );

    if ( editType == 0x01 )
    {
        tcmLog_notice ( "editType == 0x01" );
        editAction = * ( ( uint8_t * ) ( prMsg + 1 ) + curValueLen );
        curValueLen += 1;
        if ( editAction == 0x01 )
        {
            strcpy ( editActionStr, "on" );
        }
        else
        {
            strcpy ( editActionStr, "off" );
        }

        strcpy ( p_result[TAB_PARA_GEAR_DP_ID - 1], editDIIdStr );
        strcpy ( p_result[TAB_PARA_GEAR_DO_ID - 1], editDOIdStr );
        strcpy ( p_result[TAB_PARA_GEAR_ACTION - 1], editActionStr );

        if ( ( ret = config_act_advCheck_tables ( ID_GEAR, p_result ) )
                != TCMRET_SUCCESS )
        {
            tcmLog_notice ( "advancd check error" );
            actResult = ACT_RESULT_FAILED;
            goto FINISH;
        }

        if ( capi_add_table_row ( ID_GEAR, 0, p_result, NULL ) )
        {
            tcmLog_notice ( "add table row error" );
            actResult = ACT_RESULT_FAILED;
            goto FINISH;
        }

        if ( config_act_refresh_apps ( ID_GEAR, APP_WRITE ) != TCMRET_SUCCESS )
        {
            tcmLog_notice ( "refresh app error" );
            actResult = ACT_RESULT_FAILED;
            goto FINISH;
        }
    }
    else if ( editType == 0x02 )
    {
        tcmLog_notice ( "edittype == 0x02" );
        int deleteGearRowIndex = 0;
        for ( i = 1 ; i <= MAXTABLE_GEAR; i++ )
        {
            if ( capi_read_table_row ( ID_GEAR, i, p_result ) )
            {
                continue;
            }
            else
            {
                if ( ( strcasecmp ( p_result[TAB_PARA_GEAR_DP_ID - 1],
                                    editDIIdStr ) == 0 ) &&
                        ( strcasecmp ( p_result[TAB_PARA_GEAR_DO_ID - 1],
                                       editDOIdStr ) == 0 ) )
                {
                    tcmLog_notice ( "find the linkage which row index is %d",
                                    deleteGearRowIndex );
                    deleteGearRowIndex = i;
                    break;
                }
            }
        }

        if ( deleteGearRowIndex != 0 )
        {
            if ( -1 == capi_delete_table_row ( ID_GEAR, deleteGearRowIndex ) )
            {
                actResult = ACT_RESULT_FAILED;
                goto FINISH;
            }
        }
    }

    capi_write_to_flash();
FINISH:

    curValueLen = 0;
    memcpy ( prValue, ( uint8_t * ) ( prMsg + 1 ) + curValueLen,
             prMsg->msg.valueLen );
    curValueLen += prMsg->msg.valueLen;

    * ( prValue + curValueLen ) = actResult;
    curValueLen += 1;

    *prValueLen = curValueLen;
    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBEditAlarmLinkageInfo ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBEditAlarmLinkageInfo ( hdMsg,
            value,
            &valueLen )
       )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO, 
                              valueLen, value);
}


tcmRet processIDPToMFCBClearAlarmSound ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    int devType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    ehSendMsgToAllDevice(devType, EVT_MFCB_TO_IDP_CLEAR_ALARM_SOUND, 0, NULL);

    * ( prValue + 0 ) = 0x01;
    *prValueLen = 1;
    return TCMRET_SUCCESS;
}


tcmRet handleIDPToMFCBClearAlarmSound ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBClearAlarmSound ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_CLEAR_ALARM_SOUND failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_CLEAR_ALARM_SOUND OK" );
    }

    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_CLEAR_ALARM_SOUND, 
                              valueLen, value);
}

int updateDoorStatusByFd( homeMsgCtrlBlock* prMsg, int fd, int* doorID, 
                                                           int* doorStatus )
{
   int devIndex = getDeviceIndexByFd(fd);
   int ODPTypeFlag = -1;
   if( devIndex < 0 || devIndex > MAXTABLE_DEVICES )
   {
      return -1;
   }

   char typeTmp[MAX_VALUE_LEN+1];
   char ODPTypeTmp[MAX_VALUE_LEN+1];
   memset( typeTmp, 0, sizeof(typeTmp) );
   memset( ODPTypeTmp, 0, sizeof(ODPTypeTmp) );
   
   if( !capi_read_table_parameter( ID_UNIT_INFO, devIndex, TAB_PARA_UNIT_TYPE, 
                                   typeTmp ) )
   if( !strcasecmp( typeTmp, "ODP" ) )
   if( !capi_read_table_parameter( ID_UNIT_INFO, devIndex, 
                                   TAB_PARA_UNIT_SUB_TYPE, ODPTypeTmp ))
   {
      if( !strcasecmp( ODPTypeTmp, "front" ))
      {
         ODPTypeFlag = DEVICE_SUBTYPE_ODP_FRONT;
         *doorID = DEVICE_SUBTYPE_ODP_FRONT;
      }
      else if( !strcasecmp( ODPTypeTmp, "back" ) )
      {
         ODPTypeFlag = DEVICE_SUBTYPE_ODP_BACK;
         *doorID = DEVICE_SUBTYPE_ODP_BACK;
      }
   }

   int reportDoorStatus = * ( ( uint8_t* ) ( prMsg + 1 ) + 0 );
   char ODPStatusStr[MAX_VALUE_LEN+1];
   memset( ODPStatusStr, 0, sizeof(ODPStatusStr) );
   *doorStatus = reportDoorStatus;
   
   tcmLog_debug( "Report door status %d", reportDoorStatus );
   if( reportDoorStatus == PPO_OUTDOOR_STATUS_OPEN )
   {
      strcpy( ODPStatusStr, "opened" );
   }
   else
   {
      strcpy( ODPStatusStr, "closed" );
   }
   
   if( ODPTypeFlag != -1 )
   {      
      if( !capi_write_table_parameter( ID_DOOR, ODPTypeFlag,
                                       TAB_PARA_DOOR_STATUS, 
                                       ODPStatusStr ) )
      {
         tcmLog_debug( "Update %d ODP status to %s succeed", ODPTypeFlag, 
                                                             ODPStatusStr);
      }
      else
      {
         tcmLog_debug( "Update %d ODP status to %s failed", ODPTypeFlag, 
                                                            ODPStatusStr);         
      }
   }
   capi_write_to_flash();
   return ODPTypeFlag;
   
}

tcmRet processODPToMFCBReportDoorStatus ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    if ( prMsg == NULL || prValue == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    int doorID = 0;
    int doorStatus = 0;
    *prValueLen = 0;
    if( -1 != updateDoorStatusByFd( prMsg, prMsg->connfd, &doorID,&doorStatus ))
    {
       tcmLog_debug( "doorID IS %d, and doorStatus is %d", doorID, doorStatus );
       *( prValue + *prValueLen ) = (uint8_t) doorID;
       *prValueLen += 1; 
       *( prValue + *prValueLen ) = (uint8_t) doorStatus;
       *prValueLen += 1;
       return TCMRET_SUCCESS;
    }
    else
    {
       return TCMRET_FAILURE;
    }
}


tcmRet handleODPToMFCBReportDoorStatus ( homeMsgCtrlBlock* hdMsg )
{
    char value[MAX_PACKET_LEN];
    uint16_t valueLen;
    int ret = TCMRET_SUCCESS;

    if ( TCMRET_SUCCESS !=  processODPToMFCBReportDoorStatus ( hdMsg, value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_ODP_TO_MFCB_REPORT_DOOR_STATUS failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_ODP_TO_MFCB_REPORT_DOOR_STATUS OK" );
    }

    ret = ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_ODP_TO_MFCB_REPORT_DOOR_STATUS, 
                              valueLen, value);

    int dType = tcm_itob(DEVICE_IDP) | tcm_itob(DEVICE_SMP);
    ehSendMsgToAllDevice(dType, EVT_MFCB_TO_IDP_CHANGE_DOOR_STATUS, 
                         valueLen, value);
    
    return ret;
}


tcmRet processIDPToMFCBGetAuthInfo ( IN homeMsgCtrlBlock* prMsg,
        OUT uint8_t* prValue,
        OUT uint16_t* prValueLen )
{
    char userName[MAX_VALUE_LEN+1];
    char passWord[MAX_VALUE_LEN+1];
    uint8_t curValueLen = 0;
    
    memset( userName, 0, MAX_VALUE_LEN+1 );
    memset( passWord, 0, MAX_VALUE_LEN+1 );
    
    capi_read_parameter ( ID_SYS, PARA_SYS_USERNAME, userName );
    capi_read_parameter ( ID_SYS, PARA_SYS_PASSWORD, passWord );

    *( prValue + curValueLen ) = strlen( userName );
    curValueLen += 1;

    memcpy( prValue + curValueLen, userName, strlen(userName) );
    curValueLen += (uint8_t) strlen(userName);

    *( prValue + curValueLen ) = strlen( passWord );
    curValueLen += 1;

    memcpy( prValue + curValueLen, passWord, strlen(passWord) );
    curValueLen += (uint8_t) strlen(passWord);
    
    *prValueLen = curValueLen;
    return TCMRET_SUCCESS;
}

tcmRet handleIDPToMFCBGetAuthInfo ( homeMsgCtrlBlock* hdMsg )
{
    uint8_t value[MAX_PACKET_LEN];
    uint16_t valueLen;

    if ( TCMRET_SUCCESS !=  processIDPToMFCBGetAuthInfo ( hdMsg,
            value,
            &valueLen ) )
    {
        tcmLog_error ( "Process CMD_IDP_TO_MFCB_GET_AUTHENTICATION_INFO failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_notice ( "Process CMD_IDP_TO_MFCB_GET_AUTHENTICATION_INFO OK" );
    }
    
    return ehSendMsgToDevice( hdMsg->connfd, hdMsg->ssl, ACK_IDP_TO_MFCB_GET_AUTHENTICATION_INFO, 
                              valueLen, value);
}

tcmRet handleMsg ( homeMsgCtrlBlock* hdMsg )
{
    if ( hdMsg == NULL )
    {
        tcmLog_notice ( "Message can not be NULL" );
        return TCMRET_INVALID_PARAMETER;
    }

    tcmRet ret = TCMRET_SUCCESS;

    switch ( hdMsg->msg.eventType )
    {

        case CMD_IDP_TO_MFCB_REGISTER :
            ret = handleIDPToMFCBPair ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_OPEN_DOOR:
            ret = handleIDPToMFCBOpenDoor ( hdMsg );
            break;

        case ACK_MFCB_TO_ODP_OPEN_DOOR:
            ret = handleMFCBToODPOpenDoorAck ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_DOOR_STATUS:
            ret = handleIDPToMFCBQueryDoorStatusCmd ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS:
            ret = handleIDPToMFCBQueryDetectPointStatus ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_SET_SECURITY_MODE:
            ret = handleIDPToMFCBSetSecurityMode ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE:
            ret = handleIDPToMFCBQueryCurrentSecurityMode ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_DETECT_POINT:
            ret = handleIDPToMFCBQueryDetectPoint ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT:
            ret = handleIDPToMFCBSetSingleDetectPoint ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE:
            ret = handleIDPToMFCBUpdateOneSecurityMode ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_CLEAR_ALARM:
            ret = handleIDPToMFCBClearAlarm ( hdMsg );
            break;

        case CMD_ODP_TO_MFCB_REPORT_CARD:
            ret = handleODPToMFCBReportCardInfoCmd ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_CARD_INFO:
            ret = handleIDPToMFCBQueryCardInfo ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_ADD_CRD_OPERATION:
            ret = handleIDPToMFCBAddCardOperation ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_EDIT_SINGLE_CRD :
            ret = handleIDPToMFCBEditSingleCard ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_ALARM_HISTORY:
            ret = handleIDPToMFCBQueryAlarmHistory ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_CAM:
            ret = handleIDPToMFCBQueryCamCmd ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_ADD_CAM:
            ret = handleIDPToMFCBAddCamCmd ( hdMsg );
            break;

        case CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO:
            ret = handleODPToMFCBQueryAccountInfo ( hdMsg );
            break;

        case CMD_ODP_TO_MFCB_REGISTER:
            ret = handleODPToMFCBPair ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_DO_CONTROL:
            ret = handleIDPToMFCBDoControlCmd ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_DO:
            ret = handleIDPToMFCBQueryDoCmd ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_SYNCHRONIZE_DEVICE:
            ret = handleIDPToMFCBSynchronizeDeviceCmd ( hdMsg );
            break;
        case CMD_IDP_TO_MFCB_TOKEN_OPERATION:
            ret = handleIDPToMFCBTokenOperation ( hdMsg );
            break;
        case CMD_IDP_TO_MFCB_EDIT_SINGLE_CAM:
            ret = handleIDPToMFCBEditSingleCam ( hdMsg );
            break;
        case CMD_IDP_TO_MFCB_KEEP_ALIVE:
            ret = handleKeepAliveCmd ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_RESET_DEFAULT:
            ret = handleIDPToMFCBResetDefault ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD:
            ret = handleIDPToMFCBChangeSecurityPassword ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD:
            ret = handleIDPToMFCBCheckSecurityPassword ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO:
            ret = handleIDPToMFCBQueryAlarmLinkageInfo ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO:
            ret = handleIDPToMFCBEditAlarmLinkageInfo ( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_CLEAR_ALARM_SOUND:
            ret = handleIDPToMFCBClearAlarmSound ( hdMsg );
            break;
            
        case CMD_ODP_TO_MFCB_REPORT_DOOR_STATUS:
            ret = handleODPToMFCBReportDoorStatus( hdMsg );
            break;

        case CMD_IDP_TO_MFCB_GET_AUTHENTICATION_INFO:
            ret = handleIDPToMFCBGetAuthInfo( hdMsg );
            break;
            
        default:
            tcmLog_notice ( "This message is undefined, so just delete it: 0x%x",
                hdMsg->msg.eventType );
            break;
    }

    return ret;

}

int startMpcThraedFunc ( void* arg )
{
    int mpcID =  arg;

    while ( 1 )
    {
        pthread_mutex_lock ( &remoteMsgMutex );

        while ( 0 == remoteMsgReadyFlag )
        {
            pthread_cond_wait ( &remoteMsgCond, &remoteMsgMutex );
        }

        remoteMsgReadyFlag -= 1;
        pthread_mutex_unlock ( &remoteMsgMutex );

        pthread_mutex_lock ( &g_homeMsgQueueMutex );
        homeMsgCtrlBlock* msgTmp = cloneEhMsg ( gehomeMsgQueue );

        if ( msgTmp != NULL )
        {
            if ( TCMRET_SUCCESS != deleteEhMsgFromQueueBySeqID (
                        &gehomeMsgQueue,
                        msgTmp->sequenceID ) )
            {
                tcmLog_error ( "MPC#%d Delete eventtype %d from queue failed",
                               mpcID, msgTmp->msg.eventType );
            }
            else
            {
                if( CMD_IDP_TO_MFCB_KEEP_ALIVE != msgTmp->msg.eventType &&
                    CMD_ODP_TO_MFCB_KEEP_ALIVE != msgTmp->msg.eventType )
                   tcmLog_debug ( "MPC#%d Delete eventtype %d from queue ok",
                                   mpcID, msgTmp->msg.eventType );
            }
        }
        pthread_mutex_unlock ( &g_homeMsgQueueMutex );

        if ( msgTmp != NULL )
        {
            if( CMD_IDP_TO_MFCB_KEEP_ALIVE != msgTmp->msg.eventType &&
                CMD_ODP_TO_MFCB_KEEP_ALIVE != msgTmp->msg.eventType )
               tcmLog_debug ( " Begin to handle message eventtype is %d",
                              msgTmp->msg.eventType );
            if ( TCMRET_SUCCESS != handleMsg ( msgTmp ) )
            {
                tcmLog_error ( "MPC#%d Handle message %d failed", mpcID,
                               msgTmp->msg.eventType );
            }
            else
            {
                if( CMD_IDP_TO_MFCB_KEEP_ALIVE != msgTmp->msg.eventType &&
                    CMD_ODP_TO_MFCB_KEEP_ALIVE != msgTmp->msg.eventType )
                   tcmLog_notice ( "MPC#%d Handle message %d ok", mpcID,
                                   msgTmp->msg.eventType );
            }

        }
        freeEhMsg ( msgTmp );
    }
}

tcmRet startMpc()
{
    int freeTidIndex = 0;
    int i;

    pthread_attr_t  attr;

    for ( i = 0; i < MAX_MPC_THREAD ; i++ )
    {
        if ( gmpcTid[i] == 0 )
        {
            freeTidIndex = i;
            break;
        }
    }

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_init " );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_setschedpolicy " );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_create ( &gmpcTid[freeTidIndex], &attr, startMpcThraedFunc,
                            ( void * ) freeTidIndex ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_create \n" );
        return TCMRET_FAILURE;
    }

    return TCMRET_SUCCESS;
}

tcmRet stopMpc()
{
    int ret = -1;
    int i;

    for ( i = 0; i < MAX_MPC_THREAD; i++ )
    {
        if ( gmpcTid[i] != 0 )
        {
            ret = pthread_cancel ( gmpcTid[i] );
            ret = pthread_join ( gmpcTid[i], NULL );

            if ( ret == 0 )
            {
                tcmLog_notice ( "Stop the %dth MPC ok\n", i );
            }
            else
            {
                tcmLog_error ( "Stop the %dth MPC failed\n", i );
            }
        }

    }

    return ret == 0 ? TCMRET_SUCCESS : TCMRET_FAILURE;
}

/* ==================== Handle Message Part (End) ==================== */
