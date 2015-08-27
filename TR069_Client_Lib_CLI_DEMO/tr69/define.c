/*This file is used by tcmAgent application and tcmAgentApi.so*/
#ifndef _DEFINE_C_
#define _DEFINE_C_

#include <stddef.h> /* for offsetof*/
#include "tr69_agent.h"
#include "os_defs.h"
#include "tcm_eid.h"
#include "conflib.h"


enum
{
    TCMAGENT_CONFIGURATION,
    TCMAGENT_STATUS,
};

enum
{
    TCMAGENT_INT= MSG_INT,
    TCMAGENT_UNSIGNED = MSG_UINT,
    TCMAGENT_BOOL = MSG_BOOL,
    TCMAGENT_STRING = MSG_STRING, 
};

#define MAX_NUM_OF_LANINTERFACE 32
#define MAX_NUM_OF_FORWARD 32
#define MAX_NUM_OF_PORTFORWARD_INSTANCE 32
#define MAX_NUM_OF_RULE_INSTANCE 32
#define MAX_NUM_OF_ROUTE_INSTANCE 32

/* For TCMAGENT_STATUS parameters*/
extern ApplyStatus get_device_info ( char *name, char *retValue );
extern ApplyStatus get_time ( char *name, char *retValue );
extern ApplyStatus get_ipPingDiagnostics ( char *name, char *retValue );
extern ApplyStatus get_forwarding ( char *name, char *retValue, int index );

/*used in tcmAgentApi.so*/
#ifdef TR069_NOT_DEFINE
ApplyStatus get_device_info ( char *name, char *retValue )
{
    return NOT_APPLIED;
}

ApplyStatus get_time ( char *name, char *retValue )
{
    return NOT_APPLIED;
}

ApplyStatus get_ipPingDiagnostics ( char *name, char *retValue )
{
    return NOT_APPLIED;
}

ApplyStatus get_forwarding ( char *name, char *retValue, int index )
{
    return NOT_APPLIED;
}
#endif

/*Global variables for saving parameter values*/

/**********************For password convert**********************/
#define PSW_Len 16
char strPSWOffset[PSW_Len+1] = "Y2010-TECOM:WiMA";

void convertPSW ( char cPassword, int* cvrtPSW, int k )
{

   if ( k >= 0 && k < 16 )
   {
      if ( cPassword != ( char ) 0 )
         *cvrtPSW = ( int ) cPassword + ( int ) strPSWOffset[k];
      else
         *cvrtPSW = 0 + ( int ) strPSWOffset[k];

   }

   if ( k >= 16 && k < PSW_Len )
   {
      if ( cPassword != ( char ) 0 )
         *cvrtPSW = ( int ) cPassword + ( int ) '7';
      else
         *cvrtPSW = 0 + ( int ) '7';
   }
}

void revertPSW ( char* cPassword, char* rvrtPSW )
{
   int i = 0;
   int tVal;
   char tBuf[3*PSW_Len+1];
   char tmpBuf[3*PSW_Len+1];

   strcpy ( tmpBuf, cPassword );

   for ( i = 0; i < 16; i++ )
   {
      if ( strcmp ( tmpBuf, "" ) )
      {
         strcpy ( tBuf, tmpBuf );
         sscanf ( tBuf, "%X:%s", &tVal, tmpBuf );
         rvrtPSW[i] = ( char ) ( tVal - ( int ) strPSWOffset[i] );
      }
   }

   for ( i = 16; i < PSW_Len; i++ )
   {
      if ( strcmp ( tmpBuf, "" ) )
      {
         strcpy ( tBuf, tmpBuf );
         sscanf ( tBuf, "%X:%s", &tVal, tmpBuf );
         rvrtPSW[i] = ( char ) ( tVal - ( int ) '7' );
      }
   }
}
/**************************************************************/

/*
#define MAX_LEN_OF_NAME  (MAX_NAME_LEN+100)
#define MAX_LEN_OF_VALUE (MAX_VALUE_LEN+100)
*/

#define MAX_LEN_OF_NAME  320
#define MAX_LEN_OF_VALUE 640
typedef struct 
{
    int conflibAppID;
    int conflibRowID;
    int conflibParID;
    char tr069Name[MAX_LEN_OF_NAME];
    int type;
    int status;
    void *baseAddress;
    int maxNumInstance;
    int min;
    int max;
    char* defaultValue;
} ConflibTR069MatchParameter;


