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

#ifndef __DLS_H__
#define __DLS_H__

#include <tcm.h>
#include <tcm_util.h>
#include <tcm_msg.h>

/** Maximum length of optional arguments to a dynamically launched app. */
#define DLS_OPT_ARGS_LENGTH   256

typedef enum
{
   DLS_NOT_RUNNING=0,  /**< not running */
   DLS_LAUNCHED=1,     /**< launched, but waiting for confirmation */
   DLS_RUNNING=2,      /**< fully up and running. */
   DLS_TERMINATE_REQUESTED=3 /**< Requested termination, but waiting for confirmation. */
} DlsState;

typedef struct
{
   DlistNode      dlist;    /** handle for doubly-linked list */
   const tcmEntityInfo *eInfo;    /**< Pointer to entity info entry */
   DlsState       state;    /**< Current state of this dynamically launched process. */
   SINT32         serverFd; /**< Server/listening socket fd */
   SINT32         commFd;   /**< Unix domain TCP connection socket file
                             *   descriptor on the smd process for
                             * inter-process communication with the
                             * application process.
                             */
   SINT32          pid;          /**< pid of the app */
   UINT32          specificEid;  /**< if this app can have multiple instances, combined pid and eid */
   SINT32          numDelayedMsgRequested; /**< number of delayed msgs requested by this app */
   SINT32          numEventInterestRequested; /**< number of event interests requested by this app */
   tcmMsgHeader*   msgQueue; /**< queue of messages waiting to be delivered to this app. */
   char   optArgs[DLS_OPT_ARGS_LENGTH];  /**< additional dynamic args, usually sent in by restart msg */

} DlsInfo;

extern SINT32 ipcListenFd;    /* SMD Unix domain IPC listening socket fd */
extern fd_set readFdsMaster;  /* need to export to oal_sysmon */
extern struct dlist_node dlsInfoHead;  /* Sched managed dual list header */
extern SINT32 maxFd;
#define UPDATE_MAXFD(f)  (maxFd = (f > maxFd) ? f : maxFd)

/**************************************************************
* Name: init_launcher                                         *
* Usage: Init launcher at start up                            *
***************************************************************/
tcmRet init_launcher();

DlsInfo *getDlsInfoByServerFd(SINT32 commFd);

DlsInfo *getDlsInfoByCommFd(SINT32 commFd);

DlsInfo *getDlsInfo(tcmEntityId eid);

#endif

