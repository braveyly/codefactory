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
* Filename:    tpciserv.h
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
#ifndef __TPCISERV__H
#define __TPCISERV__H

#include <tcm.h>

#define SCHED_MESSAGE_ADDR "/home/nick/etc/sched_msg_svr_addr"

#define SCHED_MESSAGE_BACKLOG  3

typedef enum
{
    SCHED_FAILURE,
    SCHED_SUCCESS
}SCHED_RETURN;


void sched_loop ( void );

void clean_sched( void );

/**************************************************************
* Init Sched Local Socket Resources                           *
***************************************************************/
SINT32 init_schedServerSocket();

#endif
