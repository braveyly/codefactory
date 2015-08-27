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
***********************************************************************
Filename: mainloop.c
Author: Gavin
Date: 2009-05-19
Description:

Histroy:
***********************************************************************/
#include "mainloop.h"

/* global variable define */
PAL_UINT32_T gMaxSendEnvelopes = 1;/* the paramter will depend on the acs server */
char gWanConnectionName[MAX_PARAMETER_LENGTH];/* the wan connection name should change with current configuration  */
char gMRebootCommandKey[MAX_KEY_LENGTH+1];/* before main session started, if exist MReboot, copy commandkey to this variable*/
char gScheduleCommandKey[MAX_KEY_LENGTH+1];/* when schedule inform is triggered, copy the commandkey to this variable*/
char gTransferCommandKey[MAX_KEY_LENGTH+1];/* check transfer compelete directory, get lastest commandkey, copy to this variable*/
char gLastParameterKey[MAX_KEY_LENGTH+1];/* for inform packet */
char gRequestDownloadArg[MAX_PARAMETER_LENGTH];
PAL_UINT32_T gRequestDownloadType = 0;
PAL_UINT32_T  gRetryTimes = 0; /* for inform packet */
PAL_UINT32_T gHoldRequest = 0;/* received acs information */
PAL_UINT32_T gNoMoreRequests = 0; /* received acs information */
PAL_UINT32_T gReboot = 0;   /* system need reboot after the main session */
PAL_UINT32_T gFactoryReset = 0; /* system need to reset to default value according to the acs command */
PAL_INT32_T  gSessionSocket = 0;/* main session socket */

static PAL_SEM_T  gMainSessionSem; /* used to synchronize the mainloop thread */
int gUpgradeFirmare = 0; //gavin added for upgrade_firmware http session
SessionInfo gSessionInfo;

char gRequiredInformParams[REQUIRED_PARAM_NUMBER-1][MAX_PARAMETER_LENGTH] =
{
    "InternetGatewayDevice.DeviceSummary",
    "InternetGatewayDevice.DeviceInfo.SpecVersion",
    "InternetGatewayDevice.DeviceInfo.HardwareVersion",
    "InternetGatewayDevice.DeviceInfo.SoftwareVersion",
    "InternetGatewayDevice.DeviceInfo.ProvisioningCode",
    "InternetGatewayDevice.ManagementServer.ConnectionRequestURL",
    "InternetGatewayDevice.ManagementServer.ParameterKey"
};

unsigned int need_inform ( char *name )
{
    int i = 0;
    for ( i = 0;i < ( REQUIRED_PARAM_NUMBER - 1 );i++ )
    {
        if ( strcmp ( name, gRequiredInformParams[i] ) == 0 )
            return 1;
    }
    return 0;
}

/***********************************************************************
Function: init_main_session_sem
Description:   initialize the main session sem for block and wake up actions
************************************************************************/
void init_main_session_sem ( void )
{
    pal_sem_init ( &gMainSessionSem );
    return;
}

/***********************************************************************
Function: mainloop_wakeup
Description:  Wake up main session thread
************************************************************************/
void mainloop_wakeup ()
{
    pal_sem_post ( &gMainSessionSem );
}

/***********************************************************************
Function: wait_wake_up_main_session
Description:  the main session will block here and wait wake up semaphore from other threads.

Return: void,
Input: void,
************************************************************************/
void wait_wake_up_main_session()
{
    pal_sem_wait ( &gMainSessionSem );
}

/***********************************************************************
Function: free_inform_parameter_list
Description:free inform parameter list

Return: void,
Input: InformInfoStruct &informContent,
Output:
Others:
************************************************************************/
void free_inform_parameter_list ( IN OUT InformInfoStruct * informPtr , PAL_UINT16_T paramListFlag )
{
    ParameterValueStruct * phead = NULL;
    ParameterValueStruct * pnext = NULL;
    phead = informPtr->parameterList;

    /* free device ID */
    if ( paramListFlag == 0 )
    {
        if ( informPtr->deviceId.manufacturer != NULL )
        {
            PAL_FREE ( informPtr->deviceId.manufacturer );
            informPtr->deviceId.manufacturer = NULL;
        }
        if ( informPtr->deviceId.productClass != NULL )
        {
            PAL_FREE ( informPtr->deviceId.productClass );
            informPtr->deviceId.productClass = NULL;
        }
        if ( informPtr->deviceId.oui != NULL )
        {
            PAL_FREE ( informPtr->deviceId.oui );
            informPtr->deviceId.oui = NULL;
        }
        if ( informPtr->deviceId.serialNumber != NULL )
        {
            PAL_FREE ( informPtr->deviceId.serialNumber );
            informPtr->deviceId.serialNumber = NULL;
        }
    }
    /* free parameter list */
    while ( phead != NULL )
    {
        pnext = phead->next;
        if ( phead->value != NULL )
            PAL_FREE ( phead->value );
        PAL_FREE ( phead );
        phead = pnext;
    }
    informPtr->parameterList = NULL;
}


/***********************************************************************
Function: node_convertion
Description:it's for writeonly and attribute value modification.  change writeonly value to null,
Input:
Output:
Others:
************************************************************************/
void node_value_convertion ( Tr069NameArg *nameList, Tr069ValueArg *valueList )
{
    Tr069NameArg *name = nameList;
    Tr069ValueArg *value = valueList;

    while ( NULL != name && NULL != value )
    {
        if ( ! ( name->accessType & Read ) )
        {
            if ( NULL != value )
            {
                if ( tString == value->type )
                {
                    value->value.string[0] = '\0';
                }
                else
                {
                    value->value.intv = 0;
                }
            }
        }

        name = name->next;
        value = value->next;
    }
}

/***********************************************************************
Function: node_convertion
Description:change read/write attribute value to 0 or 1
Return: void,
Input:
Output:
Others:
************************************************************************/
void node_rw_convertion ( Tr069NameArg *nameList )
{
    Tr069NameArg *name = nameList;

    while ( NULL != name )
    {
        if ( name->accessType & Write )
        {
            name->accessType = 1;
        }
        else
        {
            name->accessType = 0;
        }
        name = name->next;
    }
}

/***********************************************************************
Function: free_transfer_complete_list
Description:free transfer complete list

Return: void,
Input: TransferCompleteList *transferCompleteContent,
Output:
Others:
************************************************************************/
void free_transfer_complete_list ( IN OUT TransferCompleteList **transferCompleteP )
{
    TransferCompleteList * phead = NULL;
    TransferCompleteList * pnext = NULL;
    phead = *transferCompleteP;
    while ( phead != NULL )
    {
        pnext = phead->next;
        PAL_FREE ( phead );
        phead = pnext;
    }
    *transferCompleteP = NULL;
}

/***********************************************************************
Function: check_transfer_done_event
Description: scan the PAL_TMP_RESULT_DIR,Return transfer done event numbers

Return: PAL_INT32_T: 0,no transfer complete events; >0, there are transfer complete events.
Input: void,
Output:
Others:
************************************************************************/
PAL_INT32_T check_transfer_done_event()
{
    char fileName[MAX_FILENAME_LEN + 1];
    PAL_INT32_T i = 0;

    while ( 0 == pal_scan_dir ( PAL_TMP_RESULT_DIR, fileName ) )
    {
        if ( strstr ( fileName, DOWNLOAD_EVENTCODE_FILE )  != NULL )
        {
            i++;
        }
        if ( strstr ( fileName, UPLOAD_EVENTCODE_FILE )  != NULL )
        {
            i++;
        }
    }

    return i;
}

/***********************************************************************
Function: do_upload_immediate
Description:  call http or ftp to upload file immediately, and store the result to uploadResponsePtr

Return: Result,
Input: UploadStruct *uploadPtr
Output: UploadResponseStruct *uploadResponsePtr,  The pointer must be a valid pointer.
Others:
************************************************************************/
Result do_upload_immediate ( IN UploadStruct *uploadPtr, OUT  UploadResponseStruct *uploadResponsePtr )
{
    Result ret = TCMAGENT_OK;
    char * fileContentPtr = NULL;
    PAL_UINT32_T fileSize = 0;

    if ( uploadPtr == NULL || uploadResponsePtr == NULL )
        return PARAMETER_ERROR;

    ret = gCB.do_upload ( uploadPtr->fileType, &fileContentPtr, &fileSize );


    if ( ret == TCMAGENT_OK )
    {
        pal_datetime ( uploadResponsePtr->startTime );

        if ( strncmp ( uploadPtr->URL, "ftp", 3 )  == 0 )
            ret = ftp_upload_file ( uploadPtr->fileType, uploadPtr->URL, uploadPtr->username, uploadPtr->password, fileContentPtr, fileSize );
        else if ( strncmp ( uploadPtr->URL, "http", 4 )  == 0 )
            ret = http_upload_file ( uploadPtr->fileType, uploadPtr->URL, uploadPtr->username, uploadPtr->password, fileContentPtr, fileSize );
        else
        {
            if ( fileContentPtr != NULL )
                PAL_FREE ( fileContentPtr );
            return  UNSUPPORTED_PROTCOL_TRANSFER;
        }

        if ( ret == TCMAGENT_OK )
        {
            uploadResponsePtr->status = 0;
        }
        else
        {
            uploadResponsePtr->status = result_to_fault ( ret );
        }

        pal_datetime ( uploadResponsePtr->completeTime );
        PAL_FREE ( fileContentPtr );
    }

    return ret;
}

/***********************************************************************
Function: do_download_immediate
Description:  call http or ftp to download file immediately, and store the result to downloadResponsePtr

Return: Result,
Input: DownloadStruct *downloadPtr,
Output:        DownloadResponseStruct *downloadResponse,  The pointer must be a valid pointer.
Others:
************************************************************************/
Result do_download_immediate ( IN DownloadStruct *downloadPtr, OUT DownloadResponseStruct *downloadResponsePtr )
{
    Result ret = TCMAGENT_OK;
    char * fileContentPtr = NULL;

    if ( downloadPtr == NULL || downloadResponsePtr == NULL )
        return PARAMETER_ERROR;
    if ( downloadPtr->filesize > 1000000 || strcmp ( downloadPtr->fileType, DOWNLOAD_FIRMWARE_TYPE ) == 0 )
    {
        /* check the memory status, if necessary, free up some memory for later malloc */
        gCB.kill_other_apps();
    }

    pal_datetime ( downloadResponsePtr->startTime );
    if ( strncmp ( downloadPtr->URL, "http", 4 )  == 0 )
        ret = http_download_file ( downloadPtr->URL, downloadPtr->username, downloadPtr->password, &fileContentPtr, & ( downloadPtr->filesize ) );
    else if ( strncmp ( downloadPtr->URL, "ftp", 3 )  == 0 )
        ret = ftp_download_file ( downloadPtr->URL, downloadPtr->username, downloadPtr->password, &fileContentPtr, & ( downloadPtr->filesize ) );
    else
        return  UNSUPPORTED_PROTCOL_TRANSFER;

    if ( ( ret == TCMAGENT_OK ) && ( fileContentPtr != NULL ) )
    {
        /*call Callback write the firmware or Config to disk */
        ret = gCB.do_download ( downloadPtr->fileType, downloadPtr->targetFileName , fileContentPtr, downloadPtr->filesize );

        if ( ret == TCMAGENT_OK )
        {
            downloadResponsePtr->status = 0;
            pal_datetime ( downloadResponsePtr->completeTime );
            //gReboot = 1; //gavin add to reboot the board
            if ( strcmp ( downloadPtr->fileType, DOWNLOAD_FIRMWARE_TYPE ) == 0 )
                gUpgradeFirmare = 1; //gavin added for upgrade_firmware http session
        }
    }
    if ( fileContentPtr != NULL )
    {
        PAL_FREE ( fileContentPtr );
        fileContentPtr = NULL;
    }
    return ret;
}

