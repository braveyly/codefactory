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
Filename: soap_parser.c
Author: David
Date: 2009-05-14
Description: this file is used to parser a soap packet, and output the soap
struct, which contain fault or the RPC method and parameters

Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#include "pal.h"
#include "xml_public.h"
#include "soap_parser.h"
#include "soap_build.h"

char gId[HEADER_ID_LEN];
static PAL_INT32_T gHoldRequest = 0;
static PAL_INT32_T gNomoreRequest = 0;
static PAL_INT32_T gMaxEnvelopes = 0;


extern NameSpace *gLocalNSptr;    //local name space
extern SOAPFault *gSendFaultptr;  //send the fault

extern char current_id[HEADER_ID_LEN];
extern PAL_BOOL_T holdrequest;

extern const char soap_env1[42];
extern const char soap_enc1[42];
extern const char soap_env2[40];
extern const char soap_enc2[40];

extern const char soap_xsi[42];
extern const char soap_xsi2[42];
extern const char soap_xsd[42];
extern const char soap_cwmp[40];


XmlType    /* O - Data type */
type_cb ( XmlNode *node );  /* I - Element node */

/* function declarations */
PAL_INT32_T parse_node ( XmlNode *, SOAP *, char **, PAL_INT32_T * );
PAL_INT32_T add_remote_namespace ( char *name, char *value, SOAP * soap );

/***********************************************************************
Function: handle_soap_packet
Description:  parser the soap buffer, and get the rpc method or fault
information. if not success, generate the fault buffer and add it to the soap
list.

Return: PAL_INT32_T,
Input: char *buf, the buffer for the soap packet
       SOAP * soap,
       SOAPList *soapList, if parser error, generate and add the buffer to the
       soap list.
       PAL_INT32_T * soapSize, the size of the fault buffer.
Output: soapList, soapSize
Others:

Histroy:
************************************************************************/
Result handle_soap_packet ( char *buf, PAL_UINT32_T bufLength, SOAP * soap, char **sSoapBuf, PAL_UINT32_T * soapSize )
{
    XmlNode * xmlTree = NULL;
    PAL_INT32_T ret = 0;
    // pal_set_logger ( LOG_SOAP, T_LOG_INFO, LOG_CONSOLE, 1024,  "soap" );

    if ( *sSoapBuf != NULL )
    {
        PAL_FREE ( *sSoapBuf );
        *sSoapBuf = NULL;
    }
    if ( soap == NULL )
    {
        return POINTER_FAILURE;
    }

    free_soap ( soap );

    if ( buf == NULL )
    {
        if ( *sSoapBuf != NULL )
        {
            PAL_FREE ( *sSoapBuf );
            *sSoapBuf = NULL;
            *soapSize = 0;
        }
        return EMPTY_PACKET;
    }
    else
    {
        xmlTree = xml_load_string ( NULL,  buf, type_cb );

        if ( xmlTree == NULL )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_SOAP, __func__, " XML error\n" );
            if ( *sSoapBuf != NULL )
            {
                PAL_FREE ( *sSoapBuf );
                *sSoapBuf = NULL;
                *soapSize = 0;
            }
            return SOAP_PARSER_ERROR;
        }
    }
    ret = xml2soap ( xmlTree, soap, sSoapBuf, ( PAL_INT32_T * ) soapSize );
    if ( ret != SUCCESS_OK )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_SOAP, __func__, " parser error\n" );
        xml_delete ( xmlTree );
        return ret;  //error
    }
    else
    {
        xml_delete ( xmlTree );
        ret = soap_parser ( soap, sSoapBuf, ( PAL_INT32_T * ) soapSize );
        if ( ret == TCMAGENT_OK )
        {
            if ( *sSoapBuf != NULL )
            {
                PAL_FREE ( *sSoapBuf );
                *sSoapBuf = NULL;
                *soapSize = 0;
            }
            //soapptr = soap;
            return TCMAGENT_OK;  //error
        }
        else if ( ret == FAULT_PACKETS )
        {
            if ( *sSoapBuf != NULL )
            {
                PAL_FREE ( *sSoapBuf );
                *sSoapBuf = NULL;
                *soapSize = 0;
            }
            return FAULT_PACKET;
        }
        else
        {
            return SOAP_PARSER_ERROR;
        }

    }
}


/***********************************************************************
Function: xml2soap
Description: switch the xml node to the soap struct, if it is not a valid soap
packet, then generate the fual buf.

Return: PAL_INT32_T,
Input: XmlNode *Node,
       SOAP * soap,
       char *faultBuf,
Output: soap, faultBuf
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-14
************************************************************************/
PAL_INT32_T xml2soap ( XmlNode *Node, SOAP * soap, char **sSoapBuf, PAL_INT32_T * soapSize )
{
    XmlNode *searchNode = NULL;
    int ret = 0;
    searchNode = Node;

    if ( *sSoapBuf != NULL )
    {
        PAL_FREE ( *sSoapBuf );
        *sSoapBuf = NULL;
    }
    while ( searchNode != NULL )
    {
        if ( ( ret = parse_node ( searchNode, soap, sSoapBuf, soapSize ) ) == SUCCESS_OK )
        {
            searchNode = xml_walk_next ( searchNode, Node, XML_DESCEND );
        }
        else
        {
            return ret;
        }
    }
    if ( *sSoapBuf != NULL )
    {
        PAL_FREE ( *sSoapBuf );
        *sSoapBuf = NULL;
        *soapSize = 0;
    }
    return SUCCESS_OK;
}


