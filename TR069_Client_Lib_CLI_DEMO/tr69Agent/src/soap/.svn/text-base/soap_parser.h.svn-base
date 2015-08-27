#ifndef _SOAP_PARSER_H_
#define _SOAP_PARSER_H_

#include "soap_public.h"

PAL_INT32_T xml2soap(XmlNode *Node, SOAP * soap, char **sSoapBuf, PAL_INT32_T * soapSize);

PAL_INT32_T soap_parser(SOAP * soap, char **sSoapBuf, PAL_INT32_T * soapSize);


PAL_INT32_T parse_inform_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_transfercomplete_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_SetParameterValues_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_GetParameterValues_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_GetParameterNames_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_SetParameterAttributes_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_GetParameterAttributes_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_AddObject_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_DeleteObject_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_GetRPCMethods_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_Download_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_Reboot_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_GetQueuedTransfers_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_ScheduleInform_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_FactoryReset_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_Upload_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_RequestDownLoad_packet(SOAPElement* SOAPElementptr, SOAP * soap);
PAL_INT32_T parse_fault_packet(SOAPElement* SOAPElementptr, SOAP * soap);


char* get_remote_namespace(char *nsPre, SOAP * soap);
void free_soap(SOAP * soap);
void free_element(SOAPElement *element);

#endif
