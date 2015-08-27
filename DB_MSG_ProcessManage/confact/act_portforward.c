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
Filename: act_portforward.c
Author: Ken
Date: 2010-08-30
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_portforward.h"
#include <ral_iptables.h>
#include "act_wan.h"

tcmRet actPortForward_checking(int pfIndex, char** value)
{
    int i = 0, j = 0;

    if ( value == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    char tmp[PARA_PORTFORWARD_END -1][BUFLEN_32];
    char* pTmp[PARA_PORTFORWARD_END -1];

    /* For reading tables */
    for ( i = 0; i < PARA_PORTFORWARD_END -1; i++ )
    {
        pTmp[i] = tmp[i];
    }

    int isp = atoi(value[PARA_PORTFORWARD_START_INCOMING_PORT - 1]);
    int iep = atoi(value[PARA_PORTFORWARD_END_INCOMING_PORT - 1]);
    int fsp = atoi(value[PARA_PORTFORWARD_START_FORWARD_PORT - 1]);
    int fep = atoi(value[PARA_PORTFORWARD_END_FORWARD_PORT - 1]);
    int tmpIsp, tmpIep;

    if ( strcasecmp(value[PARA_PORTFORWARD_PROTOCOL - 1], "TCP") != 0 && strcasecmp(value[PARA_PORTFORWARD_PROTOCOL - 1], "UDP") != 0 
        && strcasecmp(value[PARA_PORTFORWARD_PROTOCOL - 1], "TCP/UDP") != 0)
    {
        tcmLog_error("The protocol is invalid");
        return TCMRET_INVALID_ARGUMENTS;
    }

    if ( isp > iep )
    {
        tcmLog_error("The incoming start port should be smaller than the incoming end port");
        return TCMRET_INVALID_ARGUMENTS;
    }

    if ( fsp > fep )
    {
        tcmLog_error("The forward start port should be smaller than the forward end port");
        return TCMRET_INVALID_ARGUMENTS;
    }

    /* The port should not be repeat */
    for ( i = 1; i <= MAXTABLE_PORTFORWARD; i++ )
    {
        if ( config_api_read_table_row(ID_PORTFORWARD, i, pTmp) )
        {
            continue;
        }

        j++;

        if ( pfIndex > 0 && pfIndex == i )
        {
            tcmLog_error("The index %d table exists, can not add it", pfIndex);
            return TCMRET_INVALID_EXIST;
        }

        tmpIsp = atoi(pTmp[PARA_PORTFORWARD_START_INCOMING_PORT - 1]);
        tmpIep = atoi(pTmp[PARA_PORTFORWARD_END_INCOMING_PORT - 1 ]);

        if ( actUtil_isSameProtocol(value[PARA_PORTFORWARD_PROTOCOL - 1], pTmp[PARA_PORTFORWARD_PROTOCOL - 1])
            && !( tmpIsp > iep || isp > tmpIep ))
        {
            tcmLog_error("The incoming port should not be repeated");
            return TCMRET_INVALID_DUPLICATE;
        }
    }

    if ( j == MAXTABLE_PORTFORWARD )
    {
        return TCMRET_REACH_MAX_NUM;
    }

    return TCMRET_SUCCESS;
}

tcmRet actPortForward_setting(int action, int pfIndex, char** value)
{
    tcmRet ret = TCMRET_SUCCESS;
    int isp, iep, fsp, fep, i;
    char serverAddr[BUFLEN_32], pfproto[BUFLEN_32];

    char tmp[PARA_PORTFORWARD_END -1][BUFLEN_32];
    char* pTmp[PARA_PORTFORWARD_END -1];

    /* For reading tables */
    for ( i = 0; i < PARA_PORTFORWARD_END -1; i++ )
    {
        pTmp[i] = tmp[i];
    }
    
    if ( action != RULE_ADD && action != RULE_DELETE )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    /* check the parameter */
    if ( action == RULE_ADD )
    {
        if ((ret = actPortForward_checking(pfIndex, value)) != TCMRET_SUCCESS )
        {
            return ret;
        }
            
        isp = atoi(value[PARA_PORTFORWARD_START_INCOMING_PORT - 1]);
        iep = atoi(value[PARA_PORTFORWARD_END_INCOMING_PORT - 1]);
        fsp = atoi(value[PARA_PORTFORWARD_START_FORWARD_PORT - 1]);
        fep = atoi(value[PARA_PORTFORWARD_END_FORWARD_PORT - 1]);

        strcpy(serverAddr ,value[PARA_PORTFORWARD_SERVER_IP - 1]);
        strcpy(pfproto, value[PARA_PORTFORWARD_PROTOCOL - 1]);
    }
    else // RULE_DELETE
    {
        if ( config_api_read_table_row(ID_PORTFORWARD, pfIndex, pTmp))
        {
            return TCMRET_CONFLIB_READ_ERROR;
        }

        isp = atoi(pTmp[PARA_PORTFORWARD_START_INCOMING_PORT - 1]);
        iep = atoi(pTmp[PARA_PORTFORWARD_END_INCOMING_PORT - 1]);
        fsp = atoi(pTmp[PARA_PORTFORWARD_START_FORWARD_PORT - 1]);
        fep = atoi(pTmp[PARA_PORTFORWARD_END_FORWARD_PORT - 1]);

        strcpy(serverAddr ,pTmp[PARA_PORTFORWARD_SERVER_IP - 1]);
        strcpy(pfproto, pTmp[PARA_PORTFORWARD_PROTOCOL - 1]);
    }

    /* Run the command */
    if ( strcasecmp(pfproto, "TCP") == 0 )
    {
        ret = ralIptables_setPortForwarding(action, INTERFACENAME_WAN, "tcp", isp, iep, fsp, fep, serverAddr);
    }
    else if( strcasecmp(pfproto, "UDP") == 0 )
    {
        ret = ralIptables_setPortForwarding(action, INTERFACENAME_WAN, "udp", isp, iep, fsp, fep, serverAddr);
    }
    else if ( strcasecmp(pfproto, "TCP/UDP") == 0 )
    {
        ret = ralIptables_setPortForwarding(action, INTERFACENAME_WAN, "tcp", isp, iep, fsp, fep, serverAddr);
        ret = ralIptables_setPortForwarding(action, INTERFACENAME_WAN, "udp", isp, iep, fsp, fep, serverAddr);
    }
    
    return ret;
}

tcmRet actPortForward_launcher()
{
    tcmRet ret = TCMRET_SUCCESS;
    int isp, iep, fsp, fep, i;

    char tmp[PARA_PORTFORWARD_END -1][BUFLEN_32];
    char* pTmp[PARA_PORTFORWARD_END -1];

    /* For reading tables */
    for ( i = 0; i < PARA_PORTFORWARD_END -1; i++ )
    {
        pTmp[i] = tmp[i];
    }

    for ( i = 1; i <= MAXTABLE_PORTFORWARD; i++ )
    {
        if ( config_api_read_table_row(ID_PORTFORWARD, i, pTmp))
        {
            return TCMRET_CONFLIB_READ_ERROR;
        }

        isp = atoi(pTmp[PARA_PORTFORWARD_START_INCOMING_PORT - 1]);
        iep = atoi(pTmp[PARA_PORTFORWARD_END_INCOMING_PORT - 1]);
        fsp = atoi(pTmp[PARA_PORTFORWARD_START_FORWARD_PORT - 1]);
        fep = atoi(pTmp[PARA_PORTFORWARD_END_FORWARD_PORT - 1]);

        /* Run the command */
        if ( strcasecmp(pTmp[PARA_PORTFORWARD_PROTOCOL - 1], "TCP") == 0 )
        {
            ret = ralIptables_setPortForwarding(RULE_ADD, INTERFACENAME_WAN, "tcp", isp, iep, fsp, fep, pTmp[PARA_PORTFORWARD_SERVER_IP - 1]);
        }
        else if( strcasecmp(pTmp[PARA_PORTFORWARD_PROTOCOL - 1], "UDP") == 0 )
        {
            ret = ralIptables_setPortForwarding(RULE_ADD, INTERFACENAME_WAN, "udp", isp, iep, fsp, fep, pTmp[PARA_PORTFORWARD_SERVER_IP - 1]);
        }
        else if ( strcasecmp(pTmp[PARA_PORTFORWARD_PROTOCOL - 1], "TCP/UDP") == 0 )
        {
            ret = ralIptables_setPortForwarding(RULE_ADD, INTERFACENAME_WAN, "tcp", isp, iep, fsp, fep, pTmp[PARA_PORTFORWARD_SERVER_IP - 1]);
            ret = ralIptables_setPortForwarding(RULE_ADD, INTERFACENAME_WAN, "udp", isp, iep, fsp, fep, pTmp[PARA_PORTFORWARD_SERVER_IP - 1]);
        }
        
        if ( ret != TCMRET_SUCCESS )
        {
            return ret;
        }
    }
    
    return ret;        
}

tcmRet actPortForward_doconfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        case APP_START:
        {
            ret = actPortForward_launcher();
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
