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
File name: event.c
Author: kiffen guan
Date: May 17th. 2009
History:
            2009-6-17   add event for stun module (Hook Guo)
************************************************************************/

#include "event.h"

extern void event_inform ( void *arg );
extern void event_download ( void *arg );
extern void event_upload ( void *arg );
extern void event_schedule ( void *arg );
extern void event_stun_notify ( void *arg );

static EventListHeader timingList, timeoutList;

PAL_SEM_T timerSem, eventSem;

/* the callback function table */
static EventFunc eventFunc[] =
{
    {0, NULL},
    {INFORM, event_inform},
    {DOWNLOAD, event_download},
    {UPLOAD, event_upload},
    {SCHEDULE, event_schedule},
    {STUN_NOTIFY, event_stun_notify}
};

extern PAL_UINT32_T gReboot;
/***********************************************************************
Function: init_list
Description:  init the EventList.

Return: according to the Result.
Input:   list
Output: 
Others:
************************************************************************/
static Result init_list ( EventListHeader *list )
{
    Result ret = TCMAGENT_OK;

    memset ( list, 0, sizeof ( EventListHeader ) );

    if ( pal_mutex_init ( &list->mutex ) )
    {
        endret ( MUTEX_INIT_FAILURE );
    }

end:

    return ret;
}

/***********************************************************************
Function: add_list
Description:  add a node to the EventList.

Return: according to the Result.
Input:   before
            list 
Output: 
Others:
************************************************************************/
static void add_list ( EventList *before, EventList *list )
{
    if ( NULL != before->next )
    {
        before->next->prev = list;
    }
    list->next = before->next;
    list->prev = before;
    before->next = list;
}

/***********************************************************************
Function: delete_list
Description:  delete a node from EventList.

Return: according to the Result.
Input:   list
Output: 
Others:
************************************************************************/
static void delete_list ( EventList *list )
{
    list->prev->next = list->next;
    if ( NULL != list->next )
    {
        list->next->prev = list->prev;
    }

    list->next = NULL;
    list->prev = NULL;
}

/***********************************************************************
Function: get_event_from_timing_list
Description:  search a event through the eventName.

Return: according to the Result.
Input:   eventName
            time
Output: prev
Others:
************************************************************************/
static EventList *get_event_from_timing_list ( char *eventName, PAL_UINT32_T time, OUT EventList **prev )
{
    EventList *list = &timingList.header;
    EventList *tmp = &timingList.header;
    EventList *existEvent = NULL;

    while ( NULL != list )
    {
        if ( 0 == strcmp ( list->event.eventName, eventName ) )
        {
            existEvent = list;
        }

        if ( time > list->time )
        {
            tmp = list;
        }

        list = list->next;
    }

    *prev = tmp;

    return existEvent;
}

/***********************************************************************
Function: timer_thread
Description:  timing the eventList every second, and put the timeout-event to the timeout list.

Return: according to the Result.
Input:   arg
Output: 
Others:
************************************************************************/
static void * timer_thread ( void *arg )
{
    EventList *timing, *timingNext;

    wait_init ();

    do
    {
        pal_sleep ( 1 );

        timimg_list_lock ();

        timing = timingList.header.next;

        while ( NULL != timing )
        {
            timing->time--;

            timingNext = timing->next;

            if ( 0 == timing->time )
            {
                delete_list ( timing );

                timeout_list_lock();
                add_list ( &timeoutList.header, timing );
                timeout_list_unlock ();
            }

            timing = timingNext;
        }

        timimg_list_unlock ();

    }
    while ( 1 );

    return NULL;
}

/***********************************************************************
Function: event_thread
Description:  get the event from the timeout list and do the function.

Return: according to the Result.
Input:   arg
Output: 
Others:
************************************************************************/
static void *event_thread ( void *arg )
{
    EventList *timeout;

    wait_init ();

    do
        {
        pal_sleep ( 1 );

        do
        {
            timeout_list_lock ();

            timeout = timeoutList.header.next;

            if ( NULL == timeout )
            {
                timeout_list_unlock ();
                break;
            }

            delete_list ( timeout );

            timeout_list_unlock ();

            timeout->event.func.event_func ( timeout->event.arg );

            if ( timeout->event.arg )
            {
                PAL_FREE ( timeout->event.arg );
            }

            PAL_FREE ( timeout );
        }
        while ( NULL != timeoutList.header.next );
    }
    while ( 1 );

    return NULL;
}

/* ---------------------  extern interface  ----------------------- */

