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
/*macro alternative*/
#define   tcmLog_debug( format, args... ) printf( format, ##args);
#define   tcmLog_notice( format, args...) printf( format, ##args);
#define   tcmLog_error(format, args...) printf( format, ##args);
typedef enum TCMRET
{
	TCMRET_SUCCESS = 0,
	TCMRET_FAILURE = -1,
	TCMRET_INVALID_PARAMETER = -2,
}tcmRet;
#if 0
typedef int tcmRet;
#define TCMRET_SUCCESS 0
#define TCMRET_FAILURE -1
#define TCMRET_INVALID_PARAMETER -2
#endif

/*Normal definition*/
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

/*1st step: install timer*/
int installTimer ( timerActFunc timerFunc, struct timeval timerPeriod, 
                   ehTimerType timerType );

/*2nd step: start timer*/
tcmRet startTimer ( int timerID );

/*3rd step: stop timer*/
tcmRet stopTimer ( int timerID );
#endif
