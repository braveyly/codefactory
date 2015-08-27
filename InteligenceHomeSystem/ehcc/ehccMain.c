#include <pthread.h>
#include <configact.h>
#include <configapi.h>
#include <tcm_eid.h>
#include <tcm_thread_msg.h>
#include "ehDeviceDef.h"
#include "ehDevice.h"
#include "ehTimer.h"
#include "ehTimeout.h"
#include "ehSslSocket.h"
#include "ehMq.h"
#include "ehIndexDevice.h"
#include "ehMpc.h"
#include "ehProto.h"
#include "ehDebug.h"

void *msgHandle = NULL;
const tcmEntityId myId = ID_EHCC;
int gTokenStatus;
int gTokenOwern;

void signalRegister( void )
{
   signal(SIGPIPE, SIG_IGN );
}

void initGlobalVariables()
{
    int i = 0;
    for ( i = 0 ; i < MAX_MPC_THREAD; i++ )
    {
        gmpcTid[ i ] = 0;
    }
    for ( i = 0 ; i < MAXTABLE_DEVICES; i++ )
    {
        g_keepAliveInfo[ i ].connfd = -1;
        g_keepAliveInfo[ i ].flag = -1;
    }

    gTokenStatus = PPI_TOKEN_STATUS_FREE;
    gTokenOwern  = 0;
}

int startAddCardEvtAct ( void* arg, void* arg2, int len )
{
    time ( &g_cardStartAddTime );
    tcmLog_debug ( "Start to register card" );
    return 0;
}

