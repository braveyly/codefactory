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

#include <interest.h>

DLIST_HEAD(evtInterestHead);

EvtInterest *interest_findByType(tcmMsgType msgType)
{
   EvtInterest *evtInterest;

   dlist_for_each_entry(evtInterest, &evtInterestHead, dlist)
   {
      if(evtInterest->type == msgType)
      {
         return evtInterest;
      }
   }
}

EvtInterestInfo *interest_findInfo(EvtInterest *evtInterest, tcmEntityId eid)
{
   EvtInterestInfo *evtInfo;

   dlist_for_each_entry(evtInfo, &(evtInterest->evtInfoHead), dlist)
   {
      if(evtInfo->eid == eid)
      {
         return evtInfo;
      }
   }
}

void interest_init(void)
{
   return;
}

void interest_cleanup(void)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;

   while(!(dlist_empty(&evtInterestHead)))
   {
      evtInterest = (EvtInterest *)evtInterestHead.next;

      while (!(dlist_empty(&(evtInterest->evtInfoHead))))
      {
         evtInfo = (EvtInterestInfo *) evtInterest->evtInfoHead.next;

         dlist_unlink((DlistNode *) evtInfo);
         tcmMem_free(evtInfo->matchData);
         tcmMem_free(evtInfo);
      }

      /* unlink the evtInterest from the dlist and free it */
      dlist_unlink((DlistNode *) evtInterest);
      tcmMem_free(evtInterest);
   }

   return;
}

tcmRet interest_register(tcmMsgType msgType, tcmEntityId interestedEid, const char *matchData)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;

   tcmLog_debug("msgType=0x%x for eid %d (0x%x) matchData=%s", msgType, interestedEid, interestedEid, matchData);

   if((evtInterest = interest_findByType(msgType)) == NULL)
   {
      evtInterest = (EvtInterest*)tcmMem_alloc(sizeof(EvtInterest), ALLOC_ZEROIZE);
      if(evtInterest == NULL)
      {
         tcmLog_error("could not allocate resources");
         return TCMRET_RESOURCE_EXCEEDED;
      }

      evtInterest->type = msgType;
      DLIST_HEAD_IN_STRUCT_INIT(evtInterest->evtInfoHead);

      dlist_append((struct dlist_node*)evtInterest, &evtInterestHead);
   }

   if((evtInfo = interest_findInfo(evtInterest, interestedEid)) != NULL)
   {
      if(tcmUtl_strcmp(evtInfo->matchData, matchData))
      {
         tcmLog_error("Different match data fields faound for same eid=%d", interestedEid);
      }
      tcmLog_error("Duplicated Registration for the same message, eid=%d", interestedEid);
      return TCMRET_INVALID_ARGUMENTS;
   }

   evtInfo = (EvtInterestInfo *)tcmMem_alloc(sizeof(EvtInterestInfo), ALLOC_ZEROIZE);
   if(evtInfo == NULL)
   {
      tcmLog_error("Could not allocate resource ");
      return TCMRET_RESOURCE_EXCEEDED;
   }

   evtInfo->eid = interestedEid;
   evtInfo->matchData = tcmMem_strdup(matchData);

   dlist_append((DlistNode *)evtInfo, &(evtInterest->evtInfoHead));

   return TCMRET_SUCCESS;
}

tcmRet interest_unregister(tcmMsgType msgType, tcmEntityId interestedEid)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;

   if((evtInterest = interest_findByType(msgType)) == NULL)
   {
      tcmLog_error("Could't not find record for msg 0x%x\n", msgType);
      return TCMRET_INVALID_ARGUMENTS;
   }

   if((evtInfo = interest_findInfo(evtInterest, interestedEid)) == NULL)
   {
      tcmLog_error("Can't find record for message type 0x%x", msgType);
      return TCMRET_INVALID_ARGUMENTS;
   }

   dlist_unlink((DlistNode *)evtInfo);
   tcmMem_free(evtInfo->matchData);
   tcmMem_free(evtInfo);

   if(dlist_empty(&(evtInterest->evtInfoHead)))
   {
      dlist_unlink((DlistNode *)evtInterest);
      tcmMem_free(evtInterest);
   }

   return TCMRET_SUCCESS;
}

void interest_unregisterAll(tcmEntityId interestedEid)
{
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;
   UBOOL8 checkAgain = TRUE;

   while(checkAgain)
   {
      checkAgain = FALSE;

      dlist_for_each_entry(evtInterest, &evtInterestHead, dlist)
      {
         if((evtInfo = interest_findInfo(evtInterest, interestedEid)) != NULL)
         {
            interest_unregister(evtInterest->type, interestedEid);
            checkAgain = TRUE;
            break;
         }
      }
   }
}

