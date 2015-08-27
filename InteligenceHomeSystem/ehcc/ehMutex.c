#include "ehMutex.h"
char* mutexModuleStr[4]={
                         "SELECT module",
                         "MPC module",
                         "MAIN module",
                         "OTHER module"}; 

char* mutexKindStr[10]={
                         "g_devQueueMutex",
                         "g_homeMsgQueueMutex",
                         "g_homeIdpTokenMutex",
                         "remoteMsgMutex",
                         "gehomeTransMsgMapQueueMutex",
                        };

void moduleMutexLock( MutexModule mdmodule, MutexKind mutexKind, pthread_mutex_t* mdmutex )
{
   tcmLog_debug( "Lock %s in %s",mutexKindStr[mutexKind],mutexModuleStr[mdmodule]);
   pthread_mutex_lock ( mdmutex );
}

void moduleMutexUnLock( MutexModule mdmodule, MutexKind mutexKind, pthread_mutex_t* mdmutex )
{
   tcmLog_debug( "UnLock %s in %s",mutexKindStr[mutexKind],mutexModuleStr[mdmodule]);
   pthread_mutex_unlock ( mdmutex );
}