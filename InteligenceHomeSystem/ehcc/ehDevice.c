#include <tcm_log.h>
#include <tcm_mem.h>
#include <conflib.h>
#include "ehDevice.h"
char* gdeviceStatusStr[6]=
{"","INITIAL","CONFIGURED","PAIRING","CONNECTED","DISCONNECTED"};

DeviceInfo* createDevice ( char* deviceMac, char* deviceIp,
                             DEVICETYPE deviceType,
                             DEVICESUBTYPE deviceSubType,
                             DEVICESTATUS deviceStatus, char* deviceDesc,
                             char* deviceUserName, char* devicePassWord,
                             int devicePairFailedTimes, int deviceFd,
                             SSL* deviceSsl )
{
    DeviceInfo* deviceTmp = tcmMem_alloc ( sizeof ( DeviceInfo ), ALLOC_ZEROIZE );

    if ( deviceTmp == NULL )
    {
        tcmLog_error ( "Alloc memory error\n" );
        return NULL;
    }

    /* First the init the device */
    memset((void*)deviceTmp, 0, sizeof(DeviceInfo));

    if ( deviceMac != NULL )
        strcpy ( deviceTmp->mac, deviceMac );
    if ( deviceIp != NULL )
        strcpy ( deviceTmp->ip, deviceIp );
    deviceTmp->type = deviceType;
    deviceTmp->subType = deviceSubType;
    deviceTmp->status = deviceStatus;
    if ( deviceDesc != NULL )
        strcpy ( deviceTmp->desc, deviceDesc );
    if ( deviceUserName != NULL )
        strcpy ( deviceTmp->userName, deviceUserName );
    if ( devicePassWord != NULL )
        strcpy ( deviceTmp->passWord, devicePassWord );
    deviceTmp->pairFailedTimes = devicePairFailedTimes;
    deviceTmp->connfd = deviceFd;
    deviceTmp->ssl    = deviceSsl;
    return deviceTmp;
}

tcmRet freeDevice ( DeviceInfo* device )
{
    if ( device == NULL )
        return TCMRET_INVALID_PARAMETER;

    tcmMem_free ( device );
    return TCMRET_SUCCESS;
}

tcmRet showDevice ( DeviceInfo* device )
{
    if ( NULL == device )
        return TCMRET_INVALID_PARAMETER;

    tcmLog_debug ( "The MAC address of this device is %s", device->mac );
    tcmLog_debug ( "The IP address of this device is %s", device->ip );
    tcmLog_debug ( "The type of this device is %d", device->type );
    tcmLog_debug ( "The status of this device is %d", device->status );
    tcmLog_debug ( "The description of this device is %s", device->desc );
    tcmLog_debug ( "The username of this device is %s", device->userName );
    tcmLog_debug ( "The password of this device is %s", device->passWord );
    tcmLog_debug ( "The pairFailedTimes of this device is %d",
                   device->pairFailedTimes );
    tcmLog_debug ( "The connfd of this device is %d", device->connfd );

    return TCMRET_SUCCESS;
}
