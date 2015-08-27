#ifndef _PAL_TIME_H_
#define _PAL_TIME_H_

#include "pal_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "time.h"

typedef   time_t PAL_TIME_T;
typedef struct
{
    PAL_INT32_T tv_sec;
    PAL_INT32_T tv_usec;
} pal_timeval_t;

PAL_TIME_T pal_time_current ( PAL_TIME_T *tp );
void pal_sleep ( PAL_UINT32_T sec );
extern void pal_datetime(char * dateTime);
extern void pal_uptime(char *uptime);

#endif