/***********************************************************************
Function: soap_parser
Description: parser the soap struct to get the fault information or the rpc
method and parameters. if parser error, then add the fault buffer to the soap
list.

Return: PAL_INT32_T, if parser success, return SUCCESSOK, else return ERRORPARSER
Input: SOAP * soap,
       SOAPList *soapList, if parser error, generate and add the buffer to the
       soap list.
Output: soap, SOAPList
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-14

    2.  Modification:
        Author:
        Date:
************************************************************************/
PAL_INT32_T soap_parser ( SOAP * soap, char **sSoapBuf, PAL_INT32_T * soapSize )
{
    SOAPElement* SOAPElementptr = NULL;

    SOAPHeader *header = NULL;
    char *tmpns, tmp[64];


    if ( soap->version != 1 && soap->version != 2 )
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_SOAP, __func__, " versionmismatch\n" );
        build_versionmismatch_fault ( sSoapBuf, soapSize );
        return ERROR_PARSER;
    }
    SOAPElementptr = soap->element;
    while ( SOAPElementptr )
    {
        strcpy ( tmp, ( char * ) get_remote_namespace ( SOAPElementptr->nstr, soap ) );
        if ( !strcmp ( SOAPElementptr->name, "ID" ) && !strcmp ( tmp, soap_cwmp ) )
        {

            header = ( SOAPHeader * ) PAL_MALLOC ( sizeof ( SOAPHeader ) );
            if ( header == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( header, 0, sizeof ( SOAPHeader ) );
            strcpy ( header->cwmpID, SOAPElementptr->data.string );
            strcpy ( gId, SOAPElementptr->data.string );
            soap->header = header;
        }
        if ( !strcmp ( SOAPElementptr->name, "HoldRequests" ) && !strcmp ( tmp, soap_cwmp ) )
        {
            if ( SOAPElementptr->data.string )
            {
                if ( 0 == strcmp ( SOAPElementptr->data.string, "1" ) || 0 == strcasecmp ( SOAPElementptr->data.string, "true" ) )
                {
                    header->cwmpHoldRequests = 1;
                }
                else
                {
                    header->cwmpHoldRequests = 0;
                }
                gHoldRequest = header->cwmpHoldRequests;
            }
            else
            {
                header->cwmpHoldRequests = 0;
                gHoldRequest = 0;
            }
        }
        if ( !strcmp ( SOAPElementptr->name, "NoMoreRequests" ) && !strcmp ( tmp, soap_cwmp ) )
        {
            if ( SOAPElementptr->data.string )
            {
                header->cwmpNoMoreRequests = atoi ( SOAPElementptr->data.string );
                gHoldRequest = header->cwmpNoMoreRequests;
            }
            else
            {
                header->cwmpNoMoreRequests = 0;
                gHoldRequest = 0;
            }
        }
        if ( !strcmp ( SOAPElementptr->name, "Body" ) && !strcmp ( tmp, soap->version == 1 ? soap_env1 : soap_env2 ) )
        {
            break;
        }
        SOAPElementptr = SOAPElementptr->next;
    } /* end of while */

    if ( SOAPElementptr == NULL )
    {
        return ERROR_PARSER;
    }
    else
    {
        SOAPElementptr = SOAPElementptr->next;
        tmpns = ( char * ) PAL_MALLOC ( NAMESPACE_LEN );
        if ( tmpns == NULL )
        {
            return ERROR_MALLOC;
        }
        memset ( tmpns, 0, NAMESPACE_LEN );
        strcpy ( tmpns, ( char * ) get_remote_namespace ( SOAPElementptr->nstr, soap ) );

        if ( !strcmp ( SOAPElementptr->name, "InformResponse" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tInformResponse;
            parse_inform_packet ( SOAPElementptr, soap );

        }
        else if ( !strcmp ( SOAPElementptr->name, "TransferCompleteResponse" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tTransferCompleteResponse;
            parse_transfercomplete_packet ( SOAPElementptr, soap );

        }
        else if ( !strcmp ( SOAPElementptr->name, "SetParameterValues" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tSetParameterValues;
            parse_SetParameterValues_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "GetParameterValues" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tGetParameterValues;
            parse_GetParameterValues_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "GetParameterNames" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tGetParameterNames;
            parse_GetParameterNames_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "SetParameterAttributes" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tSetParameterAttributes;
            parse_SetParameterAttributes_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "GetParameterAttributes" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tGetParameterAttributes;
            parse_GetParameterAttributes_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "AddObject" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tAddObject;
            parse_AddObject_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "DeleteObject" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tDeleteObject;
            parse_DeleteObject_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "GetRPCMethods" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tGetRPCMethods;
            parse_GetRPCMethods_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "Download" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tDownload;
            parse_Download_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "Reboot" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tReboot;
            parse_Reboot_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "GetQueuedTransfer" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tGetQueuedTransfer;
            parse_GetQueuedTransfers_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "ScheduleInform" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tScheduleInform;
            parse_ScheduleInform_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "FactoryReset" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tFactoryReset;
            parse_FactoryReset_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "Upload" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tUpload;
            parse_Upload_packet ( SOAPElementptr, soap );
        }
        else if ( !strcmp ( SOAPElementptr->name, "RequestDownloadResponse" ) && !strcmp ( tmpns, soap_cwmp ) )
        {
            strcpy ( soap->method_name, SOAPElementptr->name );
            soap->rpcType = tRequestDownloadResponse;
        }
        else if ( !strcmp ( SOAPElementptr->name, "Fault" ) && !strcmp ( tmpns, soap->version == 1 ? soap_env1 : soap_env2 ) )
        {
            soap->method_name[0] = 0;
            soap->rpcType = tFault;
            parse_fault_packet ( SOAPElementptr, soap );
            return FAULT_PACKETS;
        }
        else
        {
            if ( tmpns )
            {
                PAL_FREE ( tmpns );
                tmpns = NULL;
            }
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_SOAP, __func__, " no method\n" );
            return ERROR_PARSER;
        }
        if ( tmpns )
        {
            PAL_FREE ( tmpns );
            tmpns = NULL;
        }
    }

    return SUCCESS_OK;
}



PAL_INT32_T get_holdrequest ( void )
{
    return gHoldRequest;
}


PAL_INT32_T get_nomorerequest ( void )
{
    return gNomoreRequest;
}

XmlType    /* O - Data type */
type_cb ( XmlNode *node )  /* I - Element node */
{
    const char *type;   /* Type string */


    /*
     * You can lookup attributes and/or use the element name, hierarchy, etc...
     */

    if ( ( type = xml_element_get_attr ( node, "xsi:type" ) ) == NULL )
        type = node->value.element.name;

    //if (!strcmp(type, "xsd:unsignedInt") || !strcmp(type, "xsd:int"))
    //return (XML_INTEGER);
    //else
    return ( XML_TEXT );
}

const char *    /* O - Whitespace string or NULL */
whitespace_cb ( XmlNode *node, /* I - Element node */
                int         where ) /* I - Open or close tag? */
{
    XmlNode *parent;  /* Parent node */
    int  level;   /* Indentation level */
    const char *name;   /* Name of element */
    static const char *tabs = "\t\t\t\t\t\t\t\t";
    /* Tabs for indentation */


    /*
     * We can conditionally break to a new line before or after any element.
     * These are just common HTML elements...
     */

    name = node->value.element.name;

    if ( !strcmp ( name, "html" ) || !strcmp ( name, "head" ) || !strcmp ( name, "body" ) ||
            !strcmp ( name, "pre" ) || !strcmp ( name, "p" ) ||
            !strcmp ( name, "h1" ) || !strcmp ( name, "h2" ) || !strcmp ( name, "h3" ) ||
            !strcmp ( name, "h4" ) || !strcmp ( name, "h5" ) || !strcmp ( name, "h6" ) )
    {
        /*
         * Newlines before open and after close...
         */

        if ( where == XML_WS_BEFORE_OPEN || where == XML_WS_AFTER_CLOSE )
            return ( "\n" );
    }
    else if ( !strcmp ( name, "dl" ) || !strcmp ( name, "ol" ) || !strcmp ( name, "ul" ) )
    {
        /*
         * Put a newline before and after list elements...
         */

        return ( "\n" );
    }
    else if ( !strcmp ( name, "dd" ) || !strcmp ( name, "dt" ) || !strcmp ( name, "li" ) )
    {
        /*
         * Put a tab before <li>'s, <dd>'s, and <dt>'s, and a newline after them...
         */

        if ( where == XML_WS_BEFORE_OPEN )
            return ( "\t" );
        else if ( where == XML_WS_AFTER_CLOSE )
            return ( "\n" );
    }
    else if ( !strncmp ( name, "?xml", 4 ) )
    {
        return ( NULL );
    }
    else if ( where == XML_WS_BEFORE_OPEN ||
              ( ( !strcmp ( name, "choice" ) || !strcmp ( name, "option" ) ) &&
                where == XML_WS_BEFORE_CLOSE ) )
    {
        for ( level = -1, parent = node->parent;
                parent;
                level ++, parent = parent->parent );

        if ( level > 8 )
            level = 8;
        else if ( level < 0 )
            level = 0;

        return ( tabs + 8 - level );
    }
    else if ( where == XML_WS_AFTER_CLOSE ||
              ( ( !strcmp ( name, "group" ) || !strcmp ( name, "option" ) ||
                  !strcmp ( name, "choice" ) ) &&
                where == XML_WS_AFTER_OPEN ) )
        return ( "\n" );
    else if ( where == XML_WS_AFTER_OPEN && !node->child )
        return ( "\n" );

    /*
     * Return NULL for no added whitespace...
     */

    return ( NULL );
}


/***********************************************************************
Function: parse_node
Description: parse the xml node, if it has soap attribute, add the soap
elements and attributes to the soap struct.

Return: PAL_INT32_T, if parser not ok, return ERRORPARSER, else return SUCCESSOK
Input: XmlNode *Node, the Node in the XML tree
       SOAP * soap,
       SOAPList *soapList,  if the SOAP format is not ok, generate a fault
       buffer to the soapList
       char * soapSize, the size of the fault buffer
Output: soap, soapList, soapSize
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-22

    2.  Modification: change char *soapSize to PAL_INT32_T *soapSize
        Author: Hook Guo
        Date: 2009-06-22
************************************************************************/
PAL_INT32_T parse_node ( XmlNode *Node, SOAP * soap, char **sSoapBuf, PAL_INT32_T *soapSize )
{
    PAL_INT32_T attrCount, i;
    char tmp[64], *tmpptr, *charptr;
    EleSoap *EleSoapptr;
    SOAPElement *SOAPElementptr, *SOAPElementtmp;
    SOAPAttribute *SOAPAttributeptr, *SOAPAttributetmp;

    if ( Node->type == XML_ELEMENT )
    {
        SOAPElementptr = ( SOAPElement * ) PAL_MALLOC ( sizeof ( SOAPElement ) );
        if ( SOAPElementptr == NULL )
        {
            return ERROR_MALLOC;
        }
        memset ( SOAPElementptr, 0, sizeof ( SOAPElement ) );
        EleSoapptr = ( EleSoap * ) PAL_MALLOC ( sizeof ( EleSoap ) );
        if ( EleSoapptr == NULL )
        {
            return ERROR_MALLOC;
        }
        memset ( EleSoapptr, 0, sizeof ( EleSoap ) );
        SOAPElementptr->next = NULL;
        strcpy ( tmp, Node->value.element.name );
        if ( ( tmpptr = strchr ( tmp, ':' ) ) != NULL )
        {
            strcpy ( SOAPElementptr->name, tmpptr + 1 );
            *tmpptr = 0;
            strcpy ( SOAPElementptr->nstr, tmp );
        }
        else
        {
            SOAPElementptr->nstr[0] = 0;
            strcpy ( SOAPElementptr->name, tmp );
        }

        attrCount = Node->value.element.num_attrs;
        for ( i = 0; i < attrCount; i++ )
        {
            SOAPAttributeptr = ( SOAPAttribute* ) PAL_MALLOC ( sizeof ( SOAPAttribute ) );
            if ( SOAPAttributeptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( SOAPAttributeptr, 0, sizeof ( SOAPAttribute ) );
            SOAPAttributeptr->next = NULL;
            strcpy ( tmp, Node->value.element.attrs[i].name );
            if ( ( tmpptr = strchr ( tmp, ':' ) ) != NULL )
            {
                strcpy ( SOAPAttributeptr->name, tmpptr + 1 );
                *tmpptr = 0;
                strcpy ( SOAPAttributeptr->nstr, tmp );
            }
            else
            {
                SOAPAttributeptr->nstr[0] = 0;
                strcpy ( SOAPAttributeptr->name, tmp );
            }
            strcpy ( SOAPAttributeptr->data, Node->value.element.attrs[i].value );

            if ( !strcmp ( SOAPAttributeptr->nstr, "xmlns" ) )
            {
                add_remote_namespace ( SOAPAttributeptr->name, SOAPAttributeptr->data, soap );
            }

            if ( get_remote_namespace ( SOAPAttributeptr->nstr, soap ) != NULL )
            {
                strcpy ( tmp, ( char * ) get_remote_namespace ( SOAPAttributeptr->nstr, soap ) );
            }
            else
            {
                tmp[0] = 0;
            }

            if ( !strcmp ( SOAPAttributeptr->name, "mustUnderstand" ) && ( !strcmp ( SOAPAttributeptr->data, "1" ) || !strcmp ( SOAPAttributeptr->data, "true" ) )
                    && !strcmp ( tmp, soap->version == 1 ? soap_env1 : soap_env2 ) )
            {
                if ( strcmp ( SOAPElementptr->name, "ID" ) && strcmp ( SOAPElementptr->name, "HoldRequests" ) && strcmp ( SOAPElementptr->name, "NoMoreRequests" ) )
                {
                    //add fault
                    if ( build_mustunderstand_fault ( SOAPElementptr->name, SOAPElementptr->nstr, sSoapBuf, soapSize ) == SUCCESS_OK ) // add result
                    {
                        return ERROR_PARSER;
                    }
                }
                else
                {
                    EleSoapptr->mustUnderstand = 1;
                }
            }

            if ( ( !strcmp ( SOAPAttributeptr->name, "actor" ) || !strcmp ( SOAPAttributeptr->name, "role" ) )
                    && !strcmp ( tmp, soap->version == 1 ? soap_env1 : soap_env2 ) )
            {

                strcpy ( EleSoapptr->actor, SOAPAttributeptr->data );
            }


            if ( strcmp ( SOAPAttributeptr->name, "type" ) == 0
                    && ( strcmp ( tmp, soap_xsi ) == 0 || strcmp ( tmp, soap_xsi2 ) == 0 ) )
            {
                strcpy ( EleSoapptr->type, SOAPAttributeptr->data );
            }

            if ( soap->version == 1 && strcmp ( SOAPAttributeptr->name,  "arrayType" ) == 0 )
            {

                strcpy ( tmp, SOAPAttributeptr->data );
                if ( ( tmpptr = strchr ( tmp, '[' ) ) != NULL )
                {
                    charptr = strchr ( tmp, ']' );
                    *charptr = 0;
                    strcpy ( EleSoapptr->arraySize, tmpptr + 1 );
                }
                *tmpptr = 0;
                strcpy ( EleSoapptr->arrayType, tmp );

            }

            if ( soap->version == 2 && strcmp ( SOAPAttributeptr->name, "itemType" ) == 0 && strcmp ( tmp, soap_enc2 ) == 0 )
            {

                strcpy ( EleSoapptr->arrayType, SOAPAttributeptr->data );
            }

            if ( soap->version == 2 && strcmp ( SOAPAttributeptr->name, "arraySize" ) == 0 && strcmp ( tmp, soap_enc2 ) == 0 )
            {

                strcpy ( EleSoapptr->arraySize, SOAPAttributeptr->data );
            }

            if ( i == 0 )
            {
                SOAPElementptr->atts = SOAPAttributeptr;
            }
            else
            {
                SOAPAttributetmp = SOAPElementptr->atts;
                while ( SOAPAttributetmp->next )
                {
                    SOAPAttributetmp = SOAPAttributetmp->next;
                }
                SOAPAttributetmp->next = SOAPAttributeptr;
            }

            SOAPAttributeptr = SOAPAttributeptr->next;
        } /* end of for */
        SOAPElementptr->soap = EleSoapptr;

        //if the xml elment Node have no child, the value of this element is NULL
        if ( Node->child == NULL )
        {
            //SOAPElementptr->data.string = (char *)PAL_MALLOC(strlen(Node->value.text.string) + 1);
            SOAPElementptr->data.string = NULL;
            SOAPElementptr->type = XSD_STRING;
        }

        SOAPElementtmp = soap->element;
        if ( SOAPElementtmp == NULL )
        {
            soap->element = SOAPElementptr;
        }
        else
        {
            while ( SOAPElementtmp->next )
            {
                SOAPElementtmp = SOAPElementtmp->next;
            }
            SOAPElementtmp->next = SOAPElementptr;
        }
    }
    else if ( strlen ( Node->value.text.string ) > 0 )  /* david for space_handing test */
    {
        SOAPElementptr = soap->element;
        while ( SOAPElementptr->next )
        {
            SOAPElementptr = SOAPElementptr->next;
        }

        if ( Node->value.text.string )
        {
            // if the value is parted by the space, we should strcat the value
            if ( Node->prev == NULL )
            {
                SOAPElementptr->data.string = ( char * ) PAL_MALLOC ( strlen ( Node->value.text.string ) + 1 );

                if ( SOAPElementptr->data.string == NULL )
                {
                    return ERROR_PARSER;
                }
                memset ( SOAPElementptr->data.string, 0, ( strlen ( Node->value.text.string ) + 1 ) );

                strcpy ( SOAPElementptr->data.string, Node->value.text.string );
            }
            else
            {
                if ( ( SOAPElementptr->data.string = ( char * ) realloc ( SOAPElementptr->data.string, ( strlen ( SOAPElementptr->data.string ) + 1 + strlen ( Node->value.text.string ) + 1 ) ) ) == NULL )
                {
                    printf ( "realloc fail\n" );
                    return ERROR_MALLOC;
                }

                strcat ( SOAPElementptr->data.string, " " );
                strcat ( SOAPElementptr->data.string, Node->value.text.string );

            }
        }
        else
        {
            SOAPElementptr->data.string = NULL;
        }
        SOAPElementptr->type = XSD_STRING;

    }
    if ( *sSoapBuf != NULL )
    {
        PAL_FREE ( *sSoapBuf );
        *sSoapBuf = NULL;
        *soapSize = 0;
    }

    return SUCCESS_OK;
}


/***********************************************************************
Function: add_remote_namespace
Description: add a namespace to namespace list

Return: PAL_INT32_T, if add success, return SUCCESSOK , else return ERRORPARSER
Input: char *name,  name of namespace
       char *value,  value of namespace
       SOAP * soap,
Output: soap
Others:

Histroy:
    1.  Modification: Created.
        Author: David
        Date: 2009-05-22

    2.  Modification:
        Author:
        Date:
************************************************************************/
PAL_INT32_T add_remote_namespace ( char *name, char *value, SOAP * soap )
{
    NameSpace *NSptr, *NStmp;
    NSptr = ( NameSpace * ) PAL_MALLOC ( sizeof ( NameSpace ) );
    if ( NSptr == NULL )
    {
        return ERROR_PARSER;
    }
    memset ( NSptr, 0, sizeof ( NameSpace ) );
    strcpy ( NSptr->nsname, name );
    strcpy ( NSptr->nsvalue, value );
    NSptr->next = NULL;
    NStmp = soap->nameSpaces;
    if ( NStmp == NULL )
    {
        soap->nameSpaces = NSptr;
    }
    else
    {
        while ( NStmp->next )
        {
            NStmp = NStmp->next;
        }
        NStmp->next = NSptr;
    }
    if ( !strcmp ( value, soap_env1 ) )
    {
        soap->version = 1;
    }
    else if ( !strcmp ( value, soap_env2 ) )
    {
        soap->version = 2;
    }

    return SUCCESS_OK;
}

char* get_remote_namespace ( char *nsPre, SOAP * soap )
{
    NameSpace *NSptr;
    NSptr = soap->nameSpaces;

    while ( NSptr )
    {
        if ( !strcmp ( nsPre, NSptr->nsname ) )
        {
            return NSptr->nsvalue;
        }
        else
        {
            NSptr = NSptr->next;
        }
    }
    return NULL;
}

PAL_INT32_T parse_inform_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    PAL_INT32_T maxEnvelope, *maxEnvelopeptr;
    SOAPElement *SOAPElementtmp;
    SOAPElementtmp = SOAPElementptr->next;
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "MaxEnvelopes" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                maxEnvelope = atoi ( SOAPElementtmp->data.string );
            }
            else
            {
                maxEnvelope = 0;
            }
            maxEnvelopeptr = ( PAL_INT32_T * ) PAL_MALLOC ( sizeof ( PAL_INT32_T ) );
            if ( maxEnvelopeptr == NULL )
                return MALLOC_FAILURE;
            memset ( maxEnvelopeptr, 0, sizeof ( PAL_INT32_T ) );
            *maxEnvelopeptr = maxEnvelope;
            soap->parameters = maxEnvelopeptr;
            gMaxEnvelopes = maxEnvelope;
        }
        if ( !strcmp ( SOAPElementtmp->name, "Fault" ) )
        {
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_SOAP, __func__, " fault ===============\n" );
            parse_fault_packet ( SOAPElementptr, soap );
            return ERROR_PARSER;
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }

    return SUCCESS_OK;

}

