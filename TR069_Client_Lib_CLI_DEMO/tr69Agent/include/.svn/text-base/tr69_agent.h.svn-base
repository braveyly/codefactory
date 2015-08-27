/**********************************************************************
Copyright (c) 2005 Wuhan Tecom Co., Ltd.
All Rights Reserved
No portions of this material may be reproduced in any form without the
written permission of:
Wuhan Tecom Co., Ltd.
#18, Huaguang Road
Wuhan, PR China 430074
All information contained in this document is Wuhan Tecom Co., Ltd.
company private, proprietary, and trade secret.
***********************************************************************/
#ifndef _TR69_AGENT_H_
#define _TR69_AGENT_H_
#include <stdio.h>

#define DOWNLOAD_FIRMWARE_TYPE "1 Firmware Upgrade Image"
#define DOWNLOAD_CONFIG_TYPE "3 Vendor Configuration File"
#define DOWNLOAD_WEBCONTENT_TYPE "2 Web Content"

#define UPLOAD_CONFIG_TYPE "1 Vendor Configuration File"
#define UPLOAD_LOG_TYPE "2 Vendor Log File"

#define DEFAULT_X_NAME "x.txt"
#define DEFAULT_CONFIG_NAME "os.conf"
#define DEFAULT_FIRMWARE_NAME "firmware"
#define DEFAULT_LOG_NAME "messages"
#define DEFAULT_WEBCONTENT_NAME "webcontent"

typedef enum _ApplyStatus
{
    APPLY_ERROR = -1,   
    APPLIED = 0, /* for status return , create */
    NOT_APPLIED = 1 /* for validated and committed,but not applied */
}ApplyStatus;

#define CPE_CRQ_PORT  50006 /* for tcp connection request */
#define CPE_CRQ_UDP_PORT 50001 /* for udp connection request */
#define PAL_TMP_TASK_DIR "/bin/conf/task/"

#define PAL_INIT_END_NOTIFY "/tmp/tcmagent_init"

#define PAL_TMP_RESULT_DIR "/bin/conf/result/"
#define PAL_CONFIG_BASE "/bin/conf/"
#define PAL_CONFIG_FILE "/bin/conf/configuration.xml"
#define PAL_INSTANCE_FILE "/bin/conf/instance_attributes"

#define TR69_REQUEST_DOWNLOAD_FILE "/nv/tr69_conf/task/request_download"
#define PAL_CERTIFICATE_DIR "/bin/conf/cert/"

#define PAL_MSG_ID_FILE "/bin/conf/msgID"
#define PAL_TMP_MEM_DIR "/bin/conf/mem/"
#define CLI_SHOW_TREE_TMP_FILE PAL_TMP_MEM_DIR"tr69_tree.$$$$"

#define CA_CERTIFICATE_FILE "/bin/conf/cert/ca.crt"      /*for ssl and certificate authentication */
#define CPE_CERTIFICATE_FILE "/bin/conf/cert/cpe.crt"
#define CPE_KEY_FILE "/bin/conf/cert/cpe.key"

#define IS_MSG_REQUEST_VALID(type) (INIT_END <= type && type <= SET_VALUE_EX)
#define IS_MSG_RESPONSE_VALID(type) (MSG_OK == type || (NAME_ERROR <= type && type <= NAME_ERROR))
#define IS_MSG_VALUE_TYPE_VALID(type) (MSG_INT <= type && type <= MSG_STRING)

#define MAX_TR069_NAME_LEN 256
#define MAX_SEND_MSG_LEN 8192
#define MAX_RECV_MSG_LEN 8192
#define MAX_MSG_TYPE_LEN 4
#define MAX_LEN_FOR_MSG_NAME_LENGTH 4
#define MAX_LEN_FOR_MSG_VALUE_TYPE 4
#define MAX_NUM_LEN 32
/*
  * Notice: If you want to modify the enum MsgType, pleare review whether IS_MSG_REQUEST_VALID()
  * and IS_MSG_RESPONSE_VALID() need modified.
  */
typedef enum
{
    /* request */
    INIT_END = 1,
    SET_VALUE,
    GET_VALUE,
    ADD_OBJECT,
    DELETE_OBJECT,
    RESET_DEFAULT,
    SET_LOG_MODE,
    GET_LOG_MODE,
    SET_LOG_LEVEL,
    GET_LOG_LEVEL,
    GET_TR069_TREE,
    GET_TR069_TREE_SIZE,
    DIAGNOSTICS_COMPLETE,
    SET_VALUE_EX,

    /* response */
    MSG_OK = 0,
    NAME_ERROR = SET_VALUE_EX + 1,
    TYPE_ERROR,
    LENGTH_ERROR,
    NAME_LENGTH_ERROR,
    NAME_LENGTH_OPTION_LENGTH_ERROR,
    VALUE_ERROR,
    VALUE_TYPE_TYPE_ERROR,
    VALUE_TYPE_LENGTH_ERROR,
} MsgType;

/*
  * Notice: If you want to modify the enum MsgValueType, pleare review whether IS_MSG_VALUE_TYPE_VALID()
  * need modified.
  */
typedef enum _MsgValueType
{
    MSG_INT = 1,
    MSG_UINT = 2,
    MSG_BOOL = 3,
    MSG_STRING = 4,
} MsgValueType;

typedef struct _SessionInfo
{
    int attribute_changed;
} SessionInfo;
typedef int ( *cb_session_begin ) ( void );
typedef int (*cb_session_end)(SessionInfo *info);
typedef ApplyStatus ( *cb_set_value ) ( char *, char * );
typedef ApplyStatus ( *cb_get_value ) ( char *, char * );
typedef ApplyStatus(*cb_add_object)(char *, int *);
typedef ApplyStatus ( *cb_delete_object ) ( char *name );

/* to release the memory for download files */
typedef int ( *cb_kill_other_apps ) ();

typedef int ( *cb_do_upload ) ( char *, char **, unsigned int * );
typedef int ( *cb_do_download ) ( char* , char *, char *, unsigned int);
typedef int ( *cb_do_reboot ) ( void );
typedef int ( *cb_factory_reset ) ( void );
typedef int ( *cb_get_wan_ipaddress ) ( char *ipaddr );
typedef int ( *cb_get_clientkey_passwd ) ( char *, int * );

typedef struct _TcmAgentCB
{
    cb_session_begin session_begin;
    cb_session_end session_end;
    cb_set_value set_value;
    cb_get_value get_value;
    cb_add_object add_object;
    cb_delete_object delete_object;
    cb_kill_other_apps  kill_other_apps;
    cb_do_upload do_upload;
    cb_do_download do_download;
    cb_do_reboot do_reboot;
    cb_factory_reset factory_reset;
    cb_get_clientkey_passwd get_clientkey_passwd;
    cb_get_wan_ipaddress get_wan_ipaddress;/* stun,mainloop,will use it */
} TcmAgentCB;

typedef struct _MsgHeader
{
    unsigned int   totalLen;
    unsigned int   type;
} MsgHeader;

extern int get_msg_from_tcmAgent ( char *buf,   size_t bufLen );
extern int send_msg_to_tcmAgent ( char *buf,  size_t bufLen );

extern int   start_tcmAgent ( TcmAgentCB *arg );
extern void stop_tcmAgent ( void );

#endif

