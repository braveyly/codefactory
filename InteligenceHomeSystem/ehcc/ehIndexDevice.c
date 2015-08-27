#include "ehIndexDevice.h"

pthread_mutex_t g_devQueueMutex = PTHREAD_MUTEX_INITIALIZER;
DeviceConnInfo g_devArray[MAXTABLE_DEVICES];

int getEhMaxFd()
{
    int i = 0, fdtmp = gehServerFd;
    for ( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
        if ( g_devArray[i].connfd != ZERO_FD )
        {
            fdtmp = (fdtmp > g_devArray[i].connfd)?fdtmp:g_devArray[i].connfd;
        }
    }

    return fdtmp;
}

tcmRet zeroIndexDevice ( int devIndex )
{
    if ( devIndex < 1 || devIndex > MAXTABLE_DEVICES )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

#ifdef SSL_CONNECT
    if ( g_devArray[devIndex - 1].ssl != ZERO_SSL )
    {
        SSL_shutdown ( g_devArray[devIndex - 1].ssl );
        SSL_free ( g_devArray[devIndex - 1].ssl );
    }
#endif

    if ( g_devArray[devIndex - 1].connfd != ZERO_FD )
    {
        FD_CLR ( g_devArray[devIndex - 1].connfd, &readFdsMaster );
        close ( g_devArray[devIndex - 1].connfd );
     }

    g_devArray[devIndex - 1].ssl = ZERO_SSL;
    g_devArray[devIndex - 1].connfd = ZERO_FD;
    gehMaxFd = getEhMaxFd();
    return TCMRET_SUCCESS;
}

void showIndexDeviceArray()
{
    int i;
    for ( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
        if ( g_devArray[i].connfd > 0 )
            tcmLog_debug ( "Device index %d: connfd %d", i + 1, g_devArray[i].connfd );
    }
}

int getDeviceIndexByFd ( int fd )
{
    int i;
    for ( i = 0 ; i < MAXTABLE_DEVICES; i++ )
    {
        if ( g_devArray[i].connfd == fd )
        {
            return i + 1;
        }
    }
    
    return INVALID_DEVICE_INDEX;
}

int getDeviceIndexByMac ( char* arMac )
{
    if ( arMac == NULL )
    {
        return INVALID_DEVICE_INDEX;
    }

    char devMac[MAX_VALUE_LEN + 1];
    int i;
    for ( i = 0 ; i < MAXTABLE_DEVICES; i++ )
    {
        if ( !capi_read_table_parameter ( ID_UNIT_INFO, i + 1, TAB_PARA_UNIT_MAC_ADDRESS,
                                          devMac ) )
        {
            if ( strcasecmp ( devMac, arMac ) == 0 ) 
            {
                return i + 1;
            }
        }
    }
    return INVALID_DEVICE_INDEX;
}

int getDeviceIndexByNumber ( char* num )
{
    if ( num == NULL )
    {
        tcmLog_notice ( "The number of device cannot be NULL" );
        return INVALID_DEVICE_INDEX;
    }

    int i;
    char NumberTmp[MAX_VALUE_LEN + 1];
    for ( i = 0 ; i < MAXTABLE_DEVICES; i++ )
    {
        if ( !capi_read_table_parameter ( ID_UNIT_INFO, i + 1, TAB_PARA_UNIT_ACCOUNT,
                                          NumberTmp ) )
        {
            if ( strcasecmp ( NumberTmp, num ) == 0 ) 
            {
                return i + 1;
            }
        }
    }

    return INVALID_DEVICE_INDEX;
}

DeviceInfo* getDeviceInfoByDeviceIndex( int devIndex )
{
    if ( devIndex <= 0 || devIndex > MAXTABLE_DEVICES )
    {
        return NULL;
    }

    char   app_result[TAB_PARA_UNIT_INFO_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_UNIT_INFO_END-1];
    int i = 0;

    DEVICETYPE type;
    DEVICESTATUS status;
    DEVICESUBTYPE deviceSubType;
    int failedPairTimes;

    for ( i = 0; i < TAB_PARA_UNIT_INFO_END - 1; i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }

    if ( capi_read_table_row ( ID_UNIT_INFO, devIndex , p_result ) )
    {
        return NULL;;
    }

    type = actDevice_type_getIDByString(p_result[TAB_PARA_UNIT_TYPE-1]);
    deviceSubType = actDevice_subType_getIDByString(p_result[TAB_PARA_UNIT_SUB_TYPE-1]);
    status = actDevice_status_getIDByString( p_result[TAB_PARA_UNIT_STATE-1] );

    if ( strlen( p_result[TAB_PARA_UNIT_LOGIN_FAILURE_TIMES-1] ) )
    {
        failedPairTimes = atoi ( p_result[TAB_PARA_UNIT_LOGIN_FAILURE_TIMES-1] );
    }
    else
    {
        failedPairTimes = 0;
    }

    DeviceInfo* deviceTmp = createDevice (
                             p_result[TAB_PARA_UNIT_MAC_ADDRESS-1],
                             p_result[TAB_PARA_UNIT_IP_ADDRESS-1],
                             type, deviceSubType, status, 
                             p_result[TAB_PARA_UNIT_NAME-1],
                             p_result[TAB_PARA_UNIT_ACCOUNT-1],
                             p_result[TAB_PARA_UNIT_PASSWORD-1],
                             failedPairTimes,
                             g_devArray[devIndex - 1].connfd, 
                             g_devArray[devIndex - 1].ssl );

    return deviceTmp;
}

