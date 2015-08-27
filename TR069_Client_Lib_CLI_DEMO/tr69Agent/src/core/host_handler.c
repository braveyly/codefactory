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
__FILE__ name: host_handler.c
Author: kiffen guan
Date: May 17th. 2009
History:
         2009-6-11    add SET_LOG_LEVEL and SET_LOG_MODE msg
                      handler routine, adjust some text format to
                      fit for the coding standard (Hook Guo)
         2009-6-14    add SHOW_TR69_TREE msg handler routine (Hook Guo)
         2009-6-25    modified SET_VALUE, GET_VALUE msg handler routine
                      (Hook Guo)
************************************************************************/

#include "host_handler.h"
#include "tr69_agent.h"

PAL_UINT32_T gDownloadFreeIndex;
PAL_UINT32_T gUploadFreeIndex;
PAL_UINT32_T gScheduleFreeIndex;

extern PAL_MUTEX_T gStackInitSyncMutex;
extern ManagementServer gManagementServer;
extern EventCodeType gEventCode;

static PAL_INT32_T downIndex[MAX_INDEX + 1];
static PAL_INT32_T uploadIndex[MAX_INDEX + 1];
static PAL_INT32_T scheduleIndex[MAX_INDEX + 1];

char gOutputDIR[256];

#define retry() do { \
    needDelay = 1; \
    goto begin; \
} while (0)

void mk_file (char *fileName)
{
    FILE *fp;

    if (NULL != (fp = fopen (fileName, "w")))
    {
        fprintf (fp, "");
	 fclose (fp);
    }
}

/***********************************************************************
Function: get_msg_type
Description:  get the msg type from the buf.

Return: according to the Result.
Input:   bufLen - valid buffer len.
            buf
Output: bufLen - the valid buffer len except the msg type option.
            buf 
            type
Others:
************************************************************************/
static void get_msg_type ( IN OUT PAL_UINT32_T *bufLen, IN OUT char **buf, MsgType *type )
{
     *type = (MsgType) *(*buf);
  //  *type = ( MsgType ) *((MsgType *)*buf);
    *buf += sizeof ( MsgType );
    *bufLen -= sizeof ( MsgType );
}

/***********************************************************************
Function: get_msg_name
Description:  get the msg name from the buf.

Return: according to the Result.
Input:   bufLen - valid buffer len.
            buf
Output: bufLen - the valid buffer len except the msg name option.
            buf 
            name
Others:
************************************************************************/
static Result get_msg_name ( IN OUT char **buf,  IN OUT PAL_UINT32_T *bufLen, OUT char *name )
{
    PAL_UINT32_T nameLen;
    Result ret = TCMAGENT_OK;

    if ( *bufLen < MAX_LEN_FOR_MSG_NAME_LENGTH )
    {
        endret ( MSG_NAME_LENGTH_OPTION_LENGTH_ERROR );
    }
    else
    {
        nameLen = * (*buf);
        //nameLen = * ( ( PAL_UINT32_T * ) * buf );

        if ( nameLen > MAX_TR069_NAME_LEN + 1 )
        {
            endret ( MSG_NAME_LENGTH_ERROR );
        }

        *buf += sizeof ( nameLen );
        *bufLen -= ( sizeof ( nameLen ) + nameLen );
        memcpy ( name, *buf, nameLen );
        *buf += nameLen;
    }

end:

    return ( ret );
}

/***********************************************************************
Function: get_msg_value
Description:  get the msg value from the buf.

Return: according to the Result.
Input:   bufLen - valid buffer len.
            buf
Output: bufLen - the valid buffer len except the msg value option.
            buf 
            value
Others:
************************************************************************/
static Result get_msg_value ( IN OUT char **buf, IN OUT PAL_UINT32_T *bufLen
                              , Tr069ValueArg *value )
{
    Result ret = TCMAGENT_OK;

    if ( *bufLen < MAX_LEN_FOR_MSG_VALUE_TYPE )
    {
        endret ( MSG_VALUE_TYPE_LENGTH_ERROR );
    }
    else
    {
        MsgValueType type;

        type = *(*buf);
//    type = ( MsgValueType ) ( *((MsgValueType *)*buf));

        if ( !IS_MSG_VALUE_TYPE_VALID ( type ) )
        {
            endret ( MSG_VALUE_TYPE_ERROR );
        }

        *buf += sizeof ( type );
        *bufLen -= sizeof ( type );

        value->next = NULL;
        value->value.string = NULL;

        switch ( type )
        {
            case MSG_INT:
                value->type = tInt;
                memcpy (&value->value.intv, *buf, sizeof (int));
                *buf += sizeof ( PAL_INT32_T );
                *bufLen -= sizeof ( PAL_INT32_T );
                break;

            case MSG_UINT:
                value->type = tUnsigned;
                memcpy (&value->value.uintv, *buf, sizeof (unsigned int));

                *buf += sizeof ( PAL_UINT32_T );
                *bufLen -= sizeof ( PAL_INT32_T );
                break;

            case MSG_BOOL:
                value->type = tBool;
                memcpy (&value->value.boolv, *buf, sizeof (int));

                *buf += sizeof ( PAL_INT32_T );
                *bufLen -= sizeof ( PAL_INT32_T );
                break;

            case MSG_STRING:
                value->type = tString;
                value->value.string = PAL_MALLOC ( strlen ( *buf ) + 1 );
                strcpy ( value->value.string, *buf );
                *bufLen -= ( strlen ( *buf ) + 1 );
                *buf += ( strlen ( *buf ) + 1 );
                break;

            default:
                break; /* it's impossible to here, only cut warning */
        }
    }

end:

    return ( ret );
}

