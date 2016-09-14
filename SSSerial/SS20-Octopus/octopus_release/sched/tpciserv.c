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
* Filename:    tpciserv.c
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
#include <stdarg.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <tcm.h>
#include <dls.h>
#include <tcm_msg.h>
#include "tcm_log.h"
//#include <conflib.h>
#include <tpciserv.h>
#include <launcher.h>
#include <timer.h>
#include <interest.h>
#include <prctl.h>
#include "tcm_tms.h"
#include "tcm_mem.h"
#include "tcm_ast.h"
#define QLEN   10
#define NALLOC 10  /* # client structs to alloc/realloc for */

#define CONNECT_TO_LAUNCH_MSG_TIMEOUT  200


void collectApp(DlsInfo **dInfo)
{
   CollectProcessInfo collectInfo;
   SpawnedProcessInfo procInfo;
   tcmMsgHeader *msg;
   tcmRet ret;


   /*
    * Free any messages that are queued up for this app before
    * we (potentially) free the dInfo struct.
    */
   while ( (msg = (*dInfo)->msgQueue) != NULL )
   {
      (*dInfo)->msgQueue = (*dInfo)->msgQueue->next;
      msg->next = NULL;
      TCMMEM_FREE_BUF_AND_NULL_PTR(msg);
   }


   if ( (*dInfo)->commFd != TCM_INVALID_FD )
   {
      FD_CLR((*dInfo)->commFd, &readFdsMaster);
      close((*dInfo)->commFd);
      (*dInfo)->commFd = TCM_INVALID_FD;
   }

   /*
    * Collect the process if smd was the one that launched it.
    * If the process started by itself (e.g. tftp), its pid will be
    * CMS_INVALID_PID, so smd should not bother collecting it.
    */
   if ( (*dInfo)->pid == TCM_INVALID_PID )
   {
      tcmLog_debug("Do not collect %s, it was not launched by smd", (*dInfo)->eInfo->name);
   }
   else
   {
      collectInfo.collectMode = COLLECT_PID_TIMEOUT;
      collectInfo.pid = (*dInfo)->pid;
      collectInfo.timeout = 5 * MSECS_IN_SEC;  /* wait at most 5 sec */


      if ( (ret = prctl_collectProcess(&collectInfo, &procInfo)) != TCMRET_SUCCESS )
      {
         tcmLog_error("Could not collect %s (pid=%d timeout=%dms), ret=%d.  Kill it with SIGKILL.",
                      (*dInfo)->eInfo->name, (*dInfo)->pid, collectInfo.timeout, ret);
         /*
          * Send SIGKILL and collect the process, otherwise,
          * we end up with a zombie process.
          */
         prctl_signalProcess((*dInfo)->pid, SIGKILL);
         if ( (ret = prctl_collectProcess(&collectInfo, &procInfo)) != TCMRET_SUCCESS )
         {
            tcmLog_error("Still could not collect %s (pid=%d) after SIGKILL, ret=%d",
                         (*dInfo)->eInfo->name, (*dInfo)->pid, ret);
            /* this process is really stuck.  Not much I can do now.
             * Just leave it running I guess. */
         }
         else
         {
            tcmLog_debug("collected %s (pid %d) after SIGKILL", (*dInfo)->eInfo->name, (*dInfo)->pid);
         }
      }
      else
      {
         tcmLog_debug("collected %s (pid %d) signalNum=%d", (*dInfo)->eInfo->name, (*dInfo)->pid, procInfo.signalNumber);
         if ( procInfo.signalNumber == SIGILL ||
              procInfo.signalNumber == SIGABRT ||
              procInfo.signalNumber == SIGFPE ||
              procInfo.signalNumber == SIGSEGV ||
              procInfo.signalNumber == SIGPIPE ||
              procInfo.signalNumber == SIGBUS ||
              procInfo.signalNumber == SIGXCPU ||
              procInfo.signalNumber == SIGXFSZ )
         {
            tcmLog_error("%s (pid %d) exited due to uncaught signal number %d",
                         (*dInfo)->eInfo->name,
                         (*dInfo)->pid,
                         procInfo.signalNumber);

            if ( (*dInfo)->eInfo->flags & APPCFG_AUTO_RELAUNCH )
            {
               tcmLog_error("Auto relaunching %s", (*dInfo)->eInfo->name);
               launchApp(*dInfo);
               return;
            }
         }
      }
   }


   /* start monitoring server fd again (if this app is a server) */
   if ( ((*dInfo)->eInfo->flags & APPCFG_SERVER) ||
        ((*dInfo)->eInfo->eid == ID_CONSOLE) )
   {
      FD_SET((*dInfo)->serverFd, &readFdsMaster);
   }


   /*
    * Free this dInfo if
    * - it does not monitor a server fd, including consoled, and
    * - it is for a multiple instance app or it cannot have multiple instances and it currently has no registration for event interests and delayed messages.
    *
    * Multiple instance apps cannot hold on to event interest or
    * delayed messages once they exit, so we always delete their
    * dInfo structures.
    */
   if ( (((*dInfo)->eInfo->flags & APPCFG_SERVER) == 0) &&
        ((*dInfo)->eInfo->eid != ID_CONSOLE) &&
        (((*dInfo)->eInfo->flags & APPCFG_MULTIPLE_INSTANCES) ||
         ((((*dInfo)->eInfo->flags & APPCFG_MULTIPLE_INSTANCES) == 0) &&
          ((*dInfo)->numDelayedMsgRequested == 0) &&
          ((*dInfo)->numEventInterestRequested == 0))) )
   {
      tcmEntityId eid = ((*dInfo)->eInfo->flags & APPCFG_MULTIPLE_INSTANCES) ? (*dInfo)->specificEid : (*dInfo)->eInfo->eid;
      tcmLog_debug("unlink and free dInfo structure at %p for %s eid=%d (0x%x)", (*dInfo), (*dInfo)->eInfo->name, eid, eid);
      interest_unregisterAll(eid);
      dlist_unlink((DlistNode *) (*dInfo));
      TCMMEM_FREE_BUF_AND_NULL_PTR((*dInfo));
   }
   else
   {
      /* we did not free dInfo, clear some key fields */
      (*dInfo)->state = DLS_NOT_RUNNING;
      (*dInfo)->pid = TCM_INVALID_PID;
      (*dInfo)->specificEid = 0;
   }

   return;
}

