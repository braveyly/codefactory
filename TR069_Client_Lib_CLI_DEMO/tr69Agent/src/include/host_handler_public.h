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
File name: host_handler_public.h
Author: kiffen guan
Date: May 17th. 2009
************************************************************************/

#ifndef _HOST_HANDLER_PUBLIC_H_
#define _HOST_HANDLER_PUBLIC_H_

#include "../tr69_agent.h" 
#include "tr69_agent_public.h"

#define MAX_TCM_RECV_MSG_LEN MAX_SEND_MSG_LEN
#define MAX_TCM_SEND_MSG_LEN MAX_RECV_MSG_LEN
#define MAX_INDEX 65535

extern char gOutputDIR[256];

extern PAL_UINT32_T gDownloadFreeIndex; /* the next free index of postfix name of download information file */
extern PAL_UINT32_T gUploadFreeIndex; /* the next free index of postfix name of upload file */
extern PAL_UINT32_T gScheduleFreeIndex; /* the next free index of postfix name of schedule file */

extern Result init_host_handler ( void );
extern void fetch_msg_info(char *, PAL_INT32_T, MsgType *, char *, MsgValueType *, void *, PAL_INT32_T *);
extern void assemble_msg(char *, PAL_INT32_T *, MsgType, char *, MsgValueType, void *, PAL_INT32_T);

/* msg name definitions */
#define MN_GET_TR069_TREE_SIZE "GET_TR069_TREE_SIZE"
#define MN_GET_TR069_TREE "GET_TR069_TREE"
#define MN_GET_TREE_VALUE "GET_TREE_VALUE"
#define MN_GET_LOG_LEVEL "GET_LOG_LEVEL"
#define MN_GET_LOG_MODE "GET_LOG_MODE"
#define MN_SET_LOG_LEVEL "SET_LOG_LEVEL"
#define MN_SET_LOG_MODE "SET_LOG_MODE"
#define MN_SET_VALUE "SET_VALUE"
#define MN_GET_VALUE "GET_VALUE"

#define MSG_NAME_MAX_LEN 256
#define MSG_VALUE_PARAM_NAME_LEN 128

#endif /* _HOST_HANDLER_PUBLIC_H_ */

