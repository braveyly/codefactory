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

#include <fcntl.h>      /* open */
#include <syslog.h>
#include "tcm.h"
#include "tcm_log.h"
#include "tcm_eid.h"
#include "tcm_tms.h"

/** local definitions **/

/* default settings */

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/
static tcmEntityId gEid;
static tcmLogLevel             logLevel; /**< Message logging level.
                                          * This is set to one of the message
                                          * severity levels: LOG_ERR, LOG_NOTICE
                                          * or LOG_DEBUG. Messages with severity
                                          * level lower than or equal to logLevel
                                          * will be logged. Otherwise, they will
                                          * be ignored. This variable is local
                                          * to each process and is changeable
                                          * from CLI.
                                          */
static tcmLogDestination logDestination; /**< Message logging destination.
                                          * This is set to one of the
                                          * message logging destinations:
                                          * STDERR or SYSLOGD. This
                                          * variable is local to each
                                          * process and is changeable from
                                          * CLI.
                                          */
static UINT32 logHeaderMask; /**< Bitmask of which pieces of info we want
                              *   in the log line header.
                              */

void log_log(tcmLogLevel level, const char *func, UINT32 lineNum, const char *pFmt, ... )
{
   va_list     ap;
   char buf[MAX_LOG_LINE_LENGTH] = {0};
   int len=0, maxLen;
   char *logLevelStr=NULL;
   const tcmEntityInfo *einfo=NULL;
   int logTelnetFd = -1;

   maxLen = sizeof(buf);

   if (level <= logLevel)
   {
      va_start(ap, pFmt);

      if (logHeaderMask & TCMLOG_HDRMASK_APPNAME)
      {
         if ((einfo = tcmEid_getEntityInfo(gEid)) != NULL)
         {
            len = snprintf(buf, maxLen, "%s:", einfo->name);
         }
         else
         {
            len = snprintf(buf, maxLen, "unknown:");
         }
      }


      if ((logHeaderMask & TCMLOG_HDRMASK_LEVEL) && (len < maxLen))
      {
         /*
          * Only log the severity level when going to stderr
          * because syslog already logs the severity level for us.
          */
         if (logDestination == LOG_DEST_STDERR)
         {
            switch(level)
            {
            case LOG_LEVEL_ERR:
               logLevelStr = "error";
               break;
            case LOG_LEVEL_NOTICE:
               logLevelStr = "notice";
               break;
            case LOG_LEVEL_DEBUG:
               logLevelStr = "debug";
               break;
            default:
               logLevelStr = "invalid";
               break;
            }
            len += snprintf(&(buf[len]), maxLen - len, "%s:", logLevelStr);
         }
      }


      /*
       * Log timestamp for both stderr and syslog because syslog's
       * timestamp is when the syslogd gets the log, not when it was
       * generated.
       */
      if ((logHeaderMask & TCMLOG_HDRMASK_TIMESTAMP) && (len < maxLen))
      {
         tcmTimestamp ts;

         tcmTms_get(&ts);
         len += snprintf(&(buf[len]), maxLen - len, "%u.%03u:",
                         ts.sec%1000, ts.nsec/NSECS_IN_MSEC);
      }


      if ((logHeaderMask & TCMLOG_HDRMASK_LOCATION) && (len < maxLen))
      {
         len += snprintf(&(buf[len]), maxLen - len, "%s:%u:", func, lineNum);
      }


      if (len < maxLen)
      {
         maxLen -= len;
         vsnprintf(&buf[len], maxLen, pFmt, ap);
      }

      if (logDestination == LOG_DEST_STDERR)
      {
         fprintf(stderr, "%s\n", buf);
      }
      else if (logDestination == LOG_DEST_TELNET )
      {
         /* CPE use ptyp0 as the first pesudo terminal */
         logTelnetFd = open("/dev/ttyp0", O_RDWR);
         if(logTelnetFd != -1)
         {
            write((FILE *)logTelnetFd, buf, strlen(buf));
            write((FILE *)logTelnetFd, "\n", strlen("\n"));
            close(logTelnetFd);
         }
      }
      else
      {
         syslog(level, buf);
      }

      va_end(ap);
   }

}  /* End of log_log() */


void tcmLog_init(tcmEntityId eid)
{
   logLevel       = DEFAULT_LOG_LEVEL;
   logDestination = DEFAULT_LOG_DESTINATION;
   logHeaderMask  = DEFAULT_LOG_HEADER_MASK;

   gEid = eid;

   openlog(NULL, 0, (3<<3));

   return;

}  /* End of cmsLog_init() */


void tcmLog_cleanup(void)
{
   closelog();
   return;

}  /* End of tcmLog_cleanup() */


void tcmLog_setLevel(tcmLogLevel level)
{
   logLevel = level;
   return;
}


tcmLogLevel tcmLog_getLevel(void)
{
   return logLevel;
}


void tcmLog_setDestination(tcmLogDestination dest)
{
   logDestination = dest;
   return;
}


tcmLogDestination tcmLog_getDestination(void)
{
   return logDestination;
}


void tcmLog_setHeaderMask(UINT32 headerMask)
{
   logHeaderMask = headerMask;
   return;
}


UINT32 tcmLog_getHeaderMask(void)
{
   return logHeaderMask;
}