/***********************************************************************
Function: lcl_httprestart
Description: an example function call by lcl_sched_requestprocess.
             to present how to deal with a msg.

Return: static int,
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
SINT32 init_schedServerSocket()
{
   struct sockaddr_un serverAddr;
   SINT32 fd, rc;

   /* mwang_todo:this is rather brute force, if we see the file here, maybe
    * another smd is running...
    */
   unlink(SCHED_MESSAGE_ADDR);


   if ( (fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0 )
   {
      tcmLog_error("Could not create socket");
      return fd;
   }

   /*
    * Bind my server address and listen.
    */
   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sun_family = AF_LOCAL;
   strncpy(serverAddr.sun_path, SCHED_MESSAGE_ADDR, sizeof(serverAddr.sun_path));

   rc = bind(fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   if ( rc != 0 )
   {
      tcmLog_error("bind to %s failed, rc=%d errno=%d", SCHED_MESSAGE_ADDR, rc, errno);
      close(fd);
      return NULL;
   }

   rc = listen(fd, SCHED_MESSAGE_BACKLOG);
   if ( rc != 0 )
   {
      tcmLog_error("listen to %s failed, rc=%d errno=%d", SCHED_MESSAGE_ADDR, rc, errno);
      close(fd);
      return NULL;
   }

   tcmLog_notice("smd msg socket opened and ready (fd=%d)", fd);
   return fd;
}


void processMsgSimpleFooter(DlsInfo *dInfo, tcmRet rv, tcmMsgHeader **msg)
{
   tcmRet ret;
   UINT32 tmpSrc;

   tmpSrc = (*msg)->src;
   (*msg)->src = (*msg)->dst;
   (*msg)->dst = tmpSrc;

   (*msg)->flags_request = 0;
   (*msg)->flags_response = 1;
   (*msg)->dataLength = 0;
   (*msg)->wordData = rv;

   ret = tcmMsg_sendBySocket(dInfo->commFd, (*msg));
   if ( ret != TCMRET_SUCCESS )
   {
      tcmLog_error("send response for msg 0x%x failed, ret=%d", (*msg)->type, ret);
   }
   else
   {
      tcmLog_debug("sent response for msg 0x%x dst=%d data=0x%x",
                   (*msg)->type, (*msg)->dst, (*msg)->wordData);
   }

   TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);

   return;
}