PAL_INT32_T parse_transfercomplete_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    SOAPElement *SOAPElementtmp;
    SOAPElementtmp = SOAPElementptr->next;
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "Fault" ) )
        {
            parse_fault_packet ( SOAPElementptr, soap );
            return ERROR_PARSER;
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = NULL;
    return SUCCESS_OK;
}

PAL_INT32_T parse_SetParameterValues_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    ParameterListStruct *ParameterListStructptr = NULL;
    ParameterValueStruct *ParameterValueStructptr = NULL, *ParameterValueStructtmp;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;
    ParameterListStructptr = ( ParameterListStruct * ) PAL_MALLOC ( sizeof ( ParameterListStruct ) );
    if ( ParameterListStructptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( ParameterListStructptr, 0, sizeof ( ParameterListStruct ) );
    ParameterListStructptr->paramterValues = NULL;

    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "ParameterValueStruct" ) )
        {
            ParameterValueStructptr = ( ParameterValueStruct * ) PAL_MALLOC ( sizeof ( ParameterValueStruct ) );
            if ( ParameterValueStructptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( ParameterValueStructptr, 0, sizeof ( ParameterValueStruct ) );
            ParameterValueStructptr->next = NULL;
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Name" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                //if(SOAPElementtmp->data.string[strlen(SOAPElementtmp->data.string)-1] == ' ') //gavin temp_add should be removed
                //SOAPElementtmp->data.string[strlen(SOAPElementtmp->data.string)-1] ='\0';

                strcpy ( ParameterValueStructptr->name, SOAPElementtmp->data.string );
            }
            else
            {
                ParameterValueStructptr->name[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Value" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                ParameterValueStructptr->value = ( char * ) PAL_MALLOC ( strlen ( SOAPElementtmp->data.string ) + 1 );
                strcpy ( ParameterValueStructptr->value, SOAPElementtmp->data.string );
            }
            else
            {
                ParameterValueStructptr->value = ( char * ) PAL_MALLOC ( 1 );
                strcpy ( ParameterValueStructptr->value, "" );
            }

            if ( !strcmp ( SOAPElementtmp->soap->type, "xsd:string" ) )
            {
                ParameterValueStructptr->type = XSD_STRING;
            }
            else if ( !strcmp ( SOAPElementtmp->soap->type, "xsd:int" ) )
            {
                ParameterValueStructptr->type = XSD_INT;
            }
            else if ( !strcmp ( SOAPElementtmp->soap->type, "xsd:unsignedInt" ) )
            {
                ParameterValueStructptr->type = XSD_UINT;
            }
            else if ( !strcmp ( SOAPElementtmp->soap->type, "xsd:boolean" ) )
            {
                ParameterValueStructptr->type = XSD_BOOL;
            }
            ParameterValueStructtmp = ParameterListStructptr->paramterValues;
            if ( ParameterValueStructtmp == NULL )
            {
                ParameterListStructptr->paramterValues = ParameterValueStructptr;
            }
            else
            {
                while ( ParameterValueStructtmp->next )
                {
                    ParameterValueStructtmp = ParameterValueStructtmp->next;
                }
                ParameterValueStructtmp->next = ParameterValueStructptr;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "ParameterKey" ) )
        {
            strcpy ( ParameterListStructptr->parameterKey, SOAPElementtmp->data.string );
        }
        SOAPElementtmp = SOAPElementtmp->next;
    } /* end of while */
    soap->parameters = ( void * ) ParameterListStructptr;
    return SUCCESS_OK;
}

