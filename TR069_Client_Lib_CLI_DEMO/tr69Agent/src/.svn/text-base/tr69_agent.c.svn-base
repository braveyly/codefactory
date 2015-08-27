/**********************************************************************
Copyright (c) 2005 Wuhan Tecom Co., Ltd.
All Rights Reserved
No portions of this material may be reproduced in any form without the
written permission of:
Wuhan Tecom Co., Ltd.
#18, Huaguang Road
Wuhan, PR China 430074
All information contained in this document is Wuhan Tecom Co., Ltd.
company private, proprietary, and trade secret.
***********************************************************************/

#include "pal.h"
#include "tr69_agent.h"
#include "tr69_agent_public.h"
#include "tr69_tree_public.h"
#include "event_public.h"
#include "host_handler_public.h"
#include "crq_handler_public.h"
#include "mainloop_public.h"
#include "stun_public.h"
#include "mainloop_public.h"
#include <signal.h>  /* add by Hook Guo for debuging purpose */

/* Define global variables for every thread use. extern them in tr69_agent_public.h */
TcmAgentCB gCB;

PAL_MUTEX_T gTmpFileLock;
PAL_INT32_T gInitSuccess = 0;
EventCodeType gEventCode = 0;
PAL_MSG_T msgUser[MAX_MODULE];

PAL_MUTEX_T gStackInitSyncMutex;
ManagementServer gManagementServer;/* ACS Management related information */

PAL_THREAD_T gTcmAgentID;

char gRPCMethodList[16][32] =         /* CPE support the RPC Methods */
{
    "Inform",
    "TransferComplete",
    "AddObject",
    "DeleteObject",
    "GetRPCMethods",
    "SetParameterValues",
    "GetParameterValues",
    "GetParameterNames",
    "SetParameterAttributes",
    "GetParameterAttributes",
    "Download",
    "Upload",
    "GetQueuedTransfer",
    "Reboot",
    "FactoryReset",
    "ScheduleInform"
};

/* ---------    user api ---------- */
PAL_INT32_T get_msg_from_tcmAgent ( char *buf,  PAL_SIZE_T bufLen )
{
    Module src;
    return msg_recv ( TCMAGENT_CLIENT, &src, buf, bufLen );
}

PAL_INT32_T send_msg_to_tcmAgent ( char *buf, PAL_SIZE_T bufLen )
{
    return msg_send ( HOST_HANDLER, TCMAGENT_CLIENT, buf, bufLen );
}

/* -----  end ----- */

PAL_INT32_T msg_send ( Module dest, Module src, char *buf, PAL_SIZE_T bufLen )
{
    return pal_send_msg ( msgUser[ ( PAL_INT32_T ) dest], msgUser[ ( PAL_INT32_T ) src], buf, bufLen, 0 );
}


PAL_INT32_T msg_recv ( Module dest, Module *src, char *buf, PAL_SIZE_T bufLen )
{
    PAL_INT32_T ret;
    PAL_UINT32_T eventType;
    PAL_INT32_T i;

    ret = pal_recv_msg ( msgUser[ ( PAL_INT32_T ) dest], &eventType, buf, bufLen, 0 );

    for ( i = 0; i < MAX_MODULE; i++ )
    {
        if ( msgUser[i] == eventType )
        {
            *src = i;
        }
    }

    return ret;
}

static void msg_init ( void )
{
    PAL_INT32_T i;
    FILE *fp;

    if ( NULL != ( fp = fopen ( PAL_MSG_ID_FILE, "rb" ) ) )
    {
        PAL_MSG_T msgUserTmp[MAX_MODULE];

        if ( ( int ) MAX_MODULE == fread ( msgUserTmp, sizeof ( PAL_MSG_T ), ( int ) MAX_MODULE, fp ) )
        {
            for ( i = 0; i < MAX_MODULE; i++ )
            {
                pal_destroy_msg ( msgUserTmp[i] );
            }
        }

        fclose ( fp );
    }

    for ( i = 0; i < MAX_MODULE; i++ )
    {
        msgUser[i] = pal_create_msg ( i );
    }

    if ( NULL != ( fp = fopen ( PAL_MSG_ID_FILE, "wb" ) ) )
    {
        fwrite ( msgUser, sizeof ( PAL_MSG_T ), ( int ) MAX_MODULE, fp );

        fclose ( fp );
    }
}