/***********************************************************************
Function: put_msg_name
Description:  put the msg name to the buffer.

Return: according to the Result.
Input:   bufLen - valid buffer len.
            buf
            name 
            nameLen
Output: bufLen - the valid buffer len include the msg name option.
            buf 
Others:
************************************************************************/
static void put_msg_name ( IN OUT PAL_UINT32_T *bufLen,  IN OUT char **buf, char *name, PAL_UINT32_T nameLen )
{
    *(*buf) = nameLen;
    //( * ( PAL_UINT32_T * ) *buf ) = nameLen;
    *buf += MAX_LEN_FOR_MSG_NAME_LENGTH;
    memcpy ( *buf, name, nameLen );
    *buf += nameLen;
    *bufLen += ( MAX_LEN_FOR_MSG_NAME_LENGTH + nameLen );
}

/***********************************************************************
Function: put_msg_value
Description:  put the msg value to the buffer.

Return: according to the Result.
Input:   bufLen - valid buffer len.
            buf
            value
Output: bufLen - the valid buffer len include the msg name option.
            buf 
Others:
************************************************************************/
static void put_msg_value ( IN OUT PAL_UINT32_T *bufLen, IN OUT char **buf, Tr069ValueArg *value )
{
    Tr069TypeArg type = value->type;
    * (*buf) = (PAL_INT32_T)value->type;
    *buf += sizeof ( PAL_UINT32_T );
    *bufLen += MAX_LEN_FOR_MSG_VALUE_TYPE;

    switch ( type )
    {

        case MSG_INT:
        case MSG_BOOL:
        {
            memcpy (*buf, &value->value.intv, sizeof (PAL_INT32_T));
            *buf += sizeof ( PAL_INT32_T );
            *bufLen += sizeof ( PAL_INT32_T );
            break;
        }

        case MSG_UINT:
        {
            memcpy (*buf, &value->value.uintv, sizeof (PAL_UINT32_T));
            *buf += sizeof ( PAL_UINT32_T );
            *bufLen += sizeof ( PAL_UINT32_T );
            break;
        }

        case MSG_STRING:
        {
            strcpy ( *buf, value->value.string );
            *buf += ( strlen ( value->value.string ) + 1 );
            *bufLen += ( strlen ( value->value.string ) + 1 );
            break;
        }

        default :
            break; /* caller must guarantee it's impossible to here */
    }
}

/***********************************************************************
Function: get_result
Description:  get the msg return code from the result.

Return: the msg return code.
Input:   ret
Output: 
Others:
************************************************************************/
static MsgType get_result ( Result ret )
{
    MsgType type;

#if 0
    MSG_TYPE_ERROR,
    MSG_LENGTH_ERROR,      /* 30 */
    MSG_NAME_LENGTH_ERROR,
    MSG_NAME_LENGTH_OPTION_LENGTH_ERROR,
    MSG_VALUE_TYPE_LENGTH_ERROR,
    MSG_VALUE_TYPE_ERROR,
    MSG_VALUE_ERROR,
#endif
    if ( TCMAGENT_OK == ret )
    {
        type = MSG_OK;
    }
    else if ( MSG_TYPE_ERROR == ret)
    {
        type = TYPE_ERROR;
    } else {
        type = VALUE_ERROR;
    }
	
    return type;
}

