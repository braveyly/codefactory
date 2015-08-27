/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************/

#ifndef __TCM_TMR_H__
#define __TCM_TMR_H__

#include "tcm.h"
#include "tcm_util.h"


typedef void (*tcmEventHandler)(void*);


#define TCM_EVENT_TIMER_NAME_LENGTH  32


tcmRet tcmTmr_init(void **tmrHandle);


void tcmTmr_cleanup(void **tmrHandle);


tcmRet tcmTmr_set(void *tmrHandle, tcmEventHandler func, void *ctxData, UINT32 ms, const char *name);


void tcmTmr_cancel(void *tmrHandle, tcmEventHandler func, void *ctxData);


tcmRet tcmTmr_getTimeToNextEvent(const void *tmrHandle, UINT32 *ms);


UINT32 tcmTmr_getNumberOfEvents(const void *tmrHandle);


void tcmTmr_executeExpiredEvents(void *tmrHandle);


UBOOL8 tcmTmr_isEventPresent(const void *tmrHandle, tcmEventHandler func, void *ctxData);


void tcmTmr_dumpEvents(const void *tmrHandle);


#endif  /* __TCM_TMR_H__ */
