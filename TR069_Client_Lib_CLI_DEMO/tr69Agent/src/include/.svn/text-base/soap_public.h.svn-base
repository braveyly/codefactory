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
Filename: Soap_public.h
Author: David
Date: 2009-05-14
Description: this interfaces defined in this head file could be used by other
module which including this file, and the interfaces are realized in the soap
module

Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/

#ifndef _SOAP_PUBLIC_H_
#define _SOAP_PUBLIC_H_
#include "tr69_agent_public.h"

#define ATTRIBUTE_NAME_LEN 64
#define ATTRIBUTE_VALUE_LEN 256
#define NAMESPACE_PREFIX_LEN 16
#define NAMESPACE_LEN 64
#define ELEMENT_NAME_LEN 256
#define ELEMENT_VALUE_LEN 256
#define FAULT_CODE_LEN 32
#define FAULT_REASON_TEXT 128
#define FAULT_STRING_LEN 128
#define PARAMETER_NAME_LEN 256
#define PARAMETER_VALUE_LEN 256
#define HEADER_ID_LEN 64
#define MOTHED_NAME_LEN 64
#define PARAMETER_KEY_LEN 32
#define USER_NAME_LEN 256
#define PASSWORD_LEN 256
#define TIME_LEN 32
#define ACCESS_LIST_LEN 64
#define ACCESS_NUM 4
#define FILE_TYPE_LEN 64
#define URL_LEN 256
#define MANUFACTORY_LEN 64
#define EVENT_CODE_LEN 64
#define OUI_LEN 6
#define EVENT_CODE_NUM 16
#define TRANSFER_STRUCT_NUM 16
#define ARG_NAME_LEN 64
#define DOWNLOAD_STRUCT_NUM 16
#define SOAPTAGLEN 256

#define BUFFER_SIZE 400000   //gavin change buffer to prevent buffer overflow when get top value command

typedef enum _PacketType
{
    SEND_INFORM = 1,
    TRANSFER_COMPLETE,
    SET_PARAMETER_VALUES_RESP,
    GET_PARAMETER_VALUES_RESP,
    GET_PARAMETER_NAMES_RESP,
    SET_PARAMETER_ATTRIBUTES_RESP,
    GET_PARAMETER_ATTRIBUTES_RESP,
    ADD_OBJECT_RESP,
    DELETE_OBJECT_RESP,
    GET_RPCMETHODS_RESP,
    DOWNLOAD_RESP,
    REBOOT_RESP,
    GET_QUEUEDTRANSFERS_RESP,
    SCHEDULE_INFORM_RESP,
    FACTORY_RESET_RESP,
    UPLOAD_RESP,
    REQUEST_DOWNLOAD,
    FAULT
}PacketType;

typedef  enum _FaultType
{
    tOK = 0,
    tMethodNotSupported = 9000,
    tRequestDenied = 9001,
    tInternalError   = 9002,
    tInvalidArguments = 9003,
    tResourcesExceeded = 9004,
    tInvalidParameterName = 9005,
    tInvalidParameterType = 9006,
    tInvalidParameterValue = 9007,
    tAttemptSetNonWritable = 9008,
    tNotificationRequestRejected = 9009,
    tDownloadFailure = 9010,
    tUploadFailure = 9011,
    tFileTransferAuthFailure = 9012,
    tUnsupportedProtocolTransfer = 9013
} FaultType;

typedef  enum _ErrorCode
{
    SUCCESS_OK = 0,
    ERROR_MALLOC = 1,
    ERROR_PARAMETER = 2,
    ERROR_PARSER = 3,
    FAULT_PACKETS = 4
} ErrorCode;


typedef union
{
    char  *string;
    PAL_UINT32_T uintValue;
    PAL_INT32_T intValue;
    PAL_BOOL_T boolValue;
    float floatValue;
} BaseParameterValue;

typedef  enum
{ 
    XSD_INT=1,
    XSD_UINT,
    XSD_BOOL,
    XSD_STRING,
} ValueType;

typedef struct _SOAPAttribute
{
    struct _SOAPAttribute *next;
    char nstr[NAMESPACE_PREFIX_LEN];
    char name[ATTRIBUTE_NAME_LEN];
    char data[ATTRIBUTE_VALUE_LEN];
}SOAPAttribute;



typedef struct _EleSoap
{
    char type[SOAPTAGLEN];
    char arrayType[SOAPTAGLEN];
    char arraySize[SOAPTAGLEN];
    PAL_INT16_T mustUnderstand; /* a mustUnderstand element was parsed or is output */
    char actor[SOAPTAGLEN];  /* SOAP-ENV:actor or role attribute value */
}EleSoap;

