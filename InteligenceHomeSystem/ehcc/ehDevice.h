#ifndef _EH_DEVICE_
#define _EH_DEVICE_

#include "ehDeviceDef.h"

DeviceInfo* createDevice ( char* deviceMac, char* deviceIp,
                             DEVICETYPE deviceType,
                             DEVICESUBTYPE deviceSubType,
                             DEVICESTATUS deviceStatus,
                             char* deviceDesc,
                             char* deviceUserName,
                             char* devicePassWord,
                             int devicePairFailedTimes, int deviceFd,
                             SSL* deviceSsl );
tcmRet freeDevice ( DeviceInfo* device );
tcmRet showDevice ( DeviceInfo* device );

#endif