void sched_routeMessage(tcmMsgHeader **msg)
{
   tcmEntityId dstEid = (*msg)->dst;
   DlsInfo *dstDInfo;

   dstDInfo = getExistingDlsInfo(dstEid);
   if ( (*msg)->flags_bounceIfNotRunning )
   {
      if ( (dstDInfo == NULL) ||
           (dstDInfo->state == DLS_NOT_RUNNING) ||
           (dstDInfo->state == DLS_TERMINATE_REQUESTED) )
      {
         if ( (*msg)->flags_event )
         {
            TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);
         }
         else
         {
            DlsInfo *srcDInfo = getExistingDlsInfo((*msg)->src);
            processMsgSimpleFooter(srcDInfo, TCM_MSG_BOUNCED, msg);
         }
         return;
      }
   }

   if ( dstDInfo == NULL )
   {
      if ( (dstDInfo = getDlsInfo(dstEid)) == NULL )
      {
         tcmLog_error("Can't find dest eid %d, drop msg", dstEid);
         TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);
         return;
      }

      if ( dstDInfo->eInfo->flags & APPCFG_MULTIPLE_INSTANCES )
      {
         tcmLog_error("Launching multiple instance app (0x%x) to receive msg?", dstEid);
      }
   }

   sendMessageByAppState(dstDInfo, msg);
}

/**************************************************************
*  This Routine distribute message to tcmEntity registered    *
***************************************************************/
void sched_distributeEventMessage(tcmMsgHeader **msg, const DlsInfo *srcDInfo)
{
   tcmEntityId eventSrc;
   DlsInfo *targetDInfo;
   EvtInterest *evtInterest;
   EvtInterestInfo *evtInfo;
   tcmMsgHeader *newMsg = NULL;

   if ( srcDInfo != NULL )
   {
      tcmLog_notice("eventType=0x%x from %s", (*msg)->type, srcDInfo->eInfo->name);
      eventSrc = (*msg)->src;
   }
   else
   {
      tcmLog_notice("eventType=0x%x from smd", (*msg)->type);
      eventSrc = ID_SCHED;
   }


   /*
    * Look up evtInterest for this event msg type
    * loop through all interested parties,
    *  --if they are currently running, send it to them,
    *  --if they are not currently running, queue the message and launch them.
    *
    */
   evtInterest = interest_findByType((*msg)->type);
   if ( evtInterest != NULL )
   {
      dlist_for_each_entry(evtInfo, &(evtInterest->evtInfoHead), dlist)
      {
         targetDInfo = getDlsInfo(evtInfo->eid);

         if ( targetDInfo == NULL )
         {
            tcmLog_error("cannot find targetDInfo for eid 0x%x, skip it", evtInfo->eid);
         }
         else if ( (srcDInfo != NULL) && (targetDInfo == srcDInfo) )
         {
            /* source and destination dInfo match, don't send event back to sender */
            /* weird, why would an app register for an event, but then send out that event?
             * Is this an error, or should I actually send the event to the sender anyways?
             */
            tcmLog_debug("targetDinfo and srcDinfo are the same, don't send");
         }
         else if ( evtInfo->matchData && (((*msg)->dataLength == 0) ||
                                          tcmUtl_strcmp(evtInfo->matchData, ((char *) ((*msg)+1)))) )
         {
            /* interested party specified additional match data, which this event did not match */
            tcmLog_debug("no match for eid 0x%x matchData=%s", evtInfo->eid, evtInfo->matchData);
            tcmLog_debug("src event dataLen=%d", (*msg)->dataLength);
            if ( (*msg)->dataLength > 0 )
            {
               tcmLog_debug("src data=%s", (char *) ((*msg)+1));
            }
         }
         else
         {
            /* OK, we can send this event */
            tcmLog_debug("dup and send event msg");
            newMsg = tcmMsg_duplicate(*msg);
            if ( newMsg == NULL )
            {
               tcmLog_error("could not allocate msg, dropping event");
            }
            else
            {
               newMsg->src = eventSrc;
               newMsg->dst = (targetDInfo->eInfo->flags & APPCFG_MULTIPLE_INSTANCES) ?
                             targetDInfo->specificEid : targetDInfo->eInfo->eid;

               sendMessageByAppState(targetDInfo, &newMsg);
            }
         }
      }
   }

   TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);

   return;
}