/***********************************************************************
Function: init_delay_events
Description:  init the delay event to the event list such as download, upload, schedule.

Return: 
Input:   
Output: 
Others:
************************************************************************/
void init_delay_events ( void )
{
    char fileName[MAX_FILENAME_LEN + 1];
    void *arg = NULL;
    FILE *fp = NULL;
    PAL_UINT32_T size;
    EventType type;
    PAL_TIME_T happenTime;
    PAL_UINT32_T time;
    PAL_TIME_T now;
    PAL_INT32_T i, validDownIndex, validUploadIndex, validScheduleIndex;

    validDownIndex = 0;
    validUploadIndex = 0;
    validScheduleIndex = 0;

    gEventCode |= EVENT_BOOTSTRAP;
    /* scan the PAL_TMP_TASK_DIR, and add the event of delay to event list. */
    while ( 0 == pal_scan_dir ( PAL_TMP_TASK_DIR, fileName ) )
    {
        char *tmp;
        PAL_INT32_T index;
        if (0 == strcmp (fileName, NO_BOOTSTRAP_EVENTCODE_FILE))
        {
            gEventCode &= (~EVENT_BOOTSTRAP);
        }

        if ( NULL != ( tmp = strstr ( fileName, DOWNLOAD_EVENTCODE_FILE ) ) )
        {
            tmp += strlen ( DOWNLOAD_EVENTCODE_FILE );

            if ( string_to_num ( tmp, &index ) )
            {
                if (index < 0 || index > MAX_INDEX )
                {
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failure!\n", fileName );
                }
                downIndex[index] = 1;
            }
            else
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failure!\n", fileName );
                continue;
            }
        }
        else if ( NULL != ( tmp = strstr ( fileName, UPLOAD_EVENTCODE_FILE ) ) )
        {
            tmp += strlen ( UPLOAD_EVENTCODE_FILE );

            if ( string_to_num ( tmp, &index ) )
            {
                if ( index < 0 || index > MAX_INDEX )
                {
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failure!\n", fileName );
                }

                uploadIndex[index] = 1;
            }
            else
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failure!\n", fileName );
                continue;
            }

        }
        else if ( NULL != ( tmp = strstr ( fileName, SCHEDULE_EVENTCODE_FILE ) ) )
        {
            tmp += strlen ( SCHEDULE_EVENTCODE_FILE );

            if ( string_to_num ( tmp, &index ) )
            {
                if ( index < 0 || index > MAX_INDEX )
                {
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failure!\n", fileName );
                }

                scheduleIndex[index] = 1;
            }
            else
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failure!\n", fileName );
                continue;
            }
        }
    }

    for ( i = 1; i < MAX_INDEX + 1; i++ )
    {
        if ( downIndex[i] )
        {
            if ( NULL == ( arg = ( DownloadArg * ) PAL_MALLOC ( sizeof ( DownloadArg ) ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " malloc failure!\n" );
            }
            else
            {
                type = DOWNLOAD;
                size = sizeof ( DownloadArg );

                if ( i >= gDownloadFreeIndex )
                {
                    if ( MAX_INDEX == i )
                    {
                        if ( validDownIndex )
                        {
                            gDownloadFreeIndex = validDownIndex;
                        }
                        else
                        {
                            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, "no valid download index.!\n" );
                        }
                    }
                    else
                    {
                        gDownloadFreeIndex = i + 1;
                    }
                }

                strcpy ( fileName, DOWNLOAD_EVENTCODE_FILE );
                sprintf ( fileName, "%s%d", fileName, i );

                if ( NULL == ( fp = open_from_task_dir ( fileName, "rb" ) ) )
                {
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " open __FILE__ %s failureL!\n", fileName );
                }
                else
                {
                    if ( 1 != fread ( arg,  size, 1, fp ) )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failureL!\n", fileName );
                        fclose ( fp );
                    }
                    else
                    {
                        happenTime = ( ( DownloadArg * ) arg )->time;

                        if ( pal_time_current ( &now ) )
                        { 
                            time = NOW;
                        }
                        else
                        {
                            if ( now >= happenTime )
                            {
                                time = NOW;
                            }
                            else if ( ( happenTime - now ) >= EVENT_MAX_DELAY_TIME )
                            {
                                time = NOW;
                            }
                            else
                            {
                                time = ( PAL_UINT32_T ) ( happenTime - now );
                            }
                        }

                        fclose ( fp );
                        add_event ( time, type, fileName, arg );
                    }
                }
            }
        }
        else
        {
            if ( !validDownIndex )
            {
                validDownIndex = i;
            }
        }

        if ( uploadIndex[i] )
        {
            if ( NULL == ( arg = ( UploadArg * ) PAL_MALLOC ( sizeof ( UploadArg ) ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " malloc failure!\n" );
            }
            else
            {
                type = UPLOAD;
                size = sizeof ( UploadArg );

                if ( i >= gUploadFreeIndex )
                {
                    if ( MAX_INDEX == i )
                    {
                        if ( validUploadIndex )
                        {
                            gUploadFreeIndex = validUploadIndex;
                        }
                        else
                        {
                            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, "no valid upload index.!\n" );
                        }
                    }
                    else
                    {
                        gUploadFreeIndex = i + 1;
                    }
                }

                strcpy ( fileName, UPLOAD_EVENTCODE_FILE );
                sprintf ( fileName, "%s%d", fileName, i );

                if ( NULL == ( fp = open_from_task_dir ( fileName, "rb" ) ) )
                {
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " open __FILE__ %s failureL!\n", fileName );
                }
                else
                {
                    if ( 1 != fread ( arg,  size, 1, fp ) )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failureL!\n", fileName );
                        fclose ( fp );
                    }
                    else
                    {
                        happenTime = ( ( UploadArg * ) arg )->time;

                        if ( pal_time_current ( &now ) )
                        {
                            time = NOW;
                        }
                        else
                        {
                            if ( now >= happenTime )
                            {
                                time = NOW;
                            }
                            else if ( ( happenTime - now ) >= EVENT_MAX_DELAY_TIME )
                            {
                                time = NOW;
                            }
                            else
                            {
                                time = ( PAL_UINT32_T ) ( happenTime - now );
                            }
                        }

                        fclose ( fp );
                        add_event ( time, type, fileName, arg );
                    }
                }
            }
        }
        else
        {
            if ( !validUploadIndex )
            {
                validUploadIndex = i;
            }
        }

        if ( scheduleIndex[i] )
        {
            if ( NULL == ( arg = ( ScheduleArg * ) PAL_MALLOC ( sizeof ( ScheduleArg ) ) ) )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " malloc failure!\n" );
            }
            else
            {
                type = SCHEDULE;
                size = sizeof ( ScheduleArg );

                if ( i >= gScheduleFreeIndex )
                {
                    if ( MAX_INDEX == i )
                    {
                        if ( validScheduleIndex )
                        {
                            gScheduleFreeIndex = validScheduleIndex;
                        }
                        else
                        {
                            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, "no valid schedule index.!\n" );
                        }
                    }
                    else
                    {
                        gScheduleFreeIndex = i + 1;
                    }
                }

                strcpy ( fileName, SCHEDULE_EVENTCODE_FILE );
                sprintf ( fileName, "%s%d", fileName, i );

                if ( NULL == ( fp = open_from_task_dir ( fileName, "rb" ) ) )
                {
                    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " open __FILE__ %s failureL!\n", fileName );
                }
                else
                {
                    if ( 1 != fread ( arg,  size, 1, fp ) )
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " read __FILE__ %s failureL!\n", fileName );
                        fclose ( fp );
                    }
                    else
                    {
                        happenTime = ( ( ScheduleArg * ) arg )->time;

                        if ( pal_time_current ( &now ) )
                        {
                            time = NOW;
                        }
                        else
                        {
                            if ( now >= happenTime )
                            {
                                time = NOW;
                            }
                            else if ( ( happenTime - now ) >= EVENT_MAX_DELAY_TIME )
                            {
                                time = NOW;
                            }
                            else
                            {
                                time = ( PAL_UINT32_T ) ( happenTime - now );
                            }
                        }

                        fclose ( fp );
                        add_event ( time, type, fileName, arg );
                    }
                }
            }
        }
        else
        {
            if ( !validScheduleIndex )
            {
                validScheduleIndex = i;
            }
        }
    }
}

