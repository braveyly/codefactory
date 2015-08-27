#include <errno.h>
#include "pal_time.h"

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

void pal_time_format (PAL_TIME_T t, char *dateTime)
{
{
    char *fmt = "%04d-%02d-%02dT%02d:%02d:%02d";
    struct tm *_tm;

    _tm = localtime(&t);

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
}
void pal_uptime(char *uptime)
{
#define UPTIME_LEN 128
#define SEC_PER_MIN 60
#define SEC_PER_HOUR (SEC_PER_MIN * 60)
#define SEC_PER_DAY (SEC_PER_HOUR * 24)

    char *p, buf[UPTIME_LEN], uptimeStr[UPTIME_LEN];
    FILE *fp;
    PAL_INT32_T uptimeInteger, len;
    PAL_INT32_T year, month, day, hour, min, sec; 
    
    if ((fp = fopen("/proc/uptime", "r")) == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return;
    }
    
    if (fgets(buf, UPTIME_LEN, fp) == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return;
    }
    
    if (fclose(fp) == -1)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return;
    }

    p = strchr(buf, ' ');
    if (p == NULL)
    {
        fprintf(stderr, "pal_uptime failed\n");
        return;
    }
    len = p - buf;  
    memcpy(uptimeStr, buf, len);
    uptimeStr[len] = '\0';
    uptimeInteger = atoi(uptimeStr);
    
    /* FIXME: year, month should be calculated */
    year = 0;
    month = 0;

    day = uptimeInteger / SEC_PER_DAY;
    hour = (uptimeInteger % SEC_PER_DAY) / SEC_PER_HOUR;
    min = (uptimeInteger % SEC_PER_HOUR) / SEC_PER_MIN;
    sec = uptimeInteger % SEC_PER_MIN;

    sprintf(uptime, "%04d-%02d-%02dT%02d:%02d:%02d"
           , year, month, day, hour, min, sec);  
}