void sched_processRegisterEventInterest(DlsInfo *dInfo, tcmMsgHeader **msg, UBOOL8 positive)
{
   tcmRet ret;
   SINT32 delta = 0;
   char *matchData=NULL;

   if ( positive )
   {
      tcmLog_notice("%s (eid=0x%x) registering interest for event 0x%x",
                    dInfo->eInfo->name, (*msg)->src, (*msg)->wordData);

      if ( (*msg)->dataLength > 0 )
      {
         matchData = (char *) ((*msg)+1);
      }

      if ( (ret = interest_register((*msg)->wordData, (*msg)->src, matchData)) == TCMRET_SUCCESS )
      {
         delta = 1;
      }
   }
   else
   {
      tcmLog_notice("%s (eid=0x%x) unregistering interest for event 0x%x",
                    dInfo->eInfo->name, (*msg)->src, (*msg)->wordData);

      if ( (ret = interest_unregister((*msg)->wordData, (*msg)->src)) == TCMRET_SUCCESS )
      {
         delta = -1;
      }
   }

   /*
    * Let apps create duplicate registrations, we just ignore them.
    * So always return SUCCESS here.
    */
   processMsgSimpleFooter(dInfo, TCMRET_SUCCESS, msg);

   dInfo->numEventInterestRequested += delta;
   tcmAst_assert(dInfo->numEventInterestRequested >= 0);

   return;
}

/* Application Control Part */

void terminateApp(DlsInfo **dInfo)
{

   tcmLog_debug("name=%s pid=%d state=%d", (*dInfo)->eInfo->name, (*dInfo)->pid, (*dInfo)->state);

   if ( (*dInfo)->state == DLS_NOT_RUNNING )
   {
      tcmLog_debug("app is not running, do nothing");
      return;
   }

   switch ( (*dInfo)->eInfo->eid )
   {
      case ID_CONSOLE:
         tcmLog_error("Received Request to stop console");
         break;
      default:
         /*
          * If process supports messaging, we really should send it a message.
          * But most processes which support messaging does not support
          * getting a terminate message (yet).  So for now, just send the
          * app a SIGTERM.
          */
         prctl_terminateProcessForcefully((*dInfo)->pid);
         break;
   }

   collectApp(dInfo);

   return;
}

