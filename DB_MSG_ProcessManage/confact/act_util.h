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
Filename: act_util.h
Author: Ken
Date: 2010-08-17
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#ifndef __ACT_UTIL__H
#define __ACT_UTIL__H

#include <tcm_util.h>

typedef enum{
    ASTATUS_DISABLE = 0,
    ASTATUS_ENABLE,
    ASTATUS_INVALID,
}actionStatus;

UINT32 actUtil_sendMsgToSched(tcmMsgType msgType, UINT32 wordData, void *msgData, UINT32 msgDataLen);
UINT32 actUtil_statusStrToInt(char* strStatus);
tcmRet actUtil_updateDhcpdCfg( );
tcmRet actUtil_updateResolveCfg();
UBOOL8 actUtil_isSameProtocol(const char* proto1, const char* proto2);

#endif

