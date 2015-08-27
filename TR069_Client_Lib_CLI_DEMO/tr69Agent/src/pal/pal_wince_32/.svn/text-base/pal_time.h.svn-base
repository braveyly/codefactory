#ifndef _PAL_TIME_H_
#define _PAL_TIME_H_

#include <time.h>
#include <windows.h>
#include "pal_types.h"

typedef   time_t PAL_TIME_T;

typedef struct
{
    PAL_INT32_T tv_sec;
    PAL_INT32_T tv_usec;
} pal_timeval_t;

PAL_TIME_T pal_time_current ( PAL_TIME_T *tp );
void pal_sleep ( PAL_UINT32_T sec );

#endif

