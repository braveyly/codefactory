#ifndef _PAL_THREAD_H_
#define _PAL_THREAD_H_

#include <windows.h>
#include <winbase.h>
#include "pal_types.h"

typedef HANDLE PAL_THREAD_T;

typedef void * (*PAL_THREAD_START_ROUTINE)(void *);

typedef struct
{
    char threadName[30];
    PAL_SIZE_T stackSize;
    PAL_INT32_T priority;
} PAL_THREAD_ATTR_T;

extern PAL_INT32_T pal_thread_create ( PAL_THREAD_T *t, 
        PAL_THREAD_START_ROUTINE *func, PAL_THREAD_ATTR_T *a, void *arg );

extern void pal_thread_exit ( void );

#endif /* _PAL_THREAD_H_ */
