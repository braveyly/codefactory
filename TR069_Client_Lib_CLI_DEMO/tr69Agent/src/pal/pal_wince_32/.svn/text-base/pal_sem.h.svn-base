#ifndef _PAL_SEM_H_
#define _PAL_SEM_H_

#include <windows.h>
#include <winbase.h>
#include "pal_types.h"

typedef   HANDLE  PAL_SEM_T;

#define MAX_SEM_COUNT_VALUE 1024

extern PAL_INT32_T pal_sem_init ( PAL_SEM_T *sem );
extern PAL_INT32_T pal_sem_post ( PAL_SEM_T *sem );
extern PAL_INT32_T pal_sem_wait ( PAL_SEM_T *sem );
extern PAL_INT32_T pal_sem_destroy ( PAL_SEM_T *sem );

#endif /* _PAL_SEM_H_ */
