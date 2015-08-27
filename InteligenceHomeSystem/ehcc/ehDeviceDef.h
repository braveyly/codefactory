/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************
* Filename:    ehDeviceDef.h
* Author:      Nick
* Date:        2010-12-20
* Description: Define device struct.
*
* Histroy:
    1.  Revision:  0.1
  Modification:First draft
        Author:Nick
        Date:2010-12-20
***********************************************************************/
#ifndef _EH_DEVICE_DEFINE_H
#define _EH_DEVICE_DEFINE_H
#include <time.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "ehSslSocket.h"
#include "configact.h"
#include "ehMutex.h"
#include "ehDebug.h"

#define LEN_OF_MAC               18
#define MAX_LEN_IP               16
#define MAX_LEN_DEVICE_DESC      12
#define LEN_OF_USERNAME          10
#define LEN_OF_PASSWORD          10

#define ZERO_FD                  -1
#define ZERO_SSL                 NULL
/*Follow is for msg*/
#define NOT_CHANGE_RESERVE       -1
#define NOT_CHANGE_VALUELEN      -1
#define NOT_CHANGE_VALUE         -1

#define HOME_MSG_PROTOCOL_VERSION 0001

#define KEEP_ALIVE_POLLING_TIME   2
#define KEEP_ALIVE_CHECKING_TIME   10

typedef struct
{
    char ip[MAX_LEN_IP];
    char mac[LEN_OF_MAC];
    int type;
}tcmDhcpData;

typedef struct
{
    uint16_t protocolVersion;
    uint16_t payloadLen;
    uint8_t  reserve[16];
    /*Overhead is the message header*/
    uint16_t eventType;
    uint16_t valueLen;
}homeMessage;

typedef struct
{
    /*Almost it is unique in message queue, 
      its value range is from 0 to 4294967295*/
    uint32_t sequenceID;        
    struct timeval tmPoint;     /*Record the time for creating message*/
    homeMessage msg;
    int connfd;
    SSL* ssl;  /*modify create msg function*/
    struct homeMsgCtrlBlock* next;
}homeMsgCtrlBlock;

typedef struct
{
    char mac[LEN_OF_MAC];
    char ip[MAX_LEN_IP];
    DEVICETYPE  type;
    DEVICESUBTYPE subType;
    DEVICESTATUS  status;
    char desc[MAX_LEN_DEVICE_DESC];
    char userName[LEN_OF_USERNAME];
    char passWord[LEN_OF_PASSWORD];
    int  pairFailedTimes;
    int  connfd;
    SSL* ssl;  /**modify create device function**/
    struct EHOMEDEVICE *next;
}DeviceInfo;


typedef struct
{
    int  connfd;
    int  flag;
}KeepAliveInfo;

KeepAliveInfo g_keepAliveInfo[MAXTABLE_DEVICES];

typedef struct
{
    int connfd;
    SSL* ssl;
}DeviceConnInfo;

time_t g_cardStartAddTime;


typedef enum
{
    DOUT_SET_STATUS_ON          =   0x01,
    DOUT_SET_STATUS_OFF         =   0x02,
}DOSTATUS;

typedef enum
{
    DOUT_SET_STATUS             =   0x01,
    DOUT_SET_NAME               =   0x02,
    DOUT_SET_BOTH               =   0x03,
}DOTYPE;

typedef struct
{
    uint8_t devID;
    char    devName[MAX_VALUE_LEN+1];
    char    devNumber[MAX_VALUE_LEN+1];
    uint8_t devSubType;
}deviceStruct;

#ifdef AUTO_CALL_TEST
typedef struct
{
   char caller[MAX_VALUE_LEN+1];
   char callee[MAX_VALUE_LEN+1];
   int intervalTime;  //for ODP
   int duringTime;      // for IDP
   char stopper[MAX_VALUE_LEN+1];
}CallTest;
#endif

#endif
