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
File name: tr69_agent_public.h
Author: kiffen guan
Date: May 17th. 2009
History:
         2009-5-28  add EventCodeType,TR069 ERROR TYPE,HTTP SOCKET STATUS,etc. (Gavin)
         2009-6-11  add CLI in enmu Module (Hook Guo)
         2009-6-17  add ftp, stun, crq, cli error types (Hook Guo)
************************************************************************/

#ifndef _TR069_AGENT_PUBLIC_H_
#define _TR069_AGENT_PUBLIC_H_

#include <stdio.h>
#include "../tr69_agent.h"
#include "pal.h"

/* ******************************************************************
reboot,schedule inform,download,upload method will need commandkey,
the others just use related bit position to mark it.
**********************************************************************/
typedef enum _EventCodeType
{
    EVENT_NONE=0,
    EVENT_BOOTSTRAP = 1,
    EVENT_BOOT = 2,
    EVENT_PERIODIC = 4,
    EVENT_SCHEDULED = 8,
    EVENT_VALUE_CHANGE = 16,
    EVENT_KICKED = 32,
    EVENT_CONNECTION_REQUEST = 64,
    EVENT_TRANSFER_COMPLETE = 128,
    EVENT_DIAGNOSTICS = 256,
    EVENT_M_REBOOT = 512,
    EVENT_REQUEST_DOWNLOAD = 1024,
}EventCodeType;

enum _EventCode;

typedef enum _EventFileType
{
    FIRMWARE = 1,
    WEB_CONTENT,
    VENDOR_CONFIGURATION_FILE,
} EventFileType;

extern TcmAgentCB gCB;
extern PAL_MUTEX_T gTmpFileLock;
extern PAL_INT32_T gInitSuccess;
extern EventCodeType gEventCode;

#define init_successful() (gInitSuccess = 1)
#define wait_init() do { \
 if (gInitSuccess) \
     break; \
 pal_sleep (1); \
} while(1)

#define tmp_file_lock() pal_mutex_lock (&gTmpFileLock);
#define tmp_file_unlock() pal_mutex_unlock (&gTmpFileLock);

#define set_event_code(x) (gEventCode |= x)
#define unset_event_code(x) (gEventCode &= ~x)
#define reset_event_code() (gEventCode = 0)

#define OUT
#define IN

#define TIME_STR_LENGTH 32

#define NO_BOOTSTRAP_EVENTCODE_FILE "no_bootstrap_eventcode"
#define DOWNLOAD_EVENTCODE_FILE "down_eventcode."
#define SCHEDULE_EVENTCODE_FILE "schedule_eventcode."
#define UPLOAD_EVENTCODE_FILE "upload_eventcode."

#define  MREBOOT_EVENT_FILE "MREBOOT"
#define REQUEST_DOWNLOAD_FILE "request_download"
extern char gLastParameterKey[];

