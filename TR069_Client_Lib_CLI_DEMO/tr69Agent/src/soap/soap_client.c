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
Filename: soap_client.c
Author: David
Date: 2009-05-14
Description: handle the rpc method parameter, generate and parser the
parameters.

Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#include "xml_public.h"
#include "pal.h"
#include "soap_client.h"
#include "soap_build.h"

extern char gRPCMethodList[][32];

PAL_INT32_T add_inform_method ( char *sendbuf, InformInfoStruct *inform, PAL_INT32_T * soapSize )
{
    int i, eventCount, parameterCount;
    EventCodeStruct *EventCodeStructptr;
    ParameterValueStruct *ParameterValueStructptr;
    char tmp[64];

    soap_send_raw ( sendbuf, "<cwmp:Inform>", strlen ( "<cwmp:Inform>" ), soapSize );

    soap_send_raw ( sendbuf, "<DeviceId>", strlen ( "<DeviceId>" ), soapSize );
    if ( inform->deviceId.manufacturer[0] != 0 )
    {
        soap_send_raw ( sendbuf, "<Manufacturer>", strlen ( "<Manufacturer>" ), soapSize );
	if(inform->deviceId.manufacturer != NULL)
        	soap_send_raw ( sendbuf, inform->deviceId.manufacturer, strlen ( inform->deviceId.manufacturer ), soapSize );
	else
		printf("\n===Manufacturer=pointer null==\n");
        soap_send_raw ( sendbuf, "</Manufacturer>", strlen ( "</Manufacturer>" ), soapSize );
    }
    else
    {
        return ERROR_PARAMETER;
    }
    if ( inform->deviceId.oui[0] != 0 )
    {
        soap_send_raw ( sendbuf, "<OUI>", strlen ( "<OUI>" ), soapSize );
	if(inform->deviceId.oui != NULL)
       		soap_send_raw ( sendbuf, inform->deviceId.oui, strlen ( inform->deviceId.oui ), soapSize );
	else
		printf("\n==oui==pointer null==\n");
        soap_send_raw ( sendbuf, "</OUI>", strlen ( "</OUI>" ), soapSize );
    }
    else
    {
        return ERROR_PARAMETER;
    }

    if ( inform->deviceId.productClass[0] != 0 )
    {
        soap_send_raw ( sendbuf, "<ProductClass>", strlen ( "<ProductClass>" ), soapSize );
	if(inform->deviceId.productClass != NULL)
        	soap_send_raw ( sendbuf, inform->deviceId.productClass, strlen ( inform->deviceId.productClass ), soapSize );
	else
		printf("\n===ProductClass=pointer null==\n");
        soap_send_raw ( sendbuf, "</ProductClass>", strlen ( "</ProductClass>" ), soapSize );
    }
    else
    {
        return ERROR_PARAMETER;
    }

    if ( inform->deviceId.serialNumber[0] != 0 )
    {
        soap_send_raw ( sendbuf, "<SerialNumber>", strlen ( "<SerialNumber>" ), soapSize );
	if(inform->deviceId.serialNumber != NULL)
        	soap_send_raw ( sendbuf, inform->deviceId.serialNumber, strlen ( inform->deviceId.serialNumber ), soapSize );
	else
		printf("\n==SerialNumber==pointer null==\n");
        soap_send_raw ( sendbuf, "</SerialNumber>", strlen ( "</SerialNumber>" ), soapSize );
    }
    else
    {
        return ERROR_PARAMETER;
    }
	
    soap_send_raw ( sendbuf, "</DeviceId>", strlen ( "</DeviceId>" ), soapSize );



    EventCodeStructptr = inform->eventStruct;
    eventCount = 0;
    while ( strlen(EventCodeStructptr->eventCode) != 0  && eventCount < EVENT_CODE_NUM)
    {
        EventCodeStructptr++;
        eventCount++;
    }


    soap_send_raw ( sendbuf, "<Event", strlen ( "<Event" ), soapSize );
    sprintf ( tmp, "cwmp:EventStruct[%d]", eventCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );

    for ( i = 0; i < eventCount; i++ )
    {
        soap_send_raw ( sendbuf, "<EventStruct>", strlen ( "<EventStruct>" ), soapSize );
        if ( strlen(inform->eventStruct[i].eventCode) != 0 )
        {
            soap_send_raw ( sendbuf, "<EventCode>", strlen ( "<EventCode>" ), soapSize );
            soap_send_raw ( sendbuf, inform->eventStruct[i].eventCode, strlen ( inform->eventStruct[i].eventCode ), soapSize );
            soap_send_raw ( sendbuf, "</EventCode>", strlen ( "</EventCode>" ), soapSize );
        }

        soap_send_raw ( sendbuf, "<CommandKey>", strlen ( "<CommandKey>" ), soapSize );
        soap_send_raw ( sendbuf, inform->eventStruct[i].commandKey, strlen ( inform->eventStruct[i].commandKey ), soapSize );
        soap_send_raw ( sendbuf, "</CommandKey>", strlen ( "</CommandKey>" ), soapSize );

        soap_send_raw ( sendbuf, "</EventStruct>", strlen ( "</EventStruct>" ), soapSize );
    }
    soap_send_raw ( sendbuf, "</Event>", strlen ( "</Event>" ), soapSize );

    soap_send_raw ( sendbuf, "<MaxEnvelopes>", strlen ( "<MaxEnvelopes>" ), soapSize );
    sprintf ( tmp, "%d", inform->maxEnvelopes );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</MaxEnvelopes>", strlen ( "</MaxEnvelopes>" ), soapSize );

    soap_send_raw ( sendbuf, "<CurrentTime>", strlen ( "<CurrentTime>" ), soapSize );
    soap_send_raw ( sendbuf, inform->currentTime, strlen ( inform->currentTime ), soapSize );
    soap_send_raw ( sendbuf, "</CurrentTime>", strlen ( "</CurrentTime>" ), soapSize );
    sprintf ( tmp, "%d", inform->retryCount );
    soap_send_raw ( sendbuf, "<RetryCount>", strlen ( "<RetryCount>" ), soapSize );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</RetryCount>", strlen ( "</RetryCount>" ), soapSize );

    ParameterValueStructptr = inform->parameterList;
    parameterCount = 0;

    while ( ParameterValueStructptr )
    {
        ParameterValueStructptr = ParameterValueStructptr->next;
        parameterCount++;
    }

    soap_send_raw ( sendbuf, "<ParameterList ", strlen ( "<ParameterList " ), soapSize );
    sprintf ( tmp, "cwmp:ParameterValueStruct[%d]", parameterCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );

    ParameterValueStructptr = inform->parameterList;
    for ( i = 0; i < parameterCount; i++ )
    {
        soap_send_raw ( sendbuf, "<ParameterValueStruct>", strlen ( "<ParameterValueStruct>" ), soapSize );
        if ( ParameterValueStructptr->name[0] != 0 )
        {
            soap_send_raw ( sendbuf, "<Name>", strlen ( "<Name>" ), soapSize );
            soap_send_raw ( sendbuf, ParameterValueStructptr->name, strlen ( ParameterValueStructptr->name ), soapSize );
            soap_send_raw ( sendbuf, "</Name>", strlen ( "</Name>" ), soapSize );
        }
        soap_send_raw ( sendbuf, "<Value", strlen ( "<Value" ), soapSize );
        soap_add_attribute ( sendbuf, "xsi:type", get_type ( ParameterValueStructptr->type ), soapSize );
        soap_send_raw ( sendbuf, ">", 1, soapSize );
	if(ParameterValueStructptr->value !=NULL)
        	soap_send_raw ( sendbuf, ParameterValueStructptr->value, strlen ( ParameterValueStructptr->value ), soapSize );
	else
		printf("\n==[%s]=soap value null value===\n",ParameterValueStructptr->name);
        soap_send_raw ( sendbuf, "</Value>", strlen ( "</Value>" ), soapSize );

        soap_send_raw ( sendbuf, "</ParameterValueStruct>", strlen ( "</ParameterValueStruct>" ), soapSize );
        ParameterValueStructptr = ParameterValueStructptr->next;
    }
    soap_send_raw ( sendbuf, "</ParameterList>", strlen ( "</ParameterList>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:Inform>", strlen ( "</cwmp:Inform>" ),
                    soapSize );
    return SUCCESS_OK;

}

/*PAL_INT32_T add_transfercomplete_method ( char *sendbuf, TransferCompleteList *transferComplete, PAL_INT32_T * soapSize )
{
    char tmp[32];
    TransferCompleteList *TransferCompleteListptr;
    TransferCompleteListptr = transferComplete;
    while(TransferCompleteListptr)
    {
        soap_send_raw ( sendbuf, "<cwmp:TransferComplete>", strlen ( "<cwmp:TransferComplete>" ), soapSize );
        soap_send_raw ( sendbuf, "<CommandKey>", strlen ( "<CommandKey>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.commandKey, strlen ( TransferCompleteListptr->transferCompleteInfo.commandKey ), soapSize );
        soap_send_raw ( sendbuf, "</CommandKey>", strlen ( "</CommandKey>" ), soapSize );
        soap_send_raw ( sendbuf, "<FaultStruct>", strlen ( "<FaultStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<FaultCode>", strlen ( "<FaultCode>" ), soapSize );
        sprintf ( tmp, "%d", TransferCompleteListptr->transferCompleteInfo.fault.faultcode );
        soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
        soap_send_raw ( sendbuf, "</FaultCode>", strlen ( "</FaultCode>" ), soapSize );
        soap_send_raw ( sendbuf, "<FaultString>", strlen ( "<FaultString>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.fault.faultString, strlen ( TransferCompleteListptr->transferCompleteInfo.fault.faultString ), soapSize );
        soap_send_raw ( sendbuf, "</FaultString>", strlen ( "</FaultString>" ), soapSize );
        soap_send_raw ( sendbuf, "</FaultStruct>", strlen ( "</FaultStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<StartTime>", strlen ( "<StartTime>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.startTime, strlen ( TransferCompleteListptr->transferCompleteInfo.startTime), soapSize );
        soap_send_raw ( sendbuf, "</StartTime>", strlen ( "</StartTime>" ), soapSize );
        soap_send_raw ( sendbuf, "<CompleteTime>", strlen ( "<CompleteTime>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.completeTime, strlen ( TransferCompleteListptr->transferCompleteInfo.completeTime), soapSize );
        soap_send_raw ( sendbuf, "</CompleteTime>", strlen ( "</CompleteTime>" ), soapSize );
        soap_send_raw ( sendbuf, "</cwmp:TransferComplete>", strlen ( "</cwmp:TransferComplete>" ), soapSize );
        TransferCompleteListptr = TransferCompleteListptr->next;
    }
    return SUCCESS_OK;
}*/

PAL_INT32_T add_transfercomplete_method ( char *sendbuf, TransferCompleteList *transferComplete, PAL_INT32_T * soapSize )
{
    char tmp[32];
    TransferCompleteList *TransferCompleteListptr;
    PAL_INT32_T ret;
    TransferCompleteListptr = transferComplete;
    *soapSize = 0;
    while ( TransferCompleteListptr )
    {
        if ( ( ret = add_envelope_start ( sendbuf,  soapSize ) ) != SUCCESS_OK )
        {
            return PARAMETER_ERROR;
        }
        if ( ( ret = add_header_start ( sendbuf, soapSize, TRANSFER_COMPLETE ) ) != SUCCESS_OK )
        {
            return PARAMETER_ERROR;
        }
        add_header_end ( sendbuf, soapSize );
        add_body_start ( sendbuf,  soapSize );
        soap_send_raw ( sendbuf, "<cwmp:TransferComplete>", strlen ( "<cwmp:TransferComplete>" ), soapSize );
        soap_send_raw ( sendbuf, "<CommandKey>", strlen ( "<CommandKey>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.commandKey, strlen ( TransferCompleteListptr->transferCompleteInfo.commandKey ), soapSize );
        soap_send_raw ( sendbuf, "</CommandKey>", strlen ( "</CommandKey>" ), soapSize );
        soap_send_raw ( sendbuf, "<FaultStruct>", strlen ( "<FaultStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<FaultCode>", strlen ( "<FaultCode>" ), soapSize );
        sprintf ( tmp, "%d", TransferCompleteListptr->transferCompleteInfo.fault.faultcode );
        soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
        soap_send_raw ( sendbuf, "</FaultCode>", strlen ( "</FaultCode>" ), soapSize );
        soap_send_raw ( sendbuf, "<FaultString>", strlen ( "<FaultString>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.fault.faultString, strlen ( TransferCompleteListptr->transferCompleteInfo.fault.faultString ), soapSize );
        soap_send_raw ( sendbuf, "</FaultString>", strlen ( "</FaultString>" ), soapSize );
        soap_send_raw ( sendbuf, "</FaultStruct>", strlen ( "</FaultStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<StartTime>", strlen ( "<StartTime>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.startTime, strlen ( TransferCompleteListptr->transferCompleteInfo.startTime ), soapSize );
        soap_send_raw ( sendbuf, "</StartTime>", strlen ( "</StartTime>" ), soapSize );
        soap_send_raw ( sendbuf, "<CompleteTime>", strlen ( "<CompleteTime>" ), soapSize );
        soap_send_raw ( sendbuf, TransferCompleteListptr->transferCompleteInfo.completeTime, strlen ( TransferCompleteListptr->transferCompleteInfo.completeTime ), soapSize );
        soap_send_raw ( sendbuf, "</CompleteTime>", strlen ( "</CompleteTime>" ), soapSize );
        soap_send_raw ( sendbuf, "</cwmp:TransferComplete>", strlen ( "</cwmp:TransferComplete>" ), soapSize );
        add_body_end ( sendbuf,  soapSize );
        add_envelope_end ( sendbuf,  soapSize );
        TransferCompleteListptr = TransferCompleteListptr->next;
    }
    return SUCCESS_OK;
}

PAL_INT32_T add_setparametervalues_res_method ( char *sendbuf, PAL_INT32_T status, PAL_INT32_T * soapSize )
{
    char tmp[16];
    soap_send_raw ( sendbuf, "<cwmp:SetParameterValuesResponse>", strlen ( "<cwmp:SetParameterValuesResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<Status>", strlen ( "<Status>" ), soapSize );
    sprintf ( tmp, "%d", status );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</Status>", strlen ( "</Status>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:SetParameterValuesResponse>", strlen ( "</cwmp:SetParameterValuesResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_getparametervalues_res_method ( char *sendbuf, ParameterValueStruct * getParameterValue, PAL_INT32_T * soapSize )
{
    char tmp[64];
    PAL_INT32_T i, parameterCount = 0;
    ParameterValueStruct *ParameterValueStructptr;
    soap_send_raw ( sendbuf, "<cwmp:GetParameterValuesResponse>", strlen ( "<cwmp:GetParameterValuesResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<ParameterList", strlen ( "<ParameterList" ), soapSize );
    ParameterValueStructptr = getParameterValue;
    while ( ParameterValueStructptr )
    {
        parameterCount++;
        ParameterValueStructptr = ParameterValueStructptr->next;
    }
    sprintf ( tmp, "cwmp:ParameterValueStruct[%d]", parameterCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    ParameterValueStructptr = getParameterValue;
    for ( i = 0; i < parameterCount; i++ )
    {
        soap_send_raw ( sendbuf, "<ParameterValueStruct>", strlen ( "<ParameterValueStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<Name>", strlen ( "<Name>" ), soapSize );
        soap_send_raw ( sendbuf, ParameterValueStructptr->name, strlen ( ParameterValueStructptr->name ), soapSize );
        soap_send_raw ( sendbuf, "</Name>", strlen ( "</Name>" ), soapSize );
        soap_send_raw ( sendbuf, "<Value", strlen ( "<Value" ), soapSize );
        soap_add_attribute ( sendbuf, "xsi:type", get_type ( ParameterValueStructptr->type ), soapSize );
        soap_send_raw ( sendbuf, ">", 1, soapSize );
        soap_send_raw ( sendbuf, ParameterValueStructptr->value, strlen ( ParameterValueStructptr->value ), soapSize );
        soap_send_raw ( sendbuf, "</Value>", strlen ( "</Value>" ), soapSize );
        soap_send_raw ( sendbuf, "</ParameterValueStruct>", strlen ( "</ParameterValueStruct>" ), soapSize );
        ParameterValueStructptr = ParameterValueStructptr->next;
    }
    soap_send_raw ( sendbuf, "</ParameterList>", strlen ( "</ParameterList>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:GetParameterValuesResponse>", strlen ( "</cwmp:GetParameterValuesResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_getparameternames_res_method ( char *sendbuf, ParameterInfoStruct* getParameterNames, PAL_INT32_T * soapSize )
{
    char tmp[64];
    PAL_INT32_T i, parameterCount = 0;
    ParameterInfoStruct *ParameterInfoStructptr;
    soap_send_raw ( sendbuf, "<cwmp:GetParameterNamesResponse>", strlen ( "<cwmp:GetParameterNamesResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<ParameterList", strlen ( "<ParameterList" ), soapSize );
    ParameterInfoStructptr = getParameterNames;
    while ( ParameterInfoStructptr )
    {
        parameterCount++;
        ParameterInfoStructptr = ParameterInfoStructptr->next;
    }
    sprintf ( tmp, "cwmp:ParameterInfoStruct[%d]", parameterCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    ParameterInfoStructptr = getParameterNames;
    for ( i = 0; i < parameterCount; i++ )
    {
        soap_send_raw ( sendbuf, "<ParameterInfoStruct>", strlen ( "<ParameterInfoStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<Name>", strlen ( "<Name>" ), soapSize );
        soap_send_raw ( sendbuf, ParameterInfoStructptr->name, strlen ( ParameterInfoStructptr->name ), soapSize );
        soap_send_raw ( sendbuf, "</Name>", strlen ( "</Name>" ), soapSize );
        soap_send_raw ( sendbuf, "<Writable>", strlen ( "<Writable>" ), soapSize );
        sprintf ( tmp, "%d", ParameterInfoStructptr->writable );
        soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
        soap_send_raw ( sendbuf, "</Writable>", strlen ( "</Writable>" ), soapSize );
        soap_send_raw ( sendbuf, "</ParameterInfoStruct>", strlen ( "</ParameterInfoStruct>" ), soapSize );
        ParameterInfoStructptr = ParameterInfoStructptr->next;
    }
    soap_send_raw ( sendbuf, "</ParameterList>", strlen ( "</ParameterList>" ), soapSize );

    soap_send_raw ( sendbuf, "</cwmp:GetParameterNamesResponse>", strlen ( "</cwmp:GetParameterNamesResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_setparameterattributes_res_method ( char *sendbuf, PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "<cwmp:SetParameterAttributesResponse>", strlen ( "<cwmp:SetParameterAttributesResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:SetParameterAttributesResponse>", strlen ( "</cwmp:SetParameterAttributesResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_getparameterattributes_res_method ( char *sendbuf, ParameterAttributeStruct * getParameterAttribute, PAL_INT32_T * soapSize )
{
    char tmp[64] ;
    //char ** accessptr ;
    PAL_INT32_T i, j, parameterCount = 0, accessCount = 0;
    ParameterAttributeStruct *ParameterAttributeStructptr;
    soap_send_raw ( sendbuf, "<cwmp:GetParameterAttributesResponse>", strlen ( "<cwmp:GetParameterAttributesResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<ParameterList", strlen ( "<ParameterList" ), soapSize );
    ParameterAttributeStructptr = getParameterAttribute;
    while ( ParameterAttributeStructptr )
    {
        parameterCount++;
        ParameterAttributeStructptr = ParameterAttributeStructptr->next;
    }
    sprintf ( tmp, "cwmp:ParameterAttributeStruct[%d]", parameterCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    ParameterAttributeStructptr = getParameterAttribute;
    for ( i = 0; i < parameterCount; i++ )
    {
        soap_send_raw ( sendbuf, "<ParameterAttributeStruct>", strlen ( "<ParameterAttributeStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<Name>", strlen ( "<Name>" ), soapSize );
        soap_send_raw ( sendbuf, ParameterAttributeStructptr->name, strlen ( ParameterAttributeStructptr->name ), soapSize );
        soap_send_raw ( sendbuf, "</Name>", strlen ( "</Name>" ), soapSize );
        soap_send_raw ( sendbuf, "<Notification>", strlen ( "<Notification>" ), soapSize );
        sprintf ( tmp, "%d", ParameterAttributeStructptr->notification );
        soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
        soap_send_raw ( sendbuf, "</Notification>", strlen ( "</Notification>" ), soapSize );
        j = 0;
        accessCount = 0;
        while ( ParameterAttributeStructptr->accessList[j][0] != 0 )
        {
            accessCount++;
            j++;
        }
        soap_send_raw ( sendbuf, "<AccessList", strlen ( "<AccessList" ), soapSize );
        sprintf ( tmp, "xsd:string[%d]", accessCount );
        soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
        soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
        soap_send_raw ( sendbuf, ">", 1, soapSize );
        for ( j = 0; j < accessCount; j++ )
        {
            soap_send_raw ( sendbuf, "<string>", strlen ( "<string>" ), soapSize );
            soap_send_raw ( sendbuf, ParameterAttributeStructptr->accessList[j], strlen ( ParameterAttributeStructptr->accessList[j] ), soapSize );
            soap_send_raw ( sendbuf, "</string>", strlen ( "</string>" ), soapSize );
        }

        soap_send_raw ( sendbuf, "</AccessList>", strlen ( "</AccessList>" ), soapSize );

        soap_send_raw ( sendbuf, "</ParameterAttributeStruct>", strlen ( "</ParameterAttributeStruct>" ), soapSize );
        ParameterAttributeStructptr = ParameterAttributeStructptr->next;
    }
    soap_send_raw ( sendbuf, "</ParameterList>", strlen ( "</ParameterList>" ), soapSize );

    soap_send_raw ( sendbuf, "</cwmp:GetParameterAttributesResponse>", strlen ( "</cwmp:GetParameterAttributesResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_addobject_res_method ( char *sendbuf, AddObjectResponseStruct *addObject, PAL_INT32_T * soapSize )
{
    char tmp[16] ;
    soap_send_raw ( sendbuf, "<cwmp:AddObjectResponse>", strlen ( "<cwmp:AddObjectResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<InstanceNumber>", strlen ( "<InstanceNumber>" ), soapSize );
    sprintf ( tmp, "%d", addObject->instanceID );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</InstanceNumber>", strlen ( "</InstanceNumber>" ), soapSize );
    soap_send_raw ( sendbuf, "<Status>", strlen ( "<Status>" ), soapSize );
    sprintf ( tmp, "%d", addObject->status );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</Status>", strlen ( "</Status>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:AddObjectResponse>", strlen ( "</cwmp:AddObjectResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_deleteobject_res_method ( char *sendbuf, PAL_INT32_T status, PAL_INT32_T * soapSize )
{
    char tmp[16] ;
    soap_send_raw ( sendbuf, "<cwmp:DeleteObjectResponse>", strlen ( "<cwmp:DeleteObjectResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<Status>", strlen ( "<Status>" ), soapSize );
    sprintf ( tmp, "%d", status );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</Status>", strlen ( "</Status>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:DeleteObjectResponse>", strlen ( "</cwmp:DeleteObjectResponse>" ), soapSize );

    return SUCCESS_OK;
}

PAL_INT32_T add_getrpcmethods_res_method ( char *sendbuf, PAL_INT32_T * soapSize )
{
    PAL_INT32_T i, methodCount = 0;

    char tmpstr[32];
    soap_send_raw ( sendbuf, "<cwmp:GetRPCMethodsResponse>", strlen ( "<cwmp:GetRPCMethodsResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<cwmp:MethodList", strlen ( "<cwmp:MethodList" ), soapSize );

    methodCount = 16;
    sprintf ( tmpstr, "xsd:string[%d]", methodCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmpstr, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );

    for ( i = 0; i < methodCount; i++ )
    {
        soap_send_raw ( sendbuf, "<string>", strlen ( "<string>" ), soapSize );
        soap_send_raw ( sendbuf, gRPCMethodList[i], strlen ( gRPCMethodList[i] ), soapSize );
        soap_send_raw ( sendbuf, "</string>", strlen ( "</string>" ), soapSize );
    }
    soap_send_raw ( sendbuf, "</cwmp:MethodList>", strlen ( "</cwmp:MethodList>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:GetRPCMethodsResponse>", strlen ( "</cwmp:GetRPCMethodsResponse>" ), soapSize );

    return SUCCESS_OK;
}

PAL_INT32_T add_download_res_method ( char *sendbuf, DownloadResponseStruct * download, PAL_INT32_T * soapSize )
{
    char tmp[64] ;
    soap_send_raw ( sendbuf, "<cwmp:DownloadResponse>", strlen ( "<cwmp:DownloadResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<Status>", strlen ( "<Status>" ), soapSize );
    sprintf ( tmp, "%d", download->status );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</Status>", strlen ( "</Status>" ), soapSize );
    soap_send_raw ( sendbuf, "<StartTime>", strlen ( "<StartTime>" ), soapSize );
    soap_send_raw ( sendbuf, download->startTime, strlen ( download->startTime ), soapSize );
    soap_send_raw ( sendbuf, "</StartTime>", strlen ( "</StartTime>" ), soapSize );
    soap_send_raw ( sendbuf, "<CompleteTime>", strlen ( "<CompleteTime>" ), soapSize );
    soap_send_raw ( sendbuf, download->completeTime, strlen ( download->completeTime ), soapSize );
    soap_send_raw ( sendbuf, "</CompleteTime>", strlen ( "</CompleteTime>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:DownloadResponse>", strlen ( "</cwmp:DownloadResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_reboot_res_method ( char *sendbuf, PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "<cwmp:RebootResponse>", strlen ( "<cwmp:RebootResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:RebootResponse>", strlen ( "</cwmp:RebootResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_getqueuedtransfers_res_method ( char *sendbuf, TransferListStruct *getQueuedTransfer, PAL_INT32_T * soapSize )
{
    PAL_INT32_T i, transferCount = 0;
    char tmp[64];
    TransferListStruct *TransferListStructptr;
    soap_send_raw ( sendbuf, "<cwmp:GetQueuedTransfersResponse>", strlen ( "<cwmp:GetQueuedTransfersResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<cwmp:TransferList", strlen ( "<cwmp:TransferList" ), soapSize );
    TransferListStructptr = getQueuedTransfer;
    for ( i = 0; i < TRANSFER_STRUCT_NUM; i++ )
    {
        if ( TransferListStructptr->transferlist[i].commandKey[0] != 0 )
        {
            transferCount++;
        }
        else
        {
            break;
        }
    }
    sprintf ( tmp, "cwmp:QueuedTransfe[%d]", transferCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    TransferListStructptr = getQueuedTransfer;
    for ( i = 0; i < transferCount; i++ )
    {
        soap_send_raw ( sendbuf, "<QueuedTransferStruct>", strlen ( "<QueuedTransferStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<CommandKey>", strlen ( "<CommandKey>" ), soapSize );
        soap_send_raw ( sendbuf, TransferListStructptr->transferlist[i].commandKey, strlen ( TransferListStructptr->transferlist[i].commandKey ), soapSize );
        soap_send_raw ( sendbuf, "</CommandKey>", strlen ( "</CommandKey>" ), soapSize );
        soap_send_raw ( sendbuf, "<State>", strlen ( "<State>" ), soapSize );
        sprintf ( tmp, "%d", TransferListStructptr->transferlist[i].state );
        soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
        soap_send_raw ( sendbuf, "</State>", strlen ( "</State>" ), soapSize );
        soap_send_raw ( sendbuf, "</QueuedTransferStruct>", strlen ( "</QueuedTransferStruct>" ), soapSize );
    }
    soap_send_raw ( sendbuf, "</cwmp:TransferList >", strlen ( "</cwmp:TransferList >" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:GetQueuedTransfersResponse>", strlen ( "</cwmp:GetQueuedTransfersResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_scheduleinform_res_method ( char *sendbuf, PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "<cwmp:ScheduleInformResponse>", strlen ( "<cwmp:ScheduleInformResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:ScheduleInformResponse>", strlen ( "</cwmp:ScheduleInformResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_factoryreset_res_method ( char *sendbuf, PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "<cwmp:FactoryResetResponse>", strlen ( "<cwmp:FactoryResetResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:FactoryResetResponse>", strlen ( "</cwmp:FactoryResetResponse>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_upload_res_method ( char *sendbuf, UploadResponseStruct *upload, PAL_INT32_T * soapSize )
{
    char tmp[64];
    soap_send_raw ( sendbuf, "<cwmp:UploadResponse>", strlen ( "<cwmp:UploadResponse>" ), soapSize );
    soap_send_raw ( sendbuf, "<Status>", strlen ( "<Status>" ), soapSize );
    sprintf ( tmp, "%d", upload->status );
    soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    soap_send_raw ( sendbuf, "</Status>", strlen ( "</Status>" ), soapSize );
    soap_send_raw ( sendbuf, "<StartTime>", strlen ( "<StartTime>" ), soapSize );
    soap_send_raw ( sendbuf, upload->startTime, strlen ( upload->startTime ), soapSize );
    soap_send_raw ( sendbuf, "</StartTime>", strlen ( "</StartTime>" ), soapSize );
    soap_send_raw ( sendbuf, "<CompleteTime>", strlen ( "<CompleteTime>" ), soapSize );
    soap_send_raw ( sendbuf, upload->completeTime, strlen ( upload->completeTime ), soapSize );
    soap_send_raw ( sendbuf, "</CompleteTime>", strlen ( "</CompleteTime>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:UploadResponse>", strlen ( "</cwmp:UploadResponse>" ), soapSize );
    return SUCCESS_OK;

}

PAL_INT32_T add_requestdownload_method ( char *sendbuf, RequestDownloadStruct *requestDownload, PAL_INT32_T * soapSize )
{
    PAL_INT32_T i, ArgCount = 1;
    char tmp[64];
    RequestDownloadStruct *RequestDownloadStructptr;
    soap_send_raw ( sendbuf, "<cwmp:RequestDownLoad>", strlen ( "<cwmp:RequestDownLoad>" ), soapSize );
    soap_send_raw ( sendbuf, "<FileType>", strlen ( "<FileType>" ), soapSize );
    soap_send_raw ( sendbuf, requestDownload->fileType, strlen ( requestDownload->fileType ), soapSize );
    soap_send_raw ( sendbuf, "</FileType>", strlen ( "</FileType>" ), soapSize );
    soap_send_raw ( sendbuf, "<cwmp:FileTypeArg ", strlen ( "<cwmp:FileTypeArg " ), soapSize );
    for ( i = 0; i < DOWNLOAD_STRUCT_NUM; i++ )
    {
        if ( requestDownload->arglist[i].name[0] != 0 )
        {
            ArgCount++;
        }
        else
        {
            break;
        }
    }
    sprintf ( tmp, "cwmp:Arg[%d]", ArgCount );
    soap_add_attribute ( sendbuf, "xsi:type", "SOAP-ENC:Array", soapSize );
    soap_add_attribute ( sendbuf, "SOAP-ENC:arrayType", tmp, soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    RequestDownloadStructptr = requestDownload;
    for ( i = 0; i < ArgCount; i++ )
    {
        soap_send_raw ( sendbuf, "<ArgStruct>", strlen ( "<ArgStruct>" ), soapSize );
        soap_send_raw ( sendbuf, "<Name>", strlen ( "<Name>" ), soapSize );
        soap_send_raw ( sendbuf, RequestDownloadStructptr->arglist[i].name, strlen ( RequestDownloadStructptr->arglist[i].name ), soapSize );
        soap_send_raw ( sendbuf, "</Name>", strlen ( "</Name>" ), soapSize );
        soap_send_raw ( sendbuf, "<Value>", strlen ( "<Value>" ), soapSize );
        soap_send_raw ( sendbuf, RequestDownloadStructptr->arglist[i].value, strlen ( RequestDownloadStructptr->arglist[i].value ), soapSize );
        soap_send_raw ( sendbuf, "</Value>", strlen ( "</Value>" ), soapSize );
        soap_send_raw ( sendbuf, "</ArgStruct>", strlen ( "</ArgStruct>" ), soapSize );
    }
    soap_send_raw ( sendbuf, "</cwmp:FileTypeArg>", strlen ( "</cwmp:FileTypeArg>" ), soapSize );
    soap_send_raw ( sendbuf, "</cwmp:RequestDownLoad>", strlen ( "</cwmp:RequestDownLoad>" ), soapSize );

    return SUCCESS_OK;
}

char *get_type ( ValueType type )
{
    switch ( type )
    {
        case XSD_STRING:
            return "xsd:string";
        case XSD_UINT:
            return "xsd:unsignedInt";
        case XSD_INT:
            return "xsd:int";
        case XSD_BOOL:
            return "xsd:boolean";
    }
    return NULL;
}