PAL_INT32_T parse_GetParameterValues_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    GetParameterValueStruct *GetParameterValueStructptr = NULL, *GetParameterValueStructlist = NULL, *GetParameterValueStructtmp;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "string" ) )
        {
            GetParameterValueStructptr = ( GetParameterValueStruct * ) PAL_MALLOC ( sizeof ( GetParameterValueStruct ) );
            if ( GetParameterValueStructptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( GetParameterValueStructptr, 0, sizeof ( GetParameterValueStruct ) );
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( GetParameterValueStructptr->name, SOAPElementtmp->data.string );
            }
            else
            {
                GetParameterValueStructptr->name[0] = 0;
            }

            GetParameterValueStructptr->next = NULL;

            GetParameterValueStructtmp = GetParameterValueStructlist;
            if ( GetParameterValueStructtmp == NULL )
            {
                GetParameterValueStructlist = GetParameterValueStructptr;
            }
            else
            {
                while ( GetParameterValueStructtmp->next )
                {
                    GetParameterValueStructtmp = GetParameterValueStructtmp->next;
                }
                GetParameterValueStructtmp->next = GetParameterValueStructptr;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) GetParameterValueStructlist;;
    return SUCCESS_OK;
}

PAL_INT32_T parse_GetParameterNames_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    ParameterNameStruct *ParameterNameStructptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;

    ParameterNameStructptr = ( ParameterNameStruct * ) PAL_MALLOC ( sizeof ( ParameterNameStruct ) );
    if ( ParameterNameStructptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( ParameterNameStructptr, 0, sizeof ( ParameterNameStruct ) );
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "ParameterPath" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( ParameterNameStructptr->name, SOAPElementtmp->data.string );
            }
            else
            {
                ParameterNameStructptr->name[0] = 0;
            }

        }
        else if ( !strcmp ( SOAPElementtmp->name, "NextLevel" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                if ( !strcasecmp ( SOAPElementtmp->data.string, "true" ) || !strcasecmp ( SOAPElementtmp->data.string, "1" ) )
                {
                    ParameterNameStructptr->nextLevel = 1;
                }
                else
                {
                    ParameterNameStructptr->nextLevel = 0;
                }
            }
            else
            {
                ParameterNameStructptr->nextLevel = 0;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) ParameterNameStructptr;
    return SUCCESS_OK;

}
PAL_INT32_T parse_SetParameterAttributes_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    ParameterSetAttributeStruct *ParameterSetAttributeStructptr = NULL, *ParameterSetAttributeStructtmp = NULL, *ParameterSetAttributeStructlist = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;

    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "SetParameterAttributesStruct" ) )
        {
            ParameterSetAttributeStructptr = ( ParameterSetAttributeStruct * ) PAL_MALLOC ( sizeof ( ParameterSetAttributeStruct ) );
            if ( ParameterSetAttributeStructptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( ParameterSetAttributeStructptr, 0, sizeof ( ParameterSetAttributeStruct ) );
            ParameterSetAttributeStructptr->next = NULL;
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Name" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( ParameterSetAttributeStructptr->name, SOAPElementtmp->data.string );
            }
            else
            {
                ParameterSetAttributeStructptr->name[0] = 0;
            }

        }
        else if ( !strcmp ( SOAPElementtmp->name, "NotificationChange" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                if ( 0 == strcmp ( SOAPElementtmp->data.string, "1" ) || 0 == strcasecmp ( SOAPElementtmp->data.string, "true" ) )
                {
                    ParameterSetAttributeStructptr->notificationChange = 1;
                }
                else
                {
                    ParameterSetAttributeStructptr->notificationChange = 0;
                }
            }
            else
            {
                ParameterSetAttributeStructptr->notificationChange = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Notification" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                ParameterSetAttributeStructptr->notification = atoi ( SOAPElementtmp->data.string );
            }
            else
            {
                ParameterSetAttributeStructptr->notification = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "AccessListChange" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                if ( 0 == strcmp ( SOAPElementtmp->data.string, "1" ) || 0 == strcasecmp ( SOAPElementtmp->data.string, "true" ) )
                {
                    ParameterSetAttributeStructptr->accessListChange = 1;
                }
                else
                {
                    ParameterSetAttributeStructptr->accessListChange = 0;
                }
            }
            else
            {
                ParameterSetAttributeStructptr->accessListChange = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "AccessList" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( ParameterSetAttributeStructptr->accessList, SOAPElementtmp->data.string );
            }
            else
            {
                ParameterSetAttributeStructptr->accessList[0] = 0;
            }

            ParameterSetAttributeStructtmp = ParameterSetAttributeStructlist;
            if ( ParameterSetAttributeStructtmp == NULL )
            {
                ParameterSetAttributeStructlist = ParameterSetAttributeStructptr;
            }
            else
            {
                while ( ParameterSetAttributeStructtmp->next )
                {
                    ParameterSetAttributeStructtmp = ParameterSetAttributeStructtmp->next;
                }
                ParameterSetAttributeStructtmp->next = ParameterSetAttributeStructptr;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) ParameterSetAttributeStructlist;
    return SUCCESS_OK;
}
PAL_INT32_T parse_GetParameterAttributes_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    GetParameterValueStruct *GetParameterValueStructptr = NULL, *GetParameterValueStructlist =
                NULL, *GetParameterValueStructtmp = NULL;
    SOAPElement *SOAPElementtmp;
    SOAPElementtmp = SOAPElementptr->next;
    //GetParameterValueStructlist = GetParameterValueStructptr;
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "string" ) )
        {
            GetParameterValueStructptr = ( GetParameterValueStruct * ) PAL_MALLOC ( sizeof ( GetParameterValueStruct ) );
            if ( GetParameterValueStructptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( GetParameterValueStructptr, 0, sizeof ( GetParameterValueStruct ) );
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( GetParameterValueStructptr->name, SOAPElementtmp->data.string );
            }
            else
            {
                GetParameterValueStructptr->name[0] = 0;
            }

            GetParameterValueStructptr->next = NULL;

            GetParameterValueStructtmp = GetParameterValueStructlist;
            if ( GetParameterValueStructtmp == NULL )
            {
                GetParameterValueStructlist = GetParameterValueStructptr;
            }
            else
            {
                while ( GetParameterValueStructtmp->next )
                {
                    GetParameterValueStructtmp = GetParameterValueStructtmp->next;
                }
                GetParameterValueStructtmp->next = GetParameterValueStructptr;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) GetParameterValueStructlist;
    return SUCCESS_OK;

}
PAL_INT32_T parse_AddObject_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    AddObjectStruct *AddObjectStructptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;

    AddObjectStructptr = ( AddObjectStruct * ) PAL_MALLOC ( sizeof ( AddObjectStruct ) );
    if ( AddObjectStructptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( AddObjectStructptr, 0, sizeof ( AddObjectStruct ) );
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "ObjectName" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( AddObjectStructptr->name, SOAPElementtmp->data.string );
            }
            else
            {
                AddObjectStructptr->name[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "ParameterKey" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( AddObjectStructptr->parameterKey, SOAPElementtmp->data.string );
            }
            else
            {
                AddObjectStructptr->parameterKey[0] = 0;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) AddObjectStructptr;;
    return SUCCESS_OK;
}
PAL_INT32_T parse_DeleteObject_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    DeleteObjectStruct *DeleteObjectStructptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;

    DeleteObjectStructptr = ( DeleteObjectStruct * ) PAL_MALLOC ( sizeof ( DeleteObjectStruct ) );
    if ( DeleteObjectStructptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( DeleteObjectStructptr, 0, sizeof ( DeleteObjectStruct ) );
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "ObjectName" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DeleteObjectStructptr->name, SOAPElementtmp->data.string );
            }
            else
            {
                DeleteObjectStructptr->name[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "ParameterKey" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DeleteObjectStructptr->parameterKey, SOAPElementtmp->data.string );
            }
            else
            {
                DeleteObjectStructptr->parameterKey[0] = 0;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) DeleteObjectStructptr;;
    return SUCCESS_OK;
}
PAL_INT32_T parse_GetRPCMethods_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    soap->parameters = NULL;

    return SUCCESS_OK;
}
PAL_INT32_T parse_Download_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    DownloadStruct *DownloadStructptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;
    DownloadStructptr = ( DownloadStruct * ) PAL_MALLOC ( sizeof ( DownloadStruct ) );
    if ( DownloadStructptr == NULL )
        return ERROR_MALLOC;
    memset ( DownloadStructptr, 0, sizeof ( DownloadStruct ) );

    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "CommandKey" ) )
        {

            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->commandKey, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->commandKey[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "FileType" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->fileType, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->fileType[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "URL" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->URL, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->URL[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Username" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->username, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->username[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Password" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->password, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->password[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "FileSize" ) )
        {
            if ( NULL != SOAPElementtmp->data.string )
            {
                DownloadStructptr->filesize = atoi ( SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->filesize = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "TargetFileName" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->targetFileName, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->targetFileName[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "DelaySeconds" ) )
        {
            if ( NULL != SOAPElementtmp->data.string )
            {
                DownloadStructptr->delaySeconds = atoi ( SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->delaySeconds = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "SuccessURL" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->successURL, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->successURL[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "FailureURL" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( DownloadStructptr->failureURL, SOAPElementtmp->data.string );
            }
            else
            {
                DownloadStructptr->failureURL[0] = 0;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) DownloadStructptr;;
    return SUCCESS_OK;
}
PAL_INT32_T parse_Reboot_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    char *commandKey = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;
    commandKey = ( char * ) PAL_MALLOC ( PARAMETER_KEY_LEN );
    if ( commandKey == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( commandKey, 0, PARAMETER_KEY_LEN );
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "CommandKey" ) )
        {

            if ( SOAPElementtmp->data.string )
            {
                strcpy ( commandKey, SOAPElementtmp->data.string );
            }
            else
            {
                commandKey[0] = 0;
            }
        }
        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) commandKey;
    return SUCCESS_OK;
}
PAL_INT32_T parse_GetQueuedTransfers_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    soap->parameters = NULL;

    return SUCCESS_OK;
}
PAL_INT32_T parse_ScheduleInform_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    ScheduleInformStruct *ScheduleInformStructptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;
    ScheduleInformStructptr = ( ScheduleInformStruct * ) PAL_MALLOC ( sizeof ( ScheduleInformStruct ) );
    if ( ScheduleInformStructptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( ScheduleInformStructptr, 0, sizeof ( ScheduleInformStruct ) );
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "DelaySeconds" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                ScheduleInformStructptr->delaySeconds = atoi ( SOAPElementtmp->data.string );
            }
            else
            {
                ScheduleInformStructptr->delaySeconds = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "CommandKey" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( ScheduleInformStructptr->commandKey, SOAPElementtmp->data.string );
            }
            else
            {
                ScheduleInformStructptr->commandKey[0] = 0;
            }

        }

        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) ScheduleInformStructptr;
    return SUCCESS_OK;
}

