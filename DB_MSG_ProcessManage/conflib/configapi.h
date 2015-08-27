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
***********************************************************************
 Filename:    configapi.h
 Author:      Mark Yan
 Date:        2009-03-13
 Description:

 Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/

#ifndef __CONFIG_API__H
#define __CONFIG_API__H


#include "conflib.h"
#include <tcm_log.h>
/* print log to a file, if want stdout, please comment it */
//#define   LOG_TO_FILE

//#define   config_debug

#ifdef    config_debug

#define   CFG_DEBUG(stream, format, args...)   if(stream)fprintf(stream, "[Conflib] "format, ##args)
#else
#define   CFG_DEBUG(format, args...)
#endif

#ifdef   LOG_TO_FILE
#define    LOG_FILE         "/var/log/conflib.log"
#endif

#define      PARAMETER_FILE    "/home/nick/etc/os.conf"
#define      DEFAULT_FILE      "/home/nick/etc/default.conf"

#define      KEY_LOCK_CACHE_FILE   0x0f1e2d3c
#define      KEY_SHARE_MEMORY      0xc3d2e1f0

#define      MAX_CONF_LINE      255

#define      MAGIC_NUM_1     0xf0f0f0f0
#define      MAGIC_NUM_2     0x0f0f0f0f

#define      GET_MAGIC_2(buf, size) (int *)((char *)buf + size - sizeof(int))

/* We have to record the offset, because one's pointer can not used in another
   process. */
#define   OFFSET_BASE(addr, base)  ((addr)?(void *)(addr) - base:0)
#define   NEXT(type, offset, base)  (offset?(type *)(offset+base):NULL)

#define    GET_ROW_ID(app_id, row_id)               (((app_id)<<16) + ((row_id)<<8) )
#define    GET_PARA_ID(app_id, row_id, para_id )    (((app_id)<<16) + ((row_id)<<8) + (para_id) )

typedef enum
{
    APP_NODE_TYPE = 0,
    ROW_NODE_TYPE,
    PARA_NODE_TYPE,
} NODE_TYPE;

/* parameter node (2 * 4 + 24 + 64 = 96) */
typedef struct para_node
{
    int      para_id;
    char     para_name[MAX_NAME_LEN];
    char     para_val[MAX_VALUE_LEN];
    int      next;
} para_node_t;

/* row node (3 * 4 = 12) */
typedef struct row_node
{
    int   row_id;
    int   para;
    int   next;
} row_node_t;

/* application node (4 * 4 + 24 = 40) */
typedef struct app_node
{
    int     app_id;
    char    app_name[MAX_NAME_LEN];
    int     para;
    int     row;
    int     next;
} app_node_t;

void dump_para_table ( void );
#endif
