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
#ifndef __TIMER_H__
#define __TIMER_H__

#include <launcher.h>
#include <tcm.h>
#include <dls.h>
#include <tcm_util.h>
#include <tcm_eid.h>
#include <tcm_msg.h>


/*!\file timer.h
 * \brief Header file for timer event manipulation functions.
 *
 */

/** Initialize the delayed message scheduler.
 *
 * @return tcmRet enum.
 */
tcmRet timer_init(void);


/** Clean up delayed message timeruler.
 */
void timer_cleanup(void);


/** Create a delayed message event.
 *
 * @param id  (IN) The delayed msg id.
 * @param ctx (IN) The dInfo structure.
 * @param ms  (IN) The number of milliseconds in the future to send this
 *                 delayed message.
 *
 * @return tcmRet enum.
 */
tcmRet timer_set(UINT32 id, void *ctx, SINT32 ms);


/** Cancel a delayed message event.
 *
 * @param id  (IN) The delayed msg id.
 * @param ctx (IN) The dInfo structure.
 *
 * @return tcmRet enum.
 */
tcmRet timer_cancel(UINT32 id, void *ctx);


/** Get number of milliseconds to the next event.
 *
 * @return number of milliseconds to the next event.  If there are no events,
 *         then UINT32_MAX is returned.
 */
UINT32 timer_getTimeToNextEvent(void);


/** Called by smd_main to execute any expired events.
 */
void timer_process(void);


#endif /* __TIMER_H__ */