DeviceInfo* getDeviceInfoByMac ( char* mac )
{
    if ( mac == NULL || strlen ( mac ) == 0 )
    {
        tcmLog_notice ( "The MAC address of device can not be NULL or empty" );
        return NULL;
    }

    return getDeviceInfoByDeviceIndex(getDeviceIndexByMac(mac));
}

DeviceInfo* getDeviceInfoByFd ( int devFd )
{
    if ( devFd <= ZERO_FD )
    {
        tcmLog_notice ( "The connfd is invalid" );
        return NULL;
    }

    return getDeviceInfoByDeviceIndex( getDeviceIndexByFd(devFd) );
}

DeviceInfo* getDeviceInfoByNumber ( char* num )
{
    if ( num == NULL ) 
    {
        tcmLog_notice( "Number can not be NULL" );
        return NULL;
    }

    return getDeviceInfoByDeviceIndex( getDeviceIndexByNumber(num) );
}

DeviceInfo* getOutdoorDeviceInfoByType ( DEVICESUBTYPE outdoorType )
{
    if ( outdoorType != DEVICE_SUBTYPE_ODP_FRONT 
        && outdoorType != DEVICE_SUBTYPE_ODP_BACK )
    {
        tcmLog_error ( "The ODP type is invalid" );
        return NULL;
    }

    int i;
    DeviceInfo* deviceTmp;
    
    for ( i = 0; i < MAXTABLE_DEVICES ; i++ )
    {
        deviceTmp = getDeviceInfoByDeviceIndex( i + 1 );
        if ( deviceTmp != NULL )
        {
            if ( ( deviceTmp->type == DEVICE_ODP ) &&
                    ( deviceTmp->subType == outdoorType ) &&
                    ( deviceTmp->status == DEVICE_STATUS_CONNECTED ) )
            {
                tcmLog_notice ( "Find the %d type ODP",  outdoorType );
                return deviceTmp;
            }
            else
            {
                tcmMem_free( deviceTmp );
            }
        }
    }
    
    tcmLog_notice ( "Can not find the % type ODP", outdoorType );
    return NULL;
}

int addDeviceToDB ( DeviceInfo* deviceInfo )
{
    char* value[TAB_PARA_UNIT_INFO_END - 1];
    char typeStr[MAX_VALUE_LEN];
    char subTypeStr[MAX_VALUE_LEN];
    char statusStr[MAX_VALUE_LEN];
    char failedTimesStr[MAX_VALUE_LEN];
    int rowIndex = 0;

    value[TAB_PARA_UNIT_NAME-1] = deviceInfo->desc;

    if ( actDevice_type_getStringByID( typeStr, deviceInfo->type ) != TCMRET_SUCCESS )
    {
        tcmLog_error ( "Device type is error: %d", deviceInfo->type );
        return -1;
    }
    
    value[TAB_PARA_UNIT_TYPE-1] = typeStr;

    if ( actDevice_subType_getStringByID( subTypeStr, deviceInfo->subType ) != TCMRET_SUCCESS )
    {
        tcmLog_error( "Device sub type is error: %d", deviceInfo->subType);
        return -1;
    }

    value[TAB_PARA_UNIT_SUB_TYPE-1] = subTypeStr;

    value[TAB_PARA_UNIT_ACCOUNT-1] = deviceInfo->userName;
    value[TAB_PARA_UNIT_PASSWORD-1] = deviceInfo->passWord;

    if ( actDevice_status_getStringByID(statusStr, deviceInfo->status ))
    {
        tcmLog_error ( "Device status is error: %d", deviceInfo->status );
        return -1;
    }

    value[TAB_PARA_UNIT_STATE-1] = statusStr;

    sprintf ( failedTimesStr, "%d", deviceInfo->pairFailedTimes );
    value[TAB_PARA_UNIT_LOGIN_FAILURE_TIMES-1] = failedTimesStr;

    value[TAB_PARA_UNIT_MAC_ADDRESS-1] = deviceInfo->mac;
    value[TAB_PARA_UNIT_IP_ADDRESS-1] = deviceInfo->ip;
    value[TAB_PARA_UNIT_RING_SELECTED-1] = "false";
    
    if ( 0 == capi_add_table_row ( ID_UNIT_INFO, 0, value, &rowIndex ) )
    {
        if ( rowIndex > MAXTABLE_DEVICES )
        {
            capi_delete_table_row(ID_UNIT_INFO, rowIndex);
            rowIndex = -1;
        }
        
        tcmLog_debug ( "Add device %s into DB, index %d", deviceInfo->mac, rowIndex );
        capi_write_to_flash();
        return rowIndex;
    }
    else
    {
        tcmLog_error ( "Add device mac %s failed", deviceInfo->mac );
        return -1;
    }
}

