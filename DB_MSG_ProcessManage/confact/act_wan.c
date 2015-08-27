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
Filename: act_wan.c
Author: Ken
Date: 2010-08-25
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_wan.h"
#include "act_portforward.h"
#include "act_firewall.h"
#include <ral_wan.h>
#include <ral_iptables.h>

tcmRet actWan_configWan()
{
    char strWorkMode[BUFLEN_18];
    char strWanMode[BUFLEN_18];
    char strWanIp[BUFLEN_18], strWanNetmask[BUFLEN_18], strWanGateway[BUFLEN_18], strMtu[BUFLEN_18];
    char strBridgeIp[BUFLEN_18], strBridgeMaskBit[BUFLEN_8];
    char strVlanID[BUFLEN_8], strVlanSkbPrio[BUFLEN_8], strVlanPrio[BUFLEN_8];
    char strNewName[BUFLEN_18];
    char cmd[BUFLEN_512];
    int httpdPid = 0;
    
    if ( config_api_read_parameter(ID_WAN, PARA_WORK_MODE, strWorkMode) 
        || config_api_read_parameter(ID_WAN, PARA_WAN_MODE, strWanMode)
        || config_api_read_parameter(ID_WAN, PARA_WAN_IP, strWanIp) 
        || config_api_read_parameter(ID_WAN, PARA_WAN_NETMASK, strWanNetmask)
        || config_api_read_parameter(ID_WAN, PARA_WAN_GATEWAY, strWanGateway)
        || config_api_read_parameter(ID_WAN, PARA_WAN_MTU, strMtu)
        || config_api_read_parameter(ID_WAN, PARA_WAN_BRIDGEIP, strBridgeIp)
        || config_api_read_parameter(ID_WAN, PARA_WAN_BRIDGEMASKBIT, strBridgeMaskBit)
        || config_api_read_parameter(ID_WAN, PARA_WAN_VLANID, strVlanID)
        || config_api_read_parameter(ID_WAN, PARA_WAN_VLANSKBPRIO, strVlanSkbPrio)
        || config_api_read_parameter(ID_WAN, PARA_WAN_VLANPRIO, strVlanPrio))
    {
        tcmLog_error("Fail to read the wan configuration");
        return TCMRET_CONFLIB_READ_ERROR;
    }

    if ( strcasecmp("nat", strWorkMode) == 0 || strcasecmp("sroute", strWorkMode) == 0 )
    {
        if ( strcasecmp("nat", strWorkMode) == 0 )
        {
            /* Run /etc/init.d/script/nat_conf.sh */
            if( ralIptables_initNat(INTERFACENAME_WAN, INTERFACENAME_LAN) != TCMRET_SUCCESS )
            {
                tcmLog_error("Fail to init simple nat");
            }
        }
        else
        {
            /* Run /etc/init.d/script/sroute_conf.sh */
            if( ralIptables_initSroute(INTERFACENAME_WAN, INTERFACENAME_LAN) != TCMRET_SUCCESS )
            {
                tcmLog_error("Fail to init sroute");
            }
        }

        // TODO: /etc/init.d/script/nat_user_conf.sh
        config_act_refresh_apps(ID_PORTFORWARD, APP_START);
        config_act_refresh_apps(ID_FIREWALL, APP_START);
        config_act_refresh_apps(ID_DMZ, APP_START);

        sprintf(cmd, "modprobe ip_nat_ftp");
        tcm_doSystemAction("actWan_configWan", cmd);

        /* Run /etc/init.d/script/setmss.sh */
        ralIptables_setMss();

        /* Wan uses static IP */
        if ( strcasecmp("static", strWanMode) == 0 )
        {
            if (ralWan_doConfig(RULE_ADD, INTERFACENAME_WAN, strWanIp, strWanNetmask, strWanGateway, strMtu) != TCMRET_SUCCESS)
            {
                tcmLog_error("Can not config wan");
            }
        }
        else
        {
            tcmLog_debug("Use the dhcp mode to configure wan");
        }
        
    }
    else if ( strcasecmp("bridge", strWorkMode) == 0 )
    {
        sprintf(cmd, "ifconfig %s mtu 1500 up", INTERFACENAME_WAN);
        tcm_doSystemAction("actWan_configWan", cmd);

        if ( ralIptables_setBridge(RULE_ADD, INTERFACENAME_WAN, INTERFACENAME_LAN, INTERFACENAME_BRIDGE, strBridgeIp, atoi(strBridgeMaskBit)) != TCMRET_SUCCESS )
        {
            tcmLog_error("Fail to set bridge");
        }

        sprintf(cmd, "ifconfig %s 0.0.0.0", INTERFACENAME_LAN);
        tcm_doSystemAction("actWan_configWan", cmd);
    }
    else if ( strcasecmp("vlan", strWorkMode) == 0 )
    {
        if ( ralWan_setVlan(RULE_ADD, INTERFACENAME_WAN, atoi(strVlanID), atoi(strVlanSkbPrio), atoi(strVlanPrio)) != TCMRET_SUCCESS )
        {
            tcmLog_error("Fail to set vlan interface");
        }
        else
        {
            sprintf(strNewName, "%s.%d", INTERFACENAME_WAN, atoi(strVlanID));

            if ( ralIptables_setBridge(RULE_ADD, strNewName, INTERFACENAME_LAN, INTERFACENAME_BRIDGE, strBridgeIp, atoi(strBridgeMaskBit)) != TCMRET_SUCCESS )
            {
                tcmLog_error("Fail to set bridge");
            }
        }

        sprintf(cmd, "ifconfig %s 0.0.0.0", INTERFACENAME_LAN);
        tcm_doSystemAction("actWan_configWan", cmd);
    }
    else
    {
        tcmLog_error("The work mode is error: %s", strWorkMode);
        return TCMRET_CONFLIB_PARAMETER_INVALID;
    }

    /* Start Httpd */
    if ( (httpdPid = actUtil_sendMsgToSched(TCM_MSG_START_APP, ID_THTTPD, NULL, 0) == TCM_INVALID_PID ))
    {
        tcmLog_error("Failed to start thttpd");
    }
    else
    {
        tcmLog_debug("Starting thttpd, pid=%d", httpdPid);
    }
    
    return TCMRET_SUCCESS;
}


tcmRet actWan_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_START:
            /* It seems that we should wait for wimax, then to configuration IP 
               So the first time to start, need to wait */
            sleep(5);
        case APP_WRITE_RESTART:
        case APP_RESTART:
        {
            ret = actWan_configWan();
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

