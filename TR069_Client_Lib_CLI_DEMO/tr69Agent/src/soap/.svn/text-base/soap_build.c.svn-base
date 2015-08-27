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
Filename: soap_build.c
Author: David
Date: 2009-05-14
Description: build the soap buf

Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#include "pal.h"
#include "soap_build.h"
#include "soap_client.h"


NameSpace *gLocalNSptr = NULL;    //local name space
SOAPFault *gSendFaultptr = NULL;  //send the fault

char current_id[HEADER_ID_LEN];
PAL_BOOL_T holdrequest;

extern char gId[HEADER_ID_LEN];
const char soap_env1[42] = "http://schemas.xmlsoap.org/soap/envelope/";
const char soap_enc1[42] = "http://schemas.xmlsoap.org/soap/encoding/";
const char soap_env2[40] = "http://www.w3.org/2003/05/soap-envelope";
const char soap_enc2[40] = "http://www.w3.org/2003/05/soap-encoding";

const char soap_xsi[42] = "http://www.w3.org/2001/XMLSchema/instance";
const char soap_xsi2[42] = "http://www.w3.org/2001/XMLSchema-instance";
const char soap_xsd[42] = "http://www.w3.org/2001/XMLSchema";
const char soap_cwmp[40] = "urn:dslforum-org:cwmp-1-0";


/***********************************************************************
Function: init_local_namespace
Description: inintial the local namespace list for the version 1.1

Return: PAL_INT32_T,
Input: void,
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-22

    2.  Modification:
        Author:
        Date:
************************************************************************/
PAL_INT32_T init_local_namespace()
{
    add_local_namespace ( "SOAP-ENV", ( char* ) soap_env1 );
    add_local_namespace ( "SOAP-ENC", ( char* ) soap_enc1 );

    add_local_namespace ( "xsi",  ( char* ) soap_xsi2 );
    add_local_namespace ( "xsd",  ( char* ) soap_xsd );
    add_local_namespace ( "cwmp", ( char* ) soap_cwmp );
    return SUCCESS_OK;

}