SINT32 startApp(tcmEntityId eid, const char *args, UINT32 argsLength)
{
   const tcmEntityInfo *eInfo;
   DlsInfo *targetDinfo;


   eInfo = tcmEid_getEntityInfo(eid);
   if ( eInfo == NULL )
   {
      tcmLog_error("invalid eid %d", eid);
      return TCM_INVALID_PID;
   }

   if ( eInfo->flags & APPCFG_MULTIPLE_INSTANCES )
   {
      /*
       * We must explicitly get a new DlsInfo in this case because
       * at this point, there cannot possibly be a dInfo in the
       * dlsInfo dlist for this app.
       */
      targetDinfo = getNewDlsInfo(eid);
   }
   else
   {
      /*
       * getDlsInfo will return a dInfo structure if one already exists,
       * otherwise, it will create a new one.
       */
      targetDinfo = getDlsInfo(eid);
   }

   if ( targetDinfo == NULL )
   {
      tcmLog_error("Could not create new DlsInfo");
      return TCM_INVALID_PID;
   }

   /*
    * OK, we are now ready to launch targetDinfo
    */
   if ( argsLength > 0 )
   {
      if ( argsLength > DLS_OPT_ARGS_LENGTH )
      {
         tcmLog_error("optional args length %d is greater than max %d, ignored",
                      argsLength, DLS_OPT_ARGS_LENGTH);
      }
      else
      {
         tcmLog_debug("setting optional args %s", args);
         sprintf(targetDinfo->optArgs, "%s", args);
      }
   }

   if ( targetDinfo->state == DLS_NOT_RUNNING )
   {
      launchApp(targetDinfo);
   }
   else
   {
      tcmLog_error("Trying to launch multiple instance of single instance app state=%d\n", targetDinfo->state);
      
      return TCM_INVALID_PID;
   }

   return targetDinfo->pid;
}

void stopApp(UINT32 specificEid)
{
   const tcmEntityInfo *eInfo;
   DlsInfo *targetDinfo;
   tcmEntityId eid;
   UINT16 pid;

   /*
    * Extract the generic EID and the pid from the specific EID.
    */
   eid = GENERIC_EID(specificEid);
   pid = PID_IN_EID(specificEid);


   eInfo = tcmEid_getEntityInfo(eid);
   if ( eInfo == NULL )
   {
      tcmLog_error("invalid eid %d", eid);
      return;
   }

   targetDinfo = getExistingDlsInfo(specificEid);
   if ( targetDinfo == NULL )
   {
      if ( (eInfo->flags & APPCFG_MULTIPLE_INSTANCES) && (pid != TCM_INVALID_PID) )
      {
         /* caller gave us a valid pid for a multiple-instance app,
          * if we don't have a dInfo structure for it, something is wrong. */
         tcmLog_error("could not find specificEid=0x%x (pid=%d eid=%d)", specificEid, pid, eid);
      }
      else if ( (eInfo->flags & APPCFG_MULTIPLE_INSTANCES) == 0 )
      {
         /* for single instance apps, this function could have been
          * called as part of restartApp, so this app may not be running. */
         tcmLog_debug("no dinfo for %s (eid=%d), no need to stop it", eInfo->name, eid);
      }
   }
   else
   {
      /* Dont stop if app not running */
      if ( targetDinfo->state == DLS_NOT_RUNNING )
      {
         tcmLog_debug("dinfo found for %s, but already in DLS_NOT_RUNNING state", eInfo->name);
      }
      else
      {
         terminateApp(&targetDinfo);
      }
   }

   return;
}

void sched_processStartAppMsg(DlsInfo *dInfo, tcmMsgHeader **msg)
{
   SINT32 pid;
   char *args = (char *) ((*msg)+1);

   /*
    * The wordData in a CMS_MSG_START_APP should only contain the eid
    * (should not have pid in it).
    */
   pid = startApp((*msg)->wordData, args, (*msg)->dataLength);


   /* We return the pid of the process in this reply msg, not CmsRet code */
   processMsgSimpleFooter(dInfo, pid, msg);

   return;
}

void sched_processStopAppMsg(DlsInfo *dInfo, tcmMsgHeader **msg)
{

   stopApp((*msg)->wordData);

   /* always tell the caller that we stopped the APP */
   processMsgSimpleFooter(dInfo, TCMRET_SUCCESS, msg);

   return;
}

void sched_processRestartAppMsg(DlsInfo *dInfo, tcmMsgHeader **msg)
{
   SINT32 pid;
   char *args = (char *) ((*msg)+1);


   /*
    * First we have to possibly stop an existing instance of this app.
    */
   stopApp((*msg)->wordData);


   /*
    * Now start the app.  When starting an app, we just want the generic eid.
    * The pid (in the specific eid) was only needed to stop the app.
    */
   pid = startApp(GENERIC_EID((*msg)->wordData), args, (*msg)->dataLength);

   processMsgSimpleFooter(dInfo, pid, msg);

   return;
}

