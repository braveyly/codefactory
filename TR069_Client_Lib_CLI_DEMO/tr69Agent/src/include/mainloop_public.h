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
Filename: mainloop_public.h
Author: Gavin
Date: 2009-05-19
Description:

Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#ifndef _TECOM_MAINLOOP_PUBLIC_H_
#define _TECOM_MAINLOOP_PUBLIC_H_

#include "pal.h"
#include "tr69_agent_public.h"

#define MAX_NUMBER_LENGTH 32
#define MAX_RETRY_TIMES 11
#define MAX_FILE_NAME_LENGTH 64
#define MAX_PARAMETER_LENGTH 256
#define REQUIRED_PARAM_NUMBER 8
#define DEFAULT_CONNECTION_SERVICE  "InternetGatewayDevice.Layer3Forwarding.DefaultConnectionService"
#define DEFAULT_CONNECTION_NAME "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress"
#define ZERO_DATE_TIME "0000-00-00T00:00:00"
#define  DEVICEID_MANUFACTURER_NAME "InternetGatewayDevice.DeviceInfo.Manufacturer"
#define  DEVICEID_OUI_NAME "InternetGatewayDevice.DeviceInfo.ManufacturerOUI"
#define  DEVICEID_PRODUCTCLASS_NAME "InternetGatewayDevice.DeviceInfo.ProductClass"
#define  DEVICEID_SERIALNUMBER_NAME "InternetGatewayDevice.DeviceInfo.SerialNumber"

#define WAN_IPCONNECTION_FLAG_NAME "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.Enable"
#define WAN_PPPCONNECTION_FLAG_NAME "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.1.Enable"

#define MAX_RECV_ENVELOPES 1  /* currently we only accept 1 envelope in the rpc command packet */
extern PAL_UINT32_T gMaxSendEnvelopes;
extern char gMRebootCommandKey[];/* before main session started, if exist MReboot, copy commandkey to this variable*/
extern char gScheduleCommandKey[];/* when schedule inform is triggered, copy the commandkey to this variable*/
extern char gTransferCommandKey[];/* check transfer compelete directory, get lastest commandkey, copy to this variable*/
extern PAL_UINT32_T gRetryTimes; /* for inform packet */
extern PAL_UINT32_T gHoldRequest;/* received acs information */
extern PAL_UINT32_T gNoMoreRequests; /* received acs information */
extern char gRequiredInformParams[][MAX_PARAMETER_LENGTH]; /* for inform, required inform parameter names */

#define EVENTCODE_BOOTSTAP "0 BOOTSTRAP"
#define EVENTCODE_BOOT "1 BOOT"
#define EVENTCODE_PERIODIC "2 PERIODIC"
#define EVENTCODE_SCHEDULE "3 SCHEDULED"
#define EVENTCODE_VALCHANGE "4 VALUE CHANGE"
#define EVENTCODE_KICKED "5 KICKED"
#define EVENTCODE_CONNECTIONREQ "6 CONNECTION REQUEST"
#define EVENTCODE_TRANSFER "7 TRANSFER COMPLETE"
#define EVENTCODE_DIAGNOSITICS "8 DIAGNOSTICS COMPLETE"
#define EVENTCODE_REQUEST_DOWNLOAD "9 REQUEST DOWNLOAD"
#define EVENTCODE_MREBOOT "M Reboot"


/* for management server parameters  */
#define MAX_USERNAME_LENGTH 128
#define MAX_PASSWORD_LENGTH 128
#define MAX_NAME_LENGTH  256
#define IP_LENGTH 16
#define MAX_KEY_LENGTH 32
#define TIME_LENGTH 24

typedef struct _ManagementServer
{
    char acsURL[MAX_USERNAME_LENGTH+1];
    char acsUsername[MAX_USERNAME_LENGTH+1];
    char acsPassword[MAX_USERNAME_LENGTH+1];
    PAL_BOOL_T  periodicInformEnable;
    PAL_UINT32_T  periodicInformInterval;
    char periodicInformTime[TIME_LENGTH+1];
    char parameterKey[MAX_KEY_LENGTH+1];
    char connectionRequestURL[MAX_USERNAME_LENGTH+1];
    char connectionRequestUsername[MAX_USERNAME_LENGTH+1];
    char connectionRequestPassword[MAX_USERNAME_LENGTH+1];
    PAL_UINT32_T upgradesManaged;

//#ifdef TR111   /* 2009-05-26 15:31:57 */
    char udpConnectionRequestAddress[MAX_USERNAME_LENGTH+1];
    char stunServerAddress[MAX_NAME_LENGTH];
    char stunUsername[MAX_USERNAME_LENGTH+1];
    char stunPassword[MAX_PASSWORD_LENGTH+1];
    PAL_UINT32_T udpCRANotificationLimit;/*UDPConnectionRequestAddressNotificationLimit;  */
    PAL_BOOL_T stunEnable;
    PAL_UINT32_T stunServerPort;
    PAL_UINT32_T stunMaxKeepAlivePeriod;
    PAL_UINT32_T  stunMinKeepAlivePeriod;
    PAL_BOOL_T natDetected;
//#endif/* #ifdef TR111 */
} ManagementServer;

typedef enum _QueuedTransferType
{
    tNotStarted = 1,
    tInProgress = 2,
    tCompleted = 3
}QueuedTransferType;

extern ManagementServer gManagementServer;/* ACS Management related information */
extern char gRPCMethodList[][32];

/***********************************************************************
Function: main_session
Description:  The main session Entry function, normally, it will not return and will run circulately
Histroy:

************************************************************************/
extern Result main_session();

/***********************************************************************
Function: init_main_session_sem
Description:  initialize the main session sem for block and wake up actions
************************************************************************/
extern void init_main_session_sem ();

/***********************************************************************
Function: store_reboot_event
Description:  store reboot event to file
************************************************************************/
extern void store_reboot_event ( IN char *cmdKey );
extern void store_nobootstrap_event ();

extern Result check_tr069_main_loop_status();

extern unsigned int need_inform(char *name);
/***********************************************************************
Function: mainloop_wakeup
Description:  Wake up the main session
************************************************************************/
extern void mainloop_wakeup ();
#endif  /*#ifndef _TECOM_MAINLOOP_PUBLIC_H_*/