int generateAccountEvtAct ( void* arg, void* arg2, int len )
{
    if ( NULL == arg2 )
    {
        return 0;
    }

    moduleMutexLock(MAIN_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    tcmDhcpData* dhcpData = ( tcmDhcpData * ) arg2;
    int devIndex = getDeviceIndexByMac(dhcpData->mac);

    /* The device does not existed */
    if ( devIndex <= 0 )
    {
        char userName[LEN_OF_USERNAME];
        char passWord[LEN_OF_PASSWORD];
        tcmLog_debug ( "Put into DB and list. Detail info worddata %d, "
                       "IP: %s, MAC: %s, Type: %d", * ( ( int * ) arg ),
                       dhcpData->ip,
                       dhcpData->mac,
                       dhcpData->type );
        if ( TCMRET_SUCCESS != generateAccount ( dhcpData->mac,
                dhcpData->ip,
                dhcpData->type,
                userName, passWord ) )
        {
            tcmLog_error ( "Generate account failed" );
            moduleMutexUnLock(MAIN_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
            return -1;
        }

        DeviceInfo devTmp;
        memset( &devTmp, 0, sizeof(DeviceInfo)/sizeof(char) );

        if( NULL != dhcpData->mac )
           strcpy( devTmp.mac, dhcpData->mac );

        if( NULL != dhcpData->ip )
           strcpy( devTmp.ip, dhcpData->ip );
        
        devTmp.status = DEVICE_STATUS_INITIAL;
        devTmp.type = dhcpData->type;
        
        if ( dhcpData->type == DEVICE_ODP )
        {
            devTmp.subType  = DEVICE_SUBTYPE_ODP_FRONT;
        }
        else
        {
            devTmp.subType  = DEVICE_SUBTYPE_NONE;
        }
        
        strcpy( devTmp.desc, "" );
        strcpy( devTmp.userName, userName );
        strcpy( devTmp.passWord, passWord );
        
        devTmp.pairFailedTimes = 0;

        if ( addDeviceToDB( &devTmp ) <= 0 )
        {
            tcmLog_error( "Can not add device %s into DB", dhcpData->mac );
        }
    }
    else
    {
        tcmLog_debug ( "%s exist in device list, so just update its IP in DB",
                       dhcpData->mac );

        if ( capi_write_table_parameter(ID_UNIT_INFO, devIndex, 
                  TAB_PARA_UNIT_IP_ADDRESS, dhcpData->ip) )
        {
            tcmLog_error( "Update device %d %s IP to %s error", devIndex, 
                          dhcpData->mac, dhcpData->ip );
        }
        else
        {
            config_act_refresh_apps(ID_UNIT_INFO, APP_WRITE);
        }
    }
    moduleMutexUnLock(MAIN_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
}

int ODPStartPairEvtAct ( void* arg, void* arg2, int len )
{
    int rowIndex = * ( int * ) arg;
    if ( NULL != arg2 )
    {
        char* odpMac = ( char * ) arg2;
        * ( odpMac + len ) = '\0';
        tcmLog_debug ( "the ODP MAC is %s len is %d", odpMac, len );
        moduleMutexLock(MAIN_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);

        DeviceInfo* odpDeviceTmp = getDeviceInfoByMac ( odpMac );
        showIndexDeviceArray();
        if ( odpDeviceTmp != NULL &&  odpDeviceTmp->connfd != 0  &&
             odpDeviceTmp->connfd != ZERO_FD )
        {
            capi_write_table_parameter ( ID_UNIT_INFO, rowIndex,
                                         TAB_PARA_UNIT_STATE,
                                         "pairing" );
            config_act_refresh_apps ( ID_UNIT_INFO, APP_WRITE );
            capi_write_to_flash();
            if ( TCMRET_SUCCESS !=
                    sendStartPairEvtToODP ( odpDeviceTmp ) )
            {
                tcmLog_error ( "Send start pairing cmd to ODP error" );
            }
        }
        else
        {
            tcmLog_notice ( "cannot find the device %s in device list", odpMac );
        }
        moduleMutexUnLock(MAIN_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    }
}

int deleteUnitEvtAct ( void* arg, void* arg2, int len )
{
    moduleMutexLock(MAIN_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    updateIndexDeviceArrayDeleteFromDb();
    moduleMutexUnLock(MAIN_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
    sendSynDeviceEvt();
}

int synTimeEvtAct ( void* arg, void* arg2, int len )
{
    return sendSynTime ( NULL, NULL );
}

int sendRingModeEvtAct ( void* arg, void* arg2, int len )
{
    sendSecurityModeToODP();
    sendSecurityModeToIDP();
}

int sendAuthInfoEvtAct ( void* arg, void* arg2, int len )
{
    sendAuthInfoToIDP();
}


int alarmHandle ( UBOOL8 isReport, int nodeId )
{
    char confValue[MAX_VALUE_LEN];
    if ( capi_read_parameter ( ID_SECURITYMODE,
                               PARA_SECURITYMODE_USERMODE,
                               confValue ) )
    {
        tcmLog_error ( "Read error for user mode" );
        return -1;
    }

    int modeIndex = actSecurityMode_getModeIndexByModeString ( confValue ) + 1;
    if ( modeIndex <= 0 )
    {
        tcmLog_error ( "Invalide mode(index): %s", confValue );
        return -1;
    }

    int modeAppID = actSecurityMode_getModeAppIDByModeString ( confValue );
    if ( modeAppID <= 0 )
    {
        tcmLog_error ( "Invalide mode(appid): %s", confValue );
        return -1;
    }

    if ( capi_read_table_parameter ( modeAppID, nodeId,
                                     TAB_PARA_MODE_ACTIVE_STATUSE,
                                     confValue ) )
    {
        tcmLog_error ( "Read error for active mode, mode id %d, node id %d",
                       modeAppID, nodeId );
        return -1;
    }


    int visualValue;
    if ( isReport )
    {
        if ( strcasecmp ( "enable", confValue ) == 0 )
        {
            visualValue = PPI_DP_ACTIVE_ALARM;
        }
        else
        {
            visualValue = PPI_DP_INACTIVE_ALARM;
        }
    }
    else
    {
        visualValue = PPI_DP_NORMAL_STATUS;
    }
    
    return sendAlarmStatus ( ( uint8_t ) modeIndex, ( uint8_t ) nodeId,
                            ( uint8_t ) visualValue );
}

int alarmReportEvtAct ( void* arg, void* arg2, int len )
{
    return alarmHandle ( TRUE, * ( ( int* ) arg ) );
}

int alarmCancelEvtAct ( void* arg, void* arg2, int len )
{
    return alarmHandle ( FALSE, * ( ( int* ) arg ) );
}

int resetDefaultEvtAct ( void* arg, void* arg2, int len )
{
    int subType = * ( int * ) arg;
    return sendResetDefaultEvt ( subType );
}

int notifyODPNewFlashTime ( void* arg, void* arg2, int len )
{
    return sendNewFlashTimeToODP();
}

int notifyODPSecurityMode ( void* arg, void* arg2, int len )
{
    sendSecurityModeToODP();
    sendSecurityModeToIDP();
}

int sendODPOpenTimeEvtAct ( void* arg, void* arg2, int len )
{
    sendNewOpenDoorTimeToODP();
}

void refreshLogLevel( void* arg, void* arg2, int len )
{
    actUtil_setLogLevel();
}

#ifdef AUTO_CALL_TEST
void callTestEvtAct( void* arg, void* arg2, int len )
{
    CallTest* callTestTmp = ( CallTest * ) arg2;
    int* act = ( int * ) arg;
    tcmLog_debug( " caller %s", callTestTmp->caller );
    tcmLog_debug( " callee %s", callTestTmp->callee );  
    tcmLog_debug( " intervalTime %d", callTestTmp->intervalTime );
    tcmLog_debug( " duringTime %d", callTestTmp->duringTime );
    tcmLog_debug( " stopper is %s", callTestTmp->stopper );
    tcmLog_debug( " act is %d", *act );
    sendAutoCallTest( callTestTmp, *act );
}
#endif

void ehcc_main()
{
    if (  TCMRET_SUCCESS != startMpc() )
    {
        tcmLog_error ( "Start the first mpc failed\n" );
        return;
    }

    struct timeval tmv;
    tmv.tv_sec = 1800;
    tmv.tv_usec = 0;
    int timerID = installTimer ( sendSynTime , tmv, PERIOD );
    if ( TCMRET_SUCCESS != startTimer ( timerID ) )
    {
        tcmLog_notice ( "Start timer id %d error", timerID );
    }

    struct timeval tm;
    tm.tv_sec = KEEP_ALIVE_POLLING_TIME;
    tm.tv_usec = 0;
    int tmID = installTimer ( keepAliveSync , tm, PERIOD );
    if ( TCMRET_SUCCESS != startTimer ( tmID ) )
    {
        tcmLog_notice ( "Start timer id %d error", tmID );
    }

    if ( TCMRET_SUCCESS != waitforRemoteMsgbySelect() )
    {
        tcmLog_error ( "Wait for msg by select failed\n" );
        return;
    }

    while ( 1 )
    {
        sleep ( 1000 );
    }
}

int main ( int argc, char* argv[] )
{
    tcmRet ret = TCMRET_SUCCESS;

    /* Init the log library */
    tcmLog_init ( myId );

    /* Init the message library */
    if ( ( ret = tcmMsg_init ( myId, &msgHandle ) ) != TCMRET_SUCCESS )
    {
        tcmLog_cleanup();
        tcmLog_error ( "Fail to init the msg handle: %d", ret );
        return ret;
    }

    /* Init the conflib */
    if ( ( ret = capi_init_library() ) < 0 )
    {
        tcmMsg_cleanup ( &msgHandle );
        tcmLog_cleanup();
        tcmLog_error ( "Fail to init the library: %d", ret );
        return ret;
    }

    /* Init the act library */
    if ( ( ret = actLib_init ( myId, msgHandle ) ) != TCMRET_SUCCESS )
    {
        capi_uninit_library();
        tcmMsg_cleanup ( &msgHandle );
        tcmLog_cleanup();
        tcmLog_error ( "Fail to init act library: %d", ret );
        return ret;
    }

    /* Redefine the log level */
    actUtil_setLogLevel();

    signalRegister();
    
    /*init index device array from db*/
    clarifyDBDeviceStatus();

    initGlobalVariables();
    initGlobalTimeout();

#ifdef AUTO_KEEP_ALIVE
    initKeepAlive();
#endif

    tcmMsg_installEventHandle ( TCM_MSG_GENERATE_ACCOUNT,
                                generateAccountEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_START_ODP_REGISTER,
                                ODPStartPairEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_DELETE_UNIT, deleteUnitEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_REPORT_ALARM, alarmReportEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_CANCEL_ALARM, alarmCancelEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_START_CARD_REGISTER, startAddCardEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_UPDATE_CARD_INFO, updateCardInfo );
    tcmMsg_installEventHandle ( TCM_MSG_RESET_DEFAULT, resetDefaultEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_OUTDOOR_FLASH_TIME,
                                notifyODPNewFlashTime );
    tcmMsg_installEventHandle ( TCM_MSG_CHANGE_SECURITY_MODE,
                                notifyODPSecurityMode );
    tcmMsg_installEventHandle ( TCM_MSG_TIME_SYN, synTimeEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_RINGMODE_CHANGE, sendRingModeEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_CHANGE_AUTH_INFO, sendAuthInfoEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_CHANGE_ODP_OPEN_TIME, 
                                sendODPOpenTimeEvtAct );
    tcmMsg_installEventHandle ( TCM_MSG_REFRESH_LOG_LEVEL, refreshLogLevel );
    
 #ifdef AUTO_CALL_TEST
    tcmMsg_installEventHandle ( TCM_MSG_CHANGE_CALL_TEST, callTestEvtAct );
 #endif

    
    tcmMsg_startSelectThread ( msgHandle );

    ehcc_main();

    actLib_cleanup();
    capi_uninit_library();
    tcmMsg_cleanup ( &msgHandle );
    tcmLog_cleanup();
    tcmMsg_stopSelectThread();
}

