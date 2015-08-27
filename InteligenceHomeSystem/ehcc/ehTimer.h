/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************
* Filename:    ehTimer.h
* Author:      Nick
* Date:        2010-12-21
* Description: Timer.
*
* Histroy:
    1.  Revision:  0.1
  Modification:First draft
        Author:Nick
        Date:2010-12-21
***********************************************************************/
#ifndef _EH_TIMER_H_
#define _EH_TIMER_H_
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#define MAX_NUMBER_OF_TIMER 10
typedef int ( *timerActFunc ) ( void*, void* );
typedef enum EHTIMERTYPE
{
    ONCE = 1,
    PERIOD = 2,
}ehTimerType;


typedef struct EHTIMER
{
    timerActFunc tmFunc;
    ehTimerType tmType;
    struct timeval tmPeriod;
    pthread_t tmTid;
}ehTimer;

ehTimer gehTimer[MAX_NUMBER_OF_TIMER];
int installTimer ( timerActFunc timerFunc, struct timeval timerPeriod, 
                   ehTimerType timerType );
tcmRet startTimer ( int timerID );
pthread_t createTimerThread ( ehTimer* threadTimer );
tcmRet stopTimer ( int timerID );
#endif
