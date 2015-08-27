#ifndef _PAL_TIME_H_
#define _PAL_TIME_H_

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pal_types.h"

typedef time_t PAL_TIME_T;

typedef struct _PAL_TIMEVAL_T
{
    PAL_INT32_T tv_sec;
    PAL_INT32_T tv_usec;
} PAL_TIMEVAL_T;

extern PAL_TIME_T pal_time_current ( PAL_TIME_T *tp );
extern void pal_datetime(char * dateTime);
extern void pal_time_format (PAL_TIME_T t, char *dateTime);
extern void pal_uptime(char *uptime);
extern void pal_sleep ( PAL_UINT32_T sec );

#endif /* _PAL_TIME_H_ */

