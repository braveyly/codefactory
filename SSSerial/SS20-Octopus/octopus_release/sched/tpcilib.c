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
* Filename:    tpcilib.c
* Author:      Daniel Gao
* Date:        2009-03-19
* Description:
*
* Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include "tpcilib.h"

#define CLI_PATH "/var/tmp/"  /* +5 for pid = 14 chars */
#define CLI_PERM S_IRWXU   /* rwx for user only */
int csfd;  /*the fd of the client socket initialize*/

typedef enum
{
    CLIENT_SUCCESS,
    CLIENT_FAILURE,
}CLIENT_RETURN;

static CLIENT_RETURN lcl_clientinit ( const char *name, APP_ID appid , int *initfd );
static CLIENT_RETURN lcl_clientsend ( APP_ID source, TPCIMSG *msg, APP_ID target ) ;
static CLIENT_RETURN lcl_clientrecv ( APP_ID *source, TPCIMSG *message, TPCIPollingMethod PollingMethod );
static CLIENT_RETURN lcl_setipcoption();

/***********************************************************************
Function: tpci_init
Description: The client application will call this function first if it
             wants to employ TPCI mechanism.

Return: TPCIRET,  - if the return value = TPCI_SUCCESS, initialize successful
                  - if the return value = TPCI_FAILURE, initialize failure.
Input: APPID app_id, - direction: IN, the appid mark a process
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Daniel Gao
        Date: 2009-03-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
TPCIRET tpci_init ( APP_ID app_id )
{
    int ret, initfd;
    TPCIMSG msg;
    TPCIRET ipcret;
    ret = lcl_clientinit ( TPCI_SCHED , app_id, &initfd );
    if ( ret != CLIENT_SUCCESS )
    {
        return TPCI_FAILURE;
    }
    else
    {
        csfd = initfd;
        msg.event = TPCI_REGISTER_TO_SCHED;
        strcpy ( msg.attr, "register" );

        ipcret = tpci_sendmsg ( app_id, &msg , ID_SCHED );

        if ( ipcret == TPCI_SUCCESS )
        {
            return TPCI_SUCCESS;
        }
        else
        {
            return TPCI_FAILURE;
        }
    }
}



/***********************************************************************
Function: tpci_uninit
Description:  This API supplies for the client application to un-initialize
              the inter-processes communication mechanism and releases the
              resource which use for the processes communication.

Return: TPCIRET, - if the return value = TPCI_SUCCESS, un-initialize successful
                 - if the return value = TPCI_FAILURE, un-initialize failure.
Input: APPID app_id, - direction: IN, the appid mark a process
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Daniel Gao
        Date: 2009-03-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
TPCIRET tpci_uninit ( APP_ID app_id )
{
    TPCIMSG msg;
    TPCIRET ipcret;

    msg.event = TPCI_UNREGISTER_TO_SCHED;
    strcpy ( msg.attr, "unregister" );

    ipcret = tpci_sendmsg ( app_id, &msg , ID_SCHED );

    if ( ipcret == TPCI_FAILURE )
    {
        //tcmLogMsgx( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "unregister to sched failure" );
    }

    close ( csfd );
    return TPCI_SUCCESS;
}




/***********************************************************************
Function: tpci_sendmsg
Description: This API supply for the application to send message between
             the processes.

Return: TPCIRET, - if the return value = TPCI_SUCCESS, send message successful.
                 - if the return value = TPCI_FAILURE, send message failure.
Input: APPID source,   - direction: IN, the message source APPID
       TPCIMSG *msg,   - direction: IN, the message to be sent
       APPID target ,  - direction: IN, the message destination
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Daniel Gao
        Date: 2009-03-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
TPCIRET tpci_sendmsg ( APP_ID source, TPCIMSG *msg, APP_ID target )
{
    int ret;
    ret = lcl_clientsend ( source, msg, target );
    if ( ret != CLIENT_SUCCESS )
    {
        return TPCI_FAILURE;
    }
    else
    {
        return TPCI_SUCCESS;
    }
}

/***********************************************************************
Function: tpci_recvmsg
Description: This API supply for the application to receive message
             from other processes.

Return: TPCIRET, - if the return value = TPCI_SUCCESS, receive message successful
                 - if the return value = TPCI_FAILURE, receive message failure.
                 - if the return value = TPCI_NO_MSG, no message receive.
Input: APPID *source, - direction: OUT, get the source of the message.
       TPCIMSG *message,
                      - direction: OUT, the buffer for the message to be received.
       TPCIPollingMethod PollingMethod ,
                      - direction: IN, define the polling method, TPCI_BLOCKING or TPCI_NOBLOCKING.
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Daniel Gao
        Date: 2009-03-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
TPCIRET tpci_recvmsg ( APP_ID *source, TPCIMSG *message, TPCIPollingMethod PollingMethod )
{
    int ret;
    ret = lcl_clientrecv ( source, message, PollingMethod );
    if ( ret != CLIENT_SUCCESS )
    {
        return TPCI_FAILURE;
    }
    else
    {
        return TPCI_SUCCESS;
    }
}

/***********************************************************************
Function: tpci_setoption
Description:  This API supply for the application to set the option for the IPC.

Return: TPCIRET, - if the return value = TPCI_SUCCESS, receive message successful
                 - if the return value = TPCI_FAILURE, receive message failure.
Input: void,     - the parameters should be defined in future.
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Daniel Gao
        Date: 2009-03-19

    2.  Modification:
        Author:
        Date:
************************************************************************/
TPCIRET tpci_setoption ()
{
    int ret;
    ret = lcl_setipcoption();
    if ( ret != CLIENT_SUCCESS )
    {
        return TPCI_FAILURE;
    }
    else
    {
        return TPCI_SUCCESS;
    }
}