/* don't Consider the number of overflow */
PAL_INT32_T string_to_num ( char *str, OUT PAL_INT32_T *value )
{
    PAL_INT32_T ret = 1;
    PAL_INT32_T tmpValue;

    if ( NULL != str && strlen ( str ) > 0 )
    {
        PAL_INT32_T i;
        for ( i = 0; i < strlen ( str ); i++ )
        {
            if ( !isdigit ( str[i] ) && ( '-' != str[i] || i != 0 || strlen ( str ) <= 1 ) )
            {
                ret = 0;
                break;
            }
        }
    }
    else
    {
        ret = 0;
    }

    if ( ret )
    {
        if ( '-' == str[0] )
            tmpValue = 0 - atoi ( str + 1 );
        else
            tmpValue = atoi ( str );
    }
    *value = tmpValue;

    return ret;
}

FILE *open_from_task_dir ( char *fileName, char *mode )
{
    char fileDir[MAX_FILENAME_LEN + 1];

    fileDir[0] = '\0';

    strcat ( fileDir, PAL_TMP_TASK_DIR );
    strcat ( fileDir, fileName );

    return fopen ( fileDir, mode );
}

Result delete_from_task_dir ( char *fileName )
{
    Result ret = TCMAGENT_OK;
    char fileDir[MAX_FILENAME_LEN + 1];

    fileDir[0] = '\0';

    strcat ( fileDir, PAL_TMP_TASK_DIR );
    strcat ( fileDir, fileName );

    if ( remove ( fileDir ) )
    {
        ret = FILE_REMOVE_FAILURE;
    }

    return ret;
}

FILE *open_from_result_dir ( char *fileName, char *mode )
{
    char fileDir[MAX_FILENAME_LEN + 1];

    fileDir[0] = '\0';

    strcat ( fileDir, PAL_TMP_RESULT_DIR );
    strcat ( fileDir, fileName );

    return fopen ( fileDir, mode );
}

Result delete_from_result_dir ( char *fileName )
{
    Result ret = TCMAGENT_OK;
    char fileDir[MAX_FILENAME_LEN + 1];

    fileDir[0] = '\0';

    strcat ( fileDir, PAL_TMP_RESULT_DIR );
    strcat ( fileDir, fileName );

    if ( -1 == remove ( fileDir ) )
    {
        ret = FILE_REMOVE_FAILURE;
    }

    return ret;
}

void free_string_buffer ( IN OUT char ** buffer, IN  OUT PAL_UINT32_T *bufLen )
{
    if ( *buffer != NULL )
    {
        PAL_FREE ( *buffer );
        *buffer = NULL;
    }
    *bufLen = 0;
}


/***********************************************************************
Function: init_global_keys
Description:initialize the commandkey,parameterkey, etcs.

Return: void,
Input: void,
Output:
Others:
************************************************************************/
void init_global_keys()
{
    memset ( gMRebootCommandKey, 0, MAX_KEY_LENGTH + 1 );/* before main session started, if exist MReboot, copy commandkey to this variable*/
    memset ( gScheduleCommandKey, 0, MAX_KEY_LENGTH + 1 );/* when schedule inform is triggered, copy the commandkey to this variable*/
    memset ( gTransferCommandKey, 0, MAX_KEY_LENGTH + 1 );/* check transfer compelete directory, get lastest commandkey, copy to this variable*/
    memset ( gLastParameterKey, 0, MAX_KEY_LENGTH + 1 );/* for inform packet */

    return;
}


