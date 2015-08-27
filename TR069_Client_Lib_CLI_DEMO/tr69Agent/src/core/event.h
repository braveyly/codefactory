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
File name: event.h
Author: kiffen guan
Date: May 17th. 2009
************************************************************************/

#ifndef _EVENT_H_
#define _EVENT_H_

#include "tr69_agent_public.h"
#include "event_public.h"
#include "mainloop_public.h"
#include "http_public.h"
#include "ftp_public.h"
#include "pal.h"

#define endret(x) {ret = x; goto end; }
#define end() {goto end;}

#define timimg_list_lock() pal_mutex_lock(&timingList.mutex)
#define timimg_list_unlock() pal_mutex_unlock(&timingList.mutex)
#define timeout_list_lock() pal_mutex_lock(&timeoutList.mutex);
#define timeout_list_unlock() pal_mutex_unlock(&timeoutList.mutex)

typedef struct _EventFunc
{
    EventType type;
    void ( *event_func ) ( void * );
} EventFunc;

typedef struct _Event
{
    EventFunc func;
    char eventName[MAX_EVENTNAME_LEN];
    void *arg;
} Event;

typedef struct _EventList
{
    struct _EventList *next;
    struct _EventList *prev;
    PAL_UINT32_T  time;    /* how many seconds it will be timeout */
    Event event;
} EventList;

typedef struct _EventListHeader
{
    EventList header;
    PAL_MUTEX_T mutex;
} EventListHeader;

#endif /* _EVENT_H_ */