/* for compatible purpose */
void assemble_msg(IN char *sndBuf
                 , IN PAL_INT32_T *sndbufLenPtr
                 , IN MsgType msgType
                 , IN char *msgName
                 , IN MsgValueType msgValueType
                 , IN void *msgValue
                 , IN PAL_INT32_T msgValueLen)
{
    char *p = sndBuf;
    PAL_INT32_T _msgTotalLen;

    p += sizeof (PAL_INT32_T); /* skip msg total length */
    *((PAL_INT32_T *)p) = (PAL_INT32_T)msgType; /* msg type */
    p += sizeof (PAL_INT32_T); /* walk through msg type */
    *((PAL_INT32_T *)p) = strlen(msgName) + 1; /* msg name length including '\0' */
    p += sizeof (PAL_INT32_T); /* walk through msg name length */
    memcpy(p, msgName, strlen(msgName)); /* msg name */
    p += strlen(msgName);
    *p++ = '\0';
    if (msgValue != NULL)
    {               
        *((MsgValueType *)p) = msgValueType; /* msg value type */
        p += sizeof (MsgValueType); /* walk through msg value type */
        memcpy(p, msgValue, msgValueLen); /* msg value */
        p += msgValueLen; /* walk through msg value */
    }
      
    _msgTotalLen = p - sndBuf; /* msg total length */
    *((PAL_INT32_T *)sndBuf) = _msgTotalLen;

    if (sndbufLenPtr != NULL)
    {
        *sndbufLenPtr = _msgTotalLen;
    }
}

/* for compatible purpose */
void fetch_msg_info(IN char *buf
                   , IN PAL_INT32_T bufsiz
                   , OUT MsgType *msgTypePtr
                   , OUT char *msgName
                   , OUT MsgValueType *msgValueTypePtr
                   , OUT void *msgValuePtr
                   , OUT PAL_INT32_T *msgValueLenPtr)
{
    PAL_INT32_T _msgTotalLen, _msgNameLen;
    MsgType _msgType;
    MsgValueType _msgValueType;
    char _msgName[MSG_NAME_MAX_LEN + 1];

    char *p = buf;
    _msgTotalLen = *((PAL_INT32_T *)p);
    p += sizeof (PAL_INT32_T);
    _msgType = *((MsgType *)p);
    p += sizeof (MsgType);
    _msgNameLen = *((PAL_INT32_T *)p);
    p += sizeof (PAL_INT32_T);
    memcpy(_msgName, p, _msgNameLen);
    p += _msgNameLen;
    _msgName[_msgNameLen] = '\0';

    if ((p - buf) >= _msgTotalLen)
    {
        goto parseEnd_tag;
    }

    /* parse message value */
    _msgValueType = *((MsgValueType *)p);
    p += sizeof (MsgValueType);    

    if (msgValueTypePtr != NULL)
    {
        *msgValueTypePtr = _msgValueType;
    }

    if (msgValuePtr == NULL || msgValueLenPtr == NULL)
    {
        goto parseEnd_tag;
    }

    switch (_msgValueType)
    {
        case MSG_INT:
            *((PAL_INT32_T *)msgValuePtr) = *((PAL_INT32_T *)p);
            *msgValueLenPtr = sizeof (PAL_INT32_T);
            break;
        case MSG_UINT:
            *((PAL_UINT32_T *)msgValuePtr) = *((PAL_UINT32_T *)p);
            *msgValueLenPtr = sizeof (PAL_UINT32_T);
            break;
        case MSG_BOOL:
            *((PAL_BOOL_T *)msgValuePtr) = *((PAL_BOOL_T *)p);
            *msgValueLenPtr = sizeof (PAL_BOOL_T);
            break;
        case MSG_STRING:
            strcpy((char *)msgValuePtr, (char *)p);
            *msgValueLenPtr = strlen((char *)p);
            break;
        default:
            break;
    }

parseEnd_tag:
    if (msgTypePtr != NULL)
    {
        *msgTypePtr = _msgType;
    }

    if (msgName != NULL)
    {
        strcpy(msgName, _msgName);
    }

    return;
}

