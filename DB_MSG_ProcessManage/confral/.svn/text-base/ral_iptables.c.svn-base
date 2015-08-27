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
Filename: ral_iptables.c
Author: Ken
Date: 2010-08-25
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "ral_iptables.h"
#include <tcm_util.h>
#define IPTABLESRAL "/bin/iptables"

tcmRet ralIptables_setIpForwardingAndDynamicIpUser(int isEnable)
{
    char cmd[BUFLEN_256];
    /* Enable IP forwarding since it is disabled by default since */
    sprintf(cmd, "echo \"%d\" > /proc/sys/net/ipv4/ip_forward", isEnable);
    tcm_doSystemAction("ralIptables_setIpForwardingAndDynamicIpUser", cmd);

    /* Dynamic IP users */
    sprintf(cmd, "echo \"%d\" > /proc/sys/net/ipv4/ip_dynaddr", isEnable);
    tcm_doSystemAction("ralIptables_setIpForwardingAndDynamicIpUser", cmd);

    return TCMRET_SUCCESS;
}

tcmRet ralIptables_cleanIptablesTables()
{
    char cmd[BUFLEN_256];

    /* Enable simple Masquerading */
    /* Clearing any previous configuration */
    sprintf(cmd, "%s -F INPUT", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -F OUTPUT", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -F FORWARD", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -F", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -X", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -t nat -F", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -t nat -X", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -t mangle -F", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    sprintf(cmd, "%s -t mangle -X", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_cleanIptablesTables", cmd);

    return TCMRET_SUCCESS;
}

