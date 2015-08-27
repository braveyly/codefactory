#ifndef _PAL_SEM_H_
#define _PAL_SEM_H_

#include "pal_types.h"

#define _XOPEN_SOURCE 600 /* add by Hook Guo, Jun 10th. 2009 */
#include <semaphore.h>

typedef   sem_t  PAL_SEM_T;

extern PAL_INT32_T pal_sem_init ( PAL_SEM_T *sem );
extern PAL_INT32_T pal_sem_post ( PAL_SEM_T *sem );
extern PAL_INT32_T pal_sem_wait ( PAL_SEM_T *sem );
extern PAL_INT32_T pal_sem_timedwait( PAL_SEM_T *sem, PAL_INT32_T timeout );
extern PAL_INT32_T pal_sem_destroy ( PAL_SEM_T *sem );

#endif /* _PAL_SEM_H_ */

