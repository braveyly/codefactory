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

#include "tcm.h"
#include "tcm_tms.h"
#include "tcm_tmr.h"
#include "tcm_mem.h"
//#include "tcm_util.h"

/** Internal event timer structure
 */
typedef struct tcm_timer_event
{
   struct tcm_timer_event *next;      /**< pointer to the next timer. */
   tcmTimestamp            expireTms; /**< Timestamp (in the future) of when this
                                       *   timer event will expire. */
   tcmEventHandler         func;      /**< handler func to call when event expires. */
   void *                  ctxData;   /**< context data to pass to func */
   char name[TCM_EVENT_TIMER_NAME_LENGTH]; /**< name of this timer */
} tcmTimerEvent;


/** Internal timer handle. */
typedef struct
{
   tcmTimerEvent *events;     /**< Singly linked list of events */
   UINT32         numEvents;  /**< Number of events in this handle. */
} tcmTimerHandle;


tcmRet tcmTmr_init(void **tmrHandle)
{

   (*tmrHandle) = tcmMem_alloc(sizeof(tcmTimerHandle), ALLOC_ZEROIZE);
   if ((*tmrHandle) == NULL)
   {
      return TCMRET_RESOURCE_EXCEEDED;
   }

   return TCMRET_SUCCESS;
}


void tcmTmr_cleanup(void **handle)
{
   tcmTimerHandle *tmrHandle = (tcmTimerHandle *) handle;
   tcmTimerEvent *tmrEvent;

   while ((tmrEvent = tmrHandle->events) != NULL)
   {
      tmrHandle->events = tmrEvent->next;
      TCMMEM_FREE_BUF_AND_NULL_PTR(tmrEvent);
   }

   TCMMEM_FREE_BUF_AND_NULL_PTR((*handle));

   return;
}

/** This macro will evaluate TRUE if a is earlier than b */
#define IS_EARLIER_THAN(a, b) (((a)->sec < (b)->sec) || \
                               (((a)->sec == (b)->sec) && ((a)->nsec < (b)->nsec)))

tcmRet tcmTmr_set(void *handle, tcmEventHandler func, void *ctxData, UINT32 ms, const char *name)
{
   tcmTimerHandle *tmrHandle = (tcmTimerHandle *) handle;
   tcmTimerEvent *currEvent, *prevEvent, *newEvent;

   /*
    * First verify there is not a duplicate event.
    * (The original code first deleted any existing timer,
    * which is a "side-effect", bad style, but maybe tr69c requires
    * that functionality?)
    */
   if (tcmTmr_isEventPresent(handle, func, ctxData))
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   /* make sure name is not too long */
   if ((name != NULL) && (strlen(name) >= TCM_EVENT_TIMER_NAME_LENGTH))
   {
      return TCMRET_INVALID_ARGUMENTS;
   }


   /*
    * Allocate a structure for the timer event.
    */
   newEvent = tcmMem_alloc(sizeof(tcmTimerEvent), ALLOC_ZEROIZE);
   if (newEvent == NULL)
   {
      return TCMRET_RESOURCE_EXCEEDED;
   }

   /* fill in fields of new event timer structure. */
   newEvent->func = func;
   newEvent->ctxData = ctxData;

   tcmTms_get(&(newEvent->expireTms));
   tcmTms_addMilliSeconds(&(newEvent->expireTms), ms);

   if (name != NULL)
   {
      sprintf(newEvent->name, "%s", name);
   }


   /*
    * Now we just need to insert it in the correct place in the timer handle.
    * We just insert the events in absolute order, i.e. smallest expire timer
    * at the head of the queue, largest at the end of the queue.  If the
    * modem is up long enough where timestamp rollover is an issue (139 years!)
    * cmsTmr_executeExpiredEvents and cmsTmr_getTimeToNextEvent will have to
    * be careful about where they pick the next timer to expire.
    */
   if (tmrHandle->numEvents == 0)
   {
      tmrHandle->events = newEvent;
   }
   else
   {
      currEvent = tmrHandle->events;

      if (IS_EARLIER_THAN(&(newEvent->expireTms), &(currEvent->expireTms)))
      {
         /* queue at the head */
         newEvent->next = currEvent;
         tmrHandle->events = newEvent;
      }
      else
      {
         UBOOL8 done = FALSE;

         while (!done)
         {
            prevEvent = currEvent;
            currEvent = currEvent->next;

            if ((currEvent == NULL) ||
                (IS_EARLIER_THAN(&(newEvent->expireTms), &(currEvent->expireTms))))
            {
               newEvent->next = prevEvent->next;
               prevEvent->next = newEvent;
               done = TRUE;
            }
         }
      }
   }

   tmrHandle->numEvents++;

   return TCMRET_SUCCESS;
}