/***********************************************************************
Function: update_managementServer_variables
Description:

Return:  Result,
Input: void,
Output:
Others:
************************************************************************/
Result update_managementServer_variables ()
{
    Result ret = TCMAGENT_OK;

    Tr069NameArg *name = NULL, *nameTmp = NULL;
    Tr069ValueArg *value = NULL, *valueTmp = NULL;

    /*memset variables */
    memset ( &gManagementServer, 0, sizeof ( gManagementServer ) );

    /*get updated data from parameter tree  */
    if ( TCMAGENT_OK == ( ret = get_tr069_value ( "InternetGatewayDevice.ManagementServer.", &name, &value ) ) )
    {
        nameTmp = name;
        valueTmp = value;

        while ( NULL != name )
        {
            if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.URL" ) )
            {
                strcpy ( gManagementServer.acsURL, value->value.string );
            }

            if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.URL" ) )
            {
                strcpy ( gManagementServer.acsURL, value->value.string );
            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.Username" ) )
            {

                strcpy ( gManagementServer.acsUsername, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.Password" ) )
            {
                strcpy ( gManagementServer.acsPassword, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.PeriodicInformEnable" ) )
            {
            #if 0
                if ( 0 == strcmp ( value->value.string, "true" ) ||  0 == strcmp ( value->value.string, "1" ) )
                {
                    gManagementServer.periodicInformEnable = 1;
                }
                else
                {
                    gManagementServer.periodicInformEnable = 0;
                }
			#endif
                gManagementServer.periodicInformEnable = value->value.boolv;

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.PeriodicInformInterval" ) )
            {
                gManagementServer.periodicInformInterval = value->value.uintv;

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.PeriodicInformTime" ) )
            {
                strcpy ( gManagementServer.periodicInformTime, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.ParameterKey" ) )
            {
                strcpy ( gManagementServer.parameterKey, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.ConnectionRequestURL" ) )
            {
                strcpy ( gManagementServer.connectionRequestURL, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.ConnectionRequestUsername" ) )
            {
                strcpy ( gManagementServer.connectionRequestUsername, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.ConnectionRequestPassword" ) )
            {

                strcpy ( gManagementServer.connectionRequestPassword, value->value.string );
            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.UpgradesManaged" ) )
            {

                gManagementServer.upgradesManaged = value->value.uintv;
            }
#ifdef TR111
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress" ) )
            {
                strcpy ( gManagementServer.udpConnectionRequestAddress, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddressNotificationLimit" ) )
            {
                gManagementServer.udpCRANotificationLimit = value->value.uintv;

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.STUNEnable" ) )
            {
                gManagementServer.stunEnable = value->value.boolv;

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.STUNServerAddress" ) )
            {
                strcpy ( gManagementServer.stunServerAddress, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.STUNServerPort" ) )
            {
                gManagementServer.stunServerPort = value->value.uintv;

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.STUNUsername" ) )
            {
                strcpy ( gManagementServer.stunUsername, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.STUNPassword" ) )
            {
                strcpy ( gManagementServer.stunPassword, value->value.string );

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.STUNMaximumKeepAlivePeriod" ) )
            {
                gManagementServer.stunMaxKeepAlivePeriod = value->value.uintv;

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.STUNMinimumKeepAlivePeriod" ) )
            {
                gManagementServer.stunMinKeepAlivePeriod = value->value.uintv;

            }
            else if ( 0 == strcmp ( name->name, "InternetGatewayDevice.ManagementServer.NATDetected" ) )
            {
                gManagementServer.natDetected = value->value.boolv;

            }
#endif
            name = name->next;
            value = value->next;
        }

        free_tr69_name_arg ( &nameTmp );
        free_tr69_value_arg ( &valueTmp );

    }
}
/***********************************************************************
Function: tcmAgent_init
Description:

Return: Result,
Input: TcmAgentCB *arg,
Output:
Others:
************************************************************************/
Result tcmAgent_init ( TcmAgentCB *arg )
{
    Result ret = TCMAGENT_OK;
    int recovery_config = 0;
    /* set logger */
    if ( access ( "/tmp/tcmdba", F_OK ) == 0 )
    {
        pal_set_logger ( LOG_SOAP, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_HTTP, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_MAIN_LOOP, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_STUN, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_HOST_HANDLER, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_EVENT, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_TR069_TREE, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_FTP_MODULE, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
        pal_set_logger ( LOG_CRQ_MODULE, T_LOG_INFO, LOG_CONSOLE, 0, NULL );
    }
    else
    {
        pal_set_logger ( LOG_SOAP, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_HTTP, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_MAIN_LOOP, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_STUN, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_HOST_HANDLER, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_EVENT, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_TR069_TREE, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_FTP_MODULE, T_LOG_INFO, LOG_DISABLE, 0, NULL );
        pal_set_logger ( LOG_CRQ_MODULE, T_LOG_INFO, LOG_DISABLE, 0, NULL );
    }

    VERIFY_CONF_DIR ( PAL_CONFIG_BASE, 1 );
    VERIFY_CONF_DIR ( PAL_TMP_RESULT_DIR, 1 );
    VERIFY_CONF_DIR ( PAL_CERTIFICATE_DIR, 0 );
    VERIFY_CONF_DIR ( PAL_TMP_TASK_DIR, 1 );

    if ( NULL == arg )
    {
        memset ( &gCB, 0, sizeof ( gCB ) );
    }
    else
    {
        memcpy ( &gCB, arg, sizeof ( gCB ) );
    }

    if ( pal_mutex_init ( &gTmpFileLock ) )
    {
        return MUTEX_INIT_FAILURE;
    }

#ifdef VXWORKS
#else
    pal_mpool_init();//hook add pal_memory init.
#endif

    pal_init_logger ();

    //pal_set_logger ( LOG_MAIN_LOOP, T_LOG_INFO, LOG_CONSOLE, 1024,  "mainloop" );

    //pal_set_logger (LOG_HTTP, T_LOG_INFO, LOG_CONSOLE, 1024, "http");

    reset_event_code ();

    init_main_session_sem();/* initialize the sem for main loop session */

    msg_init();

    //printf("\n===init===tree==");
    if ( TCMAGENT_OK != ( ret = init_tr069_tree ( PAL_CONFIG_FILE ) ) )
    {
        return ret;
    }
    //printf("\n===init===event==");
    if ( TCMAGENT_OK != ( ret = init_event () ) )
    {
        return ret;
    }
    //printf("\n===init===host_handler==");

    if ( TCMAGENT_OK != ( ret = init_host_handler () ) )
    {
        return ret;
    }

    //printf("\n===wait init=");
    /* wait configuration init over */
    wait_init ();

    /* init the global variables for system requirement */
    init_global_keys();

    update_managementServer_variables ();

    /*  connection request  handler for tcp */
    if ( TCMAGENT_OK != ( ret = crq_http_server_start () ) )
    {
        return ret;
    }

#ifdef TR111
    /* bring up stun thread if it's enabled */
    if ( TCMAGENT_OK != ( ret = stun_client_start () ) )
    {
        return ret;
    }
#endif
    /* The agent main session, not exit  and block here*/
    if ( TCMAGENT_OK != ( ret = main_session () ) )
    {
        return ret;
    }
}

int start_tcmAgent ( TcmAgentCB *arg )
{
    pal_mutex_init ( &gStackInitSyncMutex );
    pal_mutex_lock ( &gStackInitSyncMutex );
    if ( pal_thread_create ( &gTcmAgentID, ( void * ( * ) ( void *arg ) ) tcmAgent_init, NULL, arg ) )
    {
        return -1;
    }

    pal_mutex_unlock ( &gStackInitSyncMutex );
    pal_mutex_destroy ( &gStackInitSyncMutex );

    return 0;
}

void stop_tcmAgent ( void )
{
    pal_thread_cancel ( &gTcmAgentID );
}

