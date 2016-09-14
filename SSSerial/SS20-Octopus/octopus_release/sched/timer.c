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

#include <timer.h>
#include "tcm_mem.h"
#include "tcm_log.h"

typedef struct timer_event_info
{
   struct timer_event_info *prev;
   struct timer_event_info *next;
   UINT32                  id;
   void*                   ctx;
} timerEvtInfo;

void  *tmrHandle=NULL;
timerEvtInfo *timerInfoList = NULL;

tcmRet timer_init(void)
{
   tcmRet ret;

   if((ret = tcmTmr_init(&tmrHandle)) != TCMRET_SUCCESS)
   {
      tcmLog_error("timer initialization failed ret=0x%x", ret);
   }

   return ret;
}

void timer_cleanup(void)
{
   timerEvtInfo *tmrEvtInfo;

   while((tmrEvtInfo = timerInfoList) != NULL)
   {
      timerInfoList = tmrEvtInfo->next;
      tcmMem_free(tmrEvtInfo);
   }

   tcmTmr_cleanup(&tmrHandle);
}

timerEvtInfo *getTmrInfo(UINT32 id, void *ctx)
{
   timerEvtInfo *tmrInfo = timerInfoList;

   while(tmrInfo != NULL)
   {
      if(tmrInfo->id == id && tmrInfo->ctx == ctx )
      {
         return tmrInfo;
      }
   }

   return NULL;
}

timerEvtInfo *createTmrInfo(UINT32 id, void *ctx)
{
   timerEvtInfo *tmrInfo = NULL;

   tmrInfo = tcmMem_alloc(sizeof(timerEvtInfo), ALLOC_ZEROIZE);
   if(tmrInfo != NULL)
   {
      tmrInfo->id = id;
      tmrInfo->ctx = ctx;

      tmrInfo->next = timerInfoList;
      if(timerInfoList != NULL)
      {
         timerInfoList->prev = tmrInfo;
      }
      timerInfoList = tmrInfo;
   }
   else
   {
      tcmLog_error("Can't allocate timerEvtInfo");
   }

   return tmrInfo;
}

void deleteTimerInfo(timerEvtInfo **tmrInfo)
{
   if(timerInfoList == (*tmrInfo))
   {
      timerInfoList = (*tmrInfo)->next;
      if(timerInfoList != NULL)
      {
         timerInfoList->prev = NULL;
      }
   }
   else
   {
      (*tmrInfo)->prev->next = (*tmrInfo)->next;
      if((*tmrInfo)->next != NULL)
      {
         (*tmrInfo)->next->prev = (*tmrInfo)->prev;
      }
   }

   TCMMEM_FREE_BUF_AND_NULL_PTR(*tmrInfo);
   return;
}

void timer_timerHandlerFunc(void *ctx)
{
   timerEvtInfo *tmrInfo = (timerEvtInfo *)ctx;
   void *ctx2 = tmrInfo->ctx;
   DlsInfo *dInfo = (DlsInfo *)ctx2;
   tcmMsgHeader *msg;

   msg = (tcmMsgHeader *)tcmMem_alloc(sizeof(tcmMsgHeader), ALLOC_ZEROIZE);
   msg->type = TCM_MSG_DELAYED_MSG;
   msg->src = ID_SCHED;
   msg->dst = (dInfo->eInfo->flags & APPCFG_MULTIPLE_INSTANCES)?dInfo->specificEid:dInfo->eInfo->eid;
   msg->flags_event = 1;
   msg->wordData = tmrInfo->id;

   sendMessageByAppState(dInfo, &msg);

   return;
}

tcmRet timer_set(UINT32 id, void *ctx, SINT32 ms)
{
   timerEvtInfo *tmrInfo;
   tcmRet ret;

   if((tmrInfo = createTmrInfo(id, ctx)) == NULL)
   {
      tcmLog_error("Could not allocate memory");
      return TCMRET_RESOURCE_EXCEEDED;
   }
   else
   {
      ret = tcmTmr_set(tmrHandle, timer_timerHandlerFunc, tmrInfo, ms, "delayed_msg");
      if( ret != TCMRET_SUCCESS )
      {
         tcmLog_error("Setting timer events failed, ret=0x%x", ret);
      }
   }

   return ret;
}

tcmRet timer_cancel(UINT32 id, void *ctx)
{
   timerEvtInfo *tmrInfo;
   tcmRet ret = TCMRET_INVALID_ARGUMENTS;

   if((tmrInfo = getTmrInfo(id, ctx)) != NULL)
   {
      tcmTmr_cancel(tmrHandle, timer_timerHandlerFunc, tmrInfo);
      deleteTimerInfo(&tmrInfo);
      ret = TCMRET_SUCCESS;
   }

   return ret;
}

UINT32 timer_getTimeToNextEvent(void)
{
   UINT32 ms;

   tcmTmr_getTimeToNextEvent(tmrHandle, &ms);

   return ms;
}

void timer_process(void)
{
   tcmTmr_executeExpiredEvents(tmrHandle);
}