//**************************************************************************
// Function Name: init_event
// Description  : init the event module.
// Returns       : according to the Result;
//**************************************************************************
Result init_event ( void )
{
    Result ret = TCMAGENT_OK, retTmp;
    PAL_THREAD_T timerThreadID, eventThreadID;
    PAL_THREAD_ATTR_T a, a1;
    //pal_set_logger ( LOG_EVENT, T_LOG_INFO, LOG_CONSOLE, 1024,  "event" );

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_EVENT, __func__, " event init start.\n" );

    if ( TCMAGENT_OK != ( retTmp = init_list ( &timingList ) ) )
    {
        endret ( retTmp );
    }

    if ( TCMAGENT_OK != ( retTmp = init_list ( &timeoutList ) ) )
    {
        endret ( retTmp );
    }

    if ( pal_thread_create ( &timerThreadID, timer_thread, NULL, NULL ) )
    {
        endret ( THREAD_INIT_FAILURE );
    }

    if ( pal_thread_create ( &eventThreadID, event_thread, NULL, NULL ) )
    {
        endret ( THREAD_INIT_FAILURE );
    }

end:

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_EVENT, __func__, " event init end.\n" );

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " error: %d!\n", ( PAL_INT32_T ) ret );
    }

    return ret;
}

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
Result add_event ( PAL_UINT32_T time, EventType type, char *eventName, void *arg )
{
    Result ret = TCMAGENT_OK;
    EventList *existEvent = NULL, *prev = NULL;

    timimg_list_lock ();

    if ( !IS_VALID_EVENT_TYPE ( type ) )
    {
        endret ( EVENT_TYPE_ERROR );
    }

    if ( NULL == eventName || 0 == strlen ( eventName ) )
    {
        endret ( EVENT_NAME_ERROR );
    }

    existEvent = get_event_from_timing_list ( eventName, time, &prev );

    if ( existEvent )
    {

        if ( prev == existEvent )
        {
            prev = existEvent->prev;
        }

        delete_list ( existEvent );

    }
    else
    {
        if ( NULL == ( existEvent = ( EventList * ) PAL_MALLOC ( sizeof ( EventList ) ) ) )
        {
            endret ( MALLOC_FAILURE );
        }
    }

    existEvent->time = time;
    strcpy ( existEvent->event.eventName, eventName );
    existEvent->event.arg = arg;
    existEvent->event.func.type = type;
    existEvent->event.func.event_func = eventFunc[ ( PAL_INT32_T ) type].event_func;

    add_list ( prev, existEvent );

end:

    timimg_list_unlock ();

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " error: %d!\n", ( PAL_INT32_T ) ret );
    }

    return ret;
}

//**************************************************************************
// Function Name: set_event
// Description  : set the delay time of event, if event is not existed, then it's invalid.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           time: how much seconds after it the event will happen.
//                           eventName: the name of event, echo event must have the different name.
//**************************************************************************
Result set_event ( PAL_UINT32_T time, char *eventName )
{
    Result ret = TCMAGENT_OK;
    EventList *existEvent = NULL, *prev = NULL;

    timimg_list_lock ();

    if ( NULL == eventName || 0 == strlen ( eventName ) )
    {
        endret ( EVENT_NAME_ERROR );
    }

    existEvent = get_event_from_timing_list ( eventName, time, &prev );

    if ( existEvent )
    {

        if ( prev == existEvent )
        {
            prev = existEvent->prev;
        }

        delete_list ( existEvent );

        existEvent->time = time;

        add_list ( prev, existEvent );
    }
    else
    {
        ret = EVENT_NOT_FOUND;
    }

end:

    timimg_list_unlock ();

    if ( TCMAGENT_OK != ret && EVENT_NOT_FOUND != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " error: %d!\n", ( PAL_INT32_T ) ret );
    }

    return ret;
}

