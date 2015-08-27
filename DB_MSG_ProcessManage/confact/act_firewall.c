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
Filename: act_firewall.c
Author: Ken
Date: 2010-09-01
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_firewall.h"
#include <ral_iptables.h>

tcmRet actFirewall_checking(int pfIndex, char** value)
{
    int i = 0, j = 0;

    if ( value == NULL )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    char tmp[PARA_FIREWALL_END -1][BUFLEN_32];
    char* pTmp[PARA_FIREWALL_END -1];

    /* For reading tables */
    for ( i = 0; i < PARA_FIREWALL_END - 1; i++ )
    {
        pTmp[i] = tmp[i];
    }

    if ( strcasecmp(value[PARA_FIREWALL_PROTOCOL - 1], "TCP") != 0 && strcasecmp(value[PARA_FIREWALL_PROTOCOL - 1], "UDP") != 0 
        && strcasecmp(value[PARA_FIREWALL_PROTOCOL - 1], "TCP/UDP") != 0)
    {
        tcmLog_error("The protocol is invalid");
        return TCMRET_INVALID_ARGUMENTS;
    }

    for ( i = 1; i <= MAXTABLE_FIREWALL; i++ )
    {
        if ( config_api_read_table_row(ID_FIREWALL, i, pTmp) )
        {
            continue;
        }

        j++;
        
        if ( pfIndex > 0 && pfIndex == i )
        {
            tcmLog_error("The index %d table exists, can not add it", pfIndex);
            return TCMRET_INVALID_EXIST;
        }

        if ( strcmp(value[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCIP - 1]) == 0 
            && strcmp(value[PARA_FIREWALL_SRCMASK - 1], pTmp[PARA_FIREWALL_SRCMASK - 1]) == 0
            && strcmp(value[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_SRCPORT - 1]) == 0
            && strcmp(value[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTIP - 1]) == 0
            && strcmp(value[PARA_FIREWALL_DSTMASK - 1], pTmp[PARA_FIREWALL_DSTMASK - 1]) == 0
            && strcmp(value[PARA_FIREWALL_DSTPORT - 1], pTmp[PARA_FIREWALL_DSTPORT - 1]) == 0
            && strcmp(value[PARA_FIREWALL_PROTOCOL - 1], pTmp[PARA_FIREWALL_PROTOCOL - 1]) == 0)
        {
            tcmLog_error("The same rules exists");
            return TCMRET_INVALID_SAME;
        }
    }

    if ( j == MAXTABLE_FIREWALL )
    {
        return TCMRET_REACH_MAX_NUM;
    }
    return TCMRET_SUCCESS;
}

tcmRet actFirewall_setting(int action, int pfIndex, char** value)
{
    tcmRet ret = TCMRET_SUCCESS;
    int i;

    char tmp[PARA_FIREWALL_END -1][BUFLEN_32];
    char* pTmp[PARA_FIREWALL_END -1];

    if ( action != RULE_ADD && action != RULE_DELETE )
    {
        return TCMRET_INVALID_ARGUMENTS;
    }

    /* check the parameter */
    if ( action == RULE_ADD )
    {
        /* For setting tables */
        for ( i = 0; i < PARA_FIREWALL_END - 1; i++ )
        {
            pTmp[i] = value[i];
        }
    
        if ((ret = actFirewall_checking(pfIndex, value)) != TCMRET_SUCCESS )
        {
            return ret;
        }
    }
    else // RULE_DELETE
    {
        /* For reading tables */
        for ( i = 0; i < PARA_FIREWALL_END - 1; i++ )
        {
            pTmp[i] = tmp[i];
        }
    
        if ( config_api_read_table_row(ID_FIREWALL, pfIndex, pTmp) )
        {
            return TCMRET_CONFLIB_READ_ERROR;
        }
    }

    /* Run the command */
    if ( strcasecmp(pTmp[PARA_FIREWALL_PROTOCOL - 1], "TCP") == 0 )
    {
        ret = ralIptables_setFirewall(action, NULL, "tcp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
            pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
            pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
    }
    else if( strcasecmp(pTmp[PARA_FIREWALL_PROTOCOL - 1], "UDP") == 0 )
    {
        ret = ralIptables_setFirewall(action, NULL, "udp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
            pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
            pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
    }
    else if ( strcasecmp(pTmp[PARA_FIREWALL_PROTOCOL - 1], "TCP/UDP") == 0 )
    {
        ret = ralIptables_setFirewall(action, NULL, "tcp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
            pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
            pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
        ret = ralIptables_setFirewall(action, NULL, "udp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
            pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
            pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
    }
    
    return ret;
}

tcmRet actFirewall_launcher()
{
    tcmRet ret = TCMRET_SUCCESS;
    int i;

    char tmp[PARA_PORTFORWARD_END -1][BUFLEN_32];
    char* pTmp[PARA_PORTFORWARD_END -1];

    /* For reading tables */
    for ( i = 0; i < PARA_FIREWALL_END - 1; i++ )
    {
        pTmp[i] = tmp[i];
    }

    for ( i = 1; i <= MAXTABLE_FIREWALL; i++ )
    {
        if ( config_api_read_table_row(ID_FIREWALL, i, pTmp))
        {
            return TCMRET_CONFLIB_READ_ERROR;
        }

        /* Run the command */
        if ( strcasecmp(pTmp[PARA_FIREWALL_PROTOCOL - 1], "TCP") == 0 )
        {
            ret = ralIptables_setFirewall(RULE_ADD, NULL, "tcp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
                pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
                pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
        }
        else if( strcasecmp(pTmp[PARA_FIREWALL_PROTOCOL - 1], "UDP") == 0 )
        {
            ret = ralIptables_setFirewall(RULE_ADD, NULL, "tcp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
                pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
                pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
        }
        else if ( strcasecmp(pTmp[PARA_FIREWALL_PROTOCOL - 1], "TCP/UDP") == 0 )
        {
            ret = ralIptables_setFirewall(RULE_ADD, NULL, "tcp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
                pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
                pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
            ret = ralIptables_setFirewall(RULE_ADD, NULL, "tcp", pTmp[PARA_FIREWALL_SRCIP - 1], pTmp[PARA_FIREWALL_SRCMASK - 1], 
                pTmp[PARA_FIREWALL_SRCPORT - 1], pTmp[PARA_FIREWALL_DSTIP - 1], pTmp[PARA_FIREWALL_DSTMASK - 1], 
                pTmp[PARA_FIREWALL_DSTPORT - 1], "DROP");
        }
        
        if ( ret != TCMRET_SUCCESS )
        {
            return ret;
        }
    }
    
    return ret;        
}

tcmRet actFirewall_doconfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        case APP_START:
        {
            ret = actFirewall_launcher();
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

