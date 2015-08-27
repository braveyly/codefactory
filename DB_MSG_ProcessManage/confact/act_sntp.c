/**********************************************************************
Copyright (c) 2005 Wuhan Tecom Co., Ltd.
All Rights Reserved
No portions of this material may be reproduced in any form without the
written permission of:
Wuhan Tecom Co., Ltd.
#18, Huaguang Road
Wuhan, PR China 430074
All information contained in this document is Wuhan Tecom Co., Ltd.
company private, proprietary, and trade secret.
***********************************************************************
Filename: act_sntp.c
Author: Logan
Date: 2010-08-25
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_sntp.h"

tcmRet actSntp_stop()
{
    return actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_SNTP, NULL, 0);
}

tcmRet actSntp_start()
{
    char ntpserver1[BUFLEN_32];
    char ntpserver2[BUFLEN_32];
    char ntpserver3[BUFLEN_32];
    char ntpserver4[BUFLEN_32];
    char ntpserver5[BUFLEN_32];
    char cmd[BUFLEN_128];
    char timezoneindex[BUFLEN_8];
    int timezoneindexint;
 
    int pid;
    
    if ( config_api_read_parameter(ID_SNTP, PARA_SNTP_NTPSERVER1,ntpserver1 )||config_api_read_parameter(ID_SNTP, PARA_SNTP_NTPSERVER2, ntpserver2) ||
        config_api_read_parameter(ID_SNTP, PARA_SNTP_NTPSERVER3, ntpserver3)||config_api_read_parameter(ID_SNTP, PARA_SNTP_NTPSERVER4, ntpserver4)||
        config_api_read_parameter(ID_SNTP, PARA_SNTP_NTPSERVER5, ntpserver5)||config_api_read_parameter(ID_SNTP, PARA_SNTP_TIMEZONEINDEX, timezoneindex))
    {
        tcmLog_error("Fail to read the sntp configuration");
        return TCMRET_CONFLIB_READ_ERROR;
    }
    timezoneindexint=atoi(timezoneindex);

    sprintf(cmd, "-s %s -s %s -s %s -s %s -s %s -t %d",ntpserver1,ntpserver2,ntpserver3,ntpserver4,ntpserver5,timezoneindexint);
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_SNTP, cmd, strlen(cmd)+1)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to start sntp");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Starting sntp, pid=%d", pid);
    }
     
    return TCMRET_SUCCESS;
}

tcmRet actSntp_restart()
{
    actSntp_stop();
    return actSntp_start();
}

tcmRet actSntp_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actSntp_restart();
        }
        break;

        case APP_START:
        {
           ret = actSntp_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actSntp_stop();
        }
        break;

        default:
        {
            tcmLog_error("Unsupport action: [%d]", doAct);
            ret = TCMRET_INVALID_ARGUMENTS;
        }
        break;
    }
    
    return ret;
}