/***********************************************************************
Function: handle_msg
Description:  handle the msg and get the return msg.

Return: according to the Result.
Input:   buf
            bufLen
Output: buf
            bufLen
Others:
************************************************************************/
static Result handle_msg ( IN OUT char *buf, IN OUT PAL_UINT32_T *bufLen )
{
    Result ret = TCMAGENT_OK, retTmp;
    MsgHeader *msgHeader;
    char name[MAX_TR069_NAME_LEN + 1];
    static char retBuf[MAX_TCM_SEND_MSG_LEN];
    PAL_UINT32_T rLen = *bufLen;
    PAL_UINT32_T *retTotalLen;
    MsgType retType;
    char *retPtr = retBuf;
    char *errorPtr;
    PAL_INT32_T errorLen;
    char *bufPtr = buf;
    PAL_UINT32_T type;
    PAL_INT32_T needUpdateManagementServer = 0;
    static PAL_INT32_T firstGetInitEnd = 1;

    char *currMsgSegPtr = buf;
    PAL_INT32_T nleft = *bufLen;

    /* defined by Hook Guo */
    char sndBuf[MAX_TCM_SEND_MSG_LEN + 1];
    char _msgName[MSG_NAME_MAX_LEN + 1];
    PAL_INT32_T _msgNameLen;
    MsgValueType _msgValueType;
    void *_msgValue = NULL;
    PAL_INT32_T _msgValueLen = 0;
    char _paramValue[MSG_VALUE_PARAM_NAME_LEN + 1]; /* for SET_VALUE */
    PAL_INT32_T _logLevel; /* for SET_LOG_LEVEL */
    PAL_INT32_T _logMode; /* for SET_LOG_MODE */
    PAL_INT32_T _msgTotalLen;
    /* end of define by Hook Guo */
        msgHeader = ( MsgHeader * ) buf;
    errorPtr = buf;
    errorLen = *bufLen;

    if ( *bufLen < sizeof ( MsgHeader ) )
    {
        endret ( MSG_LENGTH_ERROR );
    }

    rLen = msgHeader->totalLen;

    bufPtr = buf + sizeof ( msgHeader->totalLen );

    memset ( retBuf, 0, sizeof ( retBuf ) );
    retPtr = retBuf;
    retTotalLen = (PAL_UINT32_T *) retPtr;
    retPtr += sizeof(PAL_UINT32_T);

    if (0 == msgHeader->totalLen)
    {
        endret(TCMAGENT_OK);
    }

    do
    {
        PAL_INT32_T currMsgSegLen = *((PAL_INT32_T *)currMsgSegPtr);

        errorPtr = bufPtr;
        errorLen = rLen;
#if 0
        /* kiffen debug */
        do
        {	
            char *tmp = bufPtr;
	        MsgType type;
	        int valueType;
	        int totalLen = rLen;
	        char name[MAX_TR069_NAME_LEN + 1];
            printf ("\n handle_msg\n");
            printf (" totalLen-%d", totalLen);
            get_msg_type (&totalLen, &tmp, &type);
            printf (" type-%d", type);

	        if (INIT_END == type)
	        {
	            printf (" init_end\n");
		        break;
	        }
            else
            {
	            printf (" nameLen-%d", *tmp);
	        }
	        get_msg_name (&tmp, &totalLen, name);

            printf (" name %s", name);
        
            if (SET_VALUE == type)
            {
                valueType = *tmp;
                tmp += 4;
        	    totalLen -= 4;
        
      		    if (MSG_STRING == valueType)
     		    {
                    printf (" type-%d value-%s\n", valueType, tmp);
     			    totalLen -= (strlen(tmp) + 1);
     			    tmp += (strlen (tmp) + 1);
      		    }
                else
                {
                    int intValue;

                    memcpy (&intValue, tmp, sizeof (int));
                    printf (" type-%d value-%d\n",  valueType, intValue);
		            tmp += 4;
        		    totalLen -= 4;
      		    }
            } 
            else if (ADD_OBJECT == type) 
            {
                printf (" add\n");
            }
            else if (DELETE_OBJECT == type) 
            { 
                printf ("delete\n");
            }
        } while (0);
        /* end of kiffen debug */
#endif
        get_msg_type ( &rLen, &bufPtr, &type );
        
        if ( !IS_MSG_REQUEST_VALID ( type ) )
        {
            endret ( MSG_TYPE_ERROR );
        }

        switch ( type )
        {
            case INIT_END:
            {
                pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " host handler get init end msg.\n");

                if ( firstGetInitEnd )
                {
                    InformArg *arg;

                    load_instance_attributes();

                    init_delay_events();

                    if (NULL == (arg = PAL_MALLOC(sizeof(InformArg))))
                    {
                        pal_logger (T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " malloc failure!\n");
                    } 
                    else
                    {
                        gEventCode &= (~EVENT_VALUE_CHANGE);	//first connect ,not need value change	
                        arg->event = (EVENT_BOOT | (EVENT_BOOTSTRAP & gEventCode));	
                        /* add the inform event to event list, and set time to NOW to wake up the main loop */
                        add_event ( NOW, INFORM, INFORM_EVENT_NAME, arg );
                    }
                    firstGetInitEnd = 0;
			  
                    init_successful ();

		      		mk_file (PAL_INIT_END_NOTIFY);

                    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " host handler init end msg handle end.\n" );	

                }
				 
                break;
            }

	        case DIAGNOSTICS_COMPLETE:
	        {
		         InformArg *informArg;

		         if (NULL == (informArg = PAL_MALLOC (sizeof (InformArg))))
		         {
		             pal_logger (T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " malloc failure!\n");
		        }
                else
                {	  
		            informArg->event = EVENT_DIAGNOSTICS;
		            add_event (NOW, INFORM, INFORM_EVENT_NAME, informArg);
		        }
	            break;
	        }

            case SET_VALUE:
            {
                Tr069ValueArg value;

                if ( TCMAGENT_OK != ( retTmp = get_msg_name ( &bufPtr,  &rLen, name ) ) )
                {
                    endret ( retTmp );
                }

                value.next = NULL;
                value.value.string = NULL;

                if ( TCMAGENT_OK != ( retTmp = get_msg_value ( &bufPtr, &rLen, &value ) ) )
                {
                    endret ( retTmp );
                }

                retTmp = set_tr069_value(WEB_MODULE, name, &value);

                if (NODE_NEED_NOTIFICATION == retTmp || NODE_NEED_NOTIFICATION_PASSIVE == retTmp)
                {
                    set_event_code ( EVENT_VALUE_CHANGE );
                }

                if ( NODE_NEED_NOTIFICATION == retTmp || need_inform ( name ) )
                {
                    InformArg *arg;
                    if (NULL == (arg = PAL_MALLOC (sizeof (InformArg))))
                    {
                        pal_logger (T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER
                                   , __func__, " malloc failure!\n");
                    } 
                    else
                    {
                        arg->event = EVENT_VALUE_CHANGE;
			            add_event ( NOW, INFORM, INFORM_EVENT_NAME, arg );
                    }
                }
                if ( value.type == tString && NULL != value.value.string )
                {
                    if ( NULL != value.value.string )
                    {
                        PAL_FREE ( value.value.string );
                    }
                }
                if ( TCMAGENT_OK != retTmp && NODE_NEED_NOTIFICATION != retTmp && NODE_NEED_NOTIFICATION_PASSIVE != retTmp)
                {
                    endret ( retTmp );
                } 
                else
                {
                    if (!needUpdateManagementServer)
                    {
                        if (strstr (name, "ManagementServer"))
                        {
                            needUpdateManagementServer = 1;
                        }
                    }
                }

                break;
            }
            
            /* same as SET_VALUE case, used by stun and cli module ONLY,
             * just for compatible purpose */
            case SET_VALUE_EX:
            {
                MsgType _t = MSG_OK;
                Tr069ValueArg _v;

                /* fetch information from msg queue */
                fetch_msg_info(currMsgSegPtr
                              , currMsgSegLen
                              , NULL
                              , _msgName
                              , &_msgValueType
                              , _paramValue
                              , &_msgValueLen);
                
                switch (_msgValueType)
                {
                    case MSG_INT:
                        _v.type = tInt;
                        _v.value.intv = *((PAL_INT32_T *)_paramValue);
                        _v.next = NULL;
                        break;                        
                    case MSG_UINT:
                        _v.type = tUnsigned;
                        _v.value.uintv = *((PAL_UINT32_T *)_paramValue);
                        _v.next = NULL;
                        break;
                    case MSG_BOOL:
                        _v.type = tBool;
                        _v.value.boolv = *((PAL_BOOL_T *)_paramValue);
                        _v.next = NULL;
                        break;
                    case MSG_STRING:
                        _v.type = tString;
                        _v.value.string = strdup(_paramValue);
                        _v.next = NULL;
                        break;
                    default:
                        return -1;
                }

                /* set tr069 tree value */
                if ((ret = set_tr069_value(WEB_MODULE, _msgName, &_v)) == NODE_NEED_NOTIFICATION 
					|| ret == NODE_NEED_NOTIFICATION_PASSIVE)
                {
                    set_event_code(EVENT_VALUE_CHANGE);
                }

                /* append an notification event if needed */
                if (NODE_NEED_NOTIFICATION == ret || need_inform(name))
                {
                    InformArg *arg;
                    if (NULL == (arg = PAL_MALLOC (sizeof (InformArg))))
                    {
                        pal_logger (T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER
                                   , __func__, "%s\n", strerror(errno));
                    } 
                    else
                    {
                        arg->event = EVENT_VALUE_CHANGE;
			            add_event (NOW, INFORM, INFORM_EVENT_NAME, arg);
                    }
                }

                if (_v.type == tString && _v.value.string != NULL)
                {
                    PAL_FREE(_v.value.string);
                }

                if ((ret != TCMAGENT_OK) && (ret != NODE_NEED_NOTIFICATION) && (ret != NODE_NEED_NOTIFICATION_PASSIVE))
                {
                    /* FIXME: problem may not cause by NAME_ERROR,
                     * however, it's OK here for this type can indicate an 
                     * error anyway... */
                    _t = NAME_ERROR;
                    goto set_value_ex_endtag;
                }
                else
                {
                    if (!needUpdateManagementServer && strstr(name, "ManagementServer"))
                    {
                        char oldUrl[MAX_USERNAME_LENGTH+ 1];

			   			memset (oldUrl, 0, sizeof (oldUrl));

			   			strncpy (oldUrl, gManagementServer.acsURL, sizeof (oldUrl) - 1);
			   
                        update_managementServer_variables();

			   if (!firstGetInitEnd && strcmp (gManagementServer.acsURL, oldUrl))
			   {
                            printf("ACS url has been changed by acs server,old[%s],new[%s]", oldUrl, gManagementServer.acsURL);
                            delete_from_task_dir(NO_BOOTSTRAP_EVENTCODE_FILE);
                            gEventCode |= EVENT_BOOTSTRAP;			       
			   }
                    }
                }

set_value_ex_endtag:
                /* assemble response message */                
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , _t
                            , _msgName
                            , 0
                            , NULL
                            , 0);
                
                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                    return TCMAGENT_OK;
            }
			
            case GET_TR069_TREE_SIZE:
            {	
                PAL_UINT32_T tr69TreeSize;

                /* fetch information from msg queue */
                fetch_msg_info(currMsgSegPtr
                              , currMsgSegLen
                              , NULL
                              , _msgName
                              , &_msgValueType
                              , NULL
                              , NULL);

		        tr69TreeSize = get_tr069_tree_size();

                /* assamble response message */
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , MSG_OK /* msg type */
                            , MN_GET_TR069_TREE_SIZE /* msg name */
                            , MSG_INT /* msg value type */
                            , (void *)&tr69TreeSize /* msg value */
                            , sizeof(PAL_UINT32_T)); /* msg value len */

                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                return TCMAGENT_OK;				
            }

            case GET_VALUE:
            {
                Tr069NameArg *narg = NULL;
                Tr069ValueArg *varg = NULL;
                MsgType _t;
                Result ret2;

                /* fetch information from msg queue */
                fetch_msg_info(currMsgSegPtr
                              , currMsgSegLen
                              , NULL
                              , _msgName
                              , NULL
                              , NULL
                              , NULL);

                /* get tr069 tree value */
                ret2 = get_tr069_value(_msgName, &narg, &varg);
                
                if (ret2 != TCMAGENT_OK && ret2 != NODE_NEED_NOTIFICATION && ret2 != NODE_NEED_NOTIFICATION_PASSIVE)
                {
                    /* FIXME: problem may not cause by NAME_ERROR,
                     * however, it's OK here for this type can indicate an 
                     * error anyway... */
                    _t = NAME_ERROR;
                    _msgValue = NULL;
                    _msgValueType = 0;
                    _msgValueLen = 0;
                }
                else
                {
                    _t = MSG_OK;
                    _msgValueType = MSG_STRING;
                    _msgValue = (void *)varg->value.string;
                    _msgValueLen = strlen(varg->value.string) + 1;
                }

                /* assemble response message */
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , _t
                            , _msgName
                            , _msgValueType
                            , _msgValue
                            , _msgValueLen);
                
                if (varg != NULL && varg->value.string != NULL)
                {
                    PAL_FREE(varg->value.string);
                }

                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                    return TCMAGENT_OK;
            }

            case ADD_OBJECT:
            {
                PAL_UINT32_T index;
                Tr069ValueArg value;

                memset ( &value, 0, sizeof ( value ) );

                if ( TCMAGENT_OK != ( retTmp = get_msg_name ( &bufPtr,  &rLen, name ) ) )
                {
                    endret ( retTmp );
                }

                if ( TCMAGENT_OK != ( retTmp = add_tr069_object ( name, &index ) ) )
                {
                    endret ( retTmp );
                }

                /* assign type */
                *retPtr = ( PAL_INT32_T ) MSG_OK;
                *retTotalLen += sizeof ( PAL_INT32_T );
                retPtr += sizeof ( PAL_INT32_T );

                /* assign name */
                put_msg_name ( retTotalLen, &retPtr, name, strlen ( name ) + 1 );

                /* assign value */
                value.type = tUnsigned;
                value.value.uintv = index;
                put_msg_value ( retTotalLen, &retPtr, &value );

                break;
            }

            case DELETE_OBJECT:
            {
                if ( TCMAGENT_OK != ( retTmp = get_msg_name ( &bufPtr,  &rLen, name ) ) )
                {
                    endret ( retTmp );
                }

                if ( TCMAGENT_OK != ( retTmp = delete_tr069_object ( name ) ) )
                {
                    endret ( retTmp );
                }

                break;
            }

            case RESET_DEFAULT:
            {
                delete_from_task_dir ( NO_BOOTSTRAP_EVENTCODE_FILE );

                break;
            }

            case SET_LOG_LEVEL:
            {      
                /* fetch information from msg queue */
                fetch_msg_info(currMsgSegPtr
                              , currMsgSegLen
                              , NULL
                              , _msgName
                              , &_msgValueType
                              , (void *)&_logLevel
                              , &_msgValueLen);

                /* set log mode */
                pal_set_log_level(_logLevel);

                /* assemble response message */
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , MSG_OK
                            , MN_SET_LOG_LEVEL
                            , 0
                            , NULL
                            , 0);

                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                    return TCMAGENT_OK;
            }

            case SET_LOG_MODE:
            {
                /* fetch information from msg queue */
                fetch_msg_info(currMsgSegPtr
                              , currMsgSegLen
                              , NULL
                              , _msgName
                              , &_msgValueType
                              , (void *)&_logMode
                                  , &_msgValueLen);

                /* set log mode */
                pal_set_log_mode(_logMode);

                /* assemble response message */
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , MSG_OK
                            , MN_SET_LOG_MODE
                            , 0
                            , NULL
                            , 0);

                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                    return TCMAGENT_OK;
            }

            case GET_TR069_TREE:
            {
                /* fetch information from msg queue */
                fetch_msg_info(currMsgSegPtr
                              , currMsgSegLen
                              , NULL
                              , _msgName
                              , &_msgValueType
                              , (void *)gOutputDIR
                              , &_msgValueLen);

                /* write tr069_tree to the user specified location */
                print_tr069_nodes(); 

                /* assamble response message */        
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , MSG_OK
                            , MN_GET_TR069_TREE
                            , 0
                            , NULL
                            , 0);

                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                    return TCMAGENT_OK;
            }

            case GET_LOG_LEVEL:
            {
                Pal_LogLevel _ll;

                /* get log level */
                pal_get_log_level(&_ll); 

                /* assamble response message */
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , MSG_OK /* msg type */
                            , MN_GET_LOG_LEVEL /* msg name */
                            , MSG_INT /* msg value type */
                            , (void *)&_ll /* msg value */
                            , sizeof(_ll)); /* msg value len */

                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                    return TCMAGENT_OK;
            }

            case GET_LOG_MODE:
            {
                LogMode _lm;

                pal_get_log_mode(&_lm); 

                /* assamble response message */
                assemble_msg(sndBuf
                            , &_msgTotalLen
                            , MSG_OK /* msg type */
                            , MN_GET_LOG_MODE /* msg name */
                            , MSG_INT /* msg value type */
                            , (void *)&_lm /* msg value */
                            , sizeof(_lm)); /* msg value len */

                /* FIXME: only single message supported in this case */
                memcpy(buf, sndBuf, _msgTotalLen);
                *bufLen = _msgTotalLen;
                    return TCMAGENT_OK;
            }
        } /* end of switch */
    }
    while ( rLen > 0 );