void sched_processIsAppRuning(DlsInfo *dInfo, tcmMsgHeader **msg)
{
   DlsInfo *targetDinfo;
   tcmEntityId targetEid;
   tcmRet rv;

   rv = TCMRET_OBJECT_NOT_FOUND;

   targetEid = (tcmEntityId) (*msg)->wordData;

   tcmLog_debug("targetEid =%d", (*msg)->wordData);

   targetDinfo = getExistingDlsInfo(targetEid);

   if (targetDinfo && targetDinfo->state == DLS_RUNNING)
   {
      rv = TCMRET_SUCCESS;
   }

   tcmLog_debug("ret = %d -- if ret ==0, appl. is running.", rv);
   processMsgSimpleFooter(dInfo, rv, msg);

}

void sched_processMessage(DlsInfo *dInfo, tcmMsgHeader **msg)
{
   if ( (*msg)->flags_requeue == 1 )
   {
      tcmLog_debug("Requeing msg type 0x%x", (*msg)->type);
      (*msg)->flags_requeue = 0;
      tcmMsg_sendBySocket(dInfo->commFd, *msg);

      TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);
      return;
   }

   if ( (*msg)->dst == ID_SCHED )
   {
      if ( (*msg)->flags_event )
      {
         //Distribute Event Message
         sched_distributeEventMessage(msg, dInfo);
      }
      else
      {
         switch ( (*msg)->type )
         {
            case TCM_MSG_REGISTER_EVENT_INTEREST:
               sched_processRegisterEventInterest(dInfo,msg, TRUE);
               break;
            case TCM_MSG_UNREGISTER_EVENT_INTEREST:
               sched_processRegisterEventInterest(dInfo,msg, FALSE);
               break;
            case TCM_MSG_START_APP:
               sched_processStartAppMsg(dInfo, msg);
               break;
            case TCM_MSG_STOP_APP:
               sched_processStopAppMsg(dInfo, msg);
               break;
            case TCM_MSG_RESTART_APP:
               sched_processRestartAppMsg(dInfo,msg);
               break;
            case TCM_MSG_IS_APP_RUNNING:
               sched_processIsAppRuning(dInfo,msg);
               break;
            default:
               TCMMEM_FREE_BUF_AND_NULL_PTR(*msg);
               break;
         }
      }
   }
   else //Route Message
   {
      sched_routeMessage(msg);
   }

   return;
}

void processLaunchConfirmation(DlsInfo *dInfo)
{
   tcmMsgHeader *msg;

   dInfo->state = DLS_RUNNING;
   tcmLog_debug("%s (eid=%d) transitioning to state=%d",
                dInfo->eInfo->name, dInfo->eInfo->eid, dInfo->state);

   /* start monitoring this fd */
   FD_SET(dInfo->commFd, &readFdsMaster);
   UPDATE_MAXFD(dInfo->commFd);

   while ( (msg = dInfo->msgQueue) != NULL )
   {
      dInfo->msgQueue = dInfo->msgQueue->next;
      msg->next = NULL;
      tcmLog_debug("sending queued msg 0x%x", msg->type);
      tcmMsg_sendBySocket(dInfo->commFd, msg);

      TCMMEM_FREE_BUF_AND_NULL_PTR(msg);
   }
}

