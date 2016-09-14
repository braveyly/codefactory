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

#ifndef __TCM_LOG_H__
#define __TCM_LOG_H__


#include "tcm.h"
#include "tcm_eid.h"

typedef enum
{
   LOG_LEVEL_ERR    = 3, /**< Message at error level. */
   LOG_LEVEL_NOTICE = 5, /**< Message at notice level. */
   LOG_LEVEL_DEBUG  = 7  /**< Message at debug level. */
} tcmLogLevel;


typedef enum
{
   LOG_DEST_STDERR  = 1,  /**< Message output to stderr. */
   LOG_DEST_SYSLOG  = 2,  /**< Message output to syslog. */
   LOG_DEST_TELNET  = 3   /**< Message output to telnet clients. */
} tcmLogDestination;


/** Show application name in the log line. */
#define TCMLOG_HDRMASK_APPNAME    0x0001

/** Show log level in the log line. */
#define TCMLOG_HDRMASK_LEVEL      0x0002

/** Show timestamp in the log line. */
#define TCMLOG_HDRMASK_TIMESTAMP  0x0004

/** Show location (function name and line number) level in the log line. */
#define TCMLOG_HDRMASK_LOCATION   0x0008


/** Default log level is error messages only. */
#define DEFAULT_LOG_LEVEL        LOG_LEVEL_ERR

/** Default log destination is standard error */
#define DEFAULT_LOG_DESTINATION  LOG_DEST_STDERR

/** Default log header mask */
#define DEFAULT_LOG_HEADER_MASK (TCMLOG_HDRMASK_APPNAME|TCMLOG_HDRMASK_LEVEL|TCMLOG_HDRMASK_TIMESTAMP|TCMLOG_HDRMASK_LOCATION)


/** Maxmimu length of a single log line; messages longer than this are truncated. */
#define MAX_LOG_LINE_LENGTH      512


/** Macros Definition.
 * Applications should use these macros for message logging, instead of
 * calling the cmsLog_log function directly.
 */
#ifdef TCM_LOG0
#define tcmLog_error(args...)
#define tcmLog_notice(args...)
#define tcmLog_debug(args...)
#endif

#ifdef TCM_LOG2
#define tcmLog_error(args...)  log_log(LOG_ERR, __FUNCTION__, __LINE__, args)
#define tcmLog_notice(args...) log_log(LOG_NOTICE, __FUNCTION__, __LINE__, args)
#define tcmLog_debug(args...)
#endif

#ifdef TCM_LOG3
#define tcmLog_error(args...)  log_log(LOG_ERR, __FUNCTION__, __LINE__, args)
#define tcmLog_notice(args...) log_log(LOG_NOTICE, __FUNCTION__, __LINE__, args)
#define tcmLog_debug(args...)  log_log(LOG_DEBUG, __FUNCTION__, __LINE__, args)
#endif


/** Internal message log function; do not call this function directly.
 *
 * NOTE: Applications should NOT call this function directly from code.
 *       Use the macros defined in cms_log.h, i.e.
 *       cmsLog_error, cmsLog_notice, cmsLog_debug.
 *
 * This function performs message logging based on two control
 * variables, "logLevel" and "logDestination".  These two control
 * variables are local to each process.  Each log message has an
 * associated severity level.  If the severity level of the message is
 * numerically lower than or equal to logLevel, the message will be logged to
 * either stderr or syslogd based on logDestination setting.
 * Otherwise, the message will not be logged.
 *
 * @param level (IN) The message severity level as defined in "syslog.h".
 *                   The levels are, in order of decreasing importance:
 *                   LOG_EMERG (0)- system is unusable
 *                   LOG_ALERT (1)- action must be taken immediately
 *                   LOG_CRIT  (2)- critical conditions
 *                   LOG_ERR   (3)- error conditions
 *                   LOG_WARNING(4) - warning conditions
 *                   LOG_NOTICE(5)- normal, but significant, condition
 *                   LOG_INFO  (6)- informational message
 *                   LOG_DEBUG (7)- debug-level message
 * @param func (IN) Function name where the log message occured.
 * @param lineNum (IN) Line number where the log message occured.
 * @param pFmt (IN) The message string.
 *
 */
void log_log(tcmLogLevel level, const char *func, UINT32 lineNum, const char *pFmt, ... );

/** Message log initialization.
 * This function initializes the message log utility.  The openlog
 * function is called to open a connection to syslogd for the
 * process.  The process name string identified by entityId will
 * be prepended to every message logged by the system logger syslogd.
 *
 * @param eid (IN) The entity ID of the calling process.
 */
void tcmLog_init(tcmEntityId eid);

/** Message log cleanup.
 * This function performs all the necessary cleanup of the message
 * log utility. The closelog function is called to close the
 * descriptor being used to write to the system logger syslogd.
 *
 */
void tcmLog_cleanup(void);

/** Set process message logging level.
 * This function sets the logging level of a process.
 *
 * @param level (IN) The logging level to set.
 */
void tcmLog_setLevel(tcmLogLevel level);

/** Get process message logging level.
 * This function gets the logging level of a process.
 *
 * @return The process message logging level.
 */
tcmLogLevel tcmLog_getLevel(void);

/** Set process message logging destination.
 * This function sets the logging destination of a process.
 *
 * @param dest (IN) The process message logging destination.
 */
void tcmLog_setDestination(tcmLogDestination dest);

/** Get process message logging destination.
 * This function gets the logging destination of a process.
 *
 * @return The process message logging destination.
 */
tcmLogDestination tcmLog_getDestination(void);

/** Set process message log header mask which determines which pieces of
 * info are included in each log line.
 *
 * @param mask (IN) Bitmask of CMSLOG_HDRMASK_xxx
 */
void tcmLog_setHeaderMask(UINT32 headerMask);

/** Get process message log header mask.
 *
 * @return The process message log header mask.
 */
UINT32 tcmLog_getHeaderMask(void);


/** indicate first read */
#define BCM_SYSLOG_FIRST_READ           -2

/** indicates error */
#define BCM_SYSLOG_READ_BUFFER_ERROR    -1

/** indicates last line was read */
#define BCM_SYSLOG_READ_BUFFER_END      -3

/** max log buffer length */
#define BCM_SYSLOG_MAX_LINE_SIZE        255


/** Legacy method for reading the system log line by line.
 *
 * @param ptr     (IN) Current line to read.
 * @param buffer (OUT) Line that was read.
 * @return new ptr value for next read.
 */
//SINT32 tcmLog_readPartial(SINT32 ptr, char* buffer);

#endif /* __TCM_LOG_H__ */