typedef struct _SOAPElement
{
    struct _SOAPElement *next; /* list of elements */
    SOAPAttribute *atts; /* list of attributes */
    char nstr[NAMESPACE_PREFIX_LEN];   /* namespace string */
    char name[ELEMENT_NAME_LEN];    /* element tag name */
    BaseParameterValue data;    /* element content data */
    ValueType type;    /* optional: data type */
    EleSoap *soap;    /*the soap attribute for every element*/
}SOAPElement;





typedef struct _SOAPCode // version 1.2
{
    char SOAPValue[FAULT_CODE_LEN];
    struct _SOAPCode *SOAPSubcode;
}SOAPCode;

typedef struct _SOAPReason // version 1.2
{
    char SOAPText[FAULT_REASON_TEXT]; /* optional element of type xsd:string */
}SOAPReason;

typedef struct _SetParameterFaultStruct
{
    struct _SetParameterFaultStruct *next;
    char ParameterName[PARAMETER_NAME_LEN];
    PAL_UINT32_T  FaultCode;
    char FaultString[FAULT_STRING_LEN];
} SetParameterFaultStruct;

typedef struct _CWMPFault
{
    FaultType FaultCode;
    char FaultString[FAULT_STRING_LEN];
    SetParameterFaultStruct *SetParameterValuesFault;
} CWMPFault;


typedef struct _CWMPDetail
{
    CWMPFault *cwmpFault;
} CWMPDetail;

typedef struct _SOAPFault
{
    char faultCode[FAULT_CODE_LEN];
    char faultString[FAULT_STRING_LEN];
    char faultActor[FAULT_STRING_LEN];
    CWMPDetail *detail; /* optional element of type SOAP-ENV:Detail */
    SOAPCode *SOAPCode; /* optional element of type SOAP-ENV:Code , version 1.2*/
    SOAPReason *SOAPReason; /* optional element of type SOAP-ENV:Reason,  version 1.2*/
    char SOAPNode[FAULT_STRING_LEN]; /* optional element of type xsd:string, version 1.2 */
    char SOAPRole[FAULT_STRING_LEN]; /* optional element of type xsd:string , version 1.2*/
    CWMPDetail *SOAPDetail; /* optional element of type SOAP-ENV:Detail, version 1.2*/
}SOAPFault;

typedef struct _Namespace
{
    struct _Namespace * next;
    char  nsname[NAMESPACE_PREFIX_LEN];
    char  nsvalue[NAMESPACE_LEN];
}NameSpace;

typedef struct _SOAPHeader
{
    char  cwmpID[HEADER_ID_LEN];
    PAL_BOOL_T cwmpHoldRequests;
    PAL_BOOL_T cwmpNoMoreRequests;
}SOAPHeader;

typedef enum
{
    tFault = 0,
    tInformResponse  = 1,
    tTransferCompleteResponse,
    tGetRPCMethods,
    tUpload,
    tDownload,
    tAddObject,
    tDeleteObject,
    tSetParameterValues,
    tGetParameterValues,
    tGetParameterNames,
    tGetParameterAttributes,
    tSetParameterAttributes,
    tReboot,
    tGetQueuedTransfer,
    tRequestDownload,
    tRequestDownloadResponse,
    tScheduleInform,
    tFactoryReset
}RPCType;

typedef struct _SOAP
{
    PAL_INT16_T version;  /* 1 = SOAP1.1 and 2 = SOAP1.2 (set automatically from namespace URI in nsmap table) */
    NameSpace * nameSpaces;
    SOAPHeader *header;
    SOAPFault *fault;
    SOAPElement *element;
    char method_name[MOTHED_NAME_LEN];
    void *parameters;
    PAL_INT32_T error;
    RPCType rpcType;
}SOAP;



#if 0
typedef struct _ParameterValueStruct
{
    struct _ParameterValueStruct *next;
    char  name[PARAMETER_NAME_LEN + 1];                // string
    char  value[PARAMETER_VALUE_LEN + 1];
    ValueType type;
}ParameterValueStruct;
#endif

typedef struct _ParameterValueStruct
{
    struct _ParameterValueStruct *next;
    char name[PARAMETER_NAME_LEN + 1];
    char *value;
    ValueType type;
} ParameterValueStruct;

typedef struct _ParameterListStruct
{
    char  parameterKey[PARAMETER_KEY_LEN + 1];                // string
    ParameterValueStruct *paramterValues;
}ParameterListStruct;