tcmRet sched_processEvents()
{
   struct timeval tm;
   fd_set         readFds;
   DlsInfo        *dInfo;
   SINT32         i;
   UINT32         sleepMs;
   const UINT32   timer_lag_time = 10;
   SINT32         rv;

   sleepMs = timer_getTimeToNextEvent();
   if ( sleepMs < UINT32_MAX - timer_lag_time )
   {
      sleepMs += timer_lag_time;
   }

   tm.tv_sec = sleepMs/MSECS_IN_SEC;
   tm.tv_usec = (sleepMs % MSECS_IN_SEC)*USECS_IN_MSEC;

   //tcmLog_error("->timeout %d, usec %d", tm.tv_sec, tm.tv_usec);

   readFds = readFdsMaster;

   rv = select(maxFd+1, &readFds, NULL, NULL, &tm);
   if ( rv < 0 )
   {
      if ( errno != EINTR )
      {
         tcmLog_error("select return %d, errno=%d", rv, errno);
      }
      return TCMRET_SUCCESS;
   }
   else if ( rv == 0 )  //time out
   {
      return TCMRET_SUCCESS;
   }

   for ( i = 0; i < maxFd+1; i++ )
   {
      if ( !FD_ISSET(i, &readFds) )
      {
         continue;
      }

      if ( (dInfo = getDlsInfoByServerFd(i)) != NULL )
      {
         if ( dInfo->state != DLS_NOT_RUNNING )
         {
            tcmLog_error("Got activity on server socket");
         }
         else
         {
            launchApp(dInfo);
         }
      }

      if ( (dInfo = getDlsInfoByCommFd(i)) != NULL )
      {
         tcmMsgHeader *msg;
         tcmRet ret;

         ret = tcmMsg_receiveBySocket(dInfo->commFd, &msg, NULL);
         if ( ret == TCMRET_SUCCESS )
         {
            sched_processMessage(dInfo, &msg);
         }
         else if ( ret == TCMRET_DISCONNECTED )
         {
            tcmEntityId exitEid = dInfo->eInfo->eid;
            tcmLog_error("detected exit of %s (pid=%d) on fd %d", dInfo->eInfo->name, dInfo->pid, dInfo->commFd);

            collectApp(&dInfo);
         }
         else
         {
            tcmLog_error("error on read from fd %d for %s", dInfo->commFd, dInfo->eInfo->name);
         }
      }

      //Check newly launched process
      if ( i == ipcListenFd )
      {
         struct sockaddr_un clientAddr;
         UINT32 sockAddrSize;
         SINT32 fd;
         tcmMsgHeader *msg=NULL;
         UINT32 timeout = CONNECT_TO_LAUNCH_MSG_TIMEOUT;
         tcmRet ret;

         sockAddrSize = sizeof(clientAddr);
         if ( (fd = accept(ipcListenFd, (struct sockaddr *)&clientAddr, &sockAddrSize)) < 0 )
         {
            tcmLog_error("Accept IPC Connection failed. errno=%d", errno);
         }
         else
         {
            ret = tcmMsg_receiveBySocket(fd, &msg, &timeout);
            if ( ret != TCMRET_SUCCESS )
            {
               tcmLog_error("Couldn't receive launch message, ret=%d", ret);
               close(fd);
            }
            else
            {
               if ( msg->type != TCM_MSG_APP_LAUNCHED || msg->dst!= ID_SCHED )
               {
                  tcmLog_error("Comm link establish msg error");
                  close(fd);
               }
               else
               {
                  if ( (dInfo = getDlsInfo(msg->src)) == NULL )
                  {
                     tcmLog_error("Can't get Dls Info struct for eid %d", msg->src);
                     close(fd);
                  }
                  else
                  {
                     dInfo->commFd = fd;
                     processLaunchConfirmation(dInfo);
                  }
               }
            }

            TCMMEM_FREE_BUF_AND_NULL_PTR(msg);
         }
      }
   }


   return TCMRET_SUCCESS;
}

SINT32 sched_main()
{
   tcmRet ret;
   SINT32 exitCode=0;

   while ( keepLooping )
   {
      //Periodic/timer event processing
      timer_process();

      if ( (ret = sched_processEvents()) != TCMRET_SUCCESS )
      {
         keepLooping = FALSE;
      }
   }

   return exitCode;
}

void clean_sched()
{
   close(ipcListenFd);
   ipcListenFd = TCM_INVALID_FD;
   unlink(SCHED_MESSAGE_ADDR);

   cleanupDls();

   //Clean up interest list Res
   interest_cleanup();
}