CLIENT_RETURN lcl_clientinit ( const char *name, APP_ID appid, int *initfd )
{
    int  fd, len;
    int k;
    struct sockaddr_un un;

    /* create a UNIX domain stream socket */
    if ( ( fd = socket ( AF_UNIX, SOCK_STREAM, 0 ) ) < 0 )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "socket:%s\n", strerror ( errno ) );
        return CLIENT_FAILURE;
    }

    /* fill socket address structure with our address */
    memset ( &un, 0, sizeof ( un ) );
    un.sun_family = AF_UNIX;
    sprintf ( un.sun_path, "%s%02d", CLI_PATH, appid );
    len = offsetof ( struct sockaddr_un, sun_path ) + strlen ( un.sun_path );

    unlink ( un.sun_path );  /* in case it already exists */
    if ( bind ( fd, ( struct sockaddr * ) &un, len ) < 0 )
    {
        close ( fd );
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "bind:%s\n", strerror ( errno ) );
        return CLIENT_FAILURE;
    }
    if ( chmod ( un.sun_path, CLI_PERM ) < 0 )
    {
        close ( fd );
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "chmod:%s\n", strerror ( errno ) );
        return CLIENT_FAILURE;
    }

    /* fill socket address structure with server's address */
    memset ( &un, 0, sizeof ( un ) );
    un.sun_family = AF_UNIX;
    strcpy ( un.sun_path, name );
    len = offsetof ( struct sockaddr_un, sun_path ) + strlen ( name );
    if ( ( k = connect ( fd, ( struct sockaddr * ) & un, len ) ) < 0 )
    {
        close ( fd );
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "connect:%s\n", strerror ( errno ) );
        return CLIENT_FAILURE;
    }

    *initfd = fd;
    return CLIENT_SUCCESS;
}


CLIENT_RETURN lcl_clientsend ( APP_ID source, TPCIMSG *msg, APP_ID target )
{
    int    numByteSent = 0;
    int    msgLen;
    CSMSG  internalmsg;

    msgLen = sizeof ( CSMSG );

    internalmsg.event = msg->event;
    strcpy ( internalmsg.attr, msg->attr );
    internalmsg.source = source;
    internalmsg.dest = target;

    numByteSent = send ( csfd, &internalmsg, msgLen, 0 );

    if ( numByteSent != msgLen )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, " connect error %d: %s\n", errno, strerror ( errno ) );
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_DEBUG, " send failed. num of byte sent = %d\n", numByteSent );
        return CLIENT_FAILURE;
    }

    return CLIENT_SUCCESS;
}

CLIENT_RETURN lcl_clientrecv ( APP_ID *source, TPCIMSG *message, TPCIPollingMethod PollingMethod )
{
    int flag = 0;
    ssize_t gotlen;
    int msgLen;
    CSMSG internalmsg;

    if ( PollingMethod == TPCI_BLOCKING )
        flag |= MSG_WAITALL;

    msgLen = sizeof ( CSMSG );
    gotlen = recv ( csfd, &internalmsg, msgLen, flag );
    if ( gotlen == 0 )
    {
        //tcmLogMsgx( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_DEBUG, "connection closed by server" );
        return CLIENT_FAILURE;
    }
    else if ( gotlen == -1 )
    {
        //tcmLogMsg( MYLOG_CATEGORY_NAME, TCMLOG_PRIORITY_ERROR, "recv:%s\n", strerror ( errno ) );
        return CLIENT_FAILURE;
    }

    strcpy ( message->attr, internalmsg.attr );
    message->event = internalmsg.event;
    *source = internalmsg.source;

    return CLIENT_SUCCESS;
}

CLIENT_RETURN lcl_setipcoption()
{
    return CLIENT_SUCCESS;
}

