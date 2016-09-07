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

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dls.h>
#include "tcm_mem.h"
#include "tcm_log.h"
DLIST_HEAD(dlsInfoHead);

SINT32 maxFd = 0;      /* Updated by UPDATE_MAXFD in oal.h */
SINT32 ipcListenFd;    /* SMD Unix domain IPC listening socket fd */
fd_set readFdsMaster;  /* need to export to oal_sysmon */

SINT32 initInetServerSocket(SINT32 domain, SINT32 port, SINT32 type, SINT32 backlog)
{
   SINT32 sFd;
   SINT32 optVal;

   if((sFd = socket(domain, type, 0)) < 0)
   {
      tcmLog_error("socket creation failed. port %d errno %d", port, errno);
      return TCM_INVALID_FD;
   }

   optVal = 1;
   if( setsockopt(sFd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0)
   {
      tcmLog_error("setsockopt error %d, port %d", errno, port);
      close(sFd);
      return TCM_INVALID_FD;
   }

   if( domain == AF_INET )
   {
      struct sockaddr_in serverAddr;

      if(type == SOCK_DGRAM)
      {
         if(setsockopt(sFd, IPPROTO_IP, IP_PKTINFO, &optVal, sizeof(optVal)) < 0)
         {
            tcmLog_error("setsockopt errno %d, port %d", errno, port);
            close(sFd);
            return TCM_INVALID_FD;
         }

         memset(&serverAddr, 0, sizeof(serverAddr));
         serverAddr.sin_family = AF_INET;
         serverAddr.sin_port = htons(port);
         serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

         if(bind(sFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
         {
            tcmLog_error("Bind errno %d, port %d", errno, port);
            close(sFd);
            return TCM_INVALID_FD;
         }
      }
   }
   else  //IPV6
   {
      struct sockaddr_in6 serverAddr;

      if( type == SOCK_DGRAM )
      {
         if(setsockopt(sFd, IPPROTO_IPV6, IPV6_PKTINFO, &optVal, sizeof(optVal)) < 0)
         {
            tcmLog_error("Set sock opt error %d, port %d", errno, port);
            close(sFd);
            return TCM_INVALID_FD;
         }
      }

      memset(&serverAddr, 0, sizeof(serverAddr));
      serverAddr.sin6_family = AF_INET6;
      serverAddr.sin6_port = htons(port);
      serverAddr.sin6_addr = in6addr_any;

      if(bind(sFd, (struct sockaddr_in6 *)&serverAddr, sizeof(serverAddr)) < 0)
      {
         tcmLog_error("bind errnor %d, port %d", errno, port);
         close(sFd);
         return TCM_INVALID_FD;
      }
   }

   if(type == SOCK_STREAM)
   {
      if(listen(sFd, backlog) < 0)
      {
         tcmLog_error("listen failed errno %d, port %d", errno, port);
         close(sFd);
         return TCM_INVALID_FD;
      }
   }

   return sFd;
}

DlsInfo *insertDlsInfoEntry(tcmEntityId eid)
{
   SINT32 socketType;
   DlsInfo *dInfo;

   if((dInfo = tcmMem_alloc(sizeof(DlsInfo), ALLOC_ZEROIZE)) == NULL)
   {
      tcmLog_error("Could not allocate DlsInfo Resource for eid=%d", eid);
      return NULL;
   }

   dInfo->eInfo = tcmEid_getEntityInfo(GENERIC_EID(eid));
   if( dInfo->eInfo == NULL )
   {
      tcmLog_error("Invalid eid %d", eid);
      tcmMem_free(dInfo);
      return NULL;
   }

   dInfo->state = DLS_NOT_RUNNING;
   dInfo->commFd = TCM_INVALID_FD;
   dInfo->serverFd = TCM_INVALID_FD;
   dInfo->pid = TCM_INVALID_PID;

   if ((dInfo->eInfo->flags & APPCFG_MULTIPLE_INSTANCES) && (PID_IN_EID(eid) > 0) )
   {
      /* It means that the app is multiple instansces and the inputed eid is specificEid,
         so the app might not laucher by sched, we record the eid as specificEid.
      */
      dInfo->specificEid = eid;
   }

   if( eid == ID_CONSOLE )
   {
      dInfo->serverFd = 0;
      FD_SET(0, &readFdsMaster);
   }
   else if( dInfo->eInfo->flags && APPCFG_SERVER )
   {
      SINT32 domain;

      socketType = (dInfo->eInfo->flags & APPCFG_SERVER_TCP)?SOCK_STREAM:SOCK_DGRAM;
      domain = (dInfo->eInfo->flags & APPCFG_IPV6)?AF_INET6:AF_INET;

      dInfo->serverFd = initInetServerSocket(domain,
                                             dInfo->eInfo->port,
                                             socketType,
                                             dInfo->eInfo->backLog);
      if(dInfo->serverFd == TCM_INVALID_FD)
      {
         tcmMem_free(dInfo);
         return NULL;
      }
      else
      {
         FD_SET(dInfo->serverFd, &readFdsMaster);
         UPDATE_MAXFD(dInfo->serverFd);
      }
   }

   dlist_prepend((DlistNode *) dInfo, &dlsInfoHead);
   return dInfo;
}

DlsInfo *getExistingDlsInfo(tcmEntityId eid)
{
   DlsInfo *dInfo;

   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if(dInfo->eInfo->flags & APPCFG_MULTIPLE_INSTANCES)
      {
         if(dInfo->specificEid == eid)
         {
            return dInfo;
         }
      }
      else
      {
         if(dInfo->eInfo->eid == eid)
         {
            return dInfo;
         }
      }
   }

   return NULL;
}


/* Init DLS resource first */
tcmRet initDls(UINT32 stage)
{
   DlsInfo *dInfo = NULL;
   const tcmEntityInfo *eInfo;
   UINT32 numEntries, i = 0;

   eInfo = tcmEid_getFirstEntityInfo();
   numEntries = tcmEid_getNumberOfEntityInfoEntries();

   if(stage == 1)
   {
      for( i = 0; i < numEntries; i++)
      {
         if(eInfo->featureCompiled && (eInfo->flags & APPCFG_LAUNCH_IN_STAGE_1))
         {
            dInfo = insertDlsInfoEntry(eInfo->eid);
            if(dInfo == NULL)
            {
               tcmLog_error("Launch first Stage entry eid %d failed.", eInfo->eid);
               return TCMRET_INTERNAL_ERROR;
            }
         }

         eInfo++;
      }
   }
   else
   {
      for(i=0; i<numEntries; i++)
      {
         if((eInfo->featureCompiled) &&
            ((eInfo->flags & (APPCFG_LAUNCH_ON_BOOT | APPCFG_SERVER)) ||
             (eInfo->eid == ID_CONSOLE)))
         {
            if((dInfo = getExistingDlsInfo(eInfo->eid)) != NULL)
            {
               tcmLog_debug("Launched already eid %s", eInfo->name);
            }
            else
            {
               dInfo = insertDlsInfoEntry(eInfo->eid);
               if(dInfo == NULL)
               {
                  tcmLog_error("Failed to insert dInfo eid %s", eInfo->name);
                  return TCMRET_INTERNAL_ERROR;
               }
            }
         }

         eInfo++;
      }
   }

   return TCMRET_SUCCESS;
}

void cleanupDls()
{
   DlsInfo *dInfo;

   while(dlist_empty(&dlsInfoHead) == 0)
   {
      dInfo = (DlsInfo*)dlsInfoHead.next;

      dlist_unlink((DlistNode *)dInfo);
      if(dInfo->commFd != TCM_INVALID_FD)
      {
         close(dInfo->commFd);
         dInfo->commFd = TCM_INVALID_FD;
      }

      if(dInfo->serverFd != TCM_INVALID_FD)
      {
         close(dInfo->serverFd);
         dInfo->serverFd = TCM_INVALID_FD;
      }

      tcmMem_free(dInfo);
   }

   return;
}

DlsInfo *getDlsInfoByServerFd(SINT32 serverFd)
{
   DlsInfo *dInfo = NULL;

   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if(dInfo->serverFd == serverFd)
      {
         return dInfo;
      }
   }

   return NULL;
}

DlsInfo *getDlsInfoByCommFd(SINT32 commFd)
{
   DlsInfo  *dInfo;

   dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
   {
      if (dInfo->commFd == commFd)
      {
         return dInfo;
      }
   }

   return NULL;
}


DlsInfo *getNewDlsInfo(tcmEntityId eid)
{
   tcmLog_notice("Creating dynamically allocated dlsInfo entry for eid=0x%x", eid);
   return (insertDlsInfoEntry(eid));
}


DlsInfo *getDlsInfo(tcmEntityId eid)
{
   DlsInfo  *dInfo;

   if ((dInfo = getExistingDlsInfo(eid)) != NULL)
   {
      return dInfo;
   }

   /*
    * this eid does not have an entry created at startup.  No problem.
    * create one now and add it to the list.
    */
   return getNewDlsInfo(eid);
}


