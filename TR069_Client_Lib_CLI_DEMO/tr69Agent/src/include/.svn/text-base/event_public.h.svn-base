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
File name: event_public.h 
Author: kiffen guan 
Date: May 17th. 2009
History:
            2009-6-17   add STUN_NOTIFY EventType (Hook Guo)
************************************************************************/ 

#ifndef _EVENT_PUBLIC_H_
#define _EVENT_PUBLIC_H_

#include "tr69_agent_public.h"

#define NO_TIMEOUT -1
#define NOW 1

#define MAX_URL_LEN 256
#define MAX_USERNAME_LEN 256
#define MAX_PASSWD_LEN 256
#define MAX_TARGETNAME_LEN 256
#define MAX_FILETYPE_LEN 64
#define MAX_CMDKEY_LEN 32
#define MAX_EVENTNAME_LEN 64
#define MAX_DATETIME_LEN 64

#define IS_VALID_EVENT_TYPE(x) (INFORM <= x && x <= SCHEDULE)

#define INFORM_EVENT_NAME "inform_event"

/* STUN_NOTIFY types */
#define SN_BASE 1000
#define SN_TYPE_UDP_CONN_REQ_ADDR_CHANGED (SN_BASE + 1)
#define SN_TYPE_UDP_CONN_REQ_ARRIVED (SN_BASE + 2)
#define SN_TYPE_NAT_NOT_IN_USE (SN_BASE + 3)
#define SN_TYPE_NAT_IN_USE (SN_BASE + 4)

/* STUN_NOTIFY event names */
#define SN_NAME_UDP_CONN_REQ_ADDR_CHANGED "udp_conn_req_addr_changed_event"
#define SN_NAME_UDP_CONN_REQ_ARRIVED "udp_conn_req_arrived_event"
#define SN_NAME_NAT_NOT_IN_USE "nat_not_in_use_event"
#define SN_NAME_NAT_IN_USE "nat_in_use_event"

/* 
  * Notice: If you want to modify the enum EventType, pleare review whether IS_VALID_EVENT_TYPE() 
  * need modified.
  */
typedef enum _EventType {	
    INFORM = 1,
    DOWNLOAD,
    UPLOAD,
    STUN_NOTIFY,
    SCHEDULE,
} EventType;

typedef struct _DownloadArg {
    PAL_TIME_T time;                         
    char url[MAX_URL_LEN + 1];
    char successUrl[MAX_URL_LEN + 1];
    char failureUrl[MAX_URL_LEN + 1];
    char username[MAX_USERNAME_LEN + 1];
    char password[MAX_PASSWD_LEN + 1];
    char fileType[MAX_FILETYPE_LEN + 1];
    char targetFileName[MAX_TARGETNAME_LEN + 1];
    char commandKey[MAX_CMDKEY_LEN + 1];
    char fileName[MAX_FILENAME_LEN + 1]; /* the file which store the download inform */
    PAL_UINT32_T fileSize;
} DownloadArg;

typedef struct _DownloadResponseArg {
    PAL_INT32_T status;
    char commandKey[MAX_CMDKEY_LEN + 1];
    char startTime[MAX_DATETIME_LEN + 1];
    char completeTime[MAX_DATETIME_LEN + 1];
} DownloadResponseArg;

typedef struct _UploadArg {
    PAL_TIME_T time;                        
    char url[MAX_URL_LEN];
    char fileType[MAX_FILETYPE_LEN + 1];
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWD_LEN];
    char commandKey[MAX_CMDKEY_LEN + 1];
    char fileName[MAX_FILENAME_LEN + 1]; /* the file which store the upload inform */
} UploadArg;

typedef struct _UploadResponseArg {
    PAL_INT32_T status;
    char commandKey[MAX_CMDKEY_LEN + 1];
    char startTime[MAX_DATETIME_LEN +1];
    char completeTime[MAX_DATETIME_LEN +1];
} UploadResponseArg;

typedef struct _ScheduleArg {
    PAL_TIME_T time;                          
    char commandKey[MAX_CMDKEY_LEN + 1];
    char fileName[MAX_FILENAME_LEN + 1];
} ScheduleArg;

typedef struct _ScheduleResponseArg {
    char commandKey[MAX_CMDKEY_LEN + 1];
} ScheduleResponseArg;

typedef struct _StunNotifyArg {
    PAL_INT32_T notificationType;
    void *data;
    PAL_INT32_T dataLen;
} StunNotifyArg;

typedef struct _InformArg {
    EventCodeType event;    
} InformArg;

//**************************************************************************
// Function Name: add_event
// Description  : add a event to event List.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           time: how much seconds after it the event will happen.
//                           type:
//                           eventName: the name of event, echo event must have the different name.
//                           arg: the event function argument.
//**************************************************************************
extern Result add_event (PAL_UINT32_T time, EventType type, char *eventName, void *arg);

//**************************************************************************
// Function Name: set_event
// Description  : set the delay time of event, if event is not existed, then it's invalid.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           time: how much seconds after it the event will happen.
//                           eventName: the name of event, echo event must have the different name.
//**************************************************************************
extern Result set_event (PAL_UINT32_T time, char *eventName);

//**************************************************************************
// Function Name: delete_event
// Description  : delete a event from event list.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           eventName: the name of event, echo event must have the different name.
//**************************************************************************
extern Result delete_event (char *eventName);

//**************************************************************************
// Function Name: init_event
// Description  : init the event module.
// Returns       : according to the Result;
//**************************************************************************
extern Result init_event (void);

#endif /*_EVENT_PUBLIC_H_ */
