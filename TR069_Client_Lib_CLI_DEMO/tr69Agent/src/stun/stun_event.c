/**********************************************************************
Copyright (c) 2009 Wuhan Tecom Co., Ltd.
All Rights Reserved
No portions of this material may be reproduced in any form without the
written permission of:
Wuhan Tecom Co., Ltd.
#18, Huaguang Road
Wuhan, PR China 430074
All information contained in this document is Wuhan Tecom Co., Ltd.
company private, proprietary, and trade secret.
**********************************************************************
File name: stun_event.c
Author: Hook Guo
Date: May 15th. 2009
************************************************************************/

#include "stun.h"

StunEventHeader gStunEventList;
PAL_SEM_T gTr111MainLoopSem;

/*****************************************************************************
 Function Name: event_list_init
 Description: initiate stun event list
 Returns: NONE
 Parameter: NONE
******************************************************************************/
void stun_event_list_init()
{
    gStunEventList.eventPtr = NULL;
    gStunEventList.eventCount = 0;
    pal_mutex_init(&gStunEventList.lock);
    pal_sem_init(&gTr111MainLoopSem);
}

/*****************************************************************************
 Function Name: stun_event_list_count
 Description: return the current count of the stun event list
 Returns: current count of the stun event list
 Parameter: NONE
******************************************************************************/
PAL_INT32_T stun_event_list_count()
{
    return gStunEventList.eventCount;
}

/*****************************************************************************
 Function Name: stun_event_list_push
 Description: push an event into the stun event list
 Returns: return OK on succeed, or return STUN_ERR_FAILED is the list is full
 Parameter: @eventCode: event code number
******************************************************************************/
Result stun_event_list_push(IN PAL_INT32_T eventCode)
{
    StunEvent *eventPtr = (StunEvent *)PAL_MALLOC(sizeof(eventPtr));

    if (eventPtr == NULL)
    {
        return STUN_ERR_FAILED;
    }

    if (gStunEventList.eventCount == 0)
    {
        eventPtr->eventCode = eventCode;
        eventPtr->nextEventPtr = NULL;
        gStunEventList.eventPtr = eventPtr;
    }
    else
    {
        eventPtr->nextEventPtr = gStunEventList.eventPtr;
        gStunEventList.eventPtr = eventPtr;
    }

    gStunEventList.eventCount++;

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name: stun_event_list_pull
 Description: pull an event out of the stun event list
 Returns: the event code number pulled out
 Parameter: NONE
******************************************************************************/
PAL_INT32_T stun_event_list_pull()
{
    StunEvent *tmpPtr;
    PAL_INT32_T eventCode;

    if (gStunEventList.eventCount == 0)
    {
        return SE_NONE;
    }

    tmpPtr = gStunEventList.eventPtr;
    eventCode = tmpPtr->eventCode;

    if (gStunEventList.eventCount == 1)
    {
        PAL_FREE(gStunEventList.eventPtr);
        gStunEventList.eventPtr = NULL;
    }
    else
    {
        gStunEventList.eventPtr = (gStunEventList.eventPtr)->nextEventPtr;
        PAL_FREE(tmpPtr);
        tmpPtr = NULL;
    }

    gStunEventList.eventCount = gStunEventList.eventCount - 1;

    return eventCode;
}

/*****************************************************************************
 Function Name: stun_event_list_destroy
 Description: destroy the stun event list
 Returns: NONE
 Parameter: NONE
******************************************************************************/
void stun_event_list_clean()
{
    StunEvent *iterPtr = NULL;
    StunEvent *tmpPtr = NULL;

    pthread_mutex_lock(&gStunEventList.lock);

    if (gStunEventList.eventCount == 0)
    {
        pthread_mutex_unlock(&gStunEventList.lock);
        return;
    }

    for (iterPtr = gStunEventList.eventPtr; iterPtr != NULL;)
    {
        tmpPtr = iterPtr;
        iterPtr = iterPtr->nextEventPtr;
        PAL_FREE(tmpPtr);
        tmpPtr = NULL;
    }

    gStunEventList.eventCount = 0;

    pthread_mutex_unlock(&gStunEventList.lock);

}

/*****************************************************************************
 Function Name: stun_event_list_lock
 Description: lock the stun event list
 Returns: return OK on succeed, or return STUN_ERR_FAILED on failure
 Parameter: NONE
******************************************************************************/
Result stun_event_list_lock()
{
    if (pthread_mutex_lock(&gStunEventList.lock) != 0)
    {
        return STUN_ERR_FAILED;
    }
    else
    {
        return TCMAGENT_OK;
    }
}

/*****************************************************************************
 Function Name: stun_event_list_unlock
 Description: unlock the stun event list
 Returns: return OK on succeed, or return STUN_ERR_FAILED on failure
 Parameter: NONE
******************************************************************************/
Result stun_event_list_unlock()
{
    if (pthread_mutex_unlock(&gStunEventList.lock) != 0)
    {
        return STUN_ERR_FAILED;
    }
    else
    {
        return TCMAGENT_OK;
    }
}

/*****************************************************************************
 Function Name: stun_event_list_signal
 Description: signal the stun event list
 Returns: return OK on succeed, or return STUN_ERR_FAILED on failure
 Parameter: NONE
******************************************************************************/
Result stun_event_list_signal()
{
    if (pal_sem_post(&gTr111MainLoopSem) != 0)
    {
        return STUN_ERR_FAILED;
    }
    else
    {
        return TCMAGENT_OK;
    }
}

/*****************************************************************************
 Function Name: stun_event_list_wait
 Description: wait the stun event list
 Returns: return OK on succeed, return STUN_ERR_FAILED on failure, or return
          STUN_ERR_TIMEOUT on timeout
 Parameter: NONE
******************************************************************************/
Result stun_event_list_wait()
{
    if (pal_sem_timedwait(&gTr111MainLoopSem
                          , STUN_EVENT_LIST_POLLING_TIMEOUT) != 0)
    {
        return (errno == ETIMEDOUT)
               ? STUN_ERR_TIMEOUT : STUN_ERR_FAILED;
    }
    else
    {
        return TCMAGENT_OK;
    }
}

