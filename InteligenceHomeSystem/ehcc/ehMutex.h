#ifndef _EH_MUTEX_
#define _EH_MUTEX_
#include <pthread.h>
#include <tcm_log.h>
typedef enum
{
    SELECT_MODULE         = 0,
    MPC_MODULE            = 1,
    MAIN_MODULE           = 2,
    OTHER_MODULE          = 3,
}MutexModule;

typedef enum
{
    GDEVQUEUEMUTEX              = 0,
    GHOMEMSGQUEUEMUTEX          = 1,
    GHOMEIDPTOKENMUTEX          = 2,
    REMOTEMSGMUTEX              = 3,
    GEHOMETRANSMSGMAPQUEUEMUTEX = 4,
}MutexKind;

void moduleMutexLock( MutexModule mdmodule, MutexKind mutexKind, pthread_mutex_t* mdmutex );
void moduleMutexUnLock( MutexModule mdmodule, MutexKind mutexKind, pthread_mutex_t* mdmutex ); 
#endif
