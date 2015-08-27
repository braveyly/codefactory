#ifndef _PAL_SEM_H_
#define _PAL_SEM_H_

#include "pal_types.h"

typedef   SEM_ID  PAL_SEM_T;

PAL_INT32_T pal_sem_init ( PAL_SEM_T *sem );
PAL_INT32_T pal_sem_post ( PAL_SEM_T *sem );
PAL_INT32_T pal_sem_wait ( PAL_SEM_T *sem );
PAL_INT32_T pal_sem_destroy ( PAL_SEM_T *sem );

#endif /* _PAL_SEM_H_ */