PAL_INT32_T parse_FactoryReset_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    soap->parameters = NULL;

    return SUCCESS_OK;
}

PAL_INT32_T parse_Upload_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    UploadStruct *UploadStructptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    SOAPElementtmp = SOAPElementptr->next;

    UploadStructptr = ( UploadStruct * ) PAL_MALLOC ( sizeof ( UploadStruct ) );
    if ( UploadStructptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( UploadStructptr, 0, sizeof ( UploadStruct ) );

    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "CommandKey" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( UploadStructptr->commandKey, SOAPElementtmp->data.string );
            }
            else
            {
                UploadStructptr->commandKey[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "FileType" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( UploadStructptr->fileType, SOAPElementtmp->data.string );
            }
            else
            {
                UploadStructptr->fileType[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "URL" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( UploadStructptr->URL, SOAPElementtmp->data.string );
            }
            else
            {
                UploadStructptr->URL[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Username" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( UploadStructptr->username, SOAPElementtmp->data.string );
            }
            else
            {
                UploadStructptr->username[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Password" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( UploadStructptr->password, SOAPElementtmp->data.string );
            }
            else
            {
                UploadStructptr->password[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "DelaySeconds" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                UploadStructptr->delaySeconds = atoi ( SOAPElementtmp->data.string );
            }
            else
            {
                UploadStructptr->delaySeconds = 0;
            }
        }

        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) UploadStructptr;
    return SUCCESS_OK;
}

PAL_INT32_T parse_RequestDownLoad_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    RequestDownloadStruct *RequestDownloadStructptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    PAL_INT32_T i = 0;
    SOAPElementtmp = SOAPElementptr->next;
    RequestDownloadStructptr = ( RequestDownloadStruct * ) PAL_MALLOC ( sizeof ( RequestDownloadStruct ) );
    if ( RequestDownloadStructptr == NULL )
    {
        return ERROR_MALLOC;
    }
    memset ( RequestDownloadStructptr, 0, sizeof ( RequestDownloadStruct ) );
    while ( SOAPElementtmp )
    {
        if ( !strcmp ( SOAPElementtmp->name, "FileType" ) )
        {

            if ( SOAPElementtmp->data.string )
            {
                strcpy ( RequestDownloadStructptr->fileType, SOAPElementtmp->data.string );
            }
            else
            {
                RequestDownloadStructptr->fileType[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Name" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( RequestDownloadStructptr->arglist[i].name, SOAPElementtmp->data.string );
            }
            else
            {
                RequestDownloadStructptr->arglist[i].name[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Value" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( RequestDownloadStructptr->arglist[i++].value, SOAPElementtmp->data.string );
            }
            else
            {
                RequestDownloadStructptr->arglist[i++].value[0] = 0;
            }

        }

        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->parameters = ( void * ) RequestDownloadStructptr;
    return SUCCESS_OK;
}

PAL_INT32_T parse_fault_packet ( SOAPElement* SOAPElementptr, SOAP * soap )
{
    SOAPFault *SOAPFaultptr = NULL;
    SOAPElement *SOAPElementtmp = NULL;
    //PAL_INT32_T i = 0;
    SOAPCode *SOAPCodeptr = NULL, *SOAPCodetmp = NULL;
    SOAPReason *SOAPReasonptr = NULL;
    char tmp[64];
    SOAPElementtmp = SOAPElementptr;
    while ( SOAPElementtmp )
    {
        if ( get_remote_namespace ( SOAPElementtmp->nstr, soap ) != NULL )
        {
            strcpy ( tmp, ( char * ) get_remote_namespace ( SOAPElementtmp->nstr, soap ) );
        }
        else
        {
            tmp[0] = 0;
        }
        if ( !strcmp ( SOAPElementtmp->name, "Fault" ) )
        {
            SOAPFaultptr = ( SOAPFault * ) PAL_MALLOC ( sizeof ( SOAPFault ) );
            if ( SOAPFaultptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( SOAPFaultptr, 0, sizeof ( SOAPFault ) );
            SOAPFaultptr->detail = NULL;
            SOAPFaultptr->SOAPCode = NULL;
            SOAPFaultptr->SOAPReason = NULL;
            SOAPFaultptr->SOAPDetail = NULL;


        }
        else if ( !strcmp ( SOAPElementtmp->name, "faultcode" ) && soap->version == 1 )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( SOAPFaultptr->faultCode, SOAPElementtmp->data.string );
            }
            else
            {
                SOAPFaultptr->faultCode[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "faultstring" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( SOAPFaultptr->faultString, SOAPElementtmp->data.string );
            }
            else
            {
                SOAPFaultptr->faultString[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "faultactor" ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( SOAPFaultptr->faultActor, SOAPElementtmp->data.string );
            }
            else
            {
                SOAPFaultptr->faultActor[0] = 0;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "FaultCode" ) )
        {
            if ( soap->version == 1 )
            {
                SOAPFaultptr->detail = ( CWMPDetail * ) PAL_MALLOC ( sizeof ( CWMPDetail ) );
                if ( SOAPFaultptr->detail == NULL )
                {
                    return ERROR_MALLOC;
                }
                memset ( SOAPFaultptr->detail, 0, sizeof ( CWMPDetail ) );
                SOAPFaultptr->detail->cwmpFault = ( CWMPFault * ) PAL_MALLOC ( sizeof ( CWMPFault ) );
                if ( SOAPFaultptr->detail->cwmpFault == NULL )
                {
                    return ERROR_MALLOC;
                }
                memset ( SOAPFaultptr->detail->cwmpFault, 0, sizeof ( CWMPFault ) );
                if ( SOAPElementtmp->data.string )
                {
                    SOAPFaultptr->detail->cwmpFault->FaultCode = atoi ( SOAPElementtmp->data.string );
                }
                else
                {
                    SOAPFaultptr->detail->cwmpFault->FaultCode = 0;
                }
            }
            else if ( soap->version == 2 )
            {
                SOAPFaultptr->SOAPDetail = ( CWMPDetail * ) PAL_MALLOC ( sizeof ( CWMPDetail ) );
                if ( SOAPFaultptr->SOAPDetail == NULL )
                {
                    return ERROR_MALLOC;
                }
                memset ( SOAPFaultptr->SOAPDetail, 0, sizeof ( CWMPDetail ) );
                SOAPFaultptr->SOAPDetail->cwmpFault = ( CWMPFault * ) PAL_MALLOC ( sizeof ( CWMPFault ) );
                if ( SOAPFaultptr->SOAPDetail->cwmpFault == NULL )
                {
                    return ERROR_MALLOC;
                }
                memset ( SOAPFaultptr->SOAPDetail->cwmpFault, 0, sizeof ( CWMPFault ) );
                if ( SOAPElementtmp->data.string )
                {
                    SOAPFaultptr->SOAPDetail->cwmpFault->FaultCode = atoi ( SOAPElementtmp->data.string );
                }
                else
                {
                    SOAPFaultptr->SOAPDetail->cwmpFault->FaultCode = 0;
                }
            }

        }
        else if ( !strcmp ( SOAPElementtmp->name, "FaultString" ) )
        {
            if ( soap->version == 1 )
            {
                if ( SOAPElementtmp->data.string )
                {
                    strcpy ( SOAPFaultptr->detail->cwmpFault->FaultString, SOAPElementtmp->data.string );
                }
                else
                {
                    SOAPFaultptr->detail->cwmpFault->FaultString[0] = 0;
                }
            }
            else if ( soap->version == 2 )
            {
                if ( SOAPElementtmp->data.string )
                {
                    strcpy ( SOAPFaultptr->SOAPDetail->cwmpFault->FaultString, SOAPElementtmp->data.string );
                }
                else
                {
                    SOAPFaultptr->SOAPDetail->cwmpFault->FaultString[0] = 0;
                }
            }

        }
        else if ( !strcmp ( SOAPElementtmp->name, "Code" ) && !strcmp ( tmp, soap_env2 ) )
        {
            SOAPCodeptr = ( SOAPCode * ) PAL_MALLOC ( sizeof ( SOAPCode ) );
            if ( SOAPCodeptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( SOAPCodeptr, 0, sizeof ( SOAPCode ) );
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( SOAPCodeptr->SOAPValue, SOAPElementtmp->data.string );
            }
            else
            {
                SOAPCodeptr->SOAPValue[0] = 0;
            }
            SOAPCodeptr->SOAPSubcode = NULL;

            SOAPCodetmp = SOAPFaultptr->SOAPCode;
            if ( SOAPCodetmp == NULL )
            {
                SOAPFaultptr->SOAPCode = SOAPCodeptr;
            }
            else
            {
                while ( SOAPCodetmp->SOAPSubcode )
                {
                    SOAPCodetmp = SOAPCodetmp->SOAPSubcode;
                }
                SOAPCodetmp->SOAPSubcode = SOAPCodeptr;
            }
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Reason" ) && !strcmp ( tmp, soap_env2 ) )
        {
            SOAPReasonptr = ( SOAPReason * ) PAL_MALLOC ( sizeof ( SOAPReason ) );
            if ( SOAPReasonptr == NULL )
            {
                return ERROR_MALLOC;
            }
            memset ( SOAPReasonptr, 0, sizeof ( SOAPReason ) );
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( SOAPReasonptr->SOAPText, SOAPElementtmp->data.string );
            }
            else
            {
                SOAPReasonptr->SOAPText[0] = 0;
            }

            SOAPFaultptr->SOAPReason = SOAPReasonptr;
        }
        else if ( !strcmp ( SOAPElementtmp->name, "Node" ) && !strcmp ( tmp, soap_env2 ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( SOAPFaultptr->SOAPNode, SOAPElementtmp->data.string );
            }
            else
            {
                SOAPFaultptr->SOAPNode[0] = 0;
            }

        }
        else if ( !strcmp ( SOAPElementtmp->name, "Role" ) && !strcmp ( tmp, soap_env2 ) )
        {
            if ( SOAPElementtmp->data.string )
            {
                strcpy ( SOAPFaultptr->SOAPRole, SOAPElementtmp->data.string );
            }
            else
            {
                SOAPFaultptr->SOAPRole[0] = 0;
            }

        }


        SOAPElementtmp = SOAPElementtmp->next;
    }
    soap->fault = SOAPFaultptr;
    return SUCCESS_OK;
}

void free_soap ( SOAP * soap )
{
    if ( soap->nameSpaces != NULL )
    {
        NameSpace *NameSpaceptr, *tmp;
        tmp = NameSpaceptr = soap->nameSpaces;
        while ( NameSpaceptr )
        {
            tmp = NameSpaceptr;
            NameSpaceptr = NameSpaceptr->next;
            PAL_FREE ( tmp );
            tmp = NULL;
        }
        soap->nameSpaces = NULL;
    }
    if ( soap->header != NULL )
    {
        PAL_FREE ( soap->header );
        soap->header = NULL;
    }

    if ( soap->fault != NULL )
    {
        if ( soap->fault->detail != NULL )
        {
            if ( soap->fault->detail->cwmpFault != NULL )
            {
                if ( soap->fault->detail->cwmpFault->SetParameterValuesFault )
                {
                    SetParameterFaultStruct *SetParameterFaultStructptr, *tmp;
                    tmp = SetParameterFaultStructptr = soap->fault->detail->cwmpFault->SetParameterValuesFault;
                    while ( SetParameterFaultStructptr )
                    {
                        tmp = SetParameterFaultStructptr;
                        SetParameterFaultStructptr = SetParameterFaultStructptr->next;
                        PAL_FREE ( tmp );
                        tmp = NULL;
                    }
                    soap->fault->detail->cwmpFault->SetParameterValuesFault = NULL;
                }
                PAL_FREE ( soap->fault->detail->cwmpFault );
                soap->fault->detail->cwmpFault = NULL;
            }
            PAL_FREE ( soap->fault->detail );
            soap->fault->detail = NULL;
        }

        if ( soap->fault->SOAPCode != NULL )
        {
            SOAPCode *faultCode, *tmp;
            tmp = faultCode = soap->fault->SOAPCode;
            while ( faultCode != NULL )
            {
                tmp = faultCode;
                faultCode = faultCode->SOAPSubcode;
                PAL_FREE ( tmp );
                tmp = NULL;
            }
            soap->fault->SOAPCode = NULL;
        }

        if ( soap->fault->SOAPReason != NULL )
        {
            PAL_FREE ( soap->fault->SOAPReason );
            soap->fault->SOAPReason = NULL;
        }

        if ( soap->fault->SOAPDetail != NULL )
        {
            if ( soap->fault->SOAPDetail->cwmpFault != NULL )
            {
                if ( soap->fault->SOAPDetail->cwmpFault->SetParameterValuesFault )
                {
                    SetParameterFaultStruct *SetParameterFaultStructptr, *tmp;
                    tmp = SetParameterFaultStructptr = soap->fault->SOAPDetail->cwmpFault->SetParameterValuesFault;
                    while ( SetParameterFaultStructptr )
                    {
                        tmp = SetParameterFaultStructptr;
                        SetParameterFaultStructptr = SetParameterFaultStructptr->next;
                        PAL_FREE ( tmp );
                        tmp = NULL;
                    }
                    soap->fault->SOAPDetail->cwmpFault->SetParameterValuesFault = NULL;
                }
                PAL_FREE ( soap->fault->SOAPDetail->cwmpFault );
                soap->fault->SOAPDetail->cwmpFault = NULL;
            }
            PAL_FREE ( soap->fault->SOAPDetail );
            soap->fault->SOAPDetail = NULL;
        }
        PAL_FREE ( soap->fault );
        soap->fault = NULL;
    }

    if ( soap->element != NULL )
    {
        SOAPElement *element, *tmp;
        tmp = element = soap->element;
        while ( element != NULL )
        {
            tmp = element;
            element = element->next;
            free_element ( tmp );
            PAL_FREE ( tmp );
        }
        soap->element = NULL;
    }

    //if ( soap->method_name[0] = 0 ) ??
    if ( soap->method_name[0] != '\0' ) // by Hook Guo
    {
        if ( !strcmp ( soap->method_name, "InformResponse" ) )
        {
            PAL_INT32_T *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( PAL_INT32_T * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }

            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "TransferCompleteResponse" ) )
        {
            if ( soap->parameters != NULL )
            { //it may has memory leak
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "SetParameterValues" ) )
        {
            ParameterListStruct *tmpParameter;
            ParameterValueStruct *ParameterValueStructptr, *tmp;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( ParameterListStruct * ) soap->parameters;
                tmp = tmpParameter->paramterValues;
                while ( tmp != NULL )
                {
                    ParameterValueStructptr = tmp->next;
                    PAL_FREE ( tmp );
                    tmp = ParameterValueStructptr;
                }
                tmpParameter->paramterValues = NULL;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "GetParameterValues" ) )
        {
            GetParameterValueStruct *tmpParameter, *tmp;
            if ( soap->parameters != NULL )
            {
                tmp = ( GetParameterValueStruct * ) soap->parameters;
                while ( tmp != NULL )
                {
                    tmpParameter = tmp->next;
                    PAL_FREE ( tmp );
                    tmp = tmpParameter;
                }
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "GetParameterNames" ) )
        {
            ParameterNameStruct *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( ParameterNameStruct * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "SetParameterAttributes" ) )
        {
            ParameterSetAttributeStruct *tmpParameter, *tmp;
            if ( soap->parameters != NULL )
            {
                tmp = ( ParameterSetAttributeStruct * ) soap->parameters;
                while ( tmp != NULL )
                {
                    tmpParameter = tmp->next;
                    PAL_FREE ( tmp );
                    tmp = tmpParameter;
                }
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }

        else if ( !strcmp ( soap->method_name, "GetParameterAttributes" ) )
        {

            GetParameterValueStruct *tmpParameter, *tmp;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( GetParameterValueStruct * ) soap->parameters;

                while ( tmpParameter != NULL )
                {
                    tmp = tmpParameter;
                    tmpParameter = tmpParameter->next;
                    PAL_FREE ( tmp );
                }
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "AddObject" ) )
        {
            AddObjectStruct *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( AddObjectStruct * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "DeleteObject" ) )
        {
            DeleteObjectStruct *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( DeleteObjectStruct * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "GetRPCMethods" ) )
        {
            if ( soap->parameters != NULL )
            {
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "Download" ) )
        {
            DownloadStruct *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( DownloadStruct * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "Reboot" ) )
        {
            char *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( char * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "GetQueuedTransfers" ) )
        {
            if ( soap->parameters != NULL )
            {
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "ScheduleInform" ) )
        {
            ScheduleInformStruct *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( ScheduleInformStruct * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "FactoryReset" ) )
        {
            if ( soap->parameters != NULL )
            {
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "Upload" ) )
        {
            UploadStruct *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( UploadStruct * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
        else if ( !strcmp ( soap->method_name, "RequestDownLoad" ) )
        {
            RequestDownloadStruct *tmpParameter;
            if ( soap->parameters != NULL )
            {
                tmpParameter = ( RequestDownloadStruct * ) soap->parameters;
                PAL_FREE ( tmpParameter );
                tmpParameter = NULL;
                soap->parameters = NULL;
            }
            soap->method_name[0] = 0;
        }
    }
}


void free_element ( SOAPElement *element )
{
    if ( element->atts != NULL )
    {
        SOAPAttribute *SOAPAttributeptr, *tmp;
        tmp = SOAPAttributeptr = element->atts;
        while ( SOAPAttributeptr )
        {
            tmp = SOAPAttributeptr;
            SOAPAttributeptr = SOAPAttributeptr->next;
            PAL_FREE ( tmp );
        }
        element->atts = NULL;
    }
    if ( element->soap != NULL )
    {
        PAL_FREE ( element->soap );
        element->soap = NULL;
    }
    /* david add for memory leak 2009.7.13 */
    if ( element->data.string != NULL )
    {
        PAL_FREE ( element->data.string );
        element->data.string = NULL;
    }

}