//**************************************************************************
// Function Name: delete_event
// Description  : delete a event from event list.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           eventName: the name of event, echo event must have the different name.
//**************************************************************************
Result delete_event ( char *eventName )
{
    Result ret = TCMAGENT_OK;

    EventList *existEvent = NULL, *prev = NULL;

    timimg_list_lock ();

    if ( NULL == eventName || 0 == strlen ( eventName ) )
    {
        endret ( EVENT_NAME_ERROR );
    }

    existEvent = get_event_from_timing_list ( eventName, 0, &prev );

    if ( existEvent )
    {
        delete_list ( existEvent );

        if ( NULL != existEvent->event.arg )
        {
            PAL_FREE ( existEvent->event.arg );
        }

        PAL_FREE ( existEvent );

    }
    else
    {
        ret = EVENT_NOT_FOUND;
    }

end:

    timimg_list_unlock ();

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " error: %d!\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: event_inform
Description:  callback function of inform event, set the event code and wake up the main_session.

Return: according to the Result.
Input:   arg
Output: 
Others:
************************************************************************/
void event_inform ( void *arg )
{
    if (NULL != arg)
    {
        InformArg *argP = (InformArg *)arg;
	    gEventCode |= argP->event;
    }

    mainloop_wakeup ();
}

/***********************************************************************
Function: event_download
Description:  callback function of download event, do the download action and get the result.

Return: according to the Result.
Input:   arg
Output: 
Others:
************************************************************************/
void event_download ( void *arg )
{
    FILE *file;
    PAL_INT32_T retTmp;
    char fileName[MAX_FILENAME_LEN + 1];
    char *fileContent;
    PAL_UINT32_T fileLen;
    DownloadArg *argI;
    DownloadResponseArg response;

    if ( NULL == arg )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                   , " arg is NULL!\n" );
        end();
    }

    argI = ( DownloadArg * ) arg;

    strcpy ( fileName, argI->fileName );
	
    pal_datetime (response.startTime);

    if (strncmp(argI->url, "http", 4) == 0)
    {
        response.status = (PAL_INT32_T)http_download_file(argI->url, argI->username
                        , argI->password, &fileContent, &fileLen);	
    }
    else if (strncmp(argI->url, "ftp", 3) == 0)
    {
        response.status = ftp_download_file (argI->url, argI->username
                        , argI->password, &fileContent, &fileLen);
    }
    else
    {
        response.status = (PAL_INT32_T)UNSUPPORTED_PROTCOL_TRANSFER;
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                  , "unsupported protocol transfer\n");
        return;
    }
    strcpy ( response.commandKey, argI->commandKey );
	
    if ( TCMAGENT_OK == response.status )
    {    
        if ( TCMAGENT_OK != ( retTmp = gCB.do_download ( argI->fileType,argI->targetFileName
                , fileContent, fileLen ) ) )
        {
            response.status = retTmp;
        }		
    }

    pal_datetime (response.completeTime);

    tmp_file_lock ();

    if ( NULL == ( file = open_from_result_dir ( fileName, "wb" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                   , " open file %s failure!\n", fileName );
        tmp_file_unlock ();
        end ();
    }

    if ( 0 >= fwrite ( &response, sizeof ( response ), 1, file ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                   , " write file %s failure!\n", fileName );
    }
    fclose ( file );

        if ( TCMAGENT_OK != delete_from_task_dir ( fileName ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                   , " delete file %s failure!\n", fileName );
    }
    else 
    {
        gEventCode |= EVENT_TRANSFER_COMPLETE;
        strcpy (gTransferCommandKey, response.commandKey);
    }
    tmp_file_unlock ();

end:
    if(response.status == TCMAGENT_OK && strcmp(argI->fileType,DOWNLOAD_FIRMWARE_TYPE) == 0)
	  gReboot = 1; //gavin add to reboot the board
	
    if (NULL != fileContent)
    {
        PAL_FREE (fileContent);
    }

    return;
}

/***********************************************************************
Function: event_upload
Description:  callback function of upload event, do the upload action and get the result.

Return: according to the Result.
Input:   arg
Output: 
Others:
************************************************************************/
void event_upload ( void *arg )
{
    FILE *file;
    PAL_INT32_T retTmp;
    char fileName[MAX_FILENAME_LEN + 1];
    char *fileContent = NULL;
    PAL_UINT32_T fileLen;
    UploadArg *argI;
    UploadResponseArg response;

    if ( NULL == arg )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " arg is NULL!\n" );
        end();
    }

    memset ( &response, 0, sizeof ( response ) );

    argI = ( UploadArg * ) arg;

    strcpy ( fileName, argI->fileName );

    strcpy ( response.commandKey, argI->commandKey );

    pal_datetime (response.startTime);

        if ( TCMAGENT_OK != ( retTmp = gCB.do_upload ( argI->fileType, &fileContent, &fileLen ) ) )
    {
        response.status = retTmp;
    }
    else
    {
        if ( strncmp ( argI->url, "ftp", 3 )  == 0 )
        {
            response.status = ftp_upload_file(argI->fileType, argI->url
                            , argI->username, argI->password
                            , fileContent, fileLen);
        }
        else if ( strncmp ( argI->url, "http", 4 )  == 0 )
        {
            response.status = http_upload_file(argI->fileType, argI->url
                            , argI->username, argI->password
                            , fileContent, fileLen);
        }
        else
        {
            response.status = UNSUPPORTED_PROTCOL_TRANSFER;
            pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                      , "unsupported protocol transfer\n");
            end ();
        }
    }

    pal_datetime (response.completeTime);

    tmp_file_lock ();

    if ( NULL == ( file = open_from_result_dir ( fileName, "wb+" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                   , " open file %s failureL!\n", fileName );
        tmp_file_unlock ();
        end ();
    }

    if ( 0 > fwrite ( &response, sizeof ( response ), 1, file ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                   , " write file %s failureL!\n", fileName );
    }

    fclose ( file );

    if ( TCMAGENT_OK != delete_from_task_dir ( fileName ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                   , " delete file %s failureL!\n", fileName );
    } 
    else
    {
        gEventCode |= EVENT_TRANSFER_COMPLETE;
	    strcpy (gTransferCommandKey, response.commandKey);
    }

    tmp_file_unlock ();

end:
    if (NULL != fileContent)
    {
        PAL_FREE (fileContent);
    }

    return;
}