#define FLAG_INSTANCE_ID 0
#define FLAG_ADD_DEFAULT_OBJECT -2
#define FLAG_NO_ROW -1
#define FLAG_STATUS_STYLE  -1
#define FLAG_TEMPLATE_ROW 0

ConflibTR069MatchParameter gMatchParameters[] =
{
    { FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, "InternetGatewayDevice.LANDevice.1.", 0, 0, 0, 0, 0, 0, NULL }, 
    { FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1.", 0, 0, 0, 0, 0, 0, NULL }, 
    { FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, "InternetGatewayDevice.WANDevice.1.", 0, 0, 0, 0, 0, 0, NULL }, 
    { FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.", 0, 0, 0, 0, 0, 0, NULL }, 
    { FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.", 0, 0, 0, 0, 0, 0, NULL }, 
    { FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.", 0, 0, 0, 0, 0, 0, NULL }, 
    { FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, FLAG_ADD_DEFAULT_OBJECT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.", 0, 0, 0, 0, 0, 0, NULL }, 

	
    { ID_TR069, FLAG_NO_ROW, PARA_TR069_URL, "InternetGatewayDevice.ManagementServer.URL", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 64, "http://172.16.28.8:8080/dps/TR069"  },
    { ID_TR069, FLAG_NO_ROW, PARA_TR069_USERNAME, "InternetGatewayDevice.ManagementServer.Username", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "tr069"  },
    { ID_TR069, FLAG_NO_ROW, PARA_TR069_PASSWORD, "InternetGatewayDevice.ManagementServer.Password", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "tr069"  },
    { ID_TR069, FLAG_NO_ROW, PARA_TR069_INFORMSTATUS, "InternetGatewayDevice.ManagementServer.PeriodicInformEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_TR069, FLAG_NO_ROW, PARA_TR069_INFORMINTERVAL, "InternetGatewayDevice.ManagementServer.PeriodicInformInterval", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 65535, "30"  },
    

    { ID_LAN, FLAG_NO_ROW, PARA_LAN_IP, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1.IPInterfaceIPAddress", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 15, "192.168.0.1" },
    { ID_LAN, FLAG_NO_ROW, PARA_LAN_MASK, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1.IPInterfaceSubnetMask", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 15, "255.255.255.0" }, 
    
    { ID_SNTP, FLAG_NO_ROW, PARA_SNTP_NTPSERVER1, "InternetGatewayDevice.Time.NTPServer1", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "clock.fmt.he.net"  },
    { ID_SNTP, FLAG_NO_ROW, PARA_SNTP_NTPSERVER2, "InternetGatewayDevice.Time.NTPServer2", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "clock.nyc.he.net"  },
    { ID_SNTP, FLAG_NO_ROW, PARA_SNTP_NTPSERVER3, "InternetGatewayDevice.Time.NTPServer3", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "clock.sjc.he.net"  },
    { ID_SNTP, FLAG_NO_ROW, PARA_SNTP_NTPSERVER4, "InternetGatewayDevice.Time.NTPServer4", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "clock.via.net"  },
    { ID_SNTP, FLAG_NO_ROW, PARA_SNTP_NTPSERVER5, "InternetGatewayDevice.Time.NTPServer5", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "ntp1.tummy.com"  },
    { ID_SNTP, FLAG_NO_ROW, PARA_SNTP_TIMEZONEINDEX, "InternetGatewayDevice.Time.LocalTimeZoneIndex", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 63, "1"  },
    { ID_SNTP, FLAG_NO_ROW, FLAG_STATUS_STYLE, "InternetGatewayDevice.Time.CurrentLocalTime", TCMAGENT_STRING, TCMAGENT_STATUS,  get_time, 0, 0, 256, ""  },

    { ID_DHCPD, FLAG_NO_ROW, PARA_DHCPD_STATUS, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.DHCPServerEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_DHCPD, FLAG_NO_ROW, PARA_DHCPD_STARTIP, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.MinAddress", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 15, "192.168.0.2"  },
    { ID_DHCPD, FLAG_NO_ROW, PARA_DHCPD_ENDIP, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.MaxAddress", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 15, "192.168.0.250"  },
    { ID_DHCPD, FLAG_NO_ROW, PARA_DHCPD_DEFAULTLEASE, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.DHCPLeaseTime", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 65535, "600"  },
    { ID_DHCPD, FLAG_NO_ROW, PARA_DHCPD_MAXLEASE, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.PassthroughLease", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 65535, "7200"  },
    
    { ID_DMZ, FLAG_NO_ROW, PARA_DMZ_IP, "InternetGatewayDevice.X_GCT_SecDmzHostCfg", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 15, "198.16.12.11" },

    { ID_UPNPD, FLAG_NO_ROW, PARA_UPNPD_STATUS, "InternetGatewayDevice.X_GCT_AppCfg.UpnpCfg.Status", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },

    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_HTTP_LAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.HttpLanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_HTTP_WAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.HttpWanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_FTP_LAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.FtpLanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_FTP_WAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.FtpWanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_ICMP_LAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.IcmpLanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_ICMP_WAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.IcmpWanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_SNMP_LAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.SnmpLanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_SNMP_WAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.SnmpWanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_SSH_LAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.SshLanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_SSH_WAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.SshWanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_TELNET_LAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.TelnetLanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_TELNET_WAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.TelnetWanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_TFTP_LAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.TftpLanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },
    { ID_SERVICE, FLAG_NO_ROW, PARA_SERVICE_TFTP_WAN, "InternetGatewayDevice.X_GCT_AccessServiceCfg.APPStatus.TftpWanEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },

    { ID_ACCESSMODE, FLAG_NO_ROW, PARA_ACCESSMODE_MODE, "InternetGatewayDevice.X_GCT_AccessServiceCfg.AccessMode", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1"  },


    { ID_WAN, FLAG_NO_ROW, PARA_WORK_MODE, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ConnectionType", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "nat"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_MODE, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.AddressingType", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "dhcp"  },
    { ID_WAN, FLAG_NO_ROW, PARA_NET_MODE, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.NetMode", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "Ethernet"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_IP, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 16, "192.168.2.188"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_NETMASK, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.SubnetMask", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 16, "255.255.255.0"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_GATEWAY, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.DefaultGateway", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 16, "192.168.2.1"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_PDNS, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.DNSServers", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 16, "168.95.1.1"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_SDNS, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_SecondDNSServer", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 16, "168.95.192.1"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_MTU, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.MaxMTUSize", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 65535, "1500"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_BRIDGEIP, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_BridgeIP", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 16, "192.168.0.2"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_BRIDGEMASKBIT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_BridgeMaskBit", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 32, "24"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_VLANID, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_VLanID", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 4094, "1000"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_VLANSKBPRIO, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_VLanSKBPriority", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 65535, "0"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_VLANPRIO, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_VLanPriority", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 7, "4"  },
    { ID_WAN, FLAG_NO_ROW, PARA_WAN_DSCP, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_Dscp", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 63, "0"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_STATUS, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "0"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_MODE, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceAddressingType", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 256, "dhcp"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_IP, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceExternalIPAddress", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 256, "192.168.2.189"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_NETMASK, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceSubnetMask", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 256, "255.255.255.0"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_GATEWAY, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceDefaultGateway", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 1, 256, "192.168.2.1"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_VLANID, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceVLanID", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 4094, "1000"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_VLANSKBPRIO, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceVLanSKBPriority", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 65535, "0"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_VLANPRIO, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceVLanPriority", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 7, "4"  },
    { ID_WAN, FLAG_NO_ROW, PARA_MNG_DSCP, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.X_GCT_ManageInterfaceDscp", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 63, "0"  },

    { ID_DNSMASQ, FLAG_NO_ROW, PARA_DNSMASQ_DNS1, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.DNSServers", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 16, "192.168.20.2"  },
    { ID_DNSMASQ, FLAG_NO_ROW, PARA_DNSMASQ_DNS2, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.X_GCT_SecondDNSServer", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 16, "192.168.30.2"  },
    { ID_DNSMASQ, FLAG_NO_ROW, PARA_DNSMASQ_DOMAIN, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.DomainName", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "hinet.com.tw"  },

    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_DRIVER, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.Driver", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "/usr/wimax -D" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_SCAN_INTERVAL, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.ScanInterval", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 1000, "3" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_PKM, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.PKMEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_EAP_METHOD, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.Type", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "EAP-TTLS/MSCHAPV2" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_EAP_FRAGMENT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.EAPFragment", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 1400, "1300" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_ALPHA_DELIM, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.AlphaDelimEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_PRIVATE_KEY_PASSWORD, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.KeyPassword", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "whatever" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_IDENTITY, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.Identity", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "286810340034" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_PASSWORD, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.SharedSecret", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "1234" }, 
    { ID_SDKD, FLAG_NO_ROW, PARA_SDKD_ANONYMOUS_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.EAPProfile.1.Realm", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "{sm=2}mac@vmax.net.tw" }, 

    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_WM_AUTO_CON, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.WMAutoConEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_NSP_FIND_WT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.NSPFindWT", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 3600, "5" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_CON_POLICY,  "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.ConPolicy", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "BEST" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_SELECT_POLICY,  "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.SelectPolicy", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "ALL" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_FAIL_DELAY_TIME, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.FailDelayTime", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 3600, "2" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_MAX_FAIL_CNT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.MaxFailCnt", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 100, "3" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_MAX_FAIL_DELAY_TIME, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.MaxFailDelayTime", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 3600, "30" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_DHCPCD_AUTO_START, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.DhcpdAutoStartEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_MNG_DHCPCD_AUTO_START, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.ManagementDhcpdAutoStartEnable", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_IP_ALLOC_WT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.IPAllocWT", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 3600, "7" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_GW_PING_CHECK, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.GWPingCheck", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "0" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_PING_WT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.PingWT", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 3600, "3" }, 
    { ID_AUTOCM, FLAG_NO_ROW, PARA_AUTOCM_PING_R_CNT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.SubscriptionParameters.AutoConnection.PingReplyCnt", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 1, 3600, "3" }, 

    { ID_DDNS, FLAG_NO_ROW, PARA_DDNS_PROVIDER,  "InternetGatewayDevice.X_GCT_AppCfg.DDNS.Provider", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "dyndns.org" }, 
    { ID_DDNS, FLAG_NO_ROW, PARA_DDNS_HOSTNAME,  "InternetGatewayDevice.X_GCT_AppCfg.DDNS.HostName", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "wm5130.dyndns.org" }, 
    { ID_DDNS, FLAG_NO_ROW, PARA_DDNS_USERNAME,  "InternetGatewayDevice.X_GCT_AppCfg.DDNS.UserName", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "wimax" }, 
    { ID_DDNS, FLAG_NO_ROW, PARA_DDNS_PASSWORD,  "InternetGatewayDevice.X_GCT_AppCfg.DDNS.Password", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "wimax" }, 
    { ID_DDNS, FLAG_NO_ROW, PARA_DDNS_UPDATEPERIOD,  "InternetGatewayDevice.X_GCT_AppCfg.DDNS.UpdatePeriod", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 2419200, "3600" }, 

    { ID_LOG, FLAG_NO_ROW, PARA_LOG_STATUS, "InternetGatewayDevice.X_GCT_AppCfg.Log.Status", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1" }, 
    { ID_LOG, FLAG_NO_ROW, PARA_LOG_LEVEL,  "InternetGatewayDevice.X_GCT_AppCfg.Log.Level", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "Emergency" }, 
    { ID_LOG, FLAG_NO_ROW, PARA_LOG_MODE,  "InternetGatewayDevice.X_GCT_AppCfg.Log.Mode", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "Local" }, 
    { ID_LOG, FLAG_NO_ROW, PARA_LOG_SERVER_IP,  "InternetGatewayDevice.X_GCT_AppCfg.Log.ServerIP", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, 0, 0, 256, "192.168.0.100" }, 
    { ID_LOG, FLAG_NO_ROW, PARA_LOG_SERVER_PORT,  "InternetGatewayDevice.X_GCT_AppCfg.Log.ServerPort", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, 0, 0, 65535, "514" }, 

    { ID_IGMPPROXY, FLAG_NO_ROW, PARA_IGMP_STATUS, "InternetGatewayDevice.X_GCT_AppCfg.IGMP.Status", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, 0, 0, 1, "1" }, 

    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 65535, "" },
    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, PARA_PORTFORWARD_NAME, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.X_GCT_Name", TCMAGENT_STRING, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 32, "defaultportmap" }, 
    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, PARA_PORTFORWARD_PROTOCOL, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.PortMappingProtocol", TCMAGENT_STRING, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 16, "TCP" }, 
    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, PARA_PORTFORWARD_START_INCOMING_PORT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.ExternalPort", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 65535, "12" }, 
    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, PARA_PORTFORWARD_END_INCOMING_PORT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.X_GCT_ExternalEndPort", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,   0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 65535, "24" }, 
    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, PARA_PORTFORWARD_START_FORWARD_PORT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.InternalPort", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 65535, "36" },
    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, PARA_PORTFORWARD_END_FORWARD_PORT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.X_GCT_InternalEndPort", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 65535, "48" },
    { ID_PORTFORWARD, FLAG_TEMPLATE_ROW, PARA_PORTFORWARD_SERVER_IP, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.RemoteHost", TCMAGENT_STRING, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 16, "192.168.12.41" },     


    { ID_ACCOUNT, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.X_GCT_AppCfg.Account.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "" },
    { ID_ACCOUNT, FLAG_TEMPLATE_ROW, PARA_ACCOUNT_CNAME, "InternetGatewayDevice.X_GCT_AppCfg.Account.0.CName", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 265, "admin" }, 
    { ID_ACCOUNT, FLAG_TEMPLATE_ROW, PARA_ACCOUNT_CPASSWORD, "InternetGatewayDevice.X_GCT_AppCfg.Account.0.CPassword", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 265, "BA:96:9D:9A:9E:2D:54:45:43:4F:4D:3A:57:69:4D:41:" }, 
    { ID_ACCOUNT, FLAG_TEMPLATE_ROW, PARA_ACCOUNT_CUID, "InternetGatewayDevice.X_GCT_AppCfg.Account.0.CUID", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 265, "A100" }, 
    { ID_ACCOUNT, FLAG_TEMPLATE_ROW, PARA_ACCOUNT_CGID, "InternetGatewayDevice.X_GCT_AppCfg.Account.0.CGID", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 265, "AID" }, 


    { ID_FIREWALL, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.X_GCT_FireWallRule.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "" },
    { ID_FIREWALL, FLAG_TEMPLATE_ROW, PARA_FIREWALL_SRCIP, "InternetGatewayDevice.X_GCT_FireWallRule.0.SrcIp", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "172.16.12.48" }, 
    { ID_FIREWALL, FLAG_TEMPLATE_ROW, PARA_FIREWALL_SRCMASK, "InternetGatewayDevice.X_GCT_FireWallRule.0.SrcMaskBit", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "25" }, 
    { ID_FIREWALL, FLAG_TEMPLATE_ROW, PARA_FIREWALL_SRCPORT, "InternetGatewayDevice.X_GCT_FireWallRule.0.SrcPort", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "24" }, 
    { ID_FIREWALL, FLAG_TEMPLATE_ROW, PARA_FIREWALL_DSTIP, "InternetGatewayDevice.X_GCT_FireWallRule.0.DestIp", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "172.16.12.44" }, 
    { ID_FIREWALL, FLAG_TEMPLATE_ROW, PARA_FIREWALL_DSTMASK, "InternetGatewayDevice.X_GCT_FireWallRule.0.DestMaskBit", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "20" }, 
    { ID_FIREWALL, FLAG_TEMPLATE_ROW, PARA_FIREWALL_DSTPORT, "InternetGatewayDevice.X_GCT_FireWallRule.0.DestPort", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "26" }, 
    { ID_FIREWALL, FLAG_TEMPLATE_ROW, PARA_FIREWALL_PROTOCOL, "InternetGatewayDevice.X_GCT_FireWallRule.0.Protocal", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "TCP" }, 


    { ID_ROUTE, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.Layer3Forwarding.Forwarding.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_ROUTE_INSTANCE, 0, 65535, "" },
    { ID_ROUTE, FLAG_TEMPLATE_ROW, PARA_ROUTE_DSTIP, "InternetGatewayDevice.Layer3Forwarding.Forwarding.0.DestIPAddress", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "192.168.0.5" }, 
    { ID_ROUTE, FLAG_TEMPLATE_ROW, PARA_ROUTE_MASK, "InternetGatewayDevice.Layer3Forwarding.Forwarding.0.DestSubnetMask", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "255.255.224.0" }, 
    { ID_ROUTE, FLAG_TEMPLATE_ROW, PARA_ROUTE_NEXTHOPTYPE, "InternetGatewayDevice.Layer3Forwarding.Forwarding.0.X_GCT_NextHopType", TCMAGENT_BOOL, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 1, "1" }, 
    { ID_ROUTE, FLAG_TEMPLATE_ROW, PARA_ROUTE_NEXTHOPTYPEINTERFACE, "InternetGatewayDevice.Layer3Forwarding.Forwarding.0.X_GCT_NextHopIntf", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "LAN" }, 
    { ID_ROUTE, FLAG_TEMPLATE_ROW, PARA_ROUTE_NEXTHOPTYPEIP, "InternetGatewayDevice.Layer3Forwarding.Forwarding.0.X_GCT_NextHopIp", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "192.168.12.40" }, 
    { ID_ROUTE, FLAG_TEMPLATE_ROW, PARA_ROUTE_METRIC, "InternetGatewayDevice.Layer3Forwarding.Forwarding.0.ForwardingMetric", TCMAGENT_INT, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, -1, 65535, "4" }, 

    { ID_IPCONTROL, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.X_GCT_AccessServiceCfg.AccessHostCfg.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_ROUTE_INSTANCE, 0, 65535, "" },
    { ID_IPCONTROL, FLAG_TEMPLATE_ROW, PARA_IPCONTROL_ADDRESS, "InternetGatewayDevice.X_GCT_AccessServiceCfg.AccessHostCfg.0.ControlIp", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "172.16.28.10" }, 
    { ID_IPCONTROL, FLAG_TEMPLATE_ROW, PARA_IPCONTROL_NETMASK, "InternetGatewayDevice.X_GCT_AccessServiceCfg.AccessHostCfg.0.ControlMask", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "255.255.224.0" }, 


    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_ROUTE_INSTANCE, 0, 65535, "" },
    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, PARA_CHANNELPLAN_FREQFIRST, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.0.FirstFrequency", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 256, "2500000" }, 
    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, PARA_CHANNELPLAN_FREQLAST, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.0.LastFrequency", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 256, "2700000" }, 
    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, PARA_CHANNELPLAN_NEXTSTEP, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.0.NextFrequencyStep", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "10000" }, 
    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, PARA_CHANNELPLAN_PREAMBLES, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.0.Preambles", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 29, "1" }, 
    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, PARA_CHANNELPLAN_BANDWIDTH, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.0.Bandwidth", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "10000" }, 
    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, PARA_CHANNELPLAN_FFT, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.0.FFTSize", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "1024" }, 
    { ID_CHANNELPLAN, FLAG_TEMPLATE_ROW, PARA_CHANNELPLAN_DUPLEXMODE, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.ChannelPlan.0.DuplexMode", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "1" }, 

    { ID_HNSP, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.HomeNetworkServiceProvider.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_ROUTE_INSTANCE, 0, 65535, "" },
    { ID_HNSP, FLAG_TEMPLATE_ROW, PARA_HNSP_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.HomeNetworkServiceProvider.0.HNSPIdentifier", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 32, "0xF74809" }, 

    { ID_CAPL, FLAG_NO_ROW, PARA_APP_CAPL_SEL_POLICY, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.CAPL.NetworkSelectionPolicy", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 3, "1" }, 

    { ID_CAPL, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.CAPL.CAP.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_ROUTE_INSTANCE, 0, 65535, "" },
    { ID_CAPL, FLAG_TEMPLATE_ROW, PARA_TAB_CAPL_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.CAPL.CAP.0.NAPIdentifier", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 256, "0xF74809" }, 
    { ID_CAPL, FLAG_TEMPLATE_ROW, PARA_TAB_CAPL_PRIORITY, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.CAPL.CAP.0.NetworkSelectionPriority", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "1" }, 

    { ID_RAPL, FLAG_NO_ROW, PARA_APP_RAPL_SEL_POLICY, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.RAPL.NetworkSelectionPolicy", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_PORTFORWARD_INSTANCE, 0, 3, "1" }, 

    { ID_RAPL, FLAG_TEMPLATE_ROW, FLAG_INSTANCE_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.RAPL.RAP.", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION, 0, MAX_NUM_OF_ROUTE_INSTANCE, 0, 65535, "" },
    { ID_RAPL, FLAG_TEMPLATE_ROW, PARA_TAB_RAPL_ID, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.RAPL.RAP.0.VNSPIdentifier", TCMAGENT_STRING, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 256, "0xF74809" }, 
    { ID_RAPL, FLAG_TEMPLATE_ROW, PARA_TAB_RAPL_PRIORITY, "InternetGatewayDevice.X_WIMAXFORUM_OperatorProfile.1.WiMAXNetworkParameters.RAPL.RAP.0.NetworkSelectionPriority", TCMAGENT_UNSIGNED, TCMAGENT_CONFIGURATION,  0, MAX_NUM_OF_RULE_INSTANCE, 0, 65535, "1" }, 



};

#endif
