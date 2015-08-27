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
***********************************************************************
* Filename:    launcher.c
* Author:      Daniel Gao
* Date:        2009-03-19
* Description:
*
* Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dls.h>
#include <timer.h>
#include <launcher.h>
#include <tcm_msg.h>


/***********************************************************************
Function: app_launcher
Description: launcher the applications in the IAD

Return: int,
Input: void,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Daniel Gao
        Date: 2009-04-24

    2.  Modification:
        Author:
        Date:
************************************************************************/
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <conflib.h>
#include <tcm.h>
#include <tcm_util.h>
#include <tcm_msg.h>
#include <prctl.h>
#include <signal.h>

#define MAX_PATHLEN  128

void launchApp(DlsInfo *dInfo)
{
   SpawnProcessInfo     spawnInfo;
   SpawnedProcessInfo   procInfo;
   char                 exeBuf[BUFLEN_1024];
   char                 argsBuf[BUFLEN_1024];
   tcmRet               ret;

   /* If exe is empty, return directly */
   if ( strlen(dInfo->eInfo->path) == 0 )
      return;

   dInfo->pid = TCM_INVALID_PID;
   snprintf(exeBuf, sizeof(exeBuf), "%s", dInfo->eInfo->path);
   snprintf(argsBuf, sizeof(argsBuf), "%s", dInfo->eInfo->runArgs);  //Ignore MDM

   if(dInfo->optArgs[0] != 0)
   {
      UINT32 index, remaining, wrote;

      index = strlen(argsBuf);
      remaining = sizeof(argsBuf) - index;
      wrote = snprintf(&(argsBuf[index]), remaining, " %s", dInfo->optArgs);
      if( wrote >= remaining )
      {
         tcmLog_error("Args buf overflow");
      }
   }

   spawnInfo.exe = exeBuf;
   spawnInfo.args = argsBuf;
   spawnInfo.spawnMode = SPAWN_AND_RETURN;
   spawnInfo.stdinFd = 0;
   spawnInfo.stdoutFd = 1;
   spawnInfo.stderrFd = 2;
   spawnInfo.serverFd = dInfo->serverFd;
   spawnInfo.maxFd = maxFd;
   spawnInfo.inheritSigint = TRUE;

   tcmLog_debug("spawning %s args %s", spawnInfo.exe, spawnInfo.args);
   ret = prctl_spawnProcess(&spawnInfo, &procInfo);
   if (ret != TCMRET_SUCCESS)
   {
      tcmLog_error("could not spawn child %s args %s", dInfo->eInfo->path, argsBuf);
      /* smd timer will detect the child is not running */
   }
   else
   {
      dInfo->pid = procInfo.pid;
      if (dInfo->eInfo->flags & APPCFG_MULTIPLE_INSTANCES)
      {
         dInfo->specificEid = MAKE_SPECIFIC_EID(procInfo.pid, dInfo->eInfo->eid);
      }
      /* why not set specificEid to the generic eid when the app does not support multiple instances?
       * It will reduce a lot of checking elsewhere in the code.
       */

      tcmLog_debug("%s launched, pid %d", dInfo->eInfo->name, dInfo->pid);
   }

   if (dInfo->eInfo->flags & APPCFG_MESSAGING_CAPABLE)
   {
      dInfo->state = DLS_LAUNCHED;
   }
   else
   {
      tcmLog_debug("%s is not message capable, mark it as RUNNING without conf.", dInfo->eInfo->name);
      dInfo->state = DLS_RUNNING;
   }


   /* once we launch an app, stop monitoring its fd */
   if ((dInfo->eInfo->flags & APPCFG_SERVER) ||
       (dInfo->eInfo->eid == ID_CONSOLE))
   {
      FD_CLR(dInfo->serverFd, &readFdsMaster);
   }

   return;

}


/**************************************************************
* Signal Event Handle registration                            *
***************************************************************/
UBOOL8 keepLooping = TRUE;

void terminalSignalHandler(SINT32 sig)
{
   tcmLog_notice("got sig %d, terminating by setting keepLooping to FALSE", sig);

   keepLooping = FALSE;
}