/***********************************************************************
Function: event_schedule
Description:  callback function of schedule event, set the event code and wake up the main session.

Return: according to the Result.
Input:   arg
Output: 
Others:
************************************************************************/
void event_schedule ( void *arg )
{
    FILE *file;
    char fileName[MAX_FILENAME_LEN + 1];
    ScheduleArg *argI;
    ScheduleResponseArg response;
	 InformArg *informArg;

    fileName[0] = '\0';

    tmp_file_lock ();

    if ( NULL == arg )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " arg is NULL!" );
        end ();
    }

    argI = ( ScheduleArg * ) arg;

    strcpy ( fileName, argI->fileName );
    strcpy ( response.commandKey, argI->commandKey );

   /*  not need to write the file to back up
    if ( NULL == ( file = open_from_result_dir ( fileName, "wb+" ) ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " open file %s failure!\n", fileName );
        end ();
    }

    if ( 0 > fwrite ( &response, sizeof ( response ), 1, file ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " write file %s failure!\n", fileName );
    }

    fclose ( file );
   */

    if ( TCMAGENT_OK != delete_from_task_dir ( fileName ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " delete file %s failure!\n", fileName );
    }

        if (NULL == (informArg = PAL_MALLOC (sizeof (InformArg))))
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__, " malloc failure!\n");
			
            set_event ( NOW, INFORM_EVENT_NAME );
        } else {
            informArg->event = EVENT_SCHEDULED;
	     strcpy (gScheduleCommandKey, response.commandKey);
	     add_event ( NOW, INFORM, INFORM_EVENT_NAME, informArg);
        }
end:

    tmp_file_unlock ();

    return;
}

/*****************************************************************************
 Function Name : event_stun_notify
 Description : function for stun notification to tr069 agent
 Returns : NONE
 Parameter : @arg
             SN_TYPE_NAT_IN_USE:           nat is in use
             SN_TYPE_NAT_NOT_IN_USE:       nat is not in use
             SN_TYPE_UDP_CONN_REQ_ARRIVED: ACS udp connection request is arrived
             SN_TYPE_UDP_CONN_REQ_ADDR_CHANGED: udp connection request address 
                                                has been changed.
******************************************************************************/
void event_stun_notify ( void *arg )
{
    PAL_INT32_T type = *((PAL_INT32_T *)arg);
    InformArg *informArgPtr = NULL;

    printf("in function event_stun_notify\n");

    informArgPtr = (InformArg *)PAL_MALLOC(sizeof(InformArg));
    if (informArgPtr == NULL)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_EVENT, __func__
                  , "%s\n", strerror(errno));
        return;
    }

    switch (type)
    {
        case SN_TYPE_UDP_CONN_REQ_ADDR_CHANGED:
        case SN_TYPE_NAT_IN_USE:
        case SN_TYPE_NAT_NOT_IN_USE:
            informArgPtr->event = EVENT_VALUE_CHANGE;
            add_event(NOW, INFORM, INFORM_EVENT_NAME, informArgPtr);
            break;
        case SN_TYPE_UDP_CONN_REQ_ARRIVED:
            informArgPtr->event = EVENT_CONNECTION_REQUEST;
            add_event(NOW, INFORM, INFORM_EVENT_NAME, informArgPtr);
            break;
        default:
            break;
    }
}

