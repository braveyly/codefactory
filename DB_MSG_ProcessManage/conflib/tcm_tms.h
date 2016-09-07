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

#ifndef __TCM_TMS_H__
#define __TCM_TMS_H__


#include "tcm.h"

/** Number of nanoseconds in 1 second. */
#define NSECS_IN_SEC 1000000000

/** Number of nanoseconds in 1 milli-second. */
#define NSECS_IN_MSEC 1000000

/** Number of nanoseconds in 1 micro-second. */
#define NSECS_IN_USEC 1000

/** Number of micro-seconds in 1 second. */
#define USECS_IN_SEC  1000000

/** Number of micro-seconds in a milli-second. */
#define USECS_IN_MSEC 1000

/** Number of milliseconds in 1 second */
#define MSECS_IN_SEC  1000

/** OS independent timestamp structure.
 */
typedef struct
{
   UINT32 sec;   /**< Number of seconds since some arbitrary point. */
   UINT32 nsec;  /**< Number of nanoseconds since some arbitrary point. */
} tcmTimestamp;

void tcmTms_get(tcmTimestamp *tms);

void tcmTms_delta(const tcmTimestamp *newTms,
                  const tcmTimestamp *oldTms,
                  tcmTimestamp *deltaTms);

UINT32 tcmTms_deltaInMilliSeconds(const tcmTimestamp *newTms,
                                  const tcmTimestamp *oldTms);


void tcmTms_addMilliSeconds(tcmTimestamp *tms, UINT32 ms);

#endif  /* __TCM_TMS_H__ */