/***********************************************************************
Function: get_local_parameter_value
Description:get parameter value from local configuration tree

Return:
Input: name, parameter name
Output:value ,parmeter value
Others:
************************************************************************/
void get_local_parameter_value ( IN char *name, OUT char ** value )
{
    Result ret;
    Tr069NameArg  * nameArg = NULL;
    Tr069ValueArg * valueArg = NULL;

    ret = get_tr069_value ( name, &nameArg, &valueArg );

    if ( ret == TCMAGENT_OK && valueArg != NULL )
    {
        if ( valueArg->type == tString )
        {
            if ( *value != NULL )
            {
                PAL_FREE ( *value );
            }
            *value = ( char * ) PAL_MALLOC ( strlen ( valueArg->value.string ) + 1 );
            if ( *value == NULL )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                             , __func__, "%s\n", strerror ( errno ) );
                return MALLOC_FAILURE;
            }
            strcpy ( *value, valueArg->value.string );
        }
        else if ( valueArg->type == tUnsigned )
        {
            if ( *value != NULL )
                PAL_FREE ( *value );
            *value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
            if ( *value == NULL )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                             , __func__, "%s\n", strerror ( errno ) );
                return MALLOC_FAILURE;
            }
            sprintf ( *value, "%d", valueArg->value.uintv );
        }
        else if ( valueArg->type == tBool )
        {
            if ( *value != NULL )
                PAL_FREE ( *value );
            *value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
            if ( *value == NULL )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                             , __func__, "%s\n", strerror ( errno ) );
                return MALLOC_FAILURE;
            }
            sprintf ( *value, "%d", valueArg->value.boolv );
        }
        else if ( valueArg->type == tInt )
        {
            if ( *value != NULL )
                PAL_FREE ( *value );
            *value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
            if ( *value == NULL )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                             , __func__, "%s\n", strerror ( errno ) );
                return MALLOC_FAILURE;
            }
            sprintf ( *value, "%d", valueArg->value.intv );
        }

        /* release tr69NameArg,tr69ValueArg  memory here */
        free_tr69_name_arg ( &nameArg );
        free_tr69_value_arg ( &valueArg );
    }
    else
    {
        if ( *value != NULL )
            PAL_FREE ( *value );
        /* error in get local parameter value  */
        *value = ( char * ) PAL_MALLOC ( 1 );
        if ( *value == NULL )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                         , __func__, "%s\n", strerror ( errno ) );
            return MALLOC_FAILURE;
        }
        *value = '\0';
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " error: %d\n", ret );
    }
}