tcmRet clarifyDBDeviceStatus()
{
    int i;
    UBOOL8 needSave = FALSE;

    char   app_result[TAB_PARA_UNIT_INFO_END-1][MAX_VALUE_LEN + 1];
    char   *p_result[TAB_PARA_UNIT_INFO_END-1];

    for ( i = 0; i < ( TAB_PARA_UNIT_INFO_END - 1 ); i++ )
    {
        app_result[i][0] = '\0';
        p_result[i] = app_result[i];
    }

    for ( i = 1 ; i < MAXTABLE_DEVICES + 1 ; i++ )
    {
        if ( capi_read_table_row ( ID_UNIT_INFO, i, p_result ) )
        {
            continue;
        }
        else
        {
            if ( strcasecmp ( p_result[TAB_PARA_UNIT_STATE-1],
                              DEVICE_STATUS_INITIAL_STRING ) == 0 ||
                 strcasecmp ( p_result[TAB_PARA_UNIT_STATE-1],
                              DEVICE_STATUS_CONFIGURED_STRING ) == 0 )
            {
                tcmLog_notice ( "Delete device %d which are not paird", i );
                if ( capi_delete_table_row ( ID_UNIT_INFO, i ) )
                {
                    tcmLog_error ( "delete unit row %d error", i );
                }
                else
                {
                    needSave = TRUE;
                }
                continue;
            }
            else if ( strcasecmp ( p_result[TAB_PARA_UNIT_STATE-1],
                                   DEVICE_STATUS_CONNECTED_STRING ) == 0 )
            {
                if ( capi_write_table_parameter ( ID_UNIT_INFO, i,
                     TAB_PARA_UNIT_STATE, DEVICE_STATUS_DISCONNECTED_STRING ) )
                {
                    tcmLog_error ( "Error to change device state "
                                   "from connected to disconnected in DB" );
                    continue;
                }
                else
                {
                    needSave = TRUE;
                }
            }

        }
    }

    if ( needSave )
    {
        capi_write_to_flash();
    }
    
    return TCMRET_SUCCESS;
}

tcmRet updateStatusByDeviceIndex(DEVICESTATUS devStatus, int devIndex)
{
    if ( devIndex <= 0 || devIndex > MAXTABLE_DEVICES )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    char status[MAX_VALUE_LEN + 1];

    if ( TCMRET_SUCCESS != actDevice_status_getStringByID( status, devStatus ) )
    {
        tcmLog_error ( "Device status is error: %d", devStatus );
        return TCMRET_INTERNAL_ERROR;
    }

    if ( 0 == capi_write_table_parameter ( ID_UNIT_INFO, devIndex,
                                           TAB_PARA_UNIT_STATE,
                                           status ) )
    {
        config_act_refresh_apps(ID_UNIT_INFO, APP_WRITE);
        tcmLog_notice ( "Update DB device status with %s OK", status );
    }
    else
    {
        tcmLog_notice ( "Update DB device status with %s failed", status );
        return TCMRET_CONFLIB_WRITE_ERROR;
    }

    return TCMRET_SUCCESS;
}

tcmRet updateSubTypeByDeviceIndex(DEVICESUBTYPE devSub, int devIndex)
{
    if ( devIndex <= 0 || devIndex > MAXTABLE_DEVICES )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    char subType[MAX_VALUE_LEN + 1];

    if ( TCMRET_SUCCESS != actDevice_subType_getStringByID( subType, devSub ) )
    {
        tcmLog_error ( "Device sub type is error: %d", devSub );
        return TCMRET_INTERNAL_ERROR;
    }

    if ( 0 == capi_write_table_parameter ( ID_UNIT_INFO, devIndex,
                                           TAB_PARA_UNIT_SUB_TYPE, subType ) )
    {
        config_act_refresh_apps(ID_UNIT_INFO, APP_WRITE);
        tcmLog_notice ( "Update DB device sub type with %s OK", subType );
    }
    else
    {
        tcmLog_notice ( "Update DB device sub type with %s failed", subType );
        return TCMRET_CONFLIB_WRITE_ERROR;
    }

    return TCMRET_SUCCESS;
}

tcmRet updateIndexDeviceArrayDeleteFromDb ()
{
    int i;
    char macTmp[MAX_VALUE_LEN + 1];
    for ( i = 0; i < MAXTABLE_DEVICES; i++ )
    {
        if ( capi_read_table_parameter ( ID_UNIT_INFO, i + 1,
                    TAB_PARA_UNIT_MAC_ADDRESS, macTmp ) )
        {
            tcmLog_debug ( "Delete device %d, and its connfd is %d", i + 1,
                           g_devArray[i].connfd );

            zeroIndexDevice ( i + 1 );
        }
    }

    return TCMRET_SUCCESS;
}

