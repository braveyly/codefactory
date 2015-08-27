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
Filename: act_util.c
Author: Ken
Date: 2010-08-17
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_util.h"
#include "act_dhcpd.h"
#include "act_dnsmasq.h"

extern actInfo actLibraryInfo;

UINT32 actUtil_sendMsgToSched(tcmMsgType msgType, UINT32 wordData, void *msgData, UINT32 msgDataLen)
{
   UINT32 ret;
   tcmMsgHeader *msg;
   char *data;
   void *msgBuf;

   /* for msg with user data */
   if (msgData != NULL && msgDataLen != 0)
   {
      msgBuf = tcmMem_alloc(sizeof(tcmMsgHeader) + msgDataLen, ALLOC_ZEROIZE);
   } 
   else
   {
      msgBuf = tcmMem_alloc(sizeof(tcmMsgHeader), ALLOC_ZEROIZE);
   }
   
   msg = (tcmMsgHeader *)msgBuf;
   
   /* initialize some common fields */
   msg->src = actLibraryInfo.eid;
   msg->dst = ID_SCHED;
   msg->flags_request = 1;
   msg->type = msgType;   
   msg->wordData = wordData;

   if (msgData != NULL && msgDataLen != 0)
   {
      data = (char *) (msg + 1);
      msg->dataLength = msgDataLen;
      memcpy(data, (char *)msgData, msgDataLen);
   }      
   
   tcmLog_debug("send msg id[0X%X] to smd, (wordData=%d) msg=%s", (UINT32) msgType, wordData, msgData);

   ret = tcmMsg_sendAndGetReply(actLibraryInfo.msgHandle, msg);

   TCMMEM_FREE_BUF_AND_NULL_PTR(msgBuf);

   return ret;
}

UINT32 actUtil_statusStrToInt(char* strStatus)
{
    if ( strStatus == NULL )
    {
        return ASTATUS_INVALID;
    }

    if ( strcmp (strStatus, "enable") == 0 || strcmp (strStatus, "Enable") == 0 )
    {
        return ASTATUS_ENABLE;
    }
    else if ( strcmp (strStatus, "disable") == 0 || strcmp (strStatus, "Disable") == 0 )
    {
        return ASTATUS_DISABLE;
    }
    else
    {
        return ASTATUS_INVALID;
    }
}


UBOOL8 actUtil_isSameProtocol(const char* proto1, const char* proto2)
{
    if ( proto1 == NULL || proto2 == NULL )
    {
        return FALSE;
    }

    char tmp1[BUFLEN_32], tmp2[BUFLEN_32];
    strcpy(tmp1, proto1);
    strcpy(tmp2, proto2);
    tcm_toLowerCase(tmp1);
    tcm_toLowerCase(tmp2);

    if ( strstr(tmp1, "tcp") != NULL && strstr(tmp2, "tcp") != NULL)
    {
        return TRUE;
    }

    if ( strstr(tmp1, "udp") != NULL && strstr(tmp2, "udp") != NULL)
    {
        return TRUE;
    }

    return FALSE;
}

tcmRet actUtil_updateDhcpdCfg()
{
    FILE *fp = NULL;
    char dhcpdStartIp[BUFLEN_18], dhcpdEndIp[BUFLEN_18], dhcpdDefaultLease[BUFLEN_16], dhcpdMaxLease[BUFLEN_16], 
         lan_ip[BUFLEN_18], lan_mask[BUFLEN_18], lan_subnet[BUFLEN_18], buf[BUFLEN_512];

    fp = fopen ( PATHDHCPDCONF, "w" );

    if ( !fp )
    {
        tcmLog_error ( "open %s error!", PATHDHCPDCONF );
        return TCMRET_INTERNAL_ERROR;
    }

    if ( config_api_read_parameter ( ID_DHCPD , PARA_DHCPD_STARTIP, dhcpdStartIp ) 
        || config_api_read_parameter(ID_DHCPD, PARA_DHCPD_ENDIP , dhcpdEndIp)
        || config_api_read_parameter(ID_DHCPD, PARA_DHCPD_MAXLEASE, dhcpdMaxLease)
        || config_api_read_parameter(ID_DHCPD, PARA_DHCPD_DEFAULTLEASE, dhcpdDefaultLease)
        || config_api_read_parameter(ID_LAN, PARA_LAN_IP, lan_ip)
        || config_api_read_parameter(ID_LAN, PARA_LAN_MASK, lan_mask))
    {
        tcmLog_error( "Fail to read the dhcpd configuration");
        fclose ( fp );
        return TCMRET_CONFLIB_READ_ERROR;
    }

    if ( !tcm_getIfSubnet (lan_ip, lan_mask, lan_subnet))
    {
        tcmLog_error( "Fail to get the lan subnet" );
        fclose ( fp );
        return TCMRET_INTERNAL_ERROR;
    }
    
    sprintf ( buf, "authoritative;\n" );
    fputs ( buf, fp );

    sprintf ( buf, "default-lease-time\t%s;\n", dhcpdDefaultLease );
    fputs ( buf, fp );

    sprintf ( buf, "max-lease-time\t%s;\n", dhcpdMaxLease );
    fputs ( buf, fp );

    sprintf ( buf, "subnet %s netmask %s {\n", lan_subnet, lan_mask );
    fputs ( buf, fp );

    sprintf ( buf, "\trange %s %s;\n", dhcpdStartIp, dhcpdEndIp );
    fputs ( buf, fp );

    sprintf ( buf, "\toption domain-name-servers %s;\n", lan_ip );
    fputs ( buf, fp );

    sprintf ( buf, "\toption routers %s;\n", lan_ip );
    fputs ( buf, fp );

    sprintf ( buf, "}\n" );
    fputs ( buf, fp );
    fclose ( fp );

    return TCMRET_SUCCESS;
}


tcmRet actUtil_updateResolveCfg()
{
    FILE *fp = NULL;
    char dns1[BUFLEN_18], dns2[BUFLEN_18], dnsdomain[BUFLEN_64], buf[BUFLEN_512];
    memset(dns1, 0, sizeof(dns1));
    memset(dns2, 0, sizeof(dns2));
    memset(dnsdomain, 0, sizeof(dnsdomain));

    fp = fopen ( PATHRESOLVEFILE, "w" );

    if ( !fp )
    {
        tcmLog_error ( "open %s error!", PATHRESOLVEFILE );
        return TCMRET_INTERNAL_ERROR;
    }

    if ( config_api_read_parameter ( ID_DNSMASQ, PARA_DNSMASQ_DNS1, dns1 ) 
        || config_api_read_parameter(ID_DNSMASQ, PARA_DNSMASQ_DNS2, dns2 )
        || config_api_read_parameter(ID_DNSMASQ, PARA_DNSMASQ_DOMAIN, dnsdomain ) )
    {
        tcmLog_error( "Fail to read the dnsmasq configuration");
        fclose ( fp );
        return TCMRET_CONFLIB_READ_ERROR;
    }
    
    sprintf ( buf, "nameserver %s\n", dns1 );
    fputs ( buf, fp );

    if ( strlen(dns2) > 0 )
    {
        sprintf ( buf, "nameserver %s\n", dns2 );
        fputs ( buf, fp );
    }

    sprintf ( buf, "search %s", dnsdomain );
    fputs ( buf, fp );
    
    fclose ( fp );

    return TCMRET_SUCCESS;
}


