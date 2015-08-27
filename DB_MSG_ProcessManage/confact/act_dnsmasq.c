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
Filename: act_dnsmasq.c
Author: Ken
Date: 2010-08-18
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_dnsmasq.h"

tcmRet actDnsmasq_stop()
{
    return actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_DNSMASQ, NULL, 0);
}

tcmRet actDnsmasq_start()
{
    char lanIp[BUFLEN_18];
    char cmd[BUFLEN_64];
    char workMode[BUFLEN_16];
    int pid;

    if ( config_api_read_parameter(ID_WAN, PARA_WORK_MODE, workMode) )
    {
        tcmLog_error("Fail to read the wan configuration");
        return TCMRET_CONFLIB_READ_ERROR;
    }

    if ( strcasecmp ("nat", workMode ) == 0 || strcasecmp ("sroute", workMode) == 0 )
    {
        if ( config_api_read_parameter(ID_LAN, PARA_LAN_IP, lanIp) )
        {
            tcmLog_error("Fail to read the lan configuration");
            return TCMRET_CONFLIB_READ_ERROR;
        }

        sprintf(cmd, "-a %s", lanIp);
        if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_DNSMASQ, cmd, strlen(cmd)+1)) == TCM_INVALID_PID )
        {
            tcmLog_error("Failed to start dnsmasq");
            return TCMRET_INTERNAL_ERROR;
        }
        else
        {
            tcmLog_debug("Starting dnsmasq, pid=%d", pid);
        }
    }
    return TCMRET_SUCCESS;
}

tcmRet actDnsmasq_restart()
{
    actDnsmasq_stop();
    return actDnsmasq_start();
}

tcmRet actDnsmasq_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actDnsmasq_restart();
        }
        break;

        case APP_START:
        {
           ret = actDnsmasq_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actDnsmasq_stop();
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