PAL_INT32_T add_local_namespace ( char *name, char * value )
{
    NameSpace *NSlist, *NSptr;

    NSptr = ( NameSpace* ) PAL_MALLOC ( sizeof ( NameSpace ) );
    if ( NSptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset(NSptr,0,sizeof(NameSpace));
    strcpy ( NSptr->nsname, name );
    strcpy ( NSptr->nsvalue, value );
    NSptr->next = NULL;
    NSlist = gLocalNSptr;
    if ( NSlist == NULL )
    {
        gLocalNSptr = NSptr;
        gLocalNSptr->next = NULL;
    }
    else
    {
        while ( NSlist->next )
        {
            NSlist = NSlist->next;
        }
        NSlist->next = NSptr;
    }
    return SUCCESS_OK;
}




/***********************************************************************
Function: generate_soap_packet
Description: generate soap packet with the parameters the main loop module
support

Return: PAL_INT32_T, if generate soap packet success return SUCCESSOK, else
return ERRORMALLOC or ERRORPARAMETER
Input: PacketType type,  the type of packet
       FaultType faultCode,  if it is a fault packet, this is the faultcode
       SetParameterFaultStruct *setparameterfault, if it is a fault packet
       about the setparameter request, this is the setparameterfault
       void *parameter,  the parameter for the packet
       SOAPList *soapList, the send buffer list
       PAL_INT32_T * soapSize , the size of the buffer.
Output: soapList, soapSize
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-22

    2.  Modification:
        Author:
        Date:
************************************************************************/
Result generate_soap_packet ( PacketType type, FaultType faultCode, SetParameterFaultStruct *setparameterfault,
                              void *parameter, char **sSendBuf, PAL_UINT32_T * soapSize )
{
    //SOAPList *soapListptr, *soapListtmp;
    SOAPFault* soapFault;
    char *sendbuf, *buffer;
    PAL_INT32_T ret;

    if ( *sSendBuf != NULL )
    {
        PAL_FREE ( *sSendBuf );
        *sSendBuf = NULL;
    }
    buffer = ( char* ) PAL_MALLOC ( BUFFER_SIZE );
    if ( buffer == NULL )
    {
        return MALLOC_FAILURE;
    }
    memset(buffer,0,BUFFER_SIZE);
    sendbuf = buffer;


    init_local_namespace();
    
    if ( ( ret = add_envelope_start ( sendbuf,  soapSize ) ) != SUCCESS_OK )

    {
        return PARAMETER_ERROR;
    }
    if ( ( ret = add_header_start ( sendbuf, (PAL_INT32_T *)soapSize, type ) ) != SUCCESS_OK )
    {
        return PARAMETER_ERROR;
    }

    add_header_end ( sendbuf, (PAL_INT32_T *)soapSize );

    add_body_start ( sendbuf, (PAL_INT32_T *)soapSize );

    if ( type == FAULT )
    { 
        soapFault = ( SOAPFault* ) PAL_MALLOC ( sizeof ( SOAPFault ) );
        if ( soapFault == NULL )
        {
            return MALLOC_FAILURE;
        }
        memset(soapFault,0,sizeof ( SOAPFault ));
        soapFault->detail = ( CWMPDetail * ) PAL_MALLOC ( sizeof ( CWMPDetail ) );
        if ( soapFault->detail == NULL )
        {
            return MALLOC_FAILURE;
        }
        memset(soapFault->detail,0,sizeof(CWMPDetail));
        soapFault->detail->cwmpFault = ( CWMPFault * ) PAL_MALLOC ( sizeof ( CWMPFault ) );
        if ( soapFault->detail->cwmpFault == NULL )
        {
            return MALLOC_FAILURE;
        }
        memset(soapFault->detail->cwmpFault,0,sizeof ( CWMPFault ));
        if ( build_fault_struct ( faultCode, setparameterfault, soapFault ) || envelope_fault ( sendbuf, soapFault,  (PAL_INT32_T *)soapSize ) )
        {  
            if ( soapFault->detail->cwmpFault != NULL )
            {
                PAL_FREE ( soapFault->detail->cwmpFault );
                soapFault->detail->cwmpFault = NULL;
            }
            if ( soapFault->detail != NULL )
            {
                PAL_FREE ( soapFault->detail );
                soapFault->detail = NULL;
            }
            if ( soapFault != NULL )
            {
                PAL_FREE ( soapFault );
                soapFault = NULL;
            }

            return PARAMETER_ERROR;
        }
        
        if ( soapFault->detail->cwmpFault != NULL )
        {
            PAL_FREE ( soapFault->detail->cwmpFault );
            soapFault->detail->cwmpFault = NULL;
        }
        if ( soapFault->detail != NULL )
        {
            PAL_FREE ( soapFault->detail );
            soapFault->detail = NULL;
        }
        if ( soapFault != NULL )
        {
            PAL_FREE ( soapFault );
            soapFault = NULL;
        }
    }
    else
    {
        if ( add_RPC_palload ( sendbuf, type, parameter, (PAL_INT32_T *)soapSize ) )
        {
            return PARAMETER_ERROR;
        }
	
        if ( type == TRANSFER_COMPLETE )
        {
            //pal_set_logger ( LOG_SOAP, T_LOG_INFO, LOG_CONSOLE, 1024,  "soap" );
             pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_SOAP, __func__, " \nend\n" );	
            if ( gLocalNSptr )
            {
                ns_free ( gLocalNSptr );
                gLocalNSptr = NULL;
            }
            sendbuf[*soapSize] = 0;
            *sSendBuf = sendbuf;
            return TCMAGENT_OK;
        }
    }

    add_body_end ( sendbuf,  (PAL_INT32_T *)soapSize );

    add_envelope_end ( sendbuf,  (PAL_INT32_T *)soapSize );
    if ( gLocalNSptr )
    {
        ns_free ( gLocalNSptr );
        gLocalNSptr = NULL;
    }

    sendbuf[*soapSize] = 0;
    *sSendBuf = sendbuf;
    return TCMAGENT_OK;
}

