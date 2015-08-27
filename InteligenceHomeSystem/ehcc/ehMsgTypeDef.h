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
* Filename:    ehMsgTypeDef.h
* Author:      Nick
* Date:        2010-12-06
* Description: Define message type among IDP, ODP and MFCB.
*
* Histroy:
    1.  Revision:  0.1
  Modification:First draft
        Author:Nick
        Date:2010-12-06

    2.  Revision:  0.2
  Modification:Assign different value ranges for MFCB and IDP message type
        Author:Nick
        Date:2010-12-06
***********************************************************************/
#ifndef _EH_MSG_TYPE_DEFINE_H
#define _EH_MSG_TYPE_DEFINE_H

//#define SSL_CONNECT
//#define AUTO_KEEP_ALIVE   1
typedef struct 
{
   int msgID;
   char* msgStr;
}homeMsgIntToStr;
#define MAX_MSG_NUM 100
#define INVALID_MSG_TYPE_ID -1
typedef enum
{
    /*Following are COMMAND message type with value from 0x1000 to 0x10FF*/
    CMD_IDP_TO_MFCB_REGISTER                    = 0x1000,
    CMD_IDP_TO_MFCB_OPEN_DOOR                   = 0x1001,
    CMD_IDP_TO_MFCB_QUERY_DOOR_STATUS           = 0x1002,
    CMD_IDP_TO_MFCB_QUERY_CARD_INFO             = 0x1003,
    CMD_IDP_TO_MFCB_ADD_CRD_OPERATION           = 0x1004,
    CMD_IDP_TO_MFCB_EDIT_SINGLE_CRD             = 0x1005,
    CMD_IDP_TO_MFCB_UPDATE_ALL_CRD              = 0x1006,
    CMD_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS   = 0x1007,
    CMD_IDP_TO_MFCB_SET_SECURITY_MODE           = 0x1008,
    CMD_IDP_TO_MFCB_QUERY_DETECT_POINT          = 0x1009,
    CMD_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT     = 0x100A,
    CMD_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE    = 0x100B,
    CMD_IDP_TO_MFCB_GET_CURRENT_SECURITY_MODE   = 0x100C,
    CMD_IDP_TO_MFCB_CLEAR_ALARM                 = 0x100D,
    CMD_IDP_TO_MFCB_QUERY_ALARM_HISTORY         = 0x100E,
    CMD_IDP_TO_MFCB_KEEP_ALIVE                  = 0x100F,
    CMD_IDP_TO_MFCB_TOKEN_OPERATION             = 0x1010,
    /*New add*/
    CMD_IDP_TO_MFCB_QUERY_CAM                   = 0x1011,
    CMD_IDP_TO_MFCB_ADD_CAM                     = 0x1012,
    CMD_IDP_TO_MFCB_EDIT_SINGLE_CAM             = 0x1013,
    CMD_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE = 0x1015,
    CMD_IDP_TO_MFCB_DO_CONTROL                  = 0x1016,
    CMD_IDP_TO_MFCB_SYNCHRONIZE_DEVICE          = 0x1017,
    CMD_IDP_TO_MFCB_QUERY_DO                    = 0x1018,
    CMD_IDP_TO_MFCB_RESET_DEFAULT               = 0x1014,
    CMD_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD    = 0x1019,
    CMD_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD     = 0x101A,
    CMD_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO    = 0x101B,
    CMD_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO     = 0x101C,
    CMD_IDP_TO_MFCB_CLEAR_ALARM_SOUND           = 0x101D,
    CMD_IDP_TO_MFCB_GET_AUTHENTICATION_INFO     = 0x101E,

    /*ODP has less message types than IDP,so type value begin from 0x80*/
    CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO          = 0x1080,   
    CMD_ODP_TO_MFCB_REGISTER                    = 0x1081,
    CMD_ODP_TO_MFCB_KEEP_ALIVE                  = 0x100F,
    CMD_ODP_TO_MFCB_REPORT_DOOR_STATUS          = 0x1083,
    CMD_ODP_TO_MFCB_TOKEN_OPERATION             = 0x1084,
    CMD_ODP_TO_MFCB_REPORT_CARD                 = 0x1087,
    CMD_MFCB_TO_ODP_OPENDOORTIME                = 0x1088,

    /*MFCB has less message types than IDP, 
      so type value begin from 0xC0 distinguished from ODP*/
    CMD_MFCB_TO_ODP_OPEN_DOOR                   = 0x1082,  

    /*Following are ACK message type with value from 0x5000 to 0x50FF*/
    ACK_IDP_TO_MFCB_REGISTER                    = 0x5000,
    ACK_IDP_TO_MFCB_OPEN_DOOR                   = 0x5001,
    ACK_IDP_TO_MFCB_QUERY_DOOR_STATUS           = 0x5002,
    ACK_IDP_TO_MFCB_QUERY_CARD_INFO             = 0x5003,
    ACK_IDP_TO_MFCB_ADD_CRD_OPERATION           = 0x5004,
    ACK_IDP_TO_MFCB_EDIT_SINGLE_CRD             = 0x5005,
    ACK_IDP_TO_MFCB_UPDATE_ALL_CRD              = 0x5006,
    ACK_IDP_TO_MFCB_QUERY_DETECT_POINT_STATUS   = 0x5007,
    ACK_IDP_TO_MFCB_SET_SECURITY_MODE           = 0x5008,
    ACK_MFCB_TO_IDP_QUERY_DETECT_POINT          = 0x5009,
    ACK_IDP_TO_MFCB_SET_SINGLE_DETECT_POINT     = 0x500A,
    ACK_IDP_TO_MFCB_UPDATE_ONE_SECURITY_MODE    = 0x500B,
    ACK_IDP_TO_MFCB_GET_CURRENT_SECURITY_MODE   = 0x500C,
    ACK_IDP_TO_MFCB_CLEAR_ALARM                 = 0x500D,
    ACK_IDP_TO_MFCB_QUERY_ALARM_HISTORY         = 0x500E,
    ACK_IDP_TO_MFCB_KEEP_ALIVE                  = 0x500F,
    ACK_IDP_TO_MFCB_TOKEN_OPERATION             = 0x5010,
    ACK_IDP_TO_MFCB_QUERY_CAM                   = 0x5011,
    ACK_IDP_TO_MFCB_ADD_CAM                     = 0x5012,
    ACK_IDP_TO_MFCB_EDIT_SINGLE_CAM             = 0x5013,
    ACK_IDP_TO_MFCB_QUERY_CURRENT_SECURITY_MODE = 0x5015,
    ACK_IDP_TO_MFCB_DO_CONTROL                  = 0x5016,
    ACK_IDP_TO_MFCB_SYNCHRONIZE_DEVICE          = 0x5017,
    ACK_IDP_TO_MFCB_QUERY_DO                    = 0x5018,
    ACK_IDP_TO_MFCB_RESET_DEFAULT               = 0x5014,
    ACK_IDP_TO_MFCB_CHANGE_SECURITY_PASSWORD    = 0x5019,
    ACK_IDP_TO_MFCB_CHECK_SECURITY_PASSWORD     = 0x501A,
    ACK_IDP_TO_MFCB_QUERY_ALARM_LINKAGE_INFO    = 0x501B,
    ACK_IDP_TO_MFCB_EDIT_ALARM_LINKAGE_INFO     = 0x501C,
    ACK_IDP_TO_MFCB_CLEAR_ALARM_SOUND           = 0x501D,
    ACK_IDP_TO_MFCB_GET_AUTHENTICATION_INFO     = 0x501E,
    
    ACK_ODP_TO_MFCB_QUERY_ACCOUNT_INFO          = 0x5080,
    ACK_ODP_TO_MFCB_REGISTER                    = 0x5081,
    //ACK_ODP_TO_MFCB_REPORT_DOOR_STATUS   = 0x5082,
    ACK_ODP_TO_MFCB_REPORT_DOOR_STATUS          = 0x5083,
    ACK_ODP_TO_MFCB_TOKEN_OPERATION             = 0x5084,
    ACK_ODP_TO_MFCB_BROADCAST_PHONENUMBER       = 0x5086,
    ACK_ODP_TO_MFCB_REPORT_CARD                 = 0x5087,
    ACK_MFCB_TO_ODP_OPENDOORTIME                = 0x5088,

    ACK_MFCB_TO_ODP_OPEN_DOOR                   = 0x5082,


    /*Following are EVENT message type with value from 0x9000 to 0x90FF*/
    EVT_MFCB_TO_IDP_ALARM_DETECT                = 0x9001,
    EVT_MFCB_TO_IDP_UPDATE_SECURITY_MODE        = 0x9002,
    EVT_MFCB_TO_IDP_SYNCHRONIZATION_TIME        = 0x9003,
    EVT_MFCB_TO_ODP_START_REGISTER              = 0x9004,

    EVT_MFCB_TO_IDP_NEW_CARD_INFO               = 0x9000,
    EVT_MFCB_TO_IDP_RESET_DEFAULT               = 0x9006,
    EVT_MFCB_TO_ODP_FLASH_ENABLE_TIME           = 0x9008,
    EVT_MFCB_TO_ODP_SECURITY_MODE               = 0x9009,
    EVT_MFCB_TO_IDP_CLEAR_ALARM_SOUND           = 0x9007,
    EVT_MFCB_TO_IDP_CHANGE_DOOR_STATUS          = 0x900A,
    EVT_MFCB_TO_IDP_UPDATE_CARD                 = 0x900B,
    EVT_MFCB_TO_IDP_SYN_DEVICE                  = 0x900C,
    EVT_MFCB_TO_IDP_CHANGE_AUTHENTICATION_INFO  = 0x900D,
    EVT_MFCB_AUTO_CALL_TEST                     = 0x9010,
}HOMEMSGTYPE;

extern char* gdeviceStatusStr[6];
#endif
