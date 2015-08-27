#include <tcm_log.h>
#include "ehTimeout.h"

tcmRet initGlobalTimeout()
{
    gcardTimeout = 360;
    gidpPairFailedTimes = 3;

    return TCMRET_SUCCESS;
}

int isTimeout ( time_t startTime, long timeoutPeriod )
{
    if ( startTime <= 0 || timeoutPeriod < 0 )
    {
        tcmLog_error ( "parameters are error\n" );
        return -1;
    }

    time_t nowTime;
    long diffTime;
    time ( &nowTime );
    diffTime = nowTime - startTime;
    tcmLog_debug ( "startTime %d nowTime %d difftime %d timeoutPeriod %d\n", 
                   startTime, nowTime, diffTime, timeoutPeriod );
    if ( diffTime <= timeoutPeriod )
    {
        tcmLog_notice ( "Now is not timeout\n" );
        return 0;
    }
    else
    {
        tcmLog_debug ( "StartTime is %s  NowTime is %s\n", 
                        asctime ( gmtime ( &startTime ) ), 
                        asctime ( gmtime ( &nowTime ) ) );
        tcmLog_notice ( "Now is timeout\n" );
        return 1;
    }

}


char* convertTimevalToChar ( struct timeval tmVal )
{
    tcmLog_debug ( "tv_sec %d, tv_usec %d", tmVal.tv_sec, tmVal.tv_usec );
    time_t tmt = tmVal.tv_sec;
    struct tm* tm1 = gmtime ( &tmt );
    /*The same with UTC
     struct tm* tm2 = localtime(&tmt );
        printf("Now LOCAL time is %s\n", asctime(tm2));
    */
    return asctime ( tm1 );
}

tcmRet showTimeVal ( struct timeval tmVal, char* preStr )
{
    tcmLog_debug ( "tv_sec %d, tv_usec %d\n", tmVal.tv_sec, tmVal.tv_usec );
    time_t tmt = tmVal.tv_sec;
    struct tm* tm1 = gmtime ( &tmt );
    tcmLog_debug ( "%s Now UTC time is %s\n", preStr, asctime ( tm1 ) );  /*ok*/

    struct tm* tm2 = localtime ( &tmt );
    tcmLog_debug ( "%s Now LOCAL time is %s\n", preStr, asctime ( tm2 ) );

    return TCMRET_SUCCESS;
}
/*
ehcc:debug:119.115:showEhMsg:202:The message creating time is 946684919   111938
ehcc:debug:119.115:convertTimevalToChar:53:tv_sec 946684919, tv_usec 111938

Now UTC time is Sat Jan  1 00:01:59 2000

Now LOCAL time is ;
ehcc:debug:119.115:showEhMsg:203:The message creating time is Thu Jan  1 00:00:59 1970
*/