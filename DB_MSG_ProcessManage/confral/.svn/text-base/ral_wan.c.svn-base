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
Filename: ral_wan.c
Author: Ken
Date: 2010-08-25
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "ral_wan.h"
#include <tcm_util.h>

tcmRet ralWan_doConfig(int action, const char* wanName, const char* wanIp, const char* wanMask, const char* wanGateway, const char* wanMtu)
{
    char cmd[BUFLEN_256];
    char strAction[BUFLEN_8];

    if (action == RULE_ADD)
    {
        strcpy(strAction, "add");
    }
    else if (action == RULE_DELETE)
    {
        strcpy(strAction, "del");
    }
    else
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    sprintf(cmd, "ifconfig %s 0.0.0.0", wanName);
    tcm_doSystemAction("ralWan_doConfig", cmd);

    if (action == RULE_ADD)
    {
        sprintf(cmd, "ifconfig %s %s netmask %s up", wanName, wanIp, wanMask );
        tcm_doSystemAction("ralWan_doConfig", cmd);
    }

    sprintf(cmd, "route %s -net %s netmask 255.255.255.255 dev %s", strAction, wanGateway, wanName);
    tcm_doSystemAction("ralWan_doConfig", cmd);

    sprintf(cmd, "route %s default gw %s", strAction, wanGateway);
    tcm_doSystemAction("ralWan_doConfig", cmd);

    if (action == RULE_ADD)
    {
        sprintf(cmd, "ifconfig %s mtu %s", wanName, wanMtu);
        tcm_doSystemAction("ralWan_doConfig", cmd);
    }
    return TCMRET_SUCCESS;
}


tcmRet ralWan_setVlan(int action, const char* infName, int vlanID, int skbPrio, int vlanPrio)
{
    char cmd[BUFLEN_256];

    if ( infName == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    if (action == RULE_ADD)
    {
        sprintf(cmd, "vconfig add %s %d", infName, vlanID);
        tcm_doSystemAction("ralWan_setVlan", cmd);

        sprintf(cmd, "ifconfig %s.%d up", infName, vlanID);
        tcm_doSystemAction("ralWan_setVlan", cmd);

        sprintf(cmd, "vconfig set_egress_map %s.%d %d %d", infName, vlanID, skbPrio, vlanPrio);
        tcm_doSystemAction("ralWan_setVlan", cmd);
    }
    else if (action == RULE_DELETE)
    {
        sprintf(cmd, "ifconfig %s.%d down", infName, vlanID);
        tcm_doSystemAction("ralWan_setVlan", cmd);
    
        sprintf(cmd, "vconfig rem %s.%d", infName, vlanID);
        tcm_doSystemAction("ralWan_setVlan", cmd);
    }
    else
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    return TCMRET_SUCCESS;
}
