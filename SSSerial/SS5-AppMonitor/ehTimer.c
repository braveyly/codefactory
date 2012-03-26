#include <errno.h>
#include "ehTimer.h"

static ehTimer gehTimer[MAX_NUMBER_OF_TIMER];
pthread_t createTimerThread ( ehTimer* threadTimer );

int installTimer ( timerActFunc timerFunc, struct timeval timerPeriod, 
                   ehTimerType timerType )
{
    tcmLog_debug ( "Start installing timer" );
    if ( timerFunc == NULL )
    {
        tcmLog_notice ( "Timer act function can not be NULL" );
        return -1;
    }

    if ( timerType == 0 )
    {
        timerType = ONCE;
    }

    int a;
    for ( a = 0;a < MAX_NUMBER_OF_TIMER;a++ )
    {
        if ( gehTimer[a].tmFunc == NULL )
        {
            gehTimer[a].tmFunc = timerFunc;
            gehTimer[a].tmPeriod = timerPeriod;
            gehTimer[a].tmType = timerType;
            tcmLog_debug ( "Install timer at the %dth position", a );
            return a;
        }
    }

    if ( a == MAX_NUMBER_OF_TIMER )
    {
        tcmLog_error ( "Install timer failed" );
        return -1;
    }

    return 0;
}

tcmRet startTimer ( int timerID )
{
    pthread_t tid;
    if ( timerID < 0 || timerID >= MAX_NUMBER_OF_TIMER )
    {
        tcmLog_error ( "The timer ID is error" );
        return TCMRET_FAILURE;
    }

    if ( -1 == ( tid = createTimerThread ( &gehTimer[timerID] ) ) )
    {
        return TCMRET_FAILURE;
    }

    gehTimer[timerID].tmTid = tid;
    return TCMRET_SUCCESS;

}

tcmRet stopTimer ( int timerID )
{
    if ( timerID < 0 || timerID >= MAX_NUMBER_OF_TIMER )
    {
        return TCMRET_INVALID_PARAMETER;
    }

    int ret = -1;
    if ( gehTimer[timerID].tmTid != 0 )
    {
        ret = pthread_cancel ( gehTimer[timerID].tmTid );
        ret = pthread_join ( gehTimer[timerID].tmTid, NULL );
    }
    gehTimer[timerID].tmFunc = NULL;
    return ret == 0 ? TCMRET_SUCCESS : TCMRET_FAILURE;
}

int timerThreadFunc ( void* arg )
{
    ehTimer *timerTmp = ( ehTimer * ) arg;
    int ret;
    if ( timerTmp->tmType == ONCE )
    {
        if (( ret = select ( 0, NULL, NULL, NULL, & ( timerTmp->tmPeriod ) ) < 0 ))
        {
            tcmLog_error ( "select error, the reason is %s", strerror ( errno ) );
            return -1;
        }
        else if ( ret == 0 )
        {
            ( * ( timerActFunc ) timerTmp->tmFunc ) ( NULL, NULL );
        }
    }
    else if ( timerTmp->tmType == PERIOD )
    {
        struct timeval tmTmp = timerTmp->tmPeriod;
        while ( 1 )
        {
            tmTmp = timerTmp->tmPeriod;
            if (( ret = select ( 0, NULL, NULL, NULL, & ( tmTmp ) ) < 0 ))
            {
                tcmLog_error ( "select error, the reason is %s", strerror ( errno ) );
                return -1;
            }
            else if ( ret == 0 )
            {
                ( * ( timerActFunc ) timerTmp->tmFunc ) ( NULL, NULL );
            }
        }
    }

    return 0;
}

pthread_t createTimerThread ( ehTimer* threadTimer )
{
    tcmLog_debug ( "Start creating timer thread" );
    pthread_t   tid1;
    pthread_attr_t  attr;

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_init " );
        return -1;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_setschedpolicy" );
        return -1;
    }

    if ( ( pthread_create ( &tid1, &attr, timerThreadFunc, threadTimer ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_create" );
        return -1;
    }

    tcmLog_debug ( "Successfully create a thread" );
    return tid1;
}