typedef enum
{
    TCMAGENT_OK = 0,

    /* ------ xml error ----- */
    XML_NEW_ELEMENT_ERROR,

    /* ------ tr069 error ----- */
    /* other error */
    CONFIG_FILE_NOT_FOUND,
    INSTANCE_FILE_NOT_FOUND,
    CONFIG_FILE_WRITE_ERROR,
    CACHE_TYPE_ERROR,

    /* node error */
    NODE_NAME_ERROR,
    NODE_NOT_FOUND,
    NODE_TYPE_ERROR,
    NODE_NUM_ERROR,
    NODE_VALUE_ERROR, /* 10 */
    NODE_ACCESSLIST_CHECK_ERROR, 
    NODE_ACCESS_ERROR,

    /* instance error */
    INSTANCES_IS_FULL,

    /* attribute error */
    ATTRIBUTE_NAME_ERROR,
    ATTRIBUTE_TYPE_ERROR,
    ATTRIBUTE_ACCESS_ERROR,
    ATTRIBUTE_NOTIFICATION_ERROR,
    ATTRIBUTE_NEEDREBOOT_ERROR,
    ATTRIBUTE_NEXTLEVELNUM_ERROR,
    ATTRIBUTE_RANGE_ERROR, /* 20 */
    ATTRIBUTE_STYLE_ERROR,  
    ATTRIBUTE_MAXNUMOFENTRIES_ERROR,
    ATTRIBUTE_DEFAULTVALLUE_ERROR,
    ATTRIBUTE_ACCESSLIST_ERROR,

    /* information */
    NODE_NEED_NOTIFICATION,   /* active */
    NODE_NEED_NOTIFICATION_PASSIVE, /* passive */
    NODE_NEED_GET_FROM_USER,

    /* ----- event error ------ */
    EVENT_NAME_ERROR,
    EVENT_TYPE_ERROR,
    EVENT_NOT_FOUND,

    /* ----- host handler error ---- */
    MSG_TYPE_ERROR, /* 30 */
    MSG_LENGTH_ERROR,     
    MSG_NAME_LENGTH_ERROR,
    MSG_NAME_LENGTH_OPTION_LENGTH_ERROR,
    MSG_VALUE_TYPE_LENGTH_ERROR,
    MSG_VALUE_TYPE_ERROR,
    MSG_VALUE_ERROR,

    /* ------ system error ------ */
    MALLOC_FAILURE,
    POINTER_FAILURE,
    MUTEX_INIT_FAILURE,
    THREAD_INIT_FAILURE, /* 40 */
    FILE_REMOVE_FAILURE,  
    MSG_INIT_FAILURE,


    /*-------SOAP PARSE STATUS -----*/
    EMPTY_PACKET,
    FAULT_PACKET,
    SOAP_PARSER_ERROR,
    PARAMETER_ERROR,

    /*--------HTTP SOCKET STATUS ----*/
    HTTP_AUTH_ERROR,
    HTTP_FORMAT_ERROR,
    SOCKET_PARAM_ERROR,
    SOCKET_IOCTL_ERROR,     /* 50 */
    SOCKET_CREATE_ERROR,   
    SOCKET_FDSET_ERROR,
    SOCKET_RW_ERROR,
    SOCKET_TIMEOUT,
    SSL_LIB_ERROR,
    SSL_NOT_SUPPORT,
    SSL_CERTIFICATE_ERROR,
    SSL_BIND_ERROR,

    /* cli ERROR type */
    CLI_ERR_FAILED,

    /* stun ERROR type */
    STUN_ERR_TIMEOUT,
    STUN_ERR_SOCKET_ERROR,
    STUN_ERR_NO_RESPONSE,
    STUN_ERR_FAILED,
    STUN_ERR_NEED_AUTH,
    STUN_ERR_UNAUTH,

    /* ftp ERROR type */
    FTP_ERR_FAILED,
    FTP_ERR_SOCK_ERROR,
    FTP_ERR_ERROR_RESP,
    FTP_ERR_RESP_UNEXPECTED,
    FTP_ERR_TIMEOUT,
    FTP_EOF,

    /* crq ERROR type */
    CRQ_ERR_SOCK_ERROR,
    CRQ_ERR_TIMEOUT,
    CRQ_ERR_FAILED,
    CRQ_ERR_INVALID,
    CRQ_ERR_NOTFOUND,

    /* ------tr069 ERROR Type------*/
    METHOD_NOT_SUPPORTED = 9000,
    REQUEST_DENIED = 9001,
    INTERNAL_ERROR  = 9002,
    INVALID_ARGUMENTS = 9003,
    RESOURCES_EXCEEDED = 9004,
    INVALID_PARAMETERNAME = 9005,
    INVALID_PARAMETERTYPE = 9006,
    INVALID_PARAMETERVALUE = 9007,
    ATTEMP_SET_NONWRITABLE = 9008,
    NOTIFICATION_REQUEST_REJECTED = 9009,
    DOWNLOAD_FAILURE = 9010,
    UPLOAD_FAILURE = 9011,
    FILE_TRANSFER_AUTHFAILURE = 9012,
    UNSUPPORTED_PROTCOL_TRANSFER = 9013
} Result;

typedef enum
{
    TCMAGENT_CLIENT = 1,
    HOST_HANDLER,
    TCMAGENT_STUN,
    CLI,
    MAX_MODULE,
} Module;

extern PAL_INT32_T msg_send ( Module dest, Module src, char *buf, PAL_SIZE_T bufLen );
extern PAL_INT32_T msg_recv ( Module dest, Module *src, char *buf, PAL_SIZE_T bufLen );

extern FILE *open_from_task_dir ( char *fileName, char *mode );
extern FILE *open_from_result_dir ( char *fileName, char *mode );
extern Result delete_from_result_dir ( char *fileName );
extern Result delete_from_task_dir ( char *fileName );
extern PAL_INT32_T string_to_num ( char *str, OUT PAL_INT32_T *value );


void free_string_buffer ( IN OUT char ** buffer, IN  OUT PAL_UINT32_T *bufLen );

Result update_managementServer_variables ();
void init_global_keys();

/* Add by Hook, 08/26/2009 03:30:29 PM */
/* verify the existence of the required directories */
#define VERIFY_CONF_DIR(pathname, exit) do {  \
  PAL_DIR_T *dir;  \
  if ((dir = pal_opendir(pathname)) == NULL) \
  { \
   if (pal_mkdir(pathname) == -1) \
   { \
    fprintf(stderr, "failed to create directory %s\n", (pathname)); \
    if (exit) \
    { \
     return -1; /* FIXME: what error code should i return ?? */ \
    } \
   } \
  } \
  else \
  { \
   pal_closedir(dir); \
  } \
 }while(0)


#endif /* _TR069_AGENT_PUBLIC_H_ */