/***********************************************************************
Function: get_transfer_list
Description:  get Transfer list for soap

Return: void ,
Input:
Output: transferListP
Others:
************************************************************************/
void get_transfer_list ( OUT TransferListStruct * transferListP )
{
    char fileName[MAX_FILENAME_LEN + 1];
    PAL_INT32_T i = 0;
    UploadResponseArg response;
    DownloadArg download;
    UploadArg upload;

    FILE *fp = NULL;

    if ( transferListP == NULL )
        return ; /* should not occur */
    memset ( transferListP, 0, sizeof ( TransferListStruct ) );

    /* Completed transfer list */
    tmp_file_lock ();

    while ( 0 == pal_scan_dir ( PAL_TMP_RESULT_DIR, fileName ) && i < TRANSFER_STRUCT_NUM )
    {
        if ( strstr ( fileName, DOWNLOAD_EVENTCODE_FILE )  != NULL  || strstr ( fileName, UPLOAD_EVENTCODE_FILE )  != NULL )
        {

            if ( NULL == ( fp = open_from_result_dir ( fileName, "rb" ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", fileName );
            }

            if ( 0 >= fread ( &response, sizeof ( response ), 1, fp ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " read: %s failed\n", fileName );
            }

            fclose ( fp );

            strcpy ( transferListP->transferlist[i].commandKey, response.commandKey );
            transferListP->transferlist[i++].state = tCompleted;

        }
    }

    /* not started list */
    while ( 0 == pal_scan_dir ( PAL_TMP_TASK_DIR, fileName ) && i < TRANSFER_STRUCT_NUM )
    {
        if ( strstr ( fileName, DOWNLOAD_EVENTCODE_FILE ) )
        {
            if ( NULL == ( fp = open_from_task_dir ( fileName, "rb" ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", fileName );
            }
            if ( 0 >= fread ( &download, sizeof ( download ), 1, fp ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " read: %s failed\n", fileName );
            }
            else
            {
                strcpy ( transferListP->transferlist[i].commandKey, download.commandKey );
                transferListP->transferlist[i++].state = tNotStarted;
            }

            fclose ( fp );

        }
        else if ( strstr ( fileName, UPLOAD_EVENTCODE_FILE ) )
        {
            if ( NULL == ( fp = open_from_task_dir ( fileName, "rb" ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", fileName );
            }
            if ( 0 >= fread ( &upload, sizeof ( upload ), 1, fp ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " read: %s failed\n", fileName );
            }
            else
            {
                strcpy ( transferListP->transferlist[i].commandKey, upload.commandKey );
                transferListP->transferlist[i++].state = tNotStarted;
            }
            fclose ( fp );
        }
    }

    tmp_file_unlock ();

    /* later .should add in progress transfer list here,memo */

    return ;

}

Result check_tr069_main_loop_status()
{

    return TCMAGENT_OK;
}


/***********************************************************************
Function: convert_schedule_to_local_event
Description:  convert soap schedule inform structure to local schedule file

Return: void ,
Input:  schInformPtr
Output: evtFileName localSchedulePtr
Others:
************************************************************************/
void convert_schedule_to_local_event ( IN ScheduleInformStruct * schInformPtr, OUT char * evtFileName, OUT ScheduleArg * localSchedulePtr )
{
    PAL_TIME_T  time;

    if ( schInformPtr == NULL || evtFileName == NULL || localSchedulePtr == NULL )
        return; /* it should be valid  */

    pal_time_current ( &time );

    sprintf ( evtFileName, "%s%ld", SCHEDULE_EVENTCODE_FILE, gScheduleFreeIndex++ );

    strcpy ( localSchedulePtr->commandKey, schInformPtr->commandKey );
    localSchedulePtr->time = schInformPtr->delaySeconds + time; /* occur time */
    strcpy ( localSchedulePtr->fileName, evtFileName );

}

/***********************************************************************
Function: convert_upload_to_local
Description:  convert soap upload structure to local upload task file

Return: void ,
Input:  uploadPtr
Output:uploadTaskPtr
Others:
************************************************************************/
void convert_upload_to_local ( IN UploadStruct * uploadPtr, OUT UploadArg *uploadTaskPtr )
{
    PAL_TIME_T time;

    if ( uploadPtr == NULL  || uploadTaskPtr == NULL )
    {
        return; /* not normal */
    }

    pal_time_current ( &time );

    uploadTaskPtr->time = time + uploadPtr->delaySeconds;

    sprintf ( uploadTaskPtr->fileName, "%s%ld", UPLOAD_EVENTCODE_FILE, gUploadFreeIndex ++ );
    strcpy ( uploadTaskPtr->commandKey, uploadPtr->commandKey );
    strcpy ( uploadTaskPtr->username, uploadPtr->username );
    strcpy ( uploadTaskPtr->password, uploadPtr->password );
    strcpy ( uploadTaskPtr->fileType, uploadPtr->fileType );
    strcpy ( uploadTaskPtr->url, uploadPtr->URL );

}

/***********************************************************************
Function: convert_dowload_to_local
Description:  convert soap download structure to local download task file

Return: void ,
Input:  downloadPtr
Output:downloadTaskPtr
Others:
************************************************************************/
void convert_dowload_to_local ( IN DownloadStruct *downloadPtr, OUT DownloadArg * downloadTaskPtr )
{
    PAL_TIME_T time;

    if ( downloadPtr == NULL  || downloadTaskPtr == NULL )
    {
        return; /* not normal */
    }

    pal_time_current ( &time );

    downloadTaskPtr->time = time + downloadPtr->delaySeconds;
    downloadTaskPtr->fileSize = downloadPtr->filesize;

    sprintf ( downloadTaskPtr->fileName, "%s%ld", DOWNLOAD_EVENTCODE_FILE, gDownloadFreeIndex ++ );
    strcpy ( downloadTaskPtr->commandKey, downloadPtr->commandKey );
    strcpy ( downloadTaskPtr->username, downloadPtr->username );
    strcpy ( downloadTaskPtr->password, downloadPtr->password );
    strcpy ( downloadTaskPtr->fileType, downloadPtr->fileType );
    strcpy ( downloadTaskPtr->url, downloadPtr->URL );
    strcpy ( downloadTaskPtr->targetFileName, downloadPtr->targetFileName );
    strcpy ( downloadTaskPtr->successUrl, downloadPtr->successURL );
    strcpy ( downloadTaskPtr->failureUrl, downloadPtr->failureURL );

}
/***********************************************************************
Function: store_schedule_event
Description:  store schedule event to file

Return: void ,
Input:  localSchedulePtr
Output:
Others:
************************************************************************/
void store_schedule_event ( IN  ScheduleArg * localSchedulePtr )
{
    FILE *fp = NULL;

    if ( localSchedulePtr == NULL )
        return; /* not normal */

    if ( NULL == ( fp = open_from_task_dir ( localSchedulePtr->fileName, "wb" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", localSchedulePtr->fileName );
    }
    if ( 0 >= fwrite ( localSchedulePtr, sizeof ( ScheduleArg ), 1, fp ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " write: %s failed\n", localSchedulePtr->fileName );
    }

    fclose ( fp );
}


/***********************************************************************
Function: store_upload_event
Description:  store upload event to file

Return: void ,
Input:  uploadTaskPtr
Output:
Others:
************************************************************************/
void store_upload_event ( IN UploadArg * uploadTaskPtr )
{
    FILE *fp = NULL;

    if ( uploadTaskPtr == NULL )
        return; /* not normal */

    if ( NULL == ( fp = open_from_task_dir ( uploadTaskPtr->fileName, "wb" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", uploadTaskPtr->fileName );
    }
    if ( 0 >= fwrite ( uploadTaskPtr, sizeof ( UploadArg ), 1, fp ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " write: %s failed\n", uploadTaskPtr->fileName );
    }

    fclose ( fp );

}

/***********************************************************************
Function: store_download_event
Description:  store download event to file

Return: void ,
Input:  downloadTaskPtr
Output:
Others:
************************************************************************/
void store_download_event ( IN DownloadArg  *downloadTaskPtr )
{
    FILE *fp = NULL;

    if ( downloadTaskPtr == NULL )
        return; /* not normal */

    if ( NULL == ( fp = open_from_task_dir ( downloadTaskPtr->fileName, "wb" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", downloadTaskPtr->fileName );
    }
    if ( 0 >= fwrite ( downloadTaskPtr, sizeof ( DownloadArg ), 1, fp ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " write: %s failed\n", downloadTaskPtr->fileName );
    }

    fclose ( fp );
}

/***********************************************************************
Function: store_reboot_event
Description:  store reboot event to file

Return: void ,
Input:  commandKey,used to inform event code's command key  "M Reboot"
Output:
Others:
************************************************************************/
void store_reboot_event ( IN char *cmdKey )
{
    FILE *fp = NULL;

    if ( cmdKey == NULL )
        return; /* not normal */

    if ( NULL == ( fp = open_from_task_dir ( MREBOOT_EVENT_FILE, "w" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", MREBOOT_EVENT_FILE );
        return;
    }
    if ( 0 >= fputs ( cmdKey, fp ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " write: %s failed\n", MREBOOT_EVENT_FILE );
    }

    fclose ( fp );
    return;
}

/***********************************************************************
Function: store_nobootstrap_event
Description:  store no bootstrap event to file

Return: void ,
Input:
Output:
Others:
************************************************************************/
void store_nobootstrap_event ()
{
    FILE *fp = NULL;

    if ( NULL == ( fp = open_from_task_dir ( NO_BOOTSTRAP_EVENTCODE_FILE, "w" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", NO_BOOTSTRAP_EVENTCODE_FILE );
        return;
    }
    else
        fputs ( "1", fp );

    fclose ( fp );
    return;
}

/***********************************************************************
Function: local_type_to_soap
Description:  convert tree type to soap type

Return: ValueType,
Input: Tr069TypeArg type,
Output:
Others:
************************************************************************/
ValueType local_type_to_soap ( IN Tr069TypeArg type )
{
    switch ( type )
    {
        case tString:
            return XSD_STRING;
            break;
        case tUnsigned:
            return XSD_UINT;
            break;
        case tBool:
            return XSD_BOOL;
            break;
        case tInt:
            return XSD_INT;
            break;
        default:
            return XSD_STRING;
            break;
    }
}

/***********************************************************************
Function: get_inform_parameter_list
Description:create inform parameter list

Return: void,
Input: InformInfoStruct &informContent,
Output:
Others:
************************************************************************/
Result get_inform_parameter_list ( OUT InformInfoStruct * informContentPtr )
{
    PAL_INT32_T i = 0;
    Tr069NameArg  *tr69NameArg = NULL, *tr69NameCur = NULL;
    Tr069ValueArg   *tr69ValueArg = NULL, *tr69ValueCur = NULL;
    ParameterValueStruct  *parameterValuePtr = NULL, *parameterValueCur = NULL;
    Result ret = TCMAGENT_OK;

    if ( informContentPtr->parameterList != NULL )
    {
        free_inform_parameter_list ( informContentPtr , 1 );
    }

    for ( i = 0; i < ( REQUIRED_PARAM_NUMBER - 1 );i++ )
    {
        parameterValuePtr = PAL_MALLOC ( sizeof ( ParameterValueStruct ) );
        if ( parameterValuePtr == NULL )
            return MALLOC_FAILURE;
        memset ( parameterValuePtr, 0, sizeof ( ParameterValueStruct ) );

        strcpy ( parameterValuePtr->name, gRequiredInformParams[i] );
        get_local_parameter_value ( gRequiredInformParams[i], & ( parameterValuePtr->value ) );
        parameterValuePtr->type = XSD_STRING;
        parameterValuePtr->next = NULL;


        if ( parameterValueCur == NULL )
        {
            informContentPtr->parameterList = parameterValuePtr;
            parameterValueCur = parameterValuePtr;
        }
        else
        {
            parameterValueCur->next = parameterValuePtr;
            parameterValueCur = parameterValuePtr;
        }

    }

    /* add default connection ip address to inform */
    {
        parameterValuePtr = PAL_MALLOC ( sizeof ( ParameterValueStruct ) );
        if ( parameterValuePtr == NULL )
            return MALLOC_FAILURE;
        memset ( parameterValuePtr, 0, sizeof ( ParameterValueStruct ) );

        get_local_parameter_value ( DEFAULT_CONNECTION_SERVICE, & ( parameterValuePtr->value ) );
        if ( parameterValuePtr->value  == NULL )
            return MALLOC_FAILURE;
        strcpy ( parameterValuePtr->name, parameterValuePtr->value );
        strcat ( parameterValuePtr->name, ".ExternalIPAddress" );
        gCB.get_wan_ipaddress ( parameterValuePtr->value );//here not release,just use the memory for ip value
        //printf("\n====connection service name[%s],ip[%s]==\n",parameterValuePtr->name,parameterValuePtr->value);
        parameterValuePtr->type = XSD_STRING;
        parameterValuePtr->next = NULL;

        parameterValueCur->next = parameterValuePtr;
        parameterValueCur = parameterValuePtr;
    }



    ret = get_tr069_notification_node ( &tr69NameArg, &tr69ValueArg );

    if ( ret == TCMAGENT_OK )
    {
        node_value_convertion ( tr69NameArg, tr69ValueArg );
        tr69NameCur = tr69NameArg;
        tr69ValueCur = tr69ValueArg;
        while ( tr69NameCur != NULL && tr69ValueCur != NULL )
        {
            for ( i = 0; i < ( REQUIRED_PARAM_NUMBER - 1 );i++ )
            {
                if ( strcmp ( tr69NameCur->name, gRequiredInformParams[i] ) == 0 )
                    break; /* ignore the required parameters */
            }
            if ( i == ( REQUIRED_PARAM_NUMBER - 1 ) )
            {
                parameterValuePtr = ( ParameterValueStruct * ) PAL_MALLOC ( sizeof ( ParameterValueStruct ) );
                if ( parameterValuePtr == NULL )
                    return MALLOC_FAILURE;
                memset ( parameterValuePtr, 0, sizeof ( ParameterValueStruct ) );

                strcpy ( parameterValuePtr->name, tr69NameCur->name );
                if ( tr69ValueCur->type == tString )
                {
                    parameterValuePtr->value = ( char * ) PAL_MALLOC ( strlen ( tr69ValueCur->value.string ) + 1 );
                    if ( parameterValuePtr->value == NULL )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                                     , __func__, "%s\n", strerror ( errno ) );
                        return MALLOC_FAILURE;
                    }
                    strcpy ( parameterValuePtr->value, tr69ValueCur->value.string );

                }
                else if ( tr69ValueCur->type == tInt )
                {
                    parameterValuePtr->value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
                    if ( parameterValuePtr->value == NULL )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                                     , __func__, "%s\n", strerror ( errno ) );
                        return MALLOC_FAILURE;
                    }
                    sprintf ( parameterValuePtr->value, "%d", tr69ValueCur->value.intv );
                }
                else if ( tr69ValueCur->type == tUnsigned )
                {
                    parameterValuePtr->value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
                    if ( parameterValuePtr->value == NULL )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                                     , __func__, "%s\n", strerror ( errno ) );
                        return MALLOC_FAILURE;
                    }
                    sprintf ( parameterValuePtr->value, "%d", tr69ValueCur->value.uintv );
                }
                else if ( tr69ValueCur->type == tBool )
                {
                    parameterValuePtr->value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
                    if ( parameterValuePtr->value == NULL )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP
                                     , __func__, "%s\n", strerror ( errno ) );
                        return MALLOC_FAILURE;
                    }
                    parameterValuePtr->value[0] = '\0';
                    sprintf ( parameterValuePtr->value, "%d", tr69ValueCur->value.boolv );
                }

                parameterValuePtr->type = local_type_to_soap ( tr69ValueArg->type );
                parameterValuePtr->next = NULL;

                parameterValueCur->next = parameterValuePtr;
                parameterValueCur = parameterValuePtr;

            } /* end of while */
            tr69NameCur = tr69NameCur->next;
            tr69ValueCur = tr69ValueCur->next;
        }
    }

    /* release tr69NameArg,tr69ValueArg  memory here */
    free_tr69_name_arg ( &tr69NameArg );
    free_tr69_value_arg ( &tr69ValueArg );

    return TCMAGENT_OK;
}

/***********************************************************************
Function: update_transfer_eventcode
Description: check transfer directory, then update related commandkey and eventcode

Return: void,
Input:
Output:
Others:
************************************************************************/
void update_transfer_eventcode()
{
    char fileName[MAX_FILENAME_LEN + 1];
    UploadResponseArg response;
    FILE *fp = NULL;
    int i = 0;

    while ( 0 == pal_scan_dir ( PAL_TMP_RESULT_DIR, fileName ) )
    {
        if ( i > 0 ) //since the pal_scan_dir now need to loop to reset the dir
            continue;
        if ( strstr ( fileName, DOWNLOAD_EVENTCODE_FILE )  != NULL  || strstr ( fileName, UPLOAD_EVENTCODE_FILE )  != NULL )
        {
            tmp_file_lock ();
            if ( NULL == ( fp = open_from_result_dir ( fileName, "rb" ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", fileName );
                tmp_file_unlock ();
            }

            if ( 0 >= fread ( &response, sizeof ( response ), 1, fp ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " read: %s failed\n", fileName );
            }

            fclose ( fp );
            tmp_file_unlock ();

            set_event_code ( EVENT_TRANSFER_COMPLETE );
            strcpy ( gTransferCommandKey, response.commandKey );

            i = 1;

        }
    }

    return ;

}

/***********************************************************************
Function: update_mreboot_eventcode
Description: check task directory, then update related commandkey and eventcode

Return: void,
Input:
Output:
Others:
************************************************************************/
void update_mreboot_eventcode()
{

    FILE *fp = NULL;

    tmp_file_lock ();
    if ( NULL == ( fp = open_from_task_dir ( MREBOOT_EVENT_FILE, "r" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", MREBOOT_EVENT_FILE );
        tmp_file_unlock ();
        return;
    }
    if ( NULL == fgets ( gMRebootCommandKey, sizeof ( gMRebootCommandKey ), fp ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " read: %s failed\n", MREBOOT_EVENT_FILE );
    }

    fclose ( fp );
    tmp_file_unlock ();

    set_event_code ( EVENT_M_REBOOT );

    return;
}



/***********************************************************************
Function: update_inform_content
Description:  it will read parameters from agent's configuration tree and system environment. and generate the
informContent, which will be used in later soap encapsulation packet.

Return: Result,
Input: OUT InformInfoStruct * informContent,
Output:
Others:
************************************************************************/
Result update_inform_content ( OUT InformInfoStruct * informContentPtr )
{
    PAL_INT32_T  i = 0;
    Result ret = TCMAGENT_OK;

    if ( informContentPtr == NULL )
        return POINTER_FAILURE;

    /* update DeviceID */
    get_local_parameter_value ( DEVICEID_MANUFACTURER_NAME, & ( informContentPtr->deviceId.manufacturer ) );
    get_local_parameter_value ( DEVICEID_OUI_NAME, & ( informContentPtr->deviceId.oui ) );
    get_local_parameter_value ( DEVICEID_PRODUCTCLASS_NAME, & ( informContentPtr->deviceId.productClass ) );
    get_local_parameter_value ( DEVICEID_SERIALNUMBER_NAME, & ( informContentPtr->deviceId.serialNumber ) );

    /*update eventStruct */
    memset ( informContentPtr->eventStruct, 0, sizeof ( EventCodeStruct ) * ( EVENT_CODE_NUM + 1 ) );
    update_transfer_eventcode();/* check transfer directory, then update related commandkey and eventcode */
    update_mreboot_eventcode();/* check task directory, then update releated commandkey and eventcode */

    if ( ( gEventCode & EVENT_BOOTSTRAP ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_BOOTSTAP );
    }
    if ( ( gEventCode & EVENT_BOOT ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_BOOT );
    }
    if ( ( gEventCode & EVENT_PERIODIC ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_PERIODIC );
    }
    if ( ( gEventCode & EVENT_SCHEDULED ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i].eventCode, EVENTCODE_SCHEDULE );
        strcpy ( informContentPtr->eventStruct[i++].commandKey, gScheduleCommandKey );
    }
    if ( ( gEventCode & EVENT_VALUE_CHANGE ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_VALCHANGE );
    }
    if ( ( gEventCode & EVENT_KICKED ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_KICKED );
    }
    if ( ( gEventCode & EVENT_CONNECTION_REQUEST ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_CONNECTIONREQ );
    }
    if ( ( gEventCode & EVENT_DIAGNOSTICS ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_DIAGNOSITICS );
    }
    if ( ( gEventCode & EVENT_TRANSFER_COMPLETE ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i].eventCode, EVENTCODE_TRANSFER );
        strcpy ( informContentPtr->eventStruct[i++].commandKey, gTransferCommandKey );
    }
    if ( ( gEventCode & EVENT_M_REBOOT ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i].eventCode, EVENTCODE_MREBOOT );
        strcpy ( informContentPtr->eventStruct[i++].commandKey, gMRebootCommandKey );
    }
    if ( ( gEventCode & EVENT_REQUEST_DOWNLOAD ) != 0 )
    {
        strcpy ( informContentPtr->eventStruct[i++].eventCode, EVENTCODE_REQUEST_DOWNLOAD );
    }

    /*update Current Time */
    pal_datetime ( informContentPtr->currentTime );


    /* update MaxEnvelopes and retry Count */
    informContentPtr->maxEnvelopes = MAX_RECV_ENVELOPES;
    informContentPtr->retryCount = gRetryTimes;

    /* update ParameterList */
    ret = get_inform_parameter_list ( informContentPtr );
    if ( ret != TCMAGENT_OK )
        free_inform_parameter_list ( informContentPtr , 1 );

    return TCMAGENT_OK;
}

/***********************************************************************
Function: update_transfer_complete_content
Description:  According to the acs server's Maximum envelopes number ,it add transferComplete event to the transferCmpList for
soap function to encapsulate packets.

Return: Result,
Input: OUT TransferCompleteList * transferCmpList,
       IN PAL_UINT32_T maxMaxEnvelopes,
Output:
Others:
************************************************************************/
Result update_transfer_complete_content ( OUT TransferCompleteList ** transferCmpListP, IN PAL_UINT32_T  maxMaxEnvelopes )
{

    char fileName[MAX_FILENAME_LEN + 1];
    PAL_INT32_T i = 0;
    UploadResponseArg response;
    FILE *fp = NULL;
    TransferCompleteList *transferListPtr = NULL, *transferListCur = NULL;

    if ( maxMaxEnvelopes <= 0 )
        return TCMAGENT_OK;

    if ( *transferCmpListP != NULL )
        free_transfer_complete_list ( transferCmpListP );

    while ( 0 == pal_scan_dir ( PAL_TMP_RESULT_DIR, fileName ) )
    {
        if ( i < maxMaxEnvelopes && ( strstr ( fileName, DOWNLOAD_EVENTCODE_FILE )  != NULL  || strstr ( fileName, UPLOAD_EVENTCODE_FILE )  != NULL ) )
        {
            tmp_file_lock ();

            if ( NULL == ( fp = open_from_result_dir ( fileName, "rb" ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", fileName );
                tmp_file_unlock ();
            }

            if ( 0 >= fread ( &response, sizeof ( response ), 1, fp ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " read: %s failed\n", fileName );
            }

            //printf("\n====read content==[%s],[%d],[%s],[%s]==\n",response.commandKey,response.status,response.startTime,response.completeTime);
            fclose ( fp );
            tmp_file_unlock ();

            transferListPtr = ( TransferCompleteList * ) PAL_MALLOC ( sizeof ( TransferCompleteList ) );
            if ( transferListPtr == NULL )
                return MALLOC_FAILURE;
            memset ( transferListPtr, 0 , sizeof ( TransferCompleteList ) );

            if ( *transferCmpListP == NULL )
            {
                *transferCmpListP = transferListPtr;
                transferListCur = transferListPtr;
            }
            else
            {
                transferListCur->next = transferListPtr;
                transferListCur = transferListPtr;
            }

            /* convert the UploadResponseArg data to TransferCompleteStruct data */
            strcpy ( transferListPtr->transferCompleteInfo.commandKey, response.commandKey );
            transferListPtr->transferCompleteInfo.fault.faultcode = response.status;
            strcpy ( transferListPtr->transferCompleteInfo.fault.faultString, get_fault_string ( transferListPtr->transferCompleteInfo.fault.faultcode ) );
            /* start time and endtime will be adjusted later */
            strcpy ( transferListPtr->transferCompleteInfo.startTime, response.startTime );
            strcpy ( transferListPtr->transferCompleteInfo.completeTime, response.completeTime );

            i++;

        }
    }

    return TCMAGENT_OK;
}

/***********************************************************************
Function: remove_transfer_done_event
Description: when a transferComplete Response received, the related transfer record list should be clear in this function.

Return: Result,
Input: IN OUT TransferCompleteList * transferCmpList,
Output:
Others:
************************************************************************/
Result remove_transfer_done_event ( IN OUT TransferCompleteList * transferCmpList )
{
    char fileName[MAX_FILENAME_LEN + 1];
    UploadResponseArg response;
    FILE *fp = NULL;
    TransferCompleteList *transferListPtr = NULL;

    if ( transferCmpList == NULL )
        return TCMAGENT_OK;

    while ( 0 == pal_scan_dir ( PAL_TMP_RESULT_DIR, fileName ) )
    {
        if ( strstr ( fileName, DOWNLOAD_EVENTCODE_FILE )  != NULL  || strstr ( fileName, UPLOAD_EVENTCODE_FILE )  != NULL )
        {
            tmp_file_lock ();
            if ( NULL == ( fp = open_from_result_dir ( fileName, "rb" ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " open: %s failed\n", fileName );
                tmp_file_unlock ();
            }

            if ( 0 > fread ( &response, sizeof ( response ), 1, fp ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " read: %s failed\n", fileName );
            }

            fclose ( fp );
            tmp_file_unlock ();

            for ( transferListPtr = transferCmpList; transferListPtr != NULL; transferListPtr = transferListPtr->next )
            {
                if ( strcmp ( transferListPtr->transferCompleteInfo.commandKey, response.commandKey ) == 0 )
                    break;
            }

            if ( transferListPtr != NULL )
            {
                /* find the file ,then delete it */
                tmp_file_lock ();
                delete_from_result_dir ( fileName );
                tmp_file_unlock();
            }

        }

    }

    return TCMAGENT_OK;

}

/***********************************************************************
Function: result_to_fault
Description: convert the ret value to tr69 soap fault code.

Return: FaultType FaultCode
Input: Result ret
Output:
Others:
************************************************************************/
FaultType result_to_fault ( Result ret )
{
    FaultType fault;

    if ( ret == TCMAGENT_OK || ret >= 9000 )
        fault = ( FaultType ) ret;
    else
    {
        switch ( ret )
        {
            case NODE_NAME_ERROR:
            case NODE_NUM_ERROR:
            case ATTRIBUTE_NAME_ERROR:
                fault = tInvalidParameterName;
                break;
            case NODE_NOT_FOUND:
            case ATTRIBUTE_ACCESS_ERROR:
                fault = tInvalidArguments;
                break;
            case NODE_TYPE_ERROR:
            case ATTRIBUTE_TYPE_ERROR:
                fault = tInvalidParameterType;
                break;
            case NODE_VALUE_ERROR:
                fault = tInvalidParameterValue;
                break;
            case NODE_ACCESS_ERROR:
                fault = tAttemptSetNonWritable;
                break;
            case INSTANCES_IS_FULL:
                fault = tResourcesExceeded;
                break;
            case NODE_ACCESSLIST_CHECK_ERROR:
                fault = tRequestDenied;
                break;
            case ATTRIBUTE_NOTIFICATION_ERROR:
                fault = tNotificationRequestRejected;
                break;
            default:
                fault = tInternalError;
                break;
        }
    }
    return fault;
}


/***********************************************************************
Function: access_string_to_type
Description: convert the access List string to tr069 tree access type

Return: tr069 tree access type
Input: char * accessList
Output:
Others:
************************************************************************/
Tr069AccessList access_string_to_type ( IN char * accessList )
{
    Tr069AccessList tr69AccessType = ALL;

    if ( accessList == NULL )
        tr69AccessType = ALL;
    else if ( strlen ( accessList ) == 0 )
        tr69AccessType = ALL;
    else if ( strcmp ( accessList, "Subscriber" ) == 0 ) /* currently only Subscirber defined */
        tr69AccessType = ACS;
    else
        tr69AccessType = ALL;

    return tr69AccessType;
}

/***********************************************************************
Function: convert_soap_to_local_value
Description: convert soap parameter value to tr069 tree value structure

Return: void
Input: paramValuePtr,notice that ,not use the next level,
Output: treeValueP,notice that ,it just return single member. not group list
Others:
************************************************************************/
void convert_soap_to_local_value ( IN ParameterValueStruct *paramValuePtr, OUT Tr069ValueArg **treeValueP )
{
    Tr069ValueArg * tr69ValuePtr = NULL;

    if ( *treeValueP != NULL )
        return;/* not normal */

    tr69ValuePtr = PAL_MALLOC ( sizeof ( Tr069ValueArg ) );
    if ( tr69ValuePtr == NULL )
        return; /* not normal */
    memset ( tr69ValuePtr, 0, sizeof ( Tr069ValueArg ) );
    *treeValueP = tr69ValuePtr;

    tr69ValuePtr->type = ( Tr069TypeArg ) ( paramValuePtr->type );

    if ( paramValuePtr->value == NULL )
        return;/*not normal */

    switch ( tr69ValuePtr->type )
    {
        case tString:
            tr69ValuePtr->value.string = strdup ( paramValuePtr->value );
            break;
        case tBool:
            if ( 0 == strcmp ( paramValuePtr->value, "1" ) || 0 == strcasecmp ( paramValuePtr->value, "true" ) )
            {
                tr69ValuePtr->value.boolv = 1;
            }
            else
            {
                tr69ValuePtr->value.boolv = 0;
            }
            break;
        case tInt:
            if ( 0 == strcasecmp ( paramValuePtr->value, "true" ) )
            {
                tr69ValuePtr->value.intv = 1;
            }
            else if ( 0 == strcasecmp ( paramValuePtr->value, "false" ) )
            {
                tr69ValuePtr->value.intv = 0;
            }
            else
            {
                tr69ValuePtr->value.intv =  atoi ( paramValuePtr->value );
            }
            break;
        case tUnsigned:
            tr69ValuePtr->value.uintv = atoi ( paramValuePtr->value );
            break;
    }
}

/***********************************************************************
Function: convert_local_name_to_soap
Description:  convert data to soap struct ,notice that *paramNameP maybe be a valid pointer,
  notice that , *paramNameP return the head pointer of list,
Return: void
Input: treeNameP,treeValueP
Output: paramNameP
Others:
************************************************************************/
void convert_local_name_to_soap ( IN Tr069NameArg ** treeNameP, OUT ParameterInfoStruct ** paramNameP )
{
    ParameterInfoStruct * paramNamePtr = NULL, *paramNameHead = NULL;
    Tr069NameArg *treeNamePtr = NULL;

    if ( *treeNameP == NULL )
    {
        return; /* not data to convert */
    }

    treeNamePtr = *treeNameP;

    if ( *paramNameP != NULL )
    {
        paramNameHead = *paramNameP;
        while ( paramNameHead->next != NULL )
        {
            paramNameHead = paramNameHead->next;
        }
    }

    while ( treeNamePtr != NULL )
    {
        paramNamePtr = ( ParameterInfoStruct * ) PAL_MALLOC ( sizeof ( ParameterInfoStruct ) );
        if ( paramNamePtr == NULL )
            return; /* not normal */
        memset ( paramNamePtr, 0, sizeof ( ParameterInfoStruct ) );

        strcpy ( paramNamePtr->name, treeNamePtr->name );
        paramNamePtr->writable = treeNamePtr->accessType;

        if ( *paramNameP == NULL )
        {
            *paramNameP = paramNamePtr;
            paramNameHead = paramNamePtr;
        }
        else if ( paramNameHead != NULL )
        {

            paramNameHead->next = paramNamePtr;
            paramNameHead = paramNamePtr;
        }
        else
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " An impossible thing occur" );
        }

        treeNamePtr = treeNamePtr->next;
    }

}


/***********************************************************************
Function: convert_soap_to_local_value
Description:  convert data to soap struct ,notice that *paramValueP maybe be a valid pointer,
  notice that , *paramValueP return the head pointer of list,
Return: void
Input: treeNameP,treeValueP
Output: paramValueP
Others:
************************************************************************/
void convert_local_value_to_soap ( IN Tr069NameArg ** treeNameP, IN Tr069ValueArg ** treeValueP, IN OUT ParameterValueStruct ** paramValueP )
{
    ParameterValueStruct * paramValuePtr = NULL, *paramValueHead = NULL;
    Tr069NameArg *treeNamePtr = NULL;
    Tr069ValueArg *treeValuePtr = NULL;

    if ( *treeNameP == NULL || *treeValueP == NULL )
    {
        return; /* not data to convert */
    }

    treeNamePtr = *treeNameP;
    treeValuePtr = *treeValueP;

    if ( *paramValueP != NULL )
    {
        paramValueHead = *paramValueP;
        while ( paramValueHead->next != NULL )
        {
            paramValueHead = paramValueHead->next;
        }
    }

    while ( treeNamePtr != NULL && treeValuePtr != NULL )
    {
        paramValuePtr = ( ParameterValueStruct * ) PAL_MALLOC ( sizeof ( ParameterValueStruct ) );
        if ( paramValuePtr == NULL )
            return; /* not normal */
        memset ( paramValuePtr, 0, sizeof ( ParameterValueStruct ) );

        strcpy ( paramValuePtr->name, treeNamePtr->name );
        switch ( treeValuePtr->type )
        {   /* change value stack to malloc mode */
            case tString:
                paramValuePtr->type = XSD_STRING;
                paramValuePtr->value = strdup ( treeValuePtr->value.string );
                break;
            case tInt:
                paramValuePtr->type = XSD_INT;
                paramValuePtr->value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
                if ( paramValuePtr->value != NULL )
                    sprintf ( paramValuePtr->value, "%d", treeValuePtr->value.intv );
                else
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, "\n====malloc failure===\n" );
                break;
            case tUnsigned:
                paramValuePtr->type = XSD_UINT;
                paramValuePtr->value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
                if ( paramValuePtr->value != NULL )
                    sprintf ( paramValuePtr->value, "%d", treeValuePtr->value.uintv );
                else
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, "\n====malloc failure===\n" );
                break;
            case tBool:
                paramValuePtr->type = XSD_BOOL;
                paramValuePtr->value = ( char * ) PAL_MALLOC ( MAX_NUMBER_LENGTH );
                if ( paramValuePtr->value != NULL )
                    sprintf ( paramValuePtr->value, "%d", treeValuePtr->value.boolv );
                else
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, "\n====malloc failure===\n" );
                break;
            default:
                paramValuePtr->type = XSD_STRING;
                paramValuePtr->value = ( char * ) PAL_MALLOC ( 1 );
                if ( paramValuePtr->value != NULL )
                    strcpy ( paramValuePtr->value, "" );
                else
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, "\n====malloc failure===\n" );
                break;
        }

        if ( *paramValueP == NULL )
        {
            *paramValueP = paramValuePtr;
            paramValueHead = paramValuePtr;
        }
        else if ( paramValueHead != NULL )
        {

            paramValueHead->next = paramValuePtr;
            paramValueHead = paramValuePtr;
        }
        else
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " An impossible thing occur" );
        }

        treeNamePtr = treeNamePtr->next;
        treeValuePtr = treeValuePtr->next;
    }

}


/***********************************************************************
Function: access_string_to_type
Description: convert data to soap struct,notice that , parameterAttributePtr return the head pointer of list
  notice that , parameterAttributePtr return the head pointer of list,
Return: void
Input: treeNameArgP ,treeAttributeArgP should keep pairs.
Output:parameterAttributeP
Others:
************************************************************************/
void convert_local_attribute_to_soap ( IN Tr069NameArg ** treeNameArgP, IN Tr069AttributeArg ** treeAttributeArgP, OUT ParameterAttributeStruct ** parameterAttributeP )
{
    ParameterAttributeStruct * parameterAttributeNext = NULL, *parameterAttributePtr = NULL;
    Tr069NameArg *treeNameArgPtr = NULL;
    Tr069AttributeArg *treeAttributeArgPtr = NULL;
    if ( *treeAttributeArgP == NULL || *treeNameArgP == NULL )
        return; /* not normal case. usually it's not NULL. */

    treeNameArgPtr = *treeNameArgP;
    treeAttributeArgPtr = *treeAttributeArgP;

    if ( *parameterAttributeP != NULL )
    {
        parameterAttributeNext = *parameterAttributeP;
        while ( parameterAttributeNext->next != NULL )
        {
            parameterAttributeNext = parameterAttributeNext->next;
        }
    }

    while ( treeNameArgPtr != NULL && treeAttributeArgPtr != NULL )
    {
        parameterAttributePtr = PAL_MALLOC ( sizeof ( ParameterAttributeStruct ) );
        if ( parameterAttributePtr == NULL )
            return;
        memset ( parameterAttributePtr, 0, sizeof ( ParameterAttributeStruct ) );

        strcpy ( parameterAttributePtr->accessList[0], treeAttributeArgPtr->accessList == ALL ? "Subscriber" : "" );
        strcpy ( parameterAttributePtr->name, treeNameArgPtr->name );
        parameterAttributePtr->notification = treeAttributeArgPtr->notification;

        if ( *parameterAttributeP == NULL )
        {
            *parameterAttributeP = parameterAttributePtr;
            parameterAttributeNext = parameterAttributePtr;
        }
        else if ( parameterAttributeNext != NULL )
        {
            parameterAttributeNext->next = parameterAttributePtr;
            parameterAttributeNext = parameterAttributePtr;
        }
        else
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " An impossible thing occur" );
        }

        treeNameArgPtr = treeNameArgPtr->next;
        treeAttributeArgPtr = treeAttributeArgPtr->next;
    }
}
/***********************************************************************
Function: handle_server_rpc_commands
Description:  main rpc commands handling function. it will get command and paramter from soapInfo,
and execute the command to get the result. after that, it will call generate_soap_packet function to generate
a soap buffer which ready to sent to remote acs server.

Return: Result,
Input:
        IN SOAP * soapInfo,
Output:
       OUT char * sendBuffer,
       OUT PAL_INT32_T * sendBufferLen,
Others:
************************************************************************/
DownloadResponseStruct g_downloadResponse;
int g_downloadFlag = 0;
Result handle_server_rpc_commands ( IN SOAP * soapInfo, OUT char  **sendBuffer, OUT PAL_UINT32_T * sendBufferLen )
{
    PAL_INT32_T status = 0;
    Result ret = TCMAGENT_OK;
    FaultType  faultCode = tOK;/* unify_later*/
    AddObjectStruct  *addObjectStructPtr = NULL;
    AddObjectResponseStruct  addObjectRes;
    DeleteObjectStruct  *deleteObjectPtr = NULL;
    ParameterSetAttributeStruct *setAttributePtr = NULL;
    Tr069AccessList tr69AccessType;
    GetParameterValueStruct *getAttributeValuePtr = NULL, *getAttributeValueHeadPtr = NULL; /*GetParamterValue and GetParameterAttribute struct same */
    ParameterAttributeStruct  * parameterAttributePtr = NULL; /* used to install response packet */
    Tr069NameArg *treeNameArgPtr = NULL;
    Tr069AttributeArg *treeAttributeArgPtr = NULL;
    Tr069ValueArg * treeValueArgPtr = NULL;
    ParameterListStruct *parameterListPtr = NULL;
    ParameterValueStruct *parameterValuePtr = NULL;
    GetParameterValueStruct  *getParameterValueHeadPtr = NULL, * getParameterValuePtr = NULL;
    ParameterNameStruct * getParameterNamesPtr = NULL;
    ParameterInfoStruct   *parameterNamesPtr = NULL;
    TransferListStruct   transferList;
    ScheduleInformStruct *scheduleInformPtr = NULL;
    ScheduleArg  localScheduleArg;
    UploadStruct *uploadPtr = NULL;
    UploadResponseStruct uploadResponse;
    UploadArg  uploadTaskArg;
    DownloadStruct *downloadPtr = NULL;
    DownloadResponseStruct downloadResponse;
    DownloadArg downloadTaskArg;
    char eventFileName[MAX_FILE_NAME_LENGTH];
    char *commandKey = NULL;
    ScheduleArg *scheduleArg = NULL;

    switch ( soapInfo->rpcType )
    {
        case tInformResponse:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method InformResponse\n" );

            /* synchronize the send envelopes */
            gMaxSendEnvelopes =  * ( ( PAL_UINT32_T * ) ( soapInfo->parameters ) );
            /*generate null packet and ready to send to acs */
            free_string_buffer ( sendBuffer, sendBufferLen );
            break;
        case tTransferCompleteResponse:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method TransferCompleteResponse\n" );

            /* other place process it , here, just ignore it*/
            free_string_buffer ( sendBuffer, sendBufferLen );
            break;
        case tAddObject:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method AddObject\n" );

            /*add object, get parameter from soapInfo */
            addObjectStructPtr = ( AddObjectStruct  * ) ( soapInfo->parameters );
            if ( addObjectStructPtr != NULL )
            {
                strcpy ( gLastParameterKey, addObjectStructPtr->parameterKey );   /* add,delete,set ,record the parameter key to the last commandkey */
                memset ( &addObjectRes, 0, sizeof ( AddObjectResponseStruct ) );

                status = gCB.add_object ( addObjectStructPtr->name, &addObjectRes.instanceID );

                if ( status == NOT_APPLIED || status == APPLIED )
                {
                    char name_tmp[PARAMETER_NAME_LEN + 1];

                    sprintf ( name_tmp, "%s%d.", addObjectStructPtr->name, addObjectRes.instanceID );

                    ret = add_tr069_object ( name_tmp, &addObjectRes.instanceID );

                    if ( ret != TCMAGENT_OK )
                    {
                        faultCode = result_to_fault ( ret );

                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                    }
                    else
                    {
                        generate_soap_packet ( ADD_OBJECT_RESP, tOK, NULL, &addObjectRes, sendBuffer, sendBufferLen );
                    }
                }
                else
                {
                    faultCode = tRequestDenied;

                    generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                }
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tDeleteObject:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method DeleteObject\n" );

            /*delete object, get parameter from soapInfo */
            deleteObjectPtr = ( DeleteObjectStruct  * ) ( soapInfo->parameters );
            if ( deleteObjectPtr != NULL )
            {
                strcpy ( gLastParameterKey, deleteObjectPtr->parameterKey );   /* add,delete,set ,record the parameter key to the last commandkey */

                status = gCB.delete_object ( deleteObjectPtr->name );

                if ( status == NOT_APPLIED || status == APPLIED )
                {
                    ret = delete_tr069_object ( deleteObjectPtr->name );

                    if ( ret != TCMAGENT_OK )
                    {
                        faultCode = result_to_fault ( ret );
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                    }
                    else
                    {
                        generate_soap_packet ( DELETE_OBJECT_RESP, tOK, NULL, &status, sendBuffer, sendBufferLen );
                    }
                }
                else
                {
                    faultCode = tRequestDenied;

                    generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                }
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tSetParameterAttributes:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method SetParameterAttributes\n" );

            /*SetParameterAttributes, get parameter from soapInfo */
            setAttributePtr = ( ParameterSetAttributeStruct  * ) ( soapInfo->parameters );
            if ( setAttributePtr != NULL )
            {
                for ( ;setAttributePtr != NULL; setAttributePtr = setAttributePtr->next )
                {
                    tr69AccessType = access_string_to_type ( setAttributePtr->accessList );
                    ret = set_tr069_attribute ( setAttributePtr->name, setAttributePtr->notificationChange, setAttributePtr->notification, setAttributePtr->accessListChange, tr69AccessType );
                    if ( ret != TCMAGENT_OK )
                    {
                        faultCode = result_to_fault ( ret );/* unify_later  */
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                        break; /* when an error occur, response server with error */
                    }
                    else
                    {
                        gSessionInfo.attribute_changed = 1;
                    }
                }
                if ( setAttributePtr == NULL )
                {
                    generate_soap_packet ( SET_PARAMETER_ATTRIBUTES_RESP, tOK, NULL, NULL, sendBuffer, sendBufferLen );
                }
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tGetParameterAttributes:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method GetParameterAttributes\n" );

            /*GetParameterAttributes, get parameter from soapInfo, treeNameArgPtr, treeAttributeArgPtr memory release here */
            getAttributeValueHeadPtr = ( GetParameterValueStruct  * ) ( soapInfo->parameters );
            if ( getAttributeValueHeadPtr != NULL )
            {
                for ( getAttributeValuePtr = getAttributeValueHeadPtr;getAttributeValuePtr != NULL; getAttributeValuePtr = getAttributeValuePtr->next )
                {
                    /* notice that ,name is empty, mean the topest entry */
                    ret = get_tr069_attribute ( getAttributeValuePtr->name, &treeNameArgPtr, &treeAttributeArgPtr );
                    if ( ret  != TCMAGENT_OK )
                    {
                        free_tr69_name_arg ( &treeNameArgPtr );
                        free_tr69_attribute_arg ( &treeAttributeArgPtr );
                        faultCode = result_to_fault ( ret );/* unify_later  */
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                        break;
                    }
                    else
                    {
                        /* notice that , parameterAttributePtr return the head pointer of list, */
                        convert_local_attribute_to_soap ( &treeNameArgPtr, &treeAttributeArgPtr, &parameterAttributePtr );
                        free_tr69_name_arg ( &treeNameArgPtr );
                        free_tr69_attribute_arg ( &treeAttributeArgPtr );
                    }
                }
                if ( getAttributeValuePtr == NULL )
                {
                    /*generate the soap packet */
                    generate_soap_packet ( GET_PARAMETER_ATTRIBUTES_RESP, tOK, NULL, parameterAttributePtr, sendBuffer, sendBufferLen );
                }
                getPrameterAttribute_free ( &parameterAttributePtr );
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tSetParameterValues:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method SetParameterValues\n" );

            /*SetParameterValues, get parameter from soapInfo */
            parameterListPtr = ( ParameterListStruct  * ) ( soapInfo->parameters );
            if ( parameterListPtr != NULL )
            {
                strcpy ( gLastParameterKey, parameterListPtr->parameterKey );/* add,delete,set ,record the parameter key to the last commandkey */
                for ( parameterValuePtr = parameterListPtr->paramterValues;parameterValuePtr != NULL; parameterValuePtr = parameterValuePtr->next )
                {
                    status =  gCB.set_value ( parameterValuePtr->name, parameterValuePtr->value );

                    if ( status != NOT_APPLIED && status != APPLIED )
                    {
                        faultCode = tInvalidParameterValue;/* unify_later  */
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " error: %d\n", status );
                        break; /* when an error occur, response server with error */
                    }
                    else
                    {
                        convert_soap_to_local_value ( parameterValuePtr, &treeValueArgPtr );/* notice that treeValueArgPtr should be single member not list */
                        ret = set_tr069_value ( ACS_MODULE, parameterValuePtr->name, treeValueArgPtr );

                        //------gavin added for wan mode switch ---------
                        if ( strcmp ( parameterValuePtr->name, WAN_IPCONNECTION_FLAG_NAME ) == 0
                                && treeValueArgPtr->value.boolv == 1 )
                        {
                            printf ( "\n===detect [%s],enable==\n", WAN_IPCONNECTION_FLAG_NAME );
                            treeValueArgPtr->value.boolv = 0;
                            ret = set_tr069_value ( ACS_MODULE, WAN_PPPCONNECTION_FLAG_NAME, treeValueArgPtr );
                        }
                        else if ( strcmp ( parameterValuePtr->name, WAN_PPPCONNECTION_FLAG_NAME ) == 0
                                  && treeValueArgPtr->value.boolv == 1 )
                        {
                            printf ( "\n===detect [%s],enable==\n", WAN_PPPCONNECTION_FLAG_NAME );
                            treeValueArgPtr->value.boolv = 0;
                            ret = set_tr069_value ( ACS_MODULE, WAN_IPCONNECTION_FLAG_NAME, treeValueArgPtr );
                        }
                        //-----------------------------------------

                        free_tr69_value_arg ( &treeValueArgPtr );
                        treeValueArgPtr = NULL;

                        if ( ret != TCMAGENT_OK )
                        {
                            faultCode = result_to_fault ( ret );/* unify_later  */
                            generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                            break; /* when an error occur, response server with error */
                        }
                    }
                }
                if ( parameterValuePtr == NULL )
                {
                    generate_soap_packet ( SET_PARAMETER_VALUES_RESP, tOK, NULL, &status, sendBuffer, sendBufferLen );
                }
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tGetParameterValues:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method GetParameterValues\n" );

            /*tGetParameterValues, get parameter from soapInfo, treeNameArgPtr, treeValueArgPtr memory release here */
            getParameterValueHeadPtr = ( GetParameterValueStruct  * ) ( soapInfo->parameters );
            if ( getParameterValueHeadPtr != NULL )
            {
                for ( getParameterValuePtr = getParameterValueHeadPtr;getParameterValuePtr != NULL; getParameterValuePtr = getParameterValuePtr->next )
                {
                    /* notice that ,name is empty, mean the topest entry */
                    ret = get_tr069_value ( getParameterValuePtr->name, &treeNameArgPtr, &treeValueArgPtr );

                    if ( ret  != TCMAGENT_OK )
                    {
                        free_tr69_name_arg ( &treeNameArgPtr );
                        free_tr69_value_arg ( &treeValueArgPtr );
                        faultCode = result_to_fault ( ret );/* unify_later  */
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                        break;
                    }
                    else
                    {
                        /* change node value for writeonly and attribute */
                        node_value_convertion ( treeNameArgPtr, treeValueArgPtr );
                        /* notice that , parameterValuePtr return the head pointer of list, */
                        convert_local_value_to_soap ( &treeNameArgPtr, &treeValueArgPtr, &parameterValuePtr );/* convert data to soap struct  */

                        free_tr69_name_arg ( &treeNameArgPtr );
                        free_tr69_value_arg ( &treeValueArgPtr );
                    }
                }
                if ( getParameterValuePtr == NULL )
                {
                    /*generate the soap packet */
                    generate_soap_packet ( GET_PARAMETER_VALUES_RESP, tOK, NULL, parameterValuePtr, sendBuffer, sendBufferLen );
                }
                getParameterValues_free ( &parameterValuePtr );
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tGetParameterNames:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method GetParameterNames\n" );

            /*GetParameterAttributes, get parameter from soapInfo, treeNameArgPtr, treeValueArgPtr memory release here */
            getParameterNamesPtr = ( ParameterNameStruct  * ) ( soapInfo->parameters );
            if ( getParameterNamesPtr != NULL )
            {
                ret = get_tr069_name ( getParameterNamesPtr->name, & treeNameArgPtr, getParameterNamesPtr->nextLevel );
                if ( ret  != TCMAGENT_OK )
                {
                    free_tr69_name_arg ( &treeNameArgPtr );

                    faultCode = tInvalidParameterName;
                    generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                    break;
                }
                else
                {
                    /*   change write and read attribute for node */
                    node_rw_convertion ( treeNameArgPtr );
                    /* notice that , parameterNamesPtr return the head pointer of list, */
                    convert_local_name_to_soap ( &treeNameArgPtr, &parameterNamesPtr );/* convert data to soap struct ,and release tree Name memory */
                    generate_soap_packet ( GET_PARAMETER_NAMES_RESP, tOK, NULL, parameterNamesPtr, sendBuffer, sendBufferLen );
                    getParameterNames_free ( &parameterNamesPtr );
                    free_tr69_name_arg ( &treeNameArgPtr ); /* fix memory leak */

                }
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }

            break;
        case tGetRPCMethods:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method GetRPCMethods\n" );

            generate_soap_packet ( GET_RPCMETHODS_RESP, tOK, NULL, NULL, sendBuffer, sendBufferLen );
            break;
        case tGetQueuedTransfer:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method GetQueuedTransfer\n" );

            get_transfer_list ( &transferList ); /* look up the transfer directory, get the updatest status */
            generate_soap_packet ( GET_QUEUEDTRANSFERS_RESP, tOK, NULL, &transferList, sendBuffer, sendBufferLen );
            break;
        case tScheduleInform:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method ScheduleInform\n" );

            scheduleInformPtr = ( ScheduleInformStruct * ) ( soapInfo->parameters );
            /* add a event of scheduleInform */
            convert_schedule_to_local_event ( scheduleInformPtr, eventFileName, &localScheduleArg );

            if ( NULL == ( scheduleArg = PAL_MALLOC ( sizeof ( ScheduleArg ) ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " malloc failure!\n" );
            }
            else
            {
                memcpy ( scheduleArg, &localScheduleArg, sizeof ( ScheduleArg ) );
                ret = add_event ( scheduleInformPtr->delaySeconds, SCHEDULE, eventFileName, scheduleArg );
            }

            /* write file to avoid reboot and information lost ,It will be applied to upload,download,scheduleInform*/
            store_schedule_event ( &localScheduleArg );

            /* generate the response packet */
            generate_soap_packet ( SCHEDULE_INFORM_RESP, tOK, NULL, NULL, sendBuffer, sendBufferLen );
            break;

        case tUpload:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method Upload\n" );

            uploadPtr = ( UploadStruct  * ) ( soapInfo->parameters );
            if ( uploadPtr != NULL )
            {
                printf ( "\n===upload delayp[%d],url[%s],filetype[%s]==\n", uploadPtr->delaySeconds, uploadPtr->URL, uploadPtr->fileType );
                /* process upload */
                if ( uploadPtr->delaySeconds == 0 )
                {
                    /* upload file immediately, then response */
                    ret = do_upload_immediate ( uploadPtr, &uploadResponse );
                    if ( ret != TCMAGENT_OK )
                    {
                        faultCode = result_to_fault ( ret );/* unify_later  */
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );

                    }
                    else
                    {
                        generate_soap_packet ( UPLOAD_RESP, tOK, NULL, &uploadResponse, sendBuffer, sendBufferLen );
                    }

                }
                else
                {
                    UploadArg *uploadArg = NULL;

                    /* add a task to do transfer later */
                    convert_upload_to_local ( uploadPtr, &uploadTaskArg );

                    if ( NULL == ( uploadArg = PAL_MALLOC ( sizeof ( UploadArg ) ) ) )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " malloc failure!\n" );
                    }
                    else
                    {
                        memcpy ( uploadArg, &uploadTaskArg, sizeof ( UploadArg ) );
                        add_event ( uploadPtr->delaySeconds, UPLOAD, uploadTaskArg.fileName, uploadArg );
                    }
                    store_upload_event ( &uploadTaskArg );/* store the task to the disk */

                    /* generate the response packet */
                    uploadResponse.status = NOT_APPLIED;
                    strcpy ( uploadResponse.startTime, ZERO_DATE_TIME );
                    strcpy ( uploadResponse.completeTime, ZERO_DATE_TIME );
                    generate_soap_packet ( UPLOAD_RESP, tOK, NULL, &uploadResponse, sendBuffer, sendBufferLen );
                }
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tDownload:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method Download\n" );
            downloadPtr = ( DownloadStruct  * ) ( soapInfo->parameters );

            if ( downloadPtr != NULL )
            {
            #if 0
                /* process download */
                if ( downloadPtr->delaySeconds == 0 )
                {
                    /* download file immediately, then response */
                    ret = do_download_immediate ( downloadPtr, &downloadResponse );

                    if ( ret != TCMAGENT_OK )
                    {
                        faultCode = result_to_fault ( ret );
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                    }
                    else
                    {
                        generate_soap_packet ( DOWNLOAD_RESP, tOK, NULL, &downloadResponse, sendBuffer, sendBufferLen );
                    }

                }
			#endif
			    if ( downloadPtr->delaySeconds == 0 && (strcmp( downloadPtr->fileType, DOWNLOAD_CONFIG_TYPE)==0 || strcmp( downloadPtr->fileType, DOWNLOAD_FIRMWARE_TYPE)==0 || strcmp( downloadPtr->fileType, DOWNLOAD_WEBCONTENT_TYPE)==0) )
                { 
                    /* download file immediately, then response */
					if( strcmp( downloadPtr->fileType, DOWNLOAD_FIRMWARE_TYPE)==0 && g_downloadFlag == 0 )
					{
                        ret = do_download_immediate ( downloadPtr, &downloadResponse );
					    memcpy( &g_downloadResponse, &downloadResponse, sizeof(DownloadResponseStruct) );
					    printf( "g_response.status %d, g_response.startTime %s g_response.completeTime %s\n",  g_downloadResponse.status, g_downloadResponse.startTime, g_downloadResponse.completeTime );					
					}
					else if( strcmp( downloadPtr->fileType, DOWNLOAD_FIRMWARE_TYPE)==0 && g_downloadFlag == 1 )
					{
					    ret = TCMAGENT_OK;
					}
					else
					{
					    ret = do_download_immediate ( downloadPtr, &downloadResponse );
					}
					
                    if ( ret != TCMAGENT_OK )
                    { 
                        faultCode = result_to_fault ( ret );
                        generate_soap_packet ( FAULT, faultCode, NULL, NULL, sendBuffer, sendBufferLen );
                    }
                    else
                    {
       					if( strcmp( downloadPtr->fileType, DOWNLOAD_FIRMWARE_TYPE)==0 && g_downloadFlag == 0 )
       					{
                            generate_soap_packet ( DOWNLOAD_RESP, tOK, NULL, &downloadResponse, sendBuffer, sendBufferLen );
						    g_downloadFlag = 1;
       					}
					    else if( strcmp( downloadPtr->fileType, DOWNLOAD_FIRMWARE_TYPE)==0 && g_downloadFlag == 1 )
						{
						    generate_soap_packet ( DOWNLOAD_RESP, tOK, NULL, &g_downloadResponse, sendBuffer, sendBufferLen );
						    g_downloadFlag = 0;			
						}
					    else if( strcmp( downloadPtr->fileType, DOWNLOAD_CONFIG_TYPE)==0 )
					    {			    
                            generate_soap_packet ( DOWNLOAD_RESP, tOK, NULL, &downloadResponse, sendBuffer, sendBufferLen );
						    gReboot = 1;
					    }
						else
						{
                            generate_soap_packet ( DOWNLOAD_RESP, tOK, NULL, &downloadResponse, sendBuffer, sendBufferLen );						
						}
                    }
                }
                else
                {
                    DownloadArg *downloadArg = NULL;
                    PAL_TIME_T time;

                    /* add a task to do transfer later */
                    convert_dowload_to_local ( downloadPtr, &downloadTaskArg );

                    if ( NULL == ( downloadArg = PAL_MALLOC ( sizeof ( DownloadArg ) ) ) )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " malloc failure!\n" );
                    }
                    else
                    {
                        memcpy ( downloadArg, &downloadTaskArg, sizeof ( DownloadArg ) );

                        add_event ( downloadPtr->delaySeconds, DOWNLOAD, downloadTaskArg.fileName, downloadArg );
                    }

                    store_download_event ( &downloadTaskArg );/* store the task to the disk */

                    /* generate the response packet */
                    downloadResponse.status = NOT_APPLIED;

                    if  ( pal_time_current ( &time ) )
                    {
                        time += ( PAL_TIME_T ) downloadPtr->delaySeconds;

                        pal_time_format ( time, downloadResponse.startTime );
                        pal_time_format ( time, downloadResponse.completeTime );
                    }
                    else
                    {
                        strcpy ( downloadResponse.startTime, ZERO_DATE_TIME );
                        strcpy ( downloadResponse.completeTime, ZERO_DATE_TIME );
                    }

                    generate_soap_packet ( DOWNLOAD_RESP, tOK, NULL, &downloadResponse, sendBuffer, sendBufferLen );
                }
            }
            else
            {
                generate_soap_packet ( FAULT, tInvalidArguments, NULL, NULL, sendBuffer, sendBufferLen );
            }
            break;
        case tReboot:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method Reboot\n" );

            commandKey = ( char  * ) ( soapInfo->parameters );
            gReboot = 1;
            /* add a cause to event code ,M Reboot */
            store_reboot_event ( commandKey );

            generate_soap_packet ( REBOOT_RESP, tOK, NULL, NULL, sendBuffer, sendBufferLen );
            break;
        case tFactoryReset:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method FactoryReset\n" );

            gCB.factory_reset();
            gReboot = 1;

            delete_from_task_dir ( NO_BOOTSTRAP_EVENTCODE_FILE );
            generate_soap_packet ( FACTORY_RESET_RESP, tOK, NULL, NULL, sendBuffer, sendBufferLen );
            break;
        case tFault:
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " get rpc method Fault\n" );

            /* send empty packet to end the session so do nothing, keep sendBuffer as Empty */
            break;
        default:
            /* send empty packet to end the session so do nothing ,keep sendBuffer as Empty */
            break;
    }

    return ret;
}

void update_request_download_eventCode ( void )
{
    char buf[PARAMETER_NAME_LEN];
    int type;
    FILE *fp;

    if ( access ( TR69_REQUEST_DOWNLOAD_FILE, R_OK ) == 0 )
    {
        if ( NULL != ( fp = fopen ( TR69_REQUEST_DOWNLOAD_FILE, "r" ) ) )
        {
            if ( fgets ( buf, sizeof ( buf ), fp ) >= 0 )
            {
                if ( buf[strlen ( buf ) - 1] == '\n' )
                {
                    buf[strlen ( buf ) - 1] = '\0';
                }

                if ( 0 < ( type = atoi ( buf ) ) )
                {
                    if ( FIRMWARE <= type && type <= VENDOR_CONFIGURATION_FILE )
                    {
                        gEventCode |= EVENT_REQUEST_DOWNLOAD;
                        gRequestDownloadType = type;

                        if ( WEB_CONTENT == type )
                        {
                            if ( fgets ( buf, sizeof ( buf ), fp ) >= 0 )
                            {
                                if ( buf[strlen ( buf ) - 1] == '\n' )
                                {
                                    buf[strlen ( buf ) - 1] = '\0';
                                }

                                strcpy ( gRequestDownloadArg, buf );
                            }
                            else
                            {
                                strcpy ( gRequestDownloadArg, "1.0" );
                            }
                        }
                    }
                }
            }

            fclose ( fp );

        }
    }
}

/***********************************************************************
Function: main_session
Description:  The main session Entry function, normally, it will not return and will run circulately

Return: Result,
Input: void,
Output:
Others:
************************************************************************/
Result main_session()
{
    Result ret;
    InformInfoStruct  informContent;
    TransferCompleteList * transferCompletePtr = NULL;
    char *sSoapBuf = NULL, *rSoapBuf = NULL;/*soap buffer for sending and receving */
    PAL_UINT32_T  sSoapBufLength = 0, rSoapBufLength = 0;/*received soap content length or sending content length */
    SOAP  rSoapInfo;
    PAL_INT32_T  i, j;
    char acsURL[MAX_USERNAME_LENGTH+1];
    unsigned int rest_time = 1;

    init_global_keys();
    memset ( &informContent, 0, sizeof ( informContent ) );
    memset ( &rSoapInfo, 0, sizeof ( SOAP ) );

    //pal_mpool_show_stutus();//debug memory
    //sleep(5);//debug memory

    while ( 1 )
    {
        /* step 0, ready and wait sendinform packet semaphore  */
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 0, ready and wait sendinform packet semaphore.\n" );
        wait_wake_up_main_session();

        /*step 1 ,initialize socket for sending and receiving packets. If failed, tryTimes adding 1 every time.*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 1 ,initialize socket for sending and receiving packets.\n" );
        gCB.session_begin();

        memset ( &gSessionInfo, 0, sizeof ( gSessionInfo ) );

        update_request_download_eventCode ();

        gRetryTimes = 0;
sessionhead_tag:

        free_string_buffer ( &sSoapBuf, &sSoapBufLength );
        free_string_buffer ( &rSoapBuf, &rSoapBufLength );

        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, "\n====a new loop===\n" );
        //pal_mpool_show_stutus();//debug memory
        //sleep(5);//debug memory

        while ( gRetryTimes < MAX_RETRY_TIMES )
        {
            ret = init_main_session_socket ( gManagementServer.acsURL );
            if ( ret != TCMAGENT_OK )
            {
                gRetryTimes++;
                for ( j = 1, rest_time = 1;j <= gRetryTimes;j++ )
                    rest_time *= 2;
                pal_sleep ( rest_time );
            }
            else
                break;
        }

        if ( gRetryTimes >= MAX_RETRY_TIMES )
        {
            /*  network or acs server maybe fail */
            goto sessiontail_tag;  //continue to wait next chance.
        }

        /*step 2 ,update Inform variables from parameter tree*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 2 ,update Inform variables from parameter tree.\n" );

        memset ( &informContent, 0, sizeof ( informContent ) );

        ret = update_inform_content ( &informContent );

        if ( ret != TCMAGENT_OK ) /* unexpected error occurs  */
            goto sessiontail_tag;

        /*step 3 ,generate soap data of inform packet*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 3 ,generate soap data of inform packet.\n" );

        ret = generate_soap_packet ( SEND_INFORM, tOK, NULL, &informContent, &sSoapBuf, &sSoapBufLength );
        free_inform_parameter_list ( &informContent , 0 );

        if ( ret != TCMAGENT_OK ) /* unexpected error occurs  */
        {
            destroy_main_session_socket();
            goto sessionhead_tag;
        }

        /*step 4, send out the inform soap data through socket to acs server, then receiving response. If any error occurs,close socket and go step 1 */
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 4, send out the inform soap data through socket to acs server, then receiving response.\n" );

        ret = send_packet_to_acs_server ( sSoapBuf, sSoapBufLength, &rSoapBuf, &rSoapBufLength );
        free_string_buffer ( &sSoapBuf, &sSoapBufLength );

        if ( ret != TCMAGENT_OK || rSoapBufLength == 0 )
        {
            free_string_buffer ( &rSoapBuf, &rSoapBufLength );//add for avoid memory leak
            gRetryTimes++;
            goto sessionhead_tag;
        }

        /*step 5, handle the correct response packet with soap interface, update MAX_ACS_EVELOPES, holdRequest ,etc. information, also set some variables*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 5, handle the correct response packet with soap interface.\n" );

        for ( i = 0;i < 3;i++ )
        {

            ret = handle_soap_packet ( rSoapBuf, rSoapBufLength, &rSoapInfo, &sSoapBuf, &sSoapBufLength );

            free_string_buffer ( &rSoapBuf, &rSoapBufLength );

            if ( ret == TCMAGENT_OK && rSoapInfo.rpcType == tInformResponse )
            {

                gRetryTimes = 0;
                if ( ( gEventCode & EVENT_BOOTSTRAP ) != 0 )
                    store_nobootstrap_event();
                if ( ( gEventCode & EVENT_M_REBOOT ) != 0 )
                    delete_from_task_dir ( MREBOOT_EVENT_FILE );
                /*
                if ((gEventCode & EVENT_REQUEST_DOWNLOAD) != 0)
                    delete_from_task_dir(REQUEST_DOWNLOAD_FILE);
                  */

                reset_event_code();
                break;
            }
            else if ( ret == SOAP_PARSER_ERROR && sSoapBufLength > 0 )
            {

                ret = send_packet_to_acs_server ( sSoapBuf, sSoapBufLength, &rSoapBuf, &rSoapBufLength );
                free_string_buffer ( &sSoapBuf, &sSoapBufLength );
                if ( ret != TCMAGENT_OK )
                {
                    destroy_main_session_socket();
                    goto sessionhead_tag;
                }
            }
            else /* if inform response is fault packet or not correct packet, then restart it form original */
            {

                destroy_main_session_socket();
                goto sessionhead_tag;
            }
        }

        if ( i == 3 )
            goto sessionhead_tag; /*error format packet is received */

        /*step 6, look up download,upload finished event, if has events, send TransferComplete packets. Notice the holdRequst actions and send multi transferComplete*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 6, look up download,upload finished event, if has events, send TransferComplete packets. \n" );

        while ( gHoldRequest != 1 && check_transfer_done_event() > 0 )
        {
            update_transfer_complete_content ( &transferCompletePtr, gMaxSendEnvelopes );
            ret = generate_soap_packet ( TRANSFER_COMPLETE, tOK, NULL, transferCompletePtr, &sSoapBuf, &sSoapBufLength );

            if ( ret != TCMAGENT_OK )
                break;  /* something wrong */

            ret = send_packet_to_acs_server ( sSoapBuf, sSoapBufLength, &rSoapBuf, &rSoapBufLength );
            free_string_buffer ( &sSoapBuf, &sSoapBufLength );
            if ( ret != TCMAGENT_OK )
            {
                destroy_main_session_socket();
                goto sessionhead_tag;
            }

            ret = handle_soap_packet ( rSoapBuf, rSoapBufLength, &rSoapInfo, &sSoapBuf, &sSoapBufLength );
            free_string_buffer ( &rSoapBuf, &rSoapBufLength );
            if ( ret == TCMAGENT_OK && rSoapInfo.rpcType == tTransferCompleteResponse )
            {
                remove_transfer_done_event ( transferCompletePtr );
                free_transfer_complete_list ( &transferCompletePtr );
            }
            else
                break; /* error packet received.. */
        }
        free_transfer_complete_list ( &transferCompletePtr );
        free_string_buffer ( &sSoapBuf, &sSoapBufLength );

        if ( gRequestDownloadType && gHoldRequest != 1 )
        {
            RequestDownloadStruct requestDownload;

            memset ( &requestDownload, 0, sizeof ( requestDownload ) );

            if ( VENDOR_CONFIGURATION_FILE == gRequestDownloadType )
            {
                strcpy ( requestDownload.fileType, "3 Vendor Configuration File" );
            }
            else if ( WEB_CONTENT == gRequestDownloadType )
            {
                strcpy ( requestDownload.fileType, "2 Web Content" );
                strcpy ( requestDownload.arglist[0].name, "Version" );
                strcpy ( requestDownload.arglist[0].value, gRequestDownloadArg );
            }
            else
            {
                strcpy ( requestDownload.fileType, "1 Firmware Upgrade" );
            }

            ret = generate_soap_packet ( REQUEST_DOWNLOAD, tOK, NULL, &requestDownload, &sSoapBuf, &sSoapBufLength );

            if ( ret != TCMAGENT_OK )
                break;  /* something wrong */

            ret = send_packet_to_acs_server ( sSoapBuf, sSoapBufLength, &rSoapBuf, &rSoapBufLength );
            free_string_buffer ( &sSoapBuf, &sSoapBufLength );
            if ( ret != TCMAGENT_OK )
            {
                destroy_main_session_socket();
                goto sessionhead_tag;
            }

            ret = handle_soap_packet ( rSoapBuf, rSoapBufLength, &rSoapInfo, &sSoapBuf, &sSoapBufLength );
            free_string_buffer ( &rSoapBuf, &rSoapBufLength );
            if ( ret == TCMAGENT_OK && rSoapInfo.rpcType == tRequestDownloadResponse )
            {
                gRequestDownloadType = 0;
                delete_from_task_dir ( REQUEST_DOWNLOAD_FILE );
            }
            else
                break; /* error packet received.. */
        }
        free_string_buffer ( &sSoapBuf, &sSoapBufLength );

        /*step 7, when no transcomplete packet sending, and send null packets to ACS, and receiving packets*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 7, when no transcomplete packet sending, and send null packets to ACS, and receiving packets. \n" );

        ret = send_packet_to_acs_server ( NULL, 0, &rSoapBuf, &rSoapBufLength );

        if ( ret != TCMAGENT_OK )
        {
            destroy_main_session_socket();
            goto sessionhead_tag;
        }

        /*step 8, handle the acs server's packet with RPC command, and reply to it. It's also a server command handle loop, when receiving null packet or no more request, or socket failed, it will exit the loop*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 8, handle the acs server's packet with RPC command, and reply to it. \n" );

        do
        {
            if ( rSoapBufLength == 0 )   /* server empty packet is received */
            {
                break;
            }

            ret = handle_soap_packet ( rSoapBuf, rSoapBufLength, &rSoapInfo, &sSoapBuf, &sSoapBufLength );

            free_string_buffer ( &rSoapBuf, &rSoapBufLength );


            if ( gNoMoreRequests == 1 )
                break;
            else if ( ret == TCMAGENT_OK )
            {
                /* main RPC command handling process */
                handle_server_rpc_commands ( &rSoapInfo, &sSoapBuf, &sSoapBufLength );

                if ( gUpgradeFirmare > 0 )   //gavin  for upgrade firmware
                {
                    gUpgradeFirmare = 0;
                    destroy_main_session_socket();
                    gRetryTimes = 0;
                    while ( gRetryTimes < MAX_RETRY_TIMES )
                    {
                        ret = init_main_session_socket ( gManagementServer.acsURL );
                        if ( ret != TCMAGENT_OK )
                        {
                            gRetryTimes++;
                            pal_sleep ( gRetryTimes );
                        }
                        else
                            break;
                    }
                }

                /*send response to server and receive new packet */
                ret = send_packet_to_acs_server ( sSoapBuf, sSoapBufLength, &rSoapBuf, &rSoapBufLength );

                free_string_buffer ( &sSoapBuf, &sSoapBufLength );

                if ( ret != TCMAGENT_OK )
                {
                    destroy_main_session_socket();
                    goto sessionhead_tag;
                }
            }
            else if ( sSoapBufLength > 0 )
            {
                ret = send_packet_to_acs_server ( sSoapBuf, sSoapBufLength, &rSoapBuf, &rSoapBufLength );
                free_string_buffer ( &sSoapBuf, &sSoapBufLength );
                if ( ret != TCMAGENT_OK )
                {
                    destroy_main_session_socket();
                    goto sessionhead_tag;
                }
            }
            else
            {
                destroy_main_session_socket();
                goto sessionhead_tag;
            }

        }
        while ( 1 );

        /*step 9, close the socket and  wait another chance to run, add a timeout inform if inform enable, then go to step 0*/
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " step 9, close the socket and  wait another chance to run, add a timeout inform if inform enable, then go to step 0 \n" );

