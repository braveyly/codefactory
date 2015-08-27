#include "pal_time.h"

#define SEC_PER_MIN 60
#define SEC_PER_HOUR (SEC_PER_MIN * 60)
#define SEC_PER_DAY (SEC_PER_HOUR * 24)

PAL_TIME_T pal_time_current ( PAL_TIME_T *tp )
{
    return time ( tp );
}

void pal_sleep ( PAL_UINT32_T sec )
{
    sleep ( sec );
}

void pal_datetime(char *dateTime)
{
    PAL_TIME_T _t;
    char *fmt = "%04d-%02d-%02dT%02d:%02d:%02d";
    struct tm *_tm;

    _t = pal_time_current(NULL);
    _tm = localtime(&_t);

    if ((_tm->tm_year + 1900) < 2008)
    {
        pal_uptime(dateTime);
    }
    else
    {
    sprintf(dateTime, fmt
           , _tm->tm_year + 1900
           , _tm->tm_mon + 1
           , _tm->tm_mday
           , _tm->tm_hour
           , _tm->tm_min
           , _tm->tm_sec);
}
}

void pal_uptime(char *uptime)
{
    
    
    PAL_UINT32_T uptimeInteger;
    PAL_UINT32_T year, month, day, hour, min, sec; 

    uptimeInteger = (PAL_UINT32_T)(tickGet() / 100);
    
    /* FIXME: year, month should be calculated */
    year = 0;
    month = 0;

    day = (PAL_UINT32_T)((PAL_UINT32_T)uptimeInteger / SEC_PER_DAY);
    hour = (PAL_UINT32_T)(((PAL_UINT32_T)uptimeInteger % SEC_PER_DAY) / SEC_PER_HOUR);
    min = (PAL_UINT32_T)(((PAL_UINT32_T)uptimeInteger % SEC_PER_HOUR) / SEC_PER_MIN);
    sec = (PAL_UINT32_T)((PAL_UINT32_T)uptimeInteger % SEC_PER_MIN);
    sprintf(uptime, "%04d-%02d-%02dT%02d:%02d:%02d"
           , year, month, day, hour, min, sec);  
}