void register_signal(void)
{
   signal(SIGTERM, terminalSignalHandler );

   signal(SIGINT, SIG_IGN);
}

/**************************************************************
*  Message Routines                                           *
***************************************************************/
void queuemsg(DlsInfo *dInfo, tcmMsgHeader **msg)
{
   (*msg)->next = dInfo->msgQueue;
   dInfo->msgQueue =(*msg);
   *msg = NULL;

   return;
}

void sendMessageByAppState(DlsInfo *dInfo, tcmMsgHeader **msg)
{
   switch(dInfo->state)
   {
      case DLS_NOT_RUNNING:
         queuemsg(dInfo, msg);
         launchApp(dInfo);
         break;
      case DLS_LAUNCHED:
         queuemsg(dInfo,msg);
         break;
      case DLS_RUNNING:
         tcmMsg_sendBySocket(dInfo->commFd, *msg);
         TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);
         break;
      case DLS_TERMINATE_REQUESTED:
         TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);
         break;
      default:
         break;
   }

   return;
}

//Responsibility:
// 1.Setup basic message exchange environment.
// 2.Check and Launch the Basic network apps here
//TODO: Init timer routines resources here?
tcmRet app_launcher(UINT32 stage)
{
    tcmRet ret = TCMRET_SUCCESS;
    DlsInfo *dInfo;

    dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
    {
      if ( ((stage == 1) && (dInfo->eInfo->flags & APPCFG_LAUNCH_IN_STAGE_1)) ||
           ((stage == 2) && (dInfo->eInfo->flags & APPCFG_LAUNCH_ON_BOOT)) )
      {
         tcmMsgHeader *msg;

         if(dInfo->eInfo->flags & APPCFG_MESSAGING_CAPABLE)
         {
            msg = (tcmMsgHeader *) tcmMem_alloc(sizeof(tcmMsgHeader), ALLOC_ZEROIZE);
            if(msg == NULL)
            {
               tcmLog_error("malloc message header failed.");
               break;
            }

            /* fill in the message */
            msg->type = TCM_MSG_SYSTEM_BOOT;
            msg->src = ID_SCHED;
            msg->dst = dInfo->eInfo->eid;
            msg->flags_event = 1;

            /*
             * An app which was marked as launch-on-boot during stage 2 could
             * have been already launched during ssk's call to cmsMdm_init.
             * So must use sendMessageByState to deliver the message, which
             * will either queue or send the message depending on the app's state.
             */
            sendMessageByAppState(dInfo, &msg);
         }

         if( dInfo->state == DLS_NOT_RUNNING )
         {
            launchApp(dInfo);
         }
         else if(dInfo->state == DLS_TERMINATE_REQUESTED)
         {
            tcmLog_error("App in terminate state, to launch app again.");
            launchApp(dInfo);
         }
      }
    }

    return ret;
}

tcmRet init_launcher()
{
   UINT32 stage = 1;

   //TODO: Signal Handle Registration
   register_signal();

   //TODO: Timer events Initialization
   timer_init();

   //Local Message Communication Init
   FD_ZERO(&readFdsMaster);
   ipcListenFd = init_schedServerSocket();
   if(ipcListenFd == NULL)
   {
      tcmLog_error("Can't Create Local socket Resources.");
      return TCMRET_INTERNAL_ERROR;
   }
   FD_SET(ipcListenFd, &readFdsMaster);   //Update File Set Res
   UPDATE_MAXFD(ipcListenFd);

   //DLS Init
   /* DLS is used for monitoring servers registered and wake up  *
   *  them when client requests got.                             */
   if( initDls(stage) != TCMRET_SUCCESS)
   {
      close(ipcListenFd);
      return TCMRET_INTERNAL_ERROR;
   }

   //Interested List Init
   interest_init();

   return TCMRET_SUCCESS;
}

void clean_launcher()
{
   //Message Resource cleanup
   clean_sched();

   //Clean Dls Resource.
   cleanupDls();
}

