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
#ifndef __PRCTL_H__
#define __PRCTL_H__

#include <tcm.h>
#include <tcm_util.h>

typedef enum
{
   SPAWN_AND_RETURN=1,  /**< spawn the process and return. */
   SPAWN_AND_WAIT  =2   /**< spawn the process and wait for it to exit.
                         *   The number of milliseconds to wait must be
                         *   specified in SpawnProcessInfo.timeout.
                         *   While waiting, the caller will block (but will
                         *   not spin in the CPU).
                         *   If the spawned process does not exit by the
                         *   timeout time, it will be terminated with a SIGKILL.
                         *   This mode will always collect the child before
                         *   returning.
                         */
} SpawnProcessMode;

typedef enum
{
   PSTAT_RUNNING=1, /**< process is running. */
   PSTAT_EXITED=2,  /**< process has exited. */
} SpawnedProcessStatus;

typedef enum
{
   COLLECT_PID=1,  /**< Collect the specified pid, waiting indefinately. */
   COLLECT_PID_TIMEOUT=2, /**< Collect the specified pid, waiting for specified amount of time only. */
   COLLECT_ANY=3,  /**< Collect any spawned child process, waiting indefinately. */
   COLLECT_ANY_TIMEOUT=4 /**< Collect any spawned child process, waiting for specified amount of time only. */
} CollectProcessMode;

typedef struct
{
   CollectProcessMode collectMode; /**< Various collect options. */
   SINT32 pid; /**< If collecting a specific process, the pid of the process to collect. */
   UINT32 timeout; /**< If using a collect mode with timeout, the number of milliseconds to wait. */
} CollectProcessInfo;

typedef struct
{
   const char *exe;  /**< full path and name of executable */
   const char *args; /**< args to pass to executable */
   SpawnProcessMode spawnMode;  /**< Various spawning options. */
   UINT32 timeout;   /**< If SPAWN_AND_WAIT, how many milliseconds to wait before returning. */
   SINT32 stdinFd;   /**< Standard input of spawned process tied to this, -1 means /dev/null */
   SINT32 stdoutFd;  /**< Standard output of spawned process tied to this, -1 means /dev/null */
   SINT32 stderrFd;  /**< Standard error of spawned process tied to this, -1 means /dev/null */
   SINT32 serverFd;  /**< Special hack for smd dynamic launch service.
                      *   This server fd, if not -1, is tied to CMS_DYNAMIC_LAUNCH_SERVER_FD */
   SINT32 maxFd;      /**< Close all fd's up to and including this, but excluding
                       *   the stdinFd, stdoutFd, stderrFd, and serverFd */
   UBOOL8 inheritSigint;  /**< Check if the process needs to inherit sigint. */
} SpawnProcessInfo;

typedef struct
{
   SINT32  pid;          /**< pid of the process that this structure is reporting on. */
   SpawnedProcessStatus status; /**< current status of process. */
   SINT32  signalNumber; /**< If process exited due to Linux signal, the signal number is stored here. */
   SINT32  exitCode;     /**< If process exited and not due to Linux signal, the exit code is stored here. */
} SpawnedProcessInfo;


#define TCM_DYNAMIC_LAUNCH_SERVER_FD   3

tcmRet prctl_spawnProcess(const SpawnProcessInfo *spawnInfo, SpawnedProcessInfo *procInfo);

tcmRet prctl_terminateProcessForcefully(SINT32 pid);

tcmRet prctl_collectProcess(const CollectProcessInfo *collectInfo, SpawnedProcessInfo *procInfo);

tcmRet prctl_signalProcess(SINT32 pid, SINT32 sig);

#endif

