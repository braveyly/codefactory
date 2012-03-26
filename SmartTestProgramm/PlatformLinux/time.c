#include <time.h>
#include <sys/time.h>
#if 0
struct tm
{
    int tm_sec;         /* seconds */
    int tm_min;         /* minutes */
    int tm_hour;        /* hours */
    int tm_mday;        /* day of the month */
    int tm_mon;         /* month */
    int tm_year;        /* year */
    int tm_wday;        /* day of the week */
    int tm_yday;        /* day in the year */
    int tm_isdst;       /* daylight saving time */
};
#endif

char* convertTimevalToChar ( struct timeval* tmVal )
{
    time_t tmt = tmVal->tv_sec;

    struct tm* tm1 = localtime ( &tmt );
    printf ( "Now time is %s\n", asctime ( tm1 ) );
    return asctime ( tm1 );
}

struct tm stringToTm ( char* stringTime )
{
    struct tm tmTmp;
    int year, month, day, hour, minute, second;
    char yearStr[5], monthStr[3], dayStr[3], hourStr[3], minStr[3], secStr[3];
    int curValueLen = 0;

    if ( strlen ( stringTime ) == 10 )
    {
        memcpy ( yearStr, stringTime + curValueLen, 4 );
        yearStr[4] = '\0';
        curValueLen += 4;

        curValueLen += 1;

        memcpy ( monthStr, stringTime + curValueLen, 2 );
        monthStr[2] = '\0';
        curValueLen += 2;

        curValueLen += 1;

        memcpy ( dayStr, stringTime + curValueLen, 2 );
        dayStr[2] = '\0';
        curValueLen += 2;
    }
    else if ( strlen ( stringTime ) )
    {
    }
}

int main()
{
    time_t timep;
    time ( &timep );
    printf ( "%d\n", timep );

    time_t nowTime;
    time ( &nowTime );

    printf ( "time_t to char* %s\n", asctime ( gmtime ( &nowTime ) ) );
    long diffTime;

    diffTime = difftime ( nowTime, timep );

    printf ( "diffTime is %d\n", diffTime );

    struct timeval val;

    if ( gettimeofday ( &val, NULL ) )
    {
        printf ( "val.tv_sec %d   val.tv_usec %d\n", val.tv_sec, val.tv_usec );
    }
    else
    {
        printf ( "val.tv_sec %d   val.tv_usec %d\n", val.tv_sec, val.tv_usec );
    }


    struct timeval val1;

    if ( gettimeofday ( &val1, NULL ) )
    {
        printf ( "val1.tv_sec %d   val1.tv_usec %d\n", val1.tv_sec, val1.tv_usec );
    }
    else
    {
        printf ( "val1.tv_sec %d   val1.tv_usec %d\n", val1.tv_sec, val1.tv_usec );
    }



    time_t tmt = val1.tv_sec;

    struct tm* tm1 = localtime ( &tmt );
    printf ( "Now time is %s\n", asctime ( tm1 ) );
    printf ( "Now time is %s\n", convertTimevalToChar ( &val1 ) );

//==================test for calender time

    struct tm *cllocal;
    time_t clt;
    clt = time ( NULL );
    printf ( "Calender t %d\n", clt );

    cllocal = localtime ( &clt );

    printf ( "Calender year %d  month %d day %d  weekday %d hour %d minite %d second %d\n", cllocal->tm_year + 1900, cllocal->tm_mon, cllocal->tm_mday, cllocal->tm_wday, cllocal->tm_hour, cllocal->tm_min, cllocal->tm_sec );

    struct tm ta;
    time_t t_of_day;
    ta.tm_year = 2020 - 1900;
    ta.tm_mon  = 1;
    ta.tm_mday = 24;
    ta.tm_hour = 0;
    ta.tm_min  = 0;
    ta.tm_sec  = 0;
    ta.tm_isdst = 0;
    t_of_day = mktime ( &ta );
    printf ( "t_of_time %d\n", t_of_day );

    time_t alarm_time;
    time ( &alarm_time );

    struct tm* alarm_p = localtime ( &alarm_time );

    char time_str[100];
    memset ( time_str, 0, 100 );
    sprintf ( time_str, "%d.", alarm_p->tm_year + 1900 );
    sprintf ( time_str + strlen ( time_str ), "%0*d.", 2, alarm_p->tm_mon + 1 );
    sprintf ( time_str + strlen ( time_str ), "%0*d ", 2, alarm_p->tm_mday );
    sprintf ( time_str + strlen ( time_str ), "%0*d:", 2, alarm_p->tm_hour );
    sprintf ( time_str + strlen ( time_str ), "%0*d:", 2, alarm_p->tm_min );
    sprintf ( time_str + strlen ( time_str ), "%0*d",  2, alarm_p->tm_sec );
    printf ( "%s", time_str );
    printf ( "length is %d\n", strlen ( time_str ) );

    time_t gty;
    gty = mktime ( alarm_p );
    printf ( "gty = %d\n", gty );
}
/*
1309337305
time_t to char* Wed Jun 29 08:48:25 2011

diffTime is 0
val.tv_sec 1309337305   val.tv_usec 71159
val1.tv_sec 1309337305   val1.tv_usec 71184
Now time is Wed Jun 29 16:48:25 2011

Now time is Wed Jun 29 16:48:25 2011

Now time is Wed Jun 29 16:48:25 2011

Calender t 1309337305
Calender year 2011  month 5 day 29
t_of_time 1582473600
2011.06.29 16:48:25length is 19
gty = 1309337305
*/
