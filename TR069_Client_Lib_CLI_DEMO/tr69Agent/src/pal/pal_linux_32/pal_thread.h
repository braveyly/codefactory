#ifndef _PAL_THREAD_H_
#define _PAL_THREAD_H_

#include <pthread.h>
#include <stdlib.h>  /* system() */
#include "pal_types.h"

typedef   pthread_t PAL_THREAD_T;

typedef struct
{
    char threadName[30];
    PAL_SIZE_T stackSize;
    PAL_INT32_T priority;
} PAL_THREAD_ATTR_T;

extern PAL_INT32_T pal_thread_create ( PAL_THREAD_T *t
                                     , void * ( *func ) ( void *arg )
                                     , PAL_THREAD_ATTR_T *a, void *arg );
extern void pal_thread_exit ( void );

/* add by Hook Guo, June 10th. 2009 */
extern PAL_INT32_T pal_thread_cancel(PAL_THREAD_T *t);

#endif /* _PAL_THREAD_H_ */

