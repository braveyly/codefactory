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
* Filename:    main.c
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcm.h>
#include <tpciserv.h>
#include <launcher.h>
#include <conflib.h>

#include <configact.h>

int main ( int argc, char *argv[] )
{
    int ret = TCMRET_SUCCESS;

    tcmLog_init(ID_SCHED);

    //Sched-self Initialization
    if( init_launcher() != TCMRET_SUCCESS )
    {
        tcmLog_error("Init Launcher failed.");
        return 0;
    }

    //Launch apps at bootup
    if ( app_launcher(1) != TCMRET_SUCCESS )
    {
        tcmLog_error("Launch Basic Network Env failed.");
        return 0;
    }

    //Launch Stage 2 Apps
    if( (ret = initDls(2)) != TCMRET_SUCCESS )
    {
        tcmLog_error("Failed to launched Stage 2 apps");
        keepLooping = FALSE;
    }
    else
    {
        app_launcher(2);
    }

    sched_main();  /* never returns */

    clean_sched();
    tcmLog_cleanup();
    return 0;
}


