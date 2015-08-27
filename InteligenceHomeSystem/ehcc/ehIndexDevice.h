#ifndef _EH_INDEX_DEVICE_
#define _EH_INDEX_DEVICE_
#include <conflib.h>
#include "ehDeviceDef.h"

#define INVALID_DEVICE_INDEX -1

extern DeviceConnInfo g_devArray[MAXTABLE_DEVICES];
extern pthread_mutex_t g_devQueueMutex;

tcmRet zeroIndexDevice ( int devIndex );
void showIndexDeviceArray();
int getDeviceIndexByFd ( int fd );
int getDeviceIndexByMac ( char* arMac );
int getDeviceIndexByNumber ( char* num );
DeviceInfo* getDeviceInfoByDeviceIndex( int devIndex );
DeviceInfo* getDeviceInfoByMac ( char* mac );
DeviceInfo* getDeviceInfoByFd ( int devFd );
DeviceInfo* getDeviceInfoByNumber ( char* num );
DeviceInfo* getOutdoorDeviceInfoByType ( DEVICESUBTYPE outdoorType );
int addDeviceToDB ( DeviceInfo* deviceInfo );
tcmRet clarifyDBDeviceStatus();
tcmRet updateStatusByDeviceIndex(DEVICESTATUS devStatus, int devIndex);
tcmRet updateSubTypeByDeviceIndex(DEVICESUBTYPE devSub, int devIndex);
tcmRet updateIndexDeviceArrayDeleteFromDb ();
tcmRet updateIndexDeviceArrayAddFromDb ();

#endif
