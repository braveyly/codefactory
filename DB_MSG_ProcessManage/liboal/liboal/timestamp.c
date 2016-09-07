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

#include <unistd.h>
#include <time.h>

#include "tcm.h"
#include "tcm_tms.h"

void tcmTms_get(tcmTimestamp *tms)
{
   struct timespec ts;
   SINT32 rc;

   if (tms == NULL)
   {
      return;
   }

   rc = clock_gettime(CLOCK_MONOTONIC, &ts);
   if (rc == 0)
   {
      tms->sec = ts.tv_sec;
      tms->nsec = ts.tv_nsec;
   }
   else
   {
      tms->sec = 0;
      tms->nsec = 0;
   }
}

void tcmTms_delta(const tcmTimestamp *newTms,
                  const tcmTimestamp *oldTms,
                  tcmTimestamp *deltaTms)
{
   if (newTms->sec >= oldTms->sec)
   {
      if (newTms->nsec >= oldTms->nsec)
      {
         /* no roll-over in the sec and nsec fields, straight subtract */
         deltaTms->nsec = newTms->nsec - oldTms->nsec;
         deltaTms->sec = newTms->sec - oldTms->sec;
      }
      else
      {
         /* no roll-over in the sec field, but roll-over in nsec field */
         deltaTms->nsec = (NSECS_IN_SEC - oldTms->nsec) + newTms->nsec;
         deltaTms->sec = newTms->sec - oldTms->sec - 1;
      }
   }
   else
   {
      if (newTms->nsec >= oldTms->nsec)
      {
         /* roll-over in the sec field, but no roll-over in the nsec field */
         deltaTms->nsec = newTms->nsec - oldTms->nsec;
         deltaTms->sec = (MAX_UINT32 - oldTms->sec) + newTms->sec + 1; /* +1 to account for time spent during 0 sec */
      }
      else
      {
         /* roll-over in the sec and nsec fields */
         deltaTms->nsec = (NSECS_IN_SEC - oldTms->nsec) + newTms->nsec;
         deltaTms->sec = (MAX_UINT32 - oldTms->sec) + newTms->sec;
      }
   }
}

UINT32 tcmTms_deltaInMilliSeconds(const tcmTimestamp *newTms,
                                  const tcmTimestamp *oldTms)
{
   tcmTimestamp deltaTms;
   UINT32 ms;

   tcmTms_delta(newTms, oldTms, &deltaTms);

   if (deltaTms.sec > MAX_UINT32 / MSECS_IN_SEC)
   {
      /* the delta seconds is larger than the UINT32 return value, so return max value */
      ms = MAX_UINT32;
   }
   else
   {
      ms = deltaTms.sec * MSECS_IN_SEC;

      if ((MAX_UINT32 - ms) < (deltaTms.nsec / NSECS_IN_MSEC))
      {
         /* overflow will occur when adding the nsec, return max value */
         ms = MAX_UINT32;
      }
      else
      {
         ms += deltaTms.nsec / NSECS_IN_MSEC;
      }
   }

   return ms;
}


void tcmTms_addMilliSeconds(tcmTimestamp *tms, UINT32 ms)
{
   UINT32 addSeconds;
   UINT32 addNano;

   addSeconds = ms / MSECS_IN_SEC;
   addNano = (ms % MSECS_IN_SEC) * NSECS_IN_MSEC;

   tms->sec += addSeconds;
   tms->nsec += addNano;

   /* check for carry-over in nsec field */
   if (tms->nsec > NSECS_IN_SEC)
   {
      /* we can't have carried over by more than 1 second */
      tms->sec++;
      tms->nsec -= NSECS_IN_SEC;
   }

   return;
}


