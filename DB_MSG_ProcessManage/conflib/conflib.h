/**********************************************************************
 Copyright (c) 2009 Wuhan Tecom Co., Ltd.
 All Rights Reserved
 No portions of this material may be reproduced in any form without the
 written permission of:
   Wuhan Tecom Co., Ltd.
   #18, Huaguang Road
   Wuhan, PR China 430074
 All information contained in this document is Wuhan Tecom Co., Ltd.
 company private, proprietary, and trade secret.
***********************************************************************
 Filename:    conflib.h
 Author:      Mark Yan
 Date:        2009-01-19
 Description:

 Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/

#ifndef __CONFLIB__H
#define __CONFLIB__H

#include "tcm.h"
#include "tcm_eid.h"
#define      MAX_NAME_LEN   240 /* the MAX length of parameter name */
#define      MAX_VALUE_LEN  640 /* the MAX length of parameter value */

/* define the ID of applications */
#define TCM_INVALID_FD  (-1)


typedef enum{
    PARA_LAN_IP = 1,
    PARA_LAN_MASK,
    PARA_LAN_END,
}lan_para_id;

typedef enum{
    PARA_DHCPD_STATUS = 1,
    PARA_DHCPD_STARTIP,
    PARA_DHCPD_ENDIP,
    PARA_DHCPD_DEFAULTLEASE,
    PARA_DHCPD_MAXLEASE,
    PARA_DHCPD_END,
}dhcpd_para_id;

typedef enum{
    PARA_WORK_MODE = 1,
    PARA_WAN_MODE,
    PARA_WAN_IP,
    PARA_WAN_NETMASK,
    PARA_WAN_GATEWAY,
    PARA_WAN_PDNS,
    PARA_WAN_SDNS,
    PARA_WAN_MTU,
    PARA_WAN_BRIDGEIP,
    PARA_WAN_BRIDGEMASKBIT,
    PARA_WAN_VLANID,
    PARA_WAN_VLANSKBPRIO,
    PARA_WAN_VLANPRIO,
    PARA_WAN_END,
}wan_para_id;

typedef enum{
    PARA_DNSMASQ_DNS1 = 1,
    PARA_DNSMASQ_DNS2,
    PARA_DNSMASQ_DOMAIN,
    PARA_DNSMASQ_END,
}dnsmasq_para_id;

typedef enum{
    PARA_SNTP_NTPSERVER1 = 1,
    PARA_SNTP_NTPSERVER2,
    PARA_SNTP_NTPSERVER3,
    PARA_SNTP_NTPSERVER4,
    PARA_SNTP_NTPSERVER5,
    PARA_SNTP_TIMEZONEINDEX,
    PARA_SNTP_END,
}sntp_para_id;

typedef enum{
    PARA_DMZ_IP = 1,
    PARA_DMZ_END,
}dmz_para_id;

/* This is the only entry table for Conflib. */
typedef enum{
    PARA_PORTFORWARD_NAME = 1,
    PARA_PORTFORWARD_PROTOCOL,
    PARA_PORTFORWARD_START_INCOMING_PORT,
    PARA_PORTFORWARD_END_INCOMING_PORT,
    PARA_PORTFORWARD_START_FORWARD_PORT,
    PARA_PORTFORWARD_END_FORWARD_PORT,
    PARA_PORTFORWARD_SERVER_IP, 
    PARA_PORTFORWARD_END,
}portforward_para_id;

typedef enum{
    PARA_FIREWALL_SRCIP = 1,
    PARA_FIREWALL_SRCMASK,
    PARA_FIREWALL_SRCPORT,
    PARA_FIREWALL_DSTIP,
    PARA_FIREWALL_DSTMASK,
    PARA_FIREWALL_DSTPORT,
    PARA_FIREWALL_PROTOCOL, 
    PARA_FIREWALL_END,
}firewall_para_id;

typedef enum{
    PARA_ROUTE_DSTIP = 1,
    PARA_ROUTE_MASK,
    PARA_ROUTE_NEXTHOPTYPE,
    PARA_ROUTE_NEXTHOPTYPEINTERFACE,
    PARA_ROUTE_NEXTHOPTYPEIP,
    PARA_ROUTE_METRIC,
    PARA_ROUTE_END,
}staticroute_para_id;

/* the following APIs provided by conflib for other application */
extern int    config_api_init_library ( void );
extern void   config_api_uninit_library ( void );
extern int    config_api_read_parameter ( int appindex, int paraindex, char *result );
extern int    config_api_write_parameter ( int appindex, int paraindex, char *val );
extern int    config_api_read_table_parameter ( int appindex, int rowindex, int paraindex, char *result );
extern int    config_api_read_table_row ( int appindex, int rowindex, char **val );
extern int    config_api_write_table_parameter ( int appindex, int rowindex, int paraindex, char *val );
extern int    config_api_add_table_row ( int appindex, int rowindex, char **val );
extern int    config_api_delete_table_row ( int appindex, int rowindex );
extern int    config_api_write_to_flash ( void );

#endif