PAL_INT32_T add_envelope_start ( char *sendbuf,  PAL_UINT32_T * soapSize )
{
    char xmlHead[100];
    strcpy(xmlHead,"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
    soap_send_raw(sendbuf,xmlHead,strlen(xmlHead),soapSize);
    soap_send_raw ( sendbuf, "<", 1, soapSize );
    soap_send_raw ( sendbuf, "SOAP-ENV:Envelope", strlen ( "SOAP-ENV:Envelope" ), soapSize );

    if ( put_loacal_namespace ( sendbuf, soapSize ) )
    {
        return ERROR_PARAMETER;
    }
    soap_add_attribute ( sendbuf, "SOAP-ENV:encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/", soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );

    return SUCCESS_OK;

}
PAL_INT32_T add_header_start ( char *sendbuf,  PAL_INT32_T * soapSize, PacketType type )
{

    soap_send_raw ( sendbuf, "<", 1, soapSize );
    soap_send_raw ( sendbuf, "SOAP-ENV:Header", strlen ( "SOAP-ENV:Header" ), soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    if ( soap_add_ID ( sendbuf, soapSize, type ) )
    {
        return ERROR_PARAMETER;
    }
    return SUCCESS_OK;

}

PAL_INT32_T add_header_end ( char *sendbuf,  PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "</SOAP-ENV:Header>", strlen ( "</SOAP-ENV:Header>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_body_start ( char *sendbuf,  PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "<", 1, soapSize );
    soap_send_raw ( sendbuf, "SOAP-ENV:Body", strlen ( "SOAP-ENV:Body" ), soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_RPC_palload ( char *sendbuf, PacketType type, void *parameter,  PAL_INT32_T * soapSize )
{
    PAL_INT32_T ret;
    //PAL_INT32_T i;
    if ( type == SEND_INFORM )
    { 
        InformInfoStruct * inform = ( InformInfoStruct * ) parameter;

        ret = add_inform_method ( sendbuf, inform, soapSize );
    }
    else if ( type == TRANSFER_COMPLETE )
    {
        TransferCompleteList *transferComplete = ( TransferCompleteList * ) parameter;
        //TransferCompleteStruct *transferComplete = ( TransferCompleteStruct * ) parameter;

        ret = add_transfercomplete_method ( sendbuf, transferComplete, soapSize );
    }
    else if ( type == SET_PARAMETER_VALUES_RESP )
    {
        PAL_INT32_T *status = ( PAL_INT32_T * ) parameter;
        ret = add_setparametervalues_res_method ( sendbuf, *status, soapSize );
    }
    else if ( type == GET_PARAMETER_VALUES_RESP )
    {
        ParameterValueStruct *getParameterValue = ( ParameterValueStruct * ) parameter;
        ret = add_getparametervalues_res_method ( sendbuf, getParameterValue, soapSize );
    }
    else if ( type == GET_PARAMETER_NAMES_RESP )
    {
        ParameterInfoStruct* getParameterNames = ( ParameterInfoStruct* ) parameter;
        ret = add_getparameternames_res_method ( sendbuf, getParameterNames, soapSize );
    }
    else if ( type == SET_PARAMETER_ATTRIBUTES_RESP )
    {
        ret = add_setparameterattributes_res_method ( sendbuf, soapSize );
    }
    else if ( type == GET_PARAMETER_ATTRIBUTES_RESP )
    {
        ParameterAttributeStruct * getParameterAttribute = ( ParameterAttributeStruct * ) parameter;
        ret = add_getparameterattributes_res_method ( sendbuf, getParameterAttribute, soapSize );
    }
    else if ( type == ADD_OBJECT_RESP )
    {
        AddObjectResponseStruct *addObject = ( AddObjectResponseStruct * ) parameter;
        ret = add_addobject_res_method ( sendbuf, addObject, soapSize );
    }
    else if ( type == DELETE_OBJECT_RESP )
    {
        PAL_INT32_T *status = ( PAL_INT32_T * ) parameter;
        ret = add_deleteobject_res_method ( sendbuf, *status, soapSize );
    }
    else if ( type == GET_RPCMETHODS_RESP )
    {
        ret = add_getrpcmethods_res_method ( sendbuf, soapSize );
    }
    else if ( type == DOWNLOAD_RESP )
    {
        DownloadResponseStruct * download = ( DownloadResponseStruct * ) parameter;
        ret = add_download_res_method ( sendbuf, download, soapSize );
    }
    else if ( type == REBOOT_RESP )
    {
        ret = add_reboot_res_method ( sendbuf, soapSize );
    }
    else if ( type == GET_QUEUEDTRANSFERS_RESP )
    {
        TransferListStruct *getQueuedTransfer = ( TransferListStruct * ) parameter;
        ret = add_getqueuedtransfers_res_method ( sendbuf, getQueuedTransfer, soapSize );
    }
    else if ( type == SCHEDULE_INFORM_RESP )
    {
        ret = add_scheduleinform_res_method ( sendbuf, soapSize );
    }
    else if ( type == FACTORY_RESET_RESP )
    {
        ret = add_factoryreset_res_method ( sendbuf, soapSize );
    }
    else if ( type == UPLOAD_RESP )
    {
        UploadResponseStruct *upload = ( UploadResponseStruct * ) parameter;
        ret = add_upload_res_method ( sendbuf, upload, soapSize );
    }
    else if ( type == REQUEST_DOWNLOAD )
    {
        RequestDownloadStruct *requestDownload = ( RequestDownloadStruct * ) parameter;
        ret = add_requestdownload_method ( sendbuf, requestDownload, soapSize );
    }
    else
    {
        return ERROR_PARAMETER;
    }

    return ret;
}

PAL_INT32_T add_body_end ( char *sendbuf,  PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "</SOAP-ENV:Body>", strlen ( "</SOAP-ENV:Body>" ), soapSize );
    return SUCCESS_OK;
}

PAL_INT32_T add_envelope_end ( char *sendbuf,  PAL_INT32_T * soapSize )
{
    soap_send_raw ( sendbuf, "</", 2, soapSize );
    soap_send_raw ( sendbuf, "SOAP-ENV:Envelope", strlen ( "SOAP-ENV:Envelope" ), soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    return SUCCESS_OK;
}
PAL_INT32_T build_fault_struct ( FaultType faultcode, SetParameterFaultStruct *setparameterfault, SOAPFault *SOAPFault )
{
    strcpy ( SOAPFault->faultCode, "Client" );
    strcpy ( SOAPFault->faultString, "CWMP fault" );
    SOAPFault->faultActor[0] = 0;

    SOAPFault->detail->cwmpFault->FaultCode = faultcode;
   
    if ( get_fault_string ( faultcode ) == NULL )
    {
        return ERROR_PARAMETER;
    }
    else
    {
    strcpy ( SOAPFault->detail->cwmpFault->FaultString, get_fault_string ( faultcode ) );
    }
    
    if(setparameterfault)
    {
    SOAPFault->detail->cwmpFault->SetParameterValuesFault = setparameterfault;
    }
    else
    {
        SOAPFault->detail->cwmpFault->SetParameterValuesFault = NULL;
    }
    return SUCCESS_OK;

}

PAL_INT32_T envelope_fault ( char *sendbuf, SOAPFault *SOAPFault,  PAL_INT32_T * soapSize )
{
    char tmp[8];
    SetParameterFaultStruct *SetParameterFaultStructPtr;
    soap_send_raw ( sendbuf, "<", 1, soapSize );
    soap_send_raw ( sendbuf, "SOAP-ENV:Fault", strlen ( "SOAP-ENV:Fault" ), soapSize );
    soap_send_raw ( sendbuf, ">", 1, soapSize );
    //sprintf(tmp, "%d", SOAPFault->detail->cwmpFault->FaultCode);
    if ( SOAPFault->faultCode[0] != 0 )
    {
        soap_send_raw ( sendbuf, "<faultcode>", strlen ( "<faultcode>" ), soapSize );
        soap_send_raw ( sendbuf, SOAPFault->faultCode, strlen ( SOAPFault->faultCode ), soapSize );
        soap_send_raw ( sendbuf, "</faultcode>", strlen ( "</faultcode>" ), soapSize );
    }
    else
    {
        return ERROR_PARAMETER;
    }

    if ( SOAPFault->faultString[0] != 0 )
    {
        soap_send_raw ( sendbuf, "<faultstring>", strlen ( "<faultstring>" ), soapSize );
        soap_send_raw ( sendbuf, SOAPFault->faultString, strlen ( SOAPFault->faultString ), soapSize );
        soap_send_raw ( sendbuf, "</faultstring>", strlen ( "</faultstring>" ), soapSize );
    }
    else
    {
        return ERROR_PARAMETER;
    }

    if ( SOAPFault->faultActor[0] != 0 )
    {
        soap_send_raw ( sendbuf, "<faultactor>", strlen ( "<faultactor>" ), soapSize );
        soap_send_raw ( sendbuf, SOAPFault->faultActor, strlen ( SOAPFault->faultActor ), soapSize );
        soap_send_raw ( sendbuf, "</faultactor>", strlen ( "</faultactor>" ), soapSize );
    }

    if ( SOAPFault->detail )
    {
        soap_send_raw ( sendbuf, "<detail>", strlen ( "<detail>" ), soapSize );
        if ( SOAPFault->detail->cwmpFault )
        {
            soap_send_raw ( sendbuf, "<cwmp:Fault>", strlen ( "<cwmp:Fault>" ), soapSize );
            if ( SOAPFault->detail->cwmpFault->FaultCode )
            {
                soap_send_raw ( sendbuf, "<FaultCode>", strlen ( "<FaultCode>" ), soapSize );
                sprintf(tmp,"%d",SOAPFault->detail->cwmpFault->FaultCode);
                soap_send_raw ( sendbuf, tmp, strlen (tmp), soapSize );
                soap_send_raw ( sendbuf, "</FaultCode>", strlen ( "</FaultCode>" ), soapSize );
            }
            if ( SOAPFault->detail->cwmpFault->FaultString[0] != 0 )
            {
                soap_send_raw ( sendbuf, "<FaultString>", strlen ( "<FaultString>" ), soapSize );
                soap_send_raw ( sendbuf, SOAPFault->detail->cwmpFault->FaultString, strlen ( SOAPFault->detail->cwmpFault->FaultString ), soapSize );
                soap_send_raw ( sendbuf, "</FaultString>", strlen ( "</FaultString>" ), soapSize );
            }
            SetParameterFaultStructPtr = SOAPFault->detail->cwmpFault->SetParameterValuesFault;
            while ( SetParameterFaultStructPtr )
            {
                soap_send_raw ( sendbuf, "<SetParameterValuesFault>", strlen ( "<SetParameterValuesFault>" ), soapSize );

                soap_send_raw ( sendbuf, "<ParameterName>", strlen ( "<ParameterName>" ), soapSize );
                soap_send_raw ( sendbuf, SetParameterFaultStructPtr->ParameterName, strlen ( SetParameterFaultStructPtr->ParameterName ), soapSize );
                soap_send_raw ( sendbuf, "</ParameterName>", strlen ( "</ParameterName>" ), soapSize );

                soap_send_raw ( sendbuf, "<FaultCode>", strlen ( "<FaultCode>" ), soapSize );
                sprintf ( tmp, "%d", SetParameterFaultStructPtr->FaultCode );
                soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
                soap_send_raw ( sendbuf, "</FaultCode>", strlen ( "</FaultCode>" ), soapSize );


                soap_send_raw ( sendbuf, "<FaultString>", strlen ( "<FaultString>" ), soapSize );
                soap_send_raw ( sendbuf, SetParameterFaultStructPtr->FaultString, strlen ( SetParameterFaultStructPtr->FaultString ), soapSize );
                soap_send_raw ( sendbuf, "</FaultString>", strlen ( "</FaultString>" ), soapSize );

                soap_send_raw ( sendbuf, "</SetParameterValuesFault>", strlen ( "</SetParameterValuesFault>" ), soapSize );

                SetParameterFaultStructPtr = SetParameterFaultStructPtr->next;

            }
            soap_send_raw ( sendbuf, "</cwmp:Fault>", strlen ( "</cwmp:Fault>" ), soapSize );
        }
        soap_send_raw ( sendbuf, "</detail>", strlen ( "</detail>" ), soapSize );
    }

    soap_send_raw ( sendbuf, "</SOAP-ENV:Fault>", strlen ( "</SOAP-ENV:Fault>" ), soapSize );
    return SUCCESS_OK;

}


PAL_INT32_T soap_send_raw ( char *sendbuf, const char *s, PAL_SIZE_T n, PAL_INT32_T *soapSize )
{
    if ( !n )
    {
        return ERROR_PARAMETER;
    }
    else
    {
        memcpy ( sendbuf + *soapSize, s, n );
        *soapSize += n;
    }
    return SUCCESS_OK;

}

PAL_INT32_T put_loacal_namespace ( char *sendbuf,  PAL_INT32_T *soapSize )
{
    NameSpace *NSptr;
    char tmp[100];
    NSptr = gLocalNSptr;
    while ( NSptr )
    {
        if ( NSptr->nsname && NSptr->nsvalue )
        {
            sprintf ( tmp, " xmlns:%s=\"%s\"", NSptr->nsname, NSptr->nsvalue );
            soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
        }
        else
        {
            return ERROR_PARAMETER;
        }
        NSptr = NSptr->next;
    }
    return SUCCESS_OK;
}

PAL_INT32_T soap_add_attribute ( char *sendbuf,  char *attrname, char *attrvalue, PAL_INT32_T *soapSize )
{
    char tmp[100];
    if ( attrname && attrvalue )
    {
        sprintf ( tmp, " %s=\"%s\"", attrname, attrvalue );
        soap_send_raw ( sendbuf, tmp, strlen ( tmp ), soapSize );
    }

    return SUCCESS_OK;
}

PAL_INT32_T soap_add_ID ( char *sendbuf,  PAL_INT32_T *soapSize, PacketType type )
{
    char id[64];
    PAL_UINT32_T tmp;

    if ( type == SEND_INFORM || type == TRANSFER_COMPLETE || type == REQUEST_DOWNLOAD)
    {
        srand ( time(NULL) );
        tmp = rand();
        sprintf ( id, "%d", tmp );
    }
    else
    {
        //get the current id
        strcpy ( id, gId );
    }

    if ( soap_send_raw ( sendbuf, "<", 1, soapSize ) || soap_send_raw ( sendbuf, "cwmp:ID", strlen ( "cwmp:ID" ), soapSize )
            || soap_add_attribute ( sendbuf, "SOAP-ENV:mustUnderstand", "1", soapSize )
            || soap_send_raw ( sendbuf, ">", 1, soapSize )
            || soap_send_raw ( sendbuf, id, strlen ( id ), soapSize )
            || soap_send_raw ( sendbuf, "</cwmp:ID>", strlen ( "</cwmp:ID>" ), soapSize ) )
    {
        return ERROR_PARAMETER;
    }

    return SUCCESS_OK;
}

PAL_INT32_T build_misunderstand_fault_struct ( SOAPFault *soapFault )
{
    strcpy ( soapFault->faultCode, "SOAP-ENV:MustUnderstand" );
    strcpy ( soapFault->faultString, "SOAP Must Understand Error" );
    soapFault->faultActor[0] = 0;
    //soapFault->detail = NULL;
    return SUCCESS_OK;
}

PAL_INT32_T build_mustunderstand_fault ( char *elename, char *elenstr, char **sSendBuf, PAL_INT32_T * soapSize )
{
    char *sendbuf, *buf;
    SOAPFault* soapFault;
    PAL_INT32_T ret;


    if ( *sSendBuf != NULL )
    {
        PAL_FREE ( *sSendBuf );
        *sSendBuf = NULL;
    }

    buf = ( char* ) PAL_MALLOC ( BUFFER_SIZE );
    if ( buf == NULL )
    {
        return ERROR_MALLOC;
    }
    memset(buf,0,BUFFER_SIZE);
    sendbuf = buf;

    init_local_namespace();
    if ( ( ret = add_envelope_start ( sendbuf,  soapSize ) ) != SUCCESS_OK )
    {
        return ret;
    }
    if ( ( ret = add_header_start ( sendbuf, soapSize, FAULT ) ) != SUCCESS_OK )
    {
        return ret;
    }
    add_header_end ( sendbuf, soapSize );
    add_body_start ( sendbuf,  soapSize );
    soapFault = ( SOAPFault* ) PAL_MALLOC ( sizeof ( SOAPFault ) );
    if ( soapFault == NULL )
    {
        return ERROR_MALLOC;
    }
    memset(soapFault,0,sizeof ( SOAPFault ));
    soapFault->detail = NULL;
    if ( build_misunderstand_fault_struct ( soapFault ) ||
            envelope_fault ( sendbuf, soapFault,  soapSize ) )
    {
        if ( soapFault )
        {
            PAL_FREE ( soapFault );
            soapFault = NULL;
        }
        return ERROR_PARAMETER;
    }
    if ( soapFault )
    {
        PAL_FREE ( soapFault );
        soapFault = NULL;
    }
    add_body_end ( sendbuf,  soapSize );
    add_envelope_end ( sendbuf,  soapSize );
    sendbuf[*soapSize] = 0;
    *sSendBuf = sendbuf;
    return SUCCESS_OK;

}

PAL_INT32_T build_versionmismatch_fault_struct ( SOAPFault *soapFault )
{
    strcpy ( soapFault->faultCode, "SOAP-ENV:VersionMismatch" );
    strcpy ( soapFault->faultString, "SOAP Version Mismatch Error" );
    soapFault->faultActor[0] = 0;
    //soapFault->detail = NULL;
    return SUCCESS_OK;
}

PAL_INT32_T build_versionmismatch_fault ( char **sSendBuf, PAL_INT32_T * soapSize )
{
    char *sendbuf, *buf;
    SOAPFault* soapFault;
    PAL_INT32_T ret;

    if ( *sSendBuf != NULL )
    {
        PAL_FREE ( *sSendBuf );
        *sSendBuf = NULL;
    }

    buf = ( char* ) PAL_MALLOC ( BUFFER_SIZE );
    if ( buf == NULL )
    {
        return ERROR_MALLOC;
    }
    memset(buf,0,BUFFER_SIZE);
    sendbuf = buf;
    init_local_namespace();
    if ( ( ret = add_envelope_start ( sendbuf,  soapSize ) ) != SUCCESS_OK )
    {
        return ret;
    }
    if ( ( ret = add_header_start ( sendbuf, soapSize, FAULT ) ) != SUCCESS_OK )
    {
        return ret;
    }
    add_header_end ( sendbuf, soapSize );
    add_body_start ( sendbuf,  soapSize );
    soapFault = ( SOAPFault* ) PAL_MALLOC ( sizeof ( SOAPFault ) );
    if ( soapFault == NULL )
    {
        return ERROR_MALLOC;
    }
    memset(soapFault,0,sizeof(SOAPFault));
    soapFault->detail = NULL;
    if ( build_versionmismatch_fault_struct ( soapFault ) ||
            envelope_fault ( sendbuf, soapFault,  soapSize ) )
    {
        if ( soapFault )
        {
            PAL_FREE ( soapFault );
            soapFault = NULL;
        }
        return ERROR_PARAMETER;
    }
    if ( soapFault )
    {
        PAL_FREE ( soapFault );
        soapFault = NULL;
    }
    add_body_end ( sendbuf,  soapSize );
    add_envelope_end ( sendbuf,  soapSize );
    sendbuf[*soapSize] = 0;
    *sSendBuf = sendbuf;
    return SUCCESS_OK;
}

char* get_fault_string ( FaultType faultcode )
{
    switch ( faultcode )
    {
        case tMethodNotSupported:
            return "Method Not Supported";
        case tRequestDenied:
            return "Request Denied";
        case tInternalError:
            return "Internal Error";
        case tInvalidArguments:
            return "Invalid Arguments";
        case tResourcesExceeded:
            return "Resource Exceed";
        case tInvalidParameterName:
            return "Invalid Parametername";
        case tInvalidParameterType:
            return "Invalid Paramtertype";
        case tInvalidParameterValue:
            return "Invalid Parametervalue";
        case tAttemptSetNonWritable:
            return "Parameter Readonly";
        case tNotificationRequestRejected:
            return "Notification Request Rejected";
        case tDownloadFailure:
            return "Download Failure";
        case tUploadFailure:
            return "Upload Failure";
        case tFileTransferAuthFailure:
            return "File Transfer Authority Failure";
        case tUnsupportedProtocolTransfer:
            return "NoT Support Protocol Transfer";
        case tOK:
            return "OK";
    }

    return "Internal Error";

}

void ns_free ( NameSpace *gLocalNSptr )
{
    NameSpace * tmp, *pre;
    pre = tmp = gLocalNSptr;
    while ( tmp )
    {
        pre = tmp;
        tmp = tmp->next;
        PAL_FREE ( pre );
        pre = NULL;
    }
    gLocalNSptr = NULL;
}

void inform_free ( InformInfoStruct *inform )
{
    ParameterValueStruct *ParameterValueStructptr, *tmp;
    if ( inform )
    {
        tmp = ParameterValueStructptr = inform->parameterList;
        while ( ParameterValueStructptr )
        {
            tmp = ParameterValueStructptr;
            ParameterValueStructptr = ParameterValueStructptr->next;
            PAL_FREE ( tmp );
            tmp = NULL;
        }
        inform->parameterList = NULL;
        PAL_FREE ( inform );
        inform = NULL;
    }
}

void getParameterValues_free ( ParameterValueStruct **getParameterValueP )
{
    ParameterValueStruct *ParameterValueStructptr, *tmp;
    if ( *getParameterValueP )
    {
        tmp = ParameterValueStructptr = *getParameterValueP;
        while ( ParameterValueStructptr )
        {
            tmp = ParameterValueStructptr;
            ParameterValueStructptr = ParameterValueStructptr->next;
            if(tmp->value != NULL)
			PAL_FREE(tmp->value);
            PAL_FREE ( tmp );
            tmp = NULL;
        }
        *getParameterValueP = NULL;
    }
}

void getParameterNames_free ( ParameterInfoStruct** getParameterNamesP )
{
    ParameterInfoStruct *ParameterInfoStructptr, *tmp;
    if ( *getParameterNamesP )
    {
        tmp = ParameterInfoStructptr = *getParameterNamesP;
        while ( ParameterInfoStructptr )
        {
            tmp = ParameterInfoStructptr;
            ParameterInfoStructptr = ParameterInfoStructptr->next;
            PAL_FREE ( tmp );
            tmp = NULL;
        }
        *getParameterNamesP = NULL;
    }
}

void getPrameterAttribute_free ( ParameterAttributeStruct ** getParameterAttributeP )
{
    ParameterAttributeStruct *ParameterAttributeStructptr, *tmp;
    if ( *getParameterAttributeP )
    {
        tmp = ParameterAttributeStructptr = *getParameterAttributeP;
        while ( ParameterAttributeStructptr )
        {
            tmp = ParameterAttributeStructptr;
            ParameterAttributeStructptr = ParameterAttributeStructptr->next;
            PAL_FREE ( tmp );
            tmp = NULL;
        }
        *getParameterAttributeP = NULL;
    }
}

void addObject_free ( AddObjectResponseStruct *addObject )
{
    if ( addObject )
    {
        PAL_FREE ( addObject );
        addObject = NULL;
    }
}

void download_free ( DownloadResponseStruct * download )
{
    if ( download )
    {
        PAL_FREE ( download );
        download = NULL;
    }
}

void getQueuedTransfer_free ( TransferListStruct *getQueuedTransfer )
{
    if ( getQueuedTransfer )
    {
        PAL_FREE ( getQueuedTransfer );
        getQueuedTransfer = NULL;
    }
}

void upload_free ( UploadResponseStruct *upload )
{
    if ( upload )
    {
        PAL_FREE ( upload );
        upload = NULL;
    }
}

void requestDownload_free ( RequestDownloadStruct *requestDownload )
{
    if ( requestDownload )
    {
        PAL_FREE ( requestDownload );
        requestDownload = NULL;
    }
}