typedef struct _GetParameterValueStruct
{
    struct _GetParameterValueStruct *next;
    char  name[PARAMETER_NAME_LEN + 1];                // string
}GetParameterValueStruct;

typedef struct _ParameterNameStruct
{
    char  name[PARAMETER_NAME_LEN + 1];                // string
    PAL_BOOL_T nextLevel;
}ParameterNameStruct;


typedef struct _ParameterInfoStruct
{
    struct _ParameterInfoStruct *next;
    char  name[PARAMETER_NAME_LEN + 1];                // string
    PAL_BOOL_T writable;
}ParameterInfoStruct;

typedef struct _ParameterSetAttributeStruct
{
    struct _ParameterSetAttributeStruct *next;
    char  name[PARAMETER_NAME_LEN + 1];                // string
    PAL_INT32_T notificationChange;
    PAL_INT32_T notification;
    PAL_INT32_T accessListChange;
    char accessList[ACCESS_LIST_LEN + 1];
}ParameterSetAttributeStruct;

typedef struct _ParameterAttributeStruct
{
    struct _ParameterAttributeStruct *next;
    char  name[PARAMETER_NAME_LEN + 1];
    PAL_UINT32_T notification;
    char accessList[ACCESS_NUM][ACCESS_LIST_LEN + 1];
}ParameterAttributeStruct;

typedef struct _AddObjectStruct
{
    char  name[PARAMETER_NAME_LEN + 1];
    char  parameterKey[PARAMETER_KEY_LEN + 1];
}AddObjectStruct;

typedef struct _AddObjectResponseStruct
{
    PAL_UINT32_T  instanceID;
    PAL_INT32_T   status;
}AddObjectResponseStruct;

typedef struct _DeleteObjectStruct
{
    char  name[PARAMETER_NAME_LEN + 1];
    char  parameterKey[PARAMETER_KEY_LEN + 1];
}DeleteObjectStruct;

typedef struct _DownloadStruct
{
    char  commandKey[PARAMETER_KEY_LEN + 1];
    char  fileType[FILE_TYPE_LEN + 1];
    char  URL[URL_LEN + 1];
    char  username[USER_NAME_LEN + 1];
    char  password[PASSWORD_LEN + 1];
    char  targetFileName[PARAMETER_NAME_LEN + 1];
    char  successURL[URL_LEN + 1];
    char failureURL[URL_LEN + 1];
    PAL_UINT32_T filesize;
    PAL_UINT32_T delaySeconds;
}DownloadStruct;

typedef struct _DownloadResponseStruct
{
    PAL_INT32_T status;
    char startTime[TIME_LEN + 1];
    char completeTime[TIME_LEN + 1];
}DownloadResponseStruct;

#if 0
typedef struct _DeviceIdStruct
{
    char  manufacturer[MANUFACTORY_LEN + 1];
    char  oui[OUI_LEN + 1];
    char  productClass[MANUFACTORY_LEN + 1];
    char  serialNumber[MANUFACTORY_LEN + 1];
}DeviceIdStruct;
#endif

typedef struct _DeviceIdStruct
{
    char *manufacturer;
    char *oui;
    char *productClass;
    char *serialNumber;
} DeviceIdStruct;

typedef struct _EventCodeStruct
{
    char  eventCode[EVENT_CODE_LEN + 1];
    char  commandKey[PARAMETER_KEY_LEN + 1];
}EventCodeStruct;

typedef struct _InformInfoStruct
{
    DeviceIdStruct deviceId;
    EventCodeStruct eventStruct[EVENT_CODE_NUM + 1];
    char currentTime[TIME_LEN + 1];
    PAL_UINT32_T maxEnvelopes;
    PAL_UINT32_T retryCount;
    ParameterValueStruct *parameterList;
}InformInfoStruct;

typedef struct _FaultStruct
{
    PAL_UINT32_T faultcode;
    char faultString[PARAMETER_NAME_LEN + 1];
}FaultStruct;

typedef struct _TransferCompleteStruct
{
    char  commandKey[PARAMETER_KEY_LEN + 1];
    FaultStruct fault;
    char startTime[TIME_LEN + 1];
    char completeTime[TIME_LEN + 1];
}TransferCompleteStruct;

/* new add for transcomplete packet */
typedef struct _TransferCompleteList
{
    TransferCompleteStruct transferCompleteInfo;
    struct _TransferCompleteList * next;
}TransferCompleteList;

typedef struct _TransferStruct
{
    char commandKey[PARAMETER_KEY_LEN + 1];
    PAL_INT32_T state;
}TransferStruct;

