#ifndef _SOAP_BUILD_H_
#define _SOAP_BUILD_H_

#include "soap_public.h"

PAL_INT32_T add_local_namespace ( char *name, char * value );

PAL_INT32_T add_envelope_start ( char *sendbuf,  PAL_UINT32_T * soapSize );
PAL_INT32_T add_header_start ( char *sendbuf,  PAL_INT32_T * soapSize, PacketType type );
PAL_INT32_T add_header_end ( char *sendbuf,  PAL_INT32_T * soapSize );
PAL_INT32_T add_body_start ( char *sendbuf,  PAL_INT32_T * soapSize );
PAL_INT32_T add_RPC_palload ( char *sendbuf, PacketType type, void *parameter,  PAL_INT32_T * soapSize );
PAL_INT32_T add_body_end ( char *sendbuf,  PAL_INT32_T * soapSize );
PAL_INT32_T add_envelope_end ( char *sendbuf,  PAL_INT32_T * soapSize );
PAL_INT32_T build_fault_struct ( FaultType faultcode, SetParameterFaultStruct *setparameterfault, SOAPFault *SOAPFault );
PAL_INT32_T envelope_fault ( char *sendbuf, SOAPFault *SOAPFault,  PAL_INT32_T * soapSize );
PAL_INT32_T soap_send_raw ( char *sendbuf, const char *s, PAL_SIZE_T n, PAL_INT32_T *soapSize );
PAL_INT32_T put_loacal_namespace ( char *sendbuf,  PAL_INT32_T *soapSize );
PAL_INT32_T soap_add_attribute ( char *sendbuf,  char *attrname, char *attrvalue, PAL_INT32_T *soapSize );
PAL_INT32_T soap_add_ID ( char *sendbuf,  PAL_INT32_T *soapSize, PacketType type );
char* get_fault_string ( FaultType faultcode );
void ns_free ( NameSpace *gLocalNSptr );

PAL_INT32_T build_versionmismatch_fault ( char **sSendBuf, PAL_INT32_T * soapSize );
PAL_INT32_T build_mustunderstand_fault ( char *elename, char *elenstr, char **sSendBuf, PAL_INT32_T * soapSize );
 
#endif /* _SOAP_BUILD_H_ */
