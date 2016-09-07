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
* Filename:    exam_client.c
* Author:      Daniel Gao
* Date:        2009-03-23
* Description:
*
* Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "tpcilib.h"
#define MYLOG_CATEGORY_NAME "tecom.newiad.tpciexample"
#include "tecomLog.h"

#define MAX_APP 10
#define UNKNOWN -1

void sigHandler ( int arg )
{
    tpci_uninit ( ID_HTTP );
    tpci_uninit ( ID_VOIP );
    //tcmLogMsgx ( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_TRACE, "bye-bye." );

    if ( tcmLogUnInit() )
    {
        printf ( "mylog_fini() failed" );
    }
    exit ( 0 );
}

void *send_msg ( void* arg )
{
    int j = 0;
    TPCIRET ret;
    TPCIMSG msg, message;
    APP_ID source = ID_HTTP;
    APP_ID dest = ID_VOIP;
    TPCIPollingMethod PollingMethod = TPCI_NOBLOCKING;

    ret = tpci_init ( ID_HTTP );
    if ( !ret )
    {
        //tcmLogMsgx( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Init failure" );
        return 0;
    }

    sleep ( 1 );

    msg.event = TPCI_HTTP_VOIP_START;
    sprintf ( msg.attr, "app %s", "http" );

    while ( 1 )
    {
        j++;

        ret = tpci_sendmsg ( source, &msg, dest );
        if ( !ret )
        {
           //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "send failure %d", j );
        }

        usleep(1000);
    }
}

void *recv_msg ( void* arg )
{
    int j = 0;
    TPCIRET ret;
    TPCIMSG message;
    APP_ID source;
    TPCIPollingMethod PollingMethod = TPCI_NOBLOCKING;

    ret = tpci_init ( ID_VOIP );
    if ( !ret )
    {
        //tcmLogMsgx( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Init failure" );
        return 0;
    }

    while ( 1 )
    {
        ret = tpci_recvmsg ( &source, &message, PollingMethod );
        j++;

        if ( ret )
        {
            //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_TRACE, "message %d recv success, source is %d, event is %d, attr is %s", j, source, message.event, message.attr );
        }
        else
        {
            //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "message %d recv failure", j );
        }

        usleep(1000);
    }
}

int main ( int argc, char* argv[] )
{
    TPCIRET ret;
    pthread_t   tid1, tid2;
    struct sched_param sched;
    pthread_attr_t  attr;

    if ( tcmLogInit ( 0 ) ) //Add timestamp to log
    {
        printf ( "mylog_init() failed" );
        return 0;
    }

    //if ( signal ( SIGTERM, sigHandler ) == SIG_ERR )
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Load sigHandler error:%s", strerror( errno ) );

    memset ( &sched, 0, sizeof ( sched ) );

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_attr_init:%s",strerror( errno )  );
        return -1;
    }


    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_attr_setschedpolicy:%s", strerror( errno ));
        return -1;
    }

    sched.sched_priority = 76;
    if ( ( pthread_attr_setschedparam ( &attr, &sched ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_attr_setschedparam:%s", strerror( errno ));
        return -1;
    }
    if ( ( pthread_create ( &tid1, &attr, send_msg, 0 ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_create:%s", strerror( errno ));
        return -1;
    }

    memset ( &sched, 0, sizeof ( sched ) );

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_attr_init:%s",strerror( errno )  );
        return -1;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_attr_setschedpolicy:%s", strerror( errno ));
        return -1;
    }

    sched.sched_priority = 80;
    if ( ( pthread_attr_setschedparam ( &attr, &sched ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_attr_setschedparam:%s", strerror( errno ));
        return -1;
    }
    if ( ( pthread_create ( &tid2, &attr, recv_msg, 0 ) != 0 ) )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "Error, pthread_create:%s", strerror( errno ));
        return -1;
    }

    while ( 1 )
    {
        sleep ( 3 );
    }
}