void tcmTmr_cancel(void *handle, tcmEventHandler func, void *ctxData)
{
   tcmTimerHandle *tmrHandle = (tcmTimerHandle *) handle;
   tcmTimerEvent *currEvent, *prevEvent;

   if ((currEvent = tmrHandle->events) == NULL)
   {
      return;
   }

   if (currEvent->func == func && currEvent->ctxData == ctxData)
   {
      /* delete from head of the queue */
      tmrHandle->events = currEvent->next;
      currEvent->next = NULL;
   }
   else
   {
      UBOOL8 done = FALSE;

      while ((currEvent != NULL) && (!done))
      {
         prevEvent = currEvent;
         currEvent = currEvent->next;

         if (currEvent != NULL && currEvent->func == func && currEvent->ctxData == ctxData)
         {
            prevEvent->next = currEvent->next;
            currEvent->next = NULL;
            done = TRUE;
         }
      }
   }

   if (currEvent != NULL)
   {
      tmrHandle->numEvents--;

      TCMMEM_FREE_BUF_AND_NULL_PTR(currEvent);
   }

   return;
}


tcmRet tcmTmr_getTimeToNextEvent(const void *handle, UINT32 *ms)
{
   tcmTimerHandle *tmrHandle = (tcmTimerHandle *) handle;
   tcmTimerEvent *currEvent;
   tcmTimestamp nowTms;

   tcmTms_get(&nowTms);
   currEvent = tmrHandle->events;

   if (currEvent == NULL)
   {
      *ms = MAX_UINT32;
      return TCMRET_NO_MORE_INSTANCES;
   }

   /* this is the same code as in dumpEvents, integrate? */
   if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
   {
      /*
       * the next event is past due (nowTms is later than currEvent),
       * so time to next event is 0.
       */
      *ms = 0;
   }
   else
   {
      /*
       * nowTms is earlier than currEvent, so currEvent is still in
       * the future.
       */
      (*ms) = tcmTms_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
   }

   return TCMRET_SUCCESS;
}


UINT32 tcmTmr_getNumberOfEvents(const void *handle)
{
   const tcmTimerHandle *tmrHandle = (const tcmTimerHandle *) handle;

   return (tmrHandle->numEvents);
}


void tcmTmr_executeExpiredEvents(void *handle)
{
   tcmTimerHandle *tmrHandle = (tcmTimerHandle *) handle;
   tcmTimerEvent *currEvent;
   tcmTimestamp nowTms;

   tcmTms_get(&nowTms);
   currEvent = tmrHandle->events;

   while ((currEvent != NULL) && (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms)))
   {
      /*
       * first remove the currEvent from the tmrHandle because
       * when we execute the callback function, it might call the
       * cmsTmr API again.
       */
      tmrHandle->events = currEvent->next;
      currEvent->next = NULL;
      tmrHandle->numEvents--;

      /* call the function */
      (*currEvent->func)(currEvent->ctxData);

      /* free the event struct */
      tcmMem_free(currEvent);

      currEvent = tmrHandle->events;
   }

   return;
}


UBOOL8 tcmTmr_isEventPresent(const void *handle, tcmEventHandler func, void *ctxData)
{
   const tcmTimerHandle *tmrHandle = (const tcmTimerHandle *) handle;
   tcmTimerEvent *tmrEvent;
   UBOOL8 found=FALSE;

   tmrEvent = tmrHandle->events;

   while ((tmrEvent != NULL) && (!found))
   {
      if (tmrEvent->func == func && tmrEvent->ctxData == ctxData)
      {
         found = TRUE;
      }
      else
      {
         tmrEvent = tmrEvent->next;
      }
   }

   return found;
}


void tcmTmr_dumpEvents(const void *handle)
{
   const tcmTimerHandle *tmrHandle = (const tcmTimerHandle *) handle;
   tcmTimerEvent *currEvent;
   tcmTimestamp nowTms;
   UINT32 expires;

   tcmTms_get(&nowTms);
   currEvent = tmrHandle->events;

   while (currEvent != NULL)
   {

      /* this is the same code as in getTimeToNextEvent, integrate? */
      if (IS_EARLIER_THAN(&(currEvent->expireTms), &nowTms))
      {
         /*
          * the currentevent is past due (nowTms is later than currEvent),
          * so expiry time is 0.
          */
         expires = 0;
      }
      else
      {
         /*
          * nowTms is earlier than currEvent, so currEvent is still in
          * the future.
          */
         expires = tcmTms_deltaInMilliSeconds(&(currEvent->expireTms), &nowTms);
      }

      currEvent = currEvent->next;
   }

   return;
}