sessiontail_tag:
        free_string_buffer ( &sSoapBuf, &sSoapBufLength );
        free_string_buffer ( &rSoapBuf, &rSoapBufLength );
        destroy_main_session_socket();
        free_soap ( &rSoapInfo );

        gCB.session_end ( &gSessionInfo );/* callback for user to do something here */

        //pal_mpool_show_stutus();//debug memory

        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " main loop session end.\n" );

        if ( gFactoryReset > 0 )
        {
            gFactoryReset = 0;
            gCB.factory_reset();
            gCB.do_reboot();
        }

        if ( gReboot > 0 )
        {
            gReboot = 0;
            gCB.do_reboot();
        }

        strcpy ( acsURL, gManagementServer.acsURL );
        update_managementServer_variables();
        if ( strcmp ( acsURL, gManagementServer.acsURL ) != 0 )
        {
            printf ( "ACS url has been changed by acs server,old[%s],new[%s]", acsURL, gManagementServer.acsURL );
            delete_from_task_dir ( NO_BOOTSTRAP_EVENTCODE_FILE );
            gEventCode |= EVENT_BOOTSTRAP;
        }

        if ( gManagementServer.periodicInformEnable > 0 )
        {
            InformArg *arg;

            /* add a periodicInformInterval inform event to wake up the main session when timeout */
            /* add the inform event to event list, and set time to NOW to wake up the main loop */
            if ( NULL == ( arg = PAL_MALLOC ( sizeof ( InformArg ) ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_MAIN_LOOP, __func__, " malloc failure!\n" );
            }
            else
            {
                memset ( arg, 0, sizeof ( InformArg ) );

                arg->event = EVENT_PERIODIC;

                add_event ( gManagementServer.periodicInformInterval , INFORM, INFORM_EVENT_NAME, arg );
            }
        }
    }

    return TCMAGENT_OK;
}




