#ifndef _PAL_THREAD_H_
#define _PAL_THREAD_H_

#include "pal_types.h"

typedef   PAL_INT32_T PAL_THREAD_T;
typedef struct
{
    char threadName[30];
    int stackSize;
    int priority;
} PAL_THREAD_ATTR_T;

PAL_INT32_T pal_thread_create ( PAL_THREAD_T *t, void * ( *func ) ( void *arg ), PAL_THREAD_ATTR_T *a, void *arg );
void pal_thread_exit ( void );
PAL_INT32_T pal_thread_cancel(PAL_THREAD_T * t);

#endif /* _PAL_THREAD_H_ */