end:

    /* error deal */
    retType = get_result ( ret );

    if (needUpdateManagementServer)
    {
        char oldUrl[MAX_USERNAME_LENGTH+ 1];

        memset (oldUrl, 0, sizeof (oldUrl));

        strncpy (oldUrl, gManagementServer.acsURL, sizeof (oldUrl) - 1);
	   
        update_managementServer_variables();

        if (!firstGetInitEnd && strcmp (gManagementServer.acsURL, oldUrl))
	 {
	     printf("ACS url has been changed by acs server,old[%s],new[%s]", oldUrl, gManagementServer.acsURL);
	     delete_from_task_dir(NO_BOOTSTRAP_EVENTCODE_FILE);
	     gEventCode |= EVENT_BOOTSTRAP;			       
	 }	
    }

    if ( TCMAGENT_OK != ret && NODE_NEED_NOTIFICATION != ret && NODE_NEED_NOTIFICATION_PASSIVE !=ret)
    {
        *retTotalLen += errorLen;
        *errorPtr = ( PAL_INT32_T ) retType;
        memcpy ( retPtr, errorPtr, errorLen );
    }
    else if ( retPtr == ( retBuf + sizeof ( PAL_INT32_T ) ) )
    {
        *retTotalLen += sizeof ( PAL_INT32_T );
        *retPtr = ( PAL_INT32_T ) retType;
    }

    memcpy ( buf, retBuf, *retTotalLen + sizeof ( *retTotalLen ) );
    *bufLen = *retTotalLen + sizeof ( *retTotalLen );

    return ret;
}

