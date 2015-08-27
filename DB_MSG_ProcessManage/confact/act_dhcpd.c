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
Filename: act_dhcpd.c
Author: Ken
Date: 2010-08-17
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_dhcpd.h"

tcmRet actDhcpd_stop()
{
    return actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_DHCPD, NULL, 0);
}

tcmRet actDhcpd_start()
{
    char cmd[BUFLEN_256];
    sprintf( cmd, "cp %s %s", PATHDHCPDLEASES, PATHDHCPDLEASEFILE );
    tcm_doSystemAction("actDhcpd_start", cmd);

    char workMode[BUFLEN_16];
    char dhcpdStatus[BUFLEN_16];
    int pid;
    
    if ( config_api_read_parameter(ID_WAN, PARA_WORK_MODE, workMode) )
    {
        tcmLog_error("Fail to read the wan configuration");
        return TCMRET_CONFLIB_READ_ERROR;
    }

    if ( strcasecmp ("nat", workMode ) == 0 || strcasecmp ("sroute", workMode) == 0 )
    {
        if ( config_api_read_parameter(ID_DHCPD, PARA_DHCPD_STATUS, dhcpdStatus) )
        {
            tcmLog_error("Fail to read the dhcpd configuration");
            return TCMRET_CONFLIB_READ_ERROR;
        }

        /* If DHCPD is enable, start it */
        if ( actUtil_statusStrToInt(dhcpdStatus) == ASTATUS_ENABLE )
        {
            sprintf(cmd, "-cf %s -lf %s -d eth0", PATHDHCPDCONF, PATHDHCPDLEASEFILE);
            if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_DHCPD, cmd, strlen(cmd)+1)) == TCM_INVALID_PID )
            {
                tcmLog_error("Failed to start dhcpd");
                return TCMRET_INTERNAL_ERROR;
            }
            else
            {
                tcmLog_debug("Starting dhcpd, pid=%d", pid);
            }

        }
    }

    return TCMRET_SUCCESS;
}

tcmRet actDhcpd_restart()
{
    actDhcpd_stop();
    return actDhcpd_start();
}

tcmRet actDhcpd_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actDhcpd_restart();
        }
        break;

        case APP_START:
        {
           ret = actDhcpd_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actDhcpd_stop();
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

