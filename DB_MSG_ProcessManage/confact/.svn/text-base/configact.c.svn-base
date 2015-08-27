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
Filename: configact.c
Author: Ken
Date: 2010-08-16
Description: Config action lib
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/

#include "configact.h"
#include "act_lan.h"
#include "act_dhcpd.h"
#include "act_dnsmasq.h"
#include "act_wan.h"
#include "act_sntp.h"
#include "act_easyapp.h"
#include "act_portforward.h"
#include "act_firewall.h"
#include "act_dmz.h"

tcmRet config_act_run( tcmEntityId APP_ID, tcmActAppID APP_ACTION )
{
    tcmRet ret = TCMRET_SUCCESS;

    switch (APP_ID)
    {
        case ID_SYSLOGD:
        {
            ret = actSyslog_doConfig(APP_ACTION);
        }
        break;

        case ID_INETD:
        {
            ret = actInetd_doConfig(APP_ACTION);
        }
        break;

        case ID_PCSCD:
        {
            ret = actPcscd_doConfig(APP_ACTION);
        }
        break;

        case ID_SDKD:
        {
            ret = actSdkd_doConfig(APP_ACTION);
        }
        break;
    
        case ID_AUTOCM:
        {
            ret = actAutocm_doConfig(APP_ACTION);
        }
        break;

        case ID_LAN:
        {
            ret = actLan_doConfig(APP_ACTION);
        }
        break;

        case ID_DHCPD:
        {
            ret = actDhcpd_doConfig(APP_ACTION);
        }
        break;

        case ID_DNSMASQ:
        {
            ret = actDnsmasq_doConfig(APP_ACTION);
        }
        break;

        case ID_WAN:
        {
            ret = actWan_doConfig(APP_ACTION);
        }
        break;

        case ID_SNTP:
        {
            ret = actSntp_doConfig(APP_ACTION);
        }
        break;

        case ID_PORTFORWARD:
        {
            ret = actPortForward_doconfig(APP_ACTION);
        }
        break;

        case ID_FIREWALL:
        {
            ret = actFirewall_doconfig(APP_ACTION);
        }
        break;

        case ID_DMZ:
        {
            ret = actDmz_doConfig(APP_ACTION);
        }
        break;

        default:
        {
            ret = TCMRET_INVALID_ARGUMENTS;
            tcmLog_error("Does not support that application [%d]", APP_ID );
        }
        break;
    }

    return ret;
}

tcmRet config_act_refresh_apps( tcmEntityId APP_ID, tcmActAppID APP_ACTION )
{
    tcmRet ret = TCMRET_SUCCESS;
    if ( APP_ID >= ID_ALLEND || APP_ID <= 0 || APP_ID == ID_END || APP_ACTION >= APP_NO_ACTION || APP_ACTION <= 0 )
    {
        tcmLog_error("ID error, ID %d is not in the range 1 ~ %d or ACTION [%d] is not right", APP_ID, ID_ALLEND - 1, APP_ACTION );
        return TCMRET_INVALID_ARGUMENTS;
    }

    if ( APP_ID > ID_END && (APP_ACTION == APP_WRITE || APP_ACTION == APP_WRITE_RESTART))
    {
        tcmLog_error("The applications after %d do not have configuration, so do not support write action");
        return TCMRET_INVALID_ARGUMENTS;
    }

    if (APP_ACTION == APP_WRITE) 
    {
        ret = config_api_write_to_flash(APP_ID);
        return ret;
    }
    else if (APP_ACTION == APP_WRITE_RESTART) 
    {
        ret = config_api_write_to_flash(APP_ID);
        if ( ret )
            return TCMRET_WRITE_FLASH_ERROR;
    }

    return config_act_run(APP_ID, APP_ACTION);
}

tcmRet config_act_refresh_tables( tcmEntityId APP_ID, tcmActTalbesID TABLES_ACTION, int tabIndex, char** value)
{
    tcmRet ret = TCMRET_INVALID_ARGUMENTS;
    if ( APP_ID >= ID_END )
    {
        tcmLog_error("ID error, ID %d is not in the range 1 ~ %d", APP_ID, ID_END - 1 );
        return TCMRET_INVALID_ARGUMENTS;
    }

    switch ( APP_ID )
    {
        case ID_PORTFORWARD:
        {
            ret = actPortForward_setting(TABLES_ACTION, tabIndex, value);
        }
        break;

        case ID_FIREWALL:
        {
            ret = actFirewall_setting(TABLES_ACTION, tabIndex, value);
        }
        break;

        default:
        {
            ret = TCMRET_INVALID_ARGUMENTS;
            tcmLog_error("Does not support that application [%d]", APP_ID );
        }
        break;
    }

    return ret;
}

