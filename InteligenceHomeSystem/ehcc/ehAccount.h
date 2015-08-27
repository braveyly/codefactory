#ifndef _EH_ACCOUNT_H_
#define _EH_ACCOUNT_H_

#include "ehDevice.h"
#include "ehDeviceDef.h"

#define ACCOUNT_PASSWORD_LENGTH 6

tcmRet generateAccount ( IN char* mac,
                         IN char* ip,
                         IN DEVICETYPE type,
                         OUT char* userName,
                         OUT char* passWord );

#endif
