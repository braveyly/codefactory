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

#ifndef __INTEREST_H__
#define __INTEREST_H__

#include <tcm.h>
#include <tcm_eid.h>
#include <tcm_msg.h>
#include "tcm_dlist.h"
//#include <tcm_util.h>

/** Structure to track a single entity that is interested in a particular event.
 *
 */
typedef struct event_interest_info
{
   DlistNode          dlist;     /**< generic doubly linked list handle */
   tcmEntityId        eid;       /**< eid of interested app. */
   char *             matchData; /**< extra matching data beyond just the msgType. */
} EvtInterestInfo;


/** Structure to keep track of all entities that are interested in a particular event.
 */
typedef struct event_interest
{
   DlistNode      dlist;       /**< generic doubly linked list handle */
   tcmMsgType     type;        /**< Event of interest, which is just a notification msg type */
   DlistNode      evtInfoHead; /**< Doubly linked list of EvtInterestInfo's for this event. */
} EvtInterest;

void interest_init(void);
void interest_cleanup(void);

/** Record that the specified eid is interested in the specified msgType.
 *
 * @param msgType (IN) the msg type that the app is interested in.
 * @param interestedEid (IN) the eid of the app.
 * @param matchData (IN) Optional additional matching data that the
 *                       caller wants to match against (in addition
 *                       to the msgType).
 *
 * @return tcmRet enum.
 */
extern tcmRet interest_register(tcmMsgType msgType, tcmEntityId interestedEid, const char *matchData);


/** Clear the interest in the specified msgType for the specified eid.
 *
 * @param msgType (IN) the msg type that the app is no longer interested in.
 * @param interestedEid (IN) the eid of the app.
 *
 * @return CmsRet enum.
 */
extern tcmRet interest_unregister(tcmMsgType msgType, tcmEntityId interestedEid);


/** Unregister this eid from all event interest registrations.
 * This is usually done if the app is a multiple instance app and it
 * has exited.
 *
 * @param interestedEid (IN) the eid of the app.
 */
void interest_unregisterAll(tcmEntityId interestedEid);

#endif

