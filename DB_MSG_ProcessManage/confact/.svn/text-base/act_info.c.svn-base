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
Filename: act_info.c
Author: Ken
Date: 2010-08-17
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 

#include "act_info.h"

actInfo actLibraryInfo = { 0 , NULL };

tcmRet actLib_init( tcmEntityId tId, void* msghd )
{
    tcmRet ret = TCMRET_SUCCESS;
    const tcmEntityInfo *tInfo;

    if ((tInfo = tcmEid_getEntityInfo(tId)) == NULL)
    {
        tcmLog_error("invalid eid %d", tId);
        return TCMRET_INVALID_ARGUMENTS;
    }

    if (tId == ID_SCHED && msghd != NULL)
    {
        tcmLog_error("sched must not pass in msgHandle to actlib, may result in deadlock");
        return TCMRET_INVALID_ARGUMENTS;
    }
    else if (tId != ID_SCHED && msghd == NULL)
    {
        tcmLog_error("must pass in msgHandle");
        return TCMRET_INVALID_ARGUMENTS;
    }

    actLibraryInfo.eid       = tId;
    actLibraryInfo.msgHandle = msghd;

    return ret;
}

void actLib_cleanup()
{
    actLibraryInfo.eid       = 0;
    actLibraryInfo.msgHandle = NULL;
}