typedef struct _TransferListStruct
{
    TransferStruct transferlist[TRANSFER_STRUCT_NUM];
}TransferListStruct;

typedef struct _ScheduleInformStruct
{
    PAL_UINT32_T delaySeconds;
    char commandKey[PARAMETER_KEY_LEN + 1];
}ScheduleInformStruct;

typedef struct _UploadStruct
{
    char  commandKey[PARAMETER_KEY_LEN + 1];
    char  fileType[FILE_TYPE_LEN + 1];
    char  URL[URL_LEN + 1];
    char  username[USER_NAME_LEN + 1];
    char  password[PASSWORD_LEN + 1];
    PAL_UINT32_T delaySeconds;
}UploadStruct;

typedef struct _UploadResponseStruct
{
    PAL_INT32_T status;
    char startTime[TIME_LEN + 1];
    char completeTime[TIME_LEN + 1];
}UploadResponseStruct;

typedef struct _FileArgStruct
{
    char  name[ARG_NAME_LEN + 1];
    char  value[PARAMETER_VALUE_LEN + 1];
}FileArgStruct;

typedef struct _RequestDownloadStruct
{
    char  fileType[FILE_TYPE_LEN + 1];
    FileArgStruct arglist[DOWNLOAD_STRUCT_NUM];
}RequestDownloadStruct;
/***********************************************************************
Function: get_holdrequest
Description:  the main loop module could get the holdrequest of current parsing packet

Return: PAL_INT32_T, the holdrequest of current parsing packet
Input: void ,
Output: the holdrequest of current parsing packet
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-14

    2.  Modification:
        Author:
        Date:
************************************************************************/
PAL_INT32_T get_holdrequest ( void );

/***********************************************************************
Function: get_nomorerequest
Description:  the main loop module could get the nomorerequest of current parsing packet

Return: PAL_INT32_T, the nomorerequest of current parsing packet
Input: void ,
Output: the nomorerequest of current parsing packet
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-14

    2.  Modification:
        Author:
        Date:
************************************************************************/
PAL_INT32_T get_nomorerequest ( void );
/***********************************************************************
Function: generate_soap_packet
Description:  the other module could call this function to generate a soap
packet.

Return: PAL_INT32_T, if generic the packet success, return SUCCESSOK, else if
malloc memery errer, return ERRORMALLOC, if parameter is not valid, return ERRORPARAMETER;
Input: PacketType type, the packet type, indicate the packet is fault or rpc
method inform,          GetRPCMethod and so on.
       FaultType faultCode,         the faultcode of the cwmp.
       SetParameterFaultStruct *setparameterfault,      the
       setparameterfaultstrct only appear in the setparametervalues method.
       void *parameter,         the parameter of rpc method
       SOAPList *soapList,         the soapList which contain the buffer we can fill
       PAL_INT32_T * soapSize ,         the size of soap buffer
Output: sendbuf, soapSize, return value
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-14

    2.  Modification:
        Author:
        Date:
************************************************************************/
Result generate_soap_packet ( PacketType type, FaultType faultCode,
                              SetParameterFaultStruct *setparameterfault,
                              void *parameter, char **sSendBuf, PAL_UINT32_T * sSendLength );

/***********************************************************************
Function: handle_soap_packet
Description:  other modules can call this function to parser a soap packet

Return: PAL_INT32_T,        if parser the soap packet success, return SUCCESSOK, else
return ERRORPARSER;
Input: char *buf, the pointer to the received soap packet
       SOAP * soap, the output soap struct which contain the fault information
       or the rpc method name and parameter
       char *faultBuf,    if it is a mistake soap method, we can generate
       the fault buf.
Output: soap, faulBuf, return value
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-14

    2.  Modification:
        Author:
        Date:
************************************************************************/

Result handle_soap_packet ( char *buf, PAL_UINT32_T bufLength, SOAP * soap, char **sSoapBuf, PAL_UINT32_T * sSoapLength );

void inform_free ( InformInfoStruct *inform );
void getParameterValues_free ( ParameterValueStruct **getParameterValue );
void getParameterNames_free ( ParameterInfoStruct** getParameterNames );
void getPrameterAttribute_free ( ParameterAttributeStruct ** getParameterAttribute );
void addObject_free ( AddObjectResponseStruct *addObject );
void download_free ( DownloadResponseStruct * download );
void getQueuedTransfer_free ( TransferListStruct *getQueuedTransfer );
void upload_free ( UploadResponseStruct *upload );
void requestDownload_free ( RequestDownloadStruct *requestDownload );

#endif

