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
Filename: act_easyapp.c
Author: Ken
Date: 2010-08-27
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_easyapp.h"

tcmRet actSyslog_stop()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_SYSLOGD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to stop syslogd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Stop syslogd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actSyslog_start()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_SYSLOGD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to start syslogd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Start syslogd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actSyslog_restart()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_RESTART_APP, ID_SYSLOGD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to restart syslogd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Restart syslogd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}


tcmRet actSyslog_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actSyslog_restart();
        }
        break;

        case APP_START:
        {
           ret = actSyslog_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actSyslog_stop();
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


tcmRet actInetd_stop()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_INETD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to stop inetd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Stop inetd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actInetd_start()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_INETD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to start inetd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Start inetd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actInetd_restart()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_RESTART_APP, ID_INETD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to restart inetd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Restart inetd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}


tcmRet actInetd_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actInetd_restart();
        }
        break;

        case APP_START:
        {
           ret = actInetd_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actInetd_stop();
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

tcmRet actPcscd_stop()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_PCSCD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to stop pcscd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Stop pcscd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actPcscd_start()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_PCSCD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to start pcscd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Start pcscd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actPcscd_restart()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_RESTART_APP, ID_PCSCD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to restart pcscd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Restart pcscd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}


tcmRet actPcscd_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actPcscd_restart();
        }
        break;

        case APP_START:
        {
           ret = actPcscd_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actPcscd_stop();
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


tcmRet actSdkd_stop()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_SDKD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to stop sdkd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Stop sdkd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actSdkd_start()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_SDKD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to start sdkd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Start sdkd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actSdkd_restart()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_RESTART_APP, ID_SDKD, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to restart sdkd");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Restart sdkd, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}


tcmRet actSdkd_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actSdkd_restart();
        }
        break;

        case APP_START:
        {
           ret = actSdkd_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actSdkd_stop();
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


tcmRet actAutocm_stop()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_STOP_APP, ID_AUTOCM, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to stop autocm");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Stop autocm, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actAutocm_start()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_AUTOCM, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to start autocm");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Start autocm, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}

tcmRet actAutocm_restart()
{
    int pid = 0;
    if ( (pid = actUtil_sendMsgToSched(TCM_MSG_RESTART_APP, ID_AUTOCM, NULL, 0)) == TCM_INVALID_PID )
    {
        tcmLog_error("Failed to restart autocm");
        return TCMRET_INTERNAL_ERROR;
    }
    else
    {
        tcmLog_debug("Restart autocm, pid=%d", pid);
    }

    return TCMRET_SUCCESS;
}


tcmRet actAutocm_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actAutocm_restart();
        }
        break;

        case APP_START:
        {
           ret = actAutocm_start(); 
        }
        break;

        case APP_STOP:
        {
            ret = actAutocm_stop();
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