tcmRet ralIptables_initIptables(const char* extIf, const char* intIf)
{
    char cmd[BUFLEN_256];

    if ( extIf == NULL || intIf == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    /* Setting filter table default policy */
    sprintf(cmd, "%s -P INPUT ACCEPT", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_initIptables", cmd);

    sprintf(cmd, "%s -P OUTPUT ACCEPT", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_initIptables", cmd);

    sprintf(cmd, "%s -P FORWARD DROP", IPTABLESRAL);
    tcm_doSystemAction("ralIptables_initIptables", cmd);

    sprintf(cmd, "%s -A FORWARD -i %s -o %s -j ACCEPT", IPTABLESRAL, extIf, intIf);
    tcm_doSystemAction("ralIptables_initIptables", cmd);

    sprintf(cmd, "%s -A FORWARD -i %s -o %s -j ACCEPT", IPTABLESRAL, intIf, extIf);
    tcm_doSystemAction("ralIptables_initIptables", cmd);

    return TCMRET_SUCCESS;
}

tcmRet ralIptables_setMss()
{
    char cmd[BUFLEN_256];

    sprintf(cmd, "modprobe xt_TCPMSS");
    tcm_doSystemAction("ralIptables_setMss", cmd);

    sprintf(cmd, "iptables -t mangle -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1360");
    tcm_doSystemAction("ralIptables_setMss", cmd);

    return TCMRET_SUCCESS;
}

tcmRet ralIptables_initNat(const char* extIf, const char* intIf)
{
    char cmd[BUFLEN_256];

    if ( extIf == NULL || intIf == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    ralIptables_setIpForwardingAndDynamicIpUser(1);
    ralIptables_cleanIptablesTables();
    ralIptables_initIptables(extIf, intIf);

    sprintf(cmd, "%s -t nat -A POSTROUTING -o %s -j MASQUERADE", IPTABLESRAL, extIf);
    tcm_doSystemAction("ralIptables_initNat", cmd);

    return TCMRET_SUCCESS;
}

tcmRet ralIptables_initSroute(const char* extIf, const char* intIf)
{
    if ( extIf == NULL || intIf == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    ralIptables_setIpForwardingAndDynamicIpUser(1);
    ralIptables_cleanIptablesTables();
    ralIptables_initIptables(extIf, intIf);

    return TCMRET_SUCCESS;
}

tcmRet ralIptables_setBridge(int action, const char* extIf, const char* intIf, const char* briIf, const char* bridgeIp, int maskbit)
{
    char cmd[BUFLEN_256];

    if (( extIf == NULL && intIf == NULL ) || ( briIf == NULL ) || ( bridgeIp == NULL ))
    {
        tcmLog_error("Invalid parameter for setting bridge");
        return TCMRET_INVALID_ARGUMENTS;
    }

    if (action == RULE_ADD)
    {
        sprintf(cmd, "brctl addbr %s", briIf);
        tcm_doSystemAction("ralIptables_initBridge", cmd);

        if ( intIf != NULL )
        {
            sprintf(cmd, "brctl addif %s %s", briIf, intIf);
            tcm_doSystemAction("ralIptables_initBridge", cmd);
        }

        if ( extIf != NULL )
        {
            sprintf(cmd, "brctl addif %s %s", briIf, extIf);
            tcm_doSystemAction("ralIptables_initBridge", cmd);
        }

        sprintf(cmd, "ip addr add %s/%d dev %s brd +", bridgeIp, maskbit, briIf);
        tcm_doSystemAction("ralIptables_initBridge", cmd);

        sprintf(cmd, "ifconfig %s up", briIf);
        tcm_doSystemAction("ralIptables_initBridge", cmd);
    }
    else if (action == RULE_DELETE)
    {
        sprintf(cmd, "ip addr del %s/%d dev %s brd +", bridgeIp, maskbit, briIf);
        tcm_doSystemAction("ralIptables_initBridge", cmd);

        sprintf(cmd, "ifconfig %s down", briIf);
        tcm_doSystemAction("ralIptables_initBridge", cmd);

        if ( intIf != NULL )
        {
            sprintf(cmd, "brctl delif %s %s", briIf, intIf);
            tcm_doSystemAction("ralIptables_initBridge", cmd);
        }

        if ( extIf != NULL )
        {
            sprintf(cmd, "brctl delif %s %s", briIf, extIf);
            tcm_doSystemAction("ralIptables_initBridge", cmd);
        }
    
        sprintf(cmd, "brctl delbr %s", briIf);
        tcm_doSystemAction("ralIptables_initBridge", cmd);

    }
    else
    {
        return TCMRET_INVALID_ARGUMENTS;
    }


    return TCMRET_SUCCESS;
}

tcmRet ralIptables_runCmdPortForwarding(char action, const char *device, const char *protocol,
                              const char *inPort, const char *srvAddress, const char *srvPortForFilter, const char *srvPortForNat) 
{
    char cmd[BUFLEN_128];
    char strDevice[BUFLEN_32];
    memset(strDevice, 0, BUFLEN_64);

    if ( device != NULL && strlen(device) > 0)
    {
        sprintf(strDevice, "-i %s", device);
    }

    if ( srvPortForFilter == NULL || srvPortForNat == NULL || srvPortForFilter[0] == '\0' || srvPortForNat[0] == '\0' ) 
    {
        sprintf(cmd, "iptables -t nat -%c PREROUTING %s -p %s --dport %s -j DNAT --to-destination %s", 
            action=='A' ? 'A':'D', strDevice, protocol, inPort, srvAddress);
        tcm_doSystemAction("ralIptables_runCmdPortForwarding", cmd);

        sprintf(cmd, "iptables -%c FORWARD %c %s -p %s -d %s --dport %s -j ACCEPT", 
            action=='A' ? 'I':'D', action=='A' ? '1':' ', strDevice, protocol, srvAddress, inPort);
        tcm_doSystemAction("ralIptables_runCmdPortForwarding", cmd);
    } 
    else 
    {
        sprintf(cmd, "iptables -t nat -%c PREROUTING %s -p %s --dport %s -j DNAT --to-destination %s:%s", 
            action=='A' ? 'A':'D', strDevice, protocol, inPort, srvAddress, srvPortForNat);

        tcm_doSystemAction("ralIptables_runCmdPortForwarding", cmd);

        sprintf(cmd, "iptables -%c FORWARD %c %s -p %s -d %s --dport %s -j ACCEPT", 
            action=='A' ? 'I':'D', action=='A' ? '1':' ', strDevice, protocol, srvAddress, srvPortForFilter);

        tcm_doSystemAction("ralIptables_runCmdPortForwarding", cmd);
    }

    return TCMRET_SUCCESS;
}

tcmRet ralIptables_setPortForwarding(int action, const char* deviceName, const char* proto, int iStartPort, int iEndPort, int fStartPort, int fEndPort, const char* serverIp)
{
    char act;
    char strPortForNat[BUFLEN_16], strPortForFilter[BUFLEN_16], strInPort[BUFLEN_16];

    if ( proto == NULL || serverIp == NULL)
    {
        tcmLog_error("Invalid parameter for setting port forwarding");
        return TCMRET_INVALID_ARGUMENTS;
    }

    if ( action == RULE_ADD )
    {
        act = 'A';
    }
    else if ( action == RULE_DELETE )
    {
        act = 'D';
    }
    else
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    if (iStartPort == iEndPort)
    {
        sprintf(strInPort, "%u", iStartPort); 
    }
    else
    {
        sprintf(strInPort, "%u:%u", iStartPort, iEndPort);
    }

    /* if incoming ports are same as forwarding, leave intPort to "" */
    if (iStartPort == fStartPort && iEndPort == fEndPort)  
    {
        strPortForNat[0] = '\0';
        strPortForFilter[0] = '\0';
    }
    else 
    {
        if (fStartPort == fEndPort)
        {
            sprintf(strPortForNat, "%u", fStartPort); 
            sprintf(strPortForFilter, "%u", fStartPort); 
        }
        else
        {
            sprintf(strPortForFilter, "%u:%u", fStartPort, fEndPort);
            sprintf(strPortForNat, "%u-%u", fStartPort, fEndPort);
        }
    }

    return ralIptables_runCmdPortForwarding(act, deviceName, proto, strInPort, serverIp, strPortForFilter, strPortForNat);
}

tcmRet ralIptables_runCmdFirewall(char action, const char* device, const char* protocol, const char* srcip, const char* srcmask,
                                         const char* srcport, const char* dstip, const char* dstmask, const char* dstport, const char* policy)
{
    char cmd[BUFLEN_256];
    char strinput[BUFLEN_32], strprotocol[BUFLEN_32], strsrcip[BUFLEN_32], strsrcport[BUFLEN_16], 
        strdstip[BUFLEN_32], strdstport[BUFLEN_16];
    memset(strinput, 0, BUFLEN_32);
    memset(strprotocol, 0, BUFLEN_32);
    memset(strsrcip, 0, BUFLEN_32);
    memset(strsrcport, 0, BUFLEN_16);
    memset(strdstip, 0, BUFLEN_32);
    memset(strdstport, 0, BUFLEN_16);

    if ( device != NULL && strlen(device) > 0)
    {
        sprintf(strinput, "-i %s", device);
    }

    if ( protocol != NULL && strlen(protocol) > 0)
    {
        sprintf(strprotocol, "-p %s", protocol);
    }

    if ( srcip != NULL && strlen(srcip) > 0 )
    {
        sprintf(strsrcip, "-s %s", srcip);
        if ( srcmask != NULL && strlen(srcmask) > 0 )
        {
            sprintf(strsrcip, "%s/%s", strsrcip, srcmask);
        }
    }

    if ( srcport != NULL && strlen(srcport) > 0 )
    {
        sprintf(strsrcport, "--sport %s", srcport);
    }

    if ( dstip != NULL && strlen(dstip) > 0 )
    {
        sprintf(strdstip, "-d %s", dstip);
        if ( dstmask != NULL && strlen(dstmask) > 0 )
        {
            sprintf(strdstip, "%s/%s", strdstip, dstmask);
        }
    }

    if ( dstport != NULL && strlen(dstport) > 0 )
    {
        sprintf(strdstport, "--dport %s", dstport);
    }

    sprintf(cmd, "iptables -%c INPUT %s %s %s %s %s %s -j %s", action, strinput, strprotocol, strsrcip, strsrcport, strdstip, strdstport, policy);
    tcm_doSystemAction("ralIptables_runCmdFirewall", cmd);

    sprintf(cmd, "iptables -%c FORWARD %s %s %s %s %s %s -j %s", action, strinput, strprotocol, strsrcip, strsrcport, strdstip, strdstport, policy);
    tcm_doSystemAction("ralIptables_runCmdFirewall", cmd);

    return TCMRET_SUCCESS;
}


tcmRet ralIptables_setFirewall(int action, const char* deviceName, const char* proto, const char* srcip, const char* srcmask, 
                                    const char* srcport, const char* dstip, const char* dstmask, const char* dstport, const char* policy)
{
    char act;

    if ( policy == NULL || (strcmp("ACCEPT", policy) != 0 && strcmp("DROP", policy) != 0))
    {
        tcmLog_error("Invalid parameter for setting firewall");
        return TCMRET_INVALID_ARGUMENTS;
    }

    if ( action == RULE_ADD )
    {
        act = 'A';
    }
    else if ( action == RULE_DELETE )
    {
        act = 'D';
    }
    else
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    return ralIptables_runCmdFirewall(act, deviceName, proto, srcip, srcmask, srcport, dstip, dstmask, dstport, policy);
}

tcmRet ralIptables_setDmz(int action, const char* dmzIp, const char* device)
{
    char act;
    char cmd[BUFLEN_128];
    char strInput[BUFLEN_16];

    if ( dmzIp == NULL )
    {
        tcmLog_error("Invalid parameter for setting dmz");
        return TCMRET_INVALID_ARGUMENTS;
    }

    if ( strlen(dmzIp) == 0 || strcmp(dmzIp, "0.0.0.0") == 0)
    {
        // no setting
        return TCMRET_SUCCESS;
    }

    if ( action == RULE_ADD )
    {
        act = 'A';
    }
    else if ( action == RULE_DELETE )
    {
        act = 'D';
    }
    else
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    if ( device != NULL && strlen(device) > 0 )
    {
        sprintf(strInput, "-i %s", device);
    }
    
    sprintf(cmd, "iptables -t nat -%c PREROUTING %s -p all -j DNAT --to-destination %s", act, strInput, dmzIp);
    tcm_doSystemAction("ralIptables_setDmz", cmd);

    return TCMRET_SUCCESS;
}