/***********************************************************************
Function: host_handler_thread
Description:  the host handler thread.

Return: void *.
Input:   arg
Output:
Others:
************************************************************************/
static void * host_handler_thread ( void *arg )
{
    Result ret = TCMAGENT_OK, retTmp;
    /*
      * because MAX_TCM_SEND_MSG_LEN is bigger than MAX_TCM_RECV_MSG_LEN, so define the buf of
      * the bigger size used to send and recv.
      */
    char msg[MAX_TCM_SEND_MSG_LEN];
    PAL_UINT32_T msgLen;
    PAL_UINT32_T needDelay = 0;
    Module src;

    gDownloadFreeIndex = 1;
    gUploadFreeIndex = 1;
    gScheduleFreeIndex = 1;

    pal_mutex_unlock(&gStackInitSyncMutex);

    do
    {
begin:
        if ( needDelay )
        {
            pal_sleep ( 1 );
        }

        /* recv msg */
        if ( ( msgLen = msg_recv ( HOST_HANDLER, &src, msg, sizeof ( msg ) ) ) < 0 )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " receive msg error!\n" );
            retry ();
        }

        /* handle msg */
        if ( TCMAGENT_OK != ( retTmp = handle_msg ( msg, &msgLen ) ) )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " handle msg error %d!\n", ( PAL_INT32_T ) retTmp );
        }

        /* send response */
        //msg_send ( src, HOST_HANDLER, msg, sizeof ( msg ) );

        needDelay = 0;

    }
    while ( 1 );

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " error %d!\n", ( PAL_INT32_T ) ret );
    }

    return NULL;
}

/***********************************************************************
Function: init_host_handler
Description:  init the host hanlde module.

Return: void *.
Input:  
Output:
Others:
************************************************************************/
Result init_host_handler ( void )
{
        Result ret = TCMAGENT_OK;
        PAL_THREAD_T hostHandlerID;
        PAL_THREAD_ATTR_T a;

        gDownloadFreeIndex = 0;
        gUploadFreeIndex = 0;
        gScheduleFreeIndex = 0;

        memset ( downIndex, 0, sizeof ( downIndex ) );
        memset ( uploadIndex, 0, sizeof ( uploadIndex ) );
        memset ( scheduleIndex, 0, sizeof ( scheduleIndex ) );

        //pal_set_logger ( LOG_HOST_HANDLER, T_LOG_INFO, LOG_CONSOLE, 1024,  "host_hander" );

        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " host handler init start.\n" );

        if ( pal_thread_create ( &hostHandlerID, host_handler_thread, NULL, NULL ) )
        {
            endret ( THREAD_INIT_FAILURE );
        }
end:

        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, " host handler init end.\n" );

        if ( TCMAGENT_OK != ret )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HOST_HANDLER, __func__, "  error %d!\n", ( PAL_INT32_T ) ret );
        }

        return ret;
}

