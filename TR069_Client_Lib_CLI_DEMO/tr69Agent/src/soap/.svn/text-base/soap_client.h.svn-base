#ifndef _SOAP_CLIENT_H_
#define _SOAP_CLIENT_H_

#include "soap_public.h"


PAL_INT32_T add_inform_method ( char *sendbuf, InformInfoStruct *inform, PAL_INT32_T * soapSize );
PAL_INT32_T add_transfercomplete_method ( char *sendbuf, TransferCompleteList *transferComplete, PAL_INT32_T * soapSize );
PAL_INT32_T add_setparametervalues_res_method ( char *sendbuf, PAL_INT32_T status, PAL_INT32_T * soapSize );
PAL_INT32_T add_getparametervalues_res_method ( char *sendbuf, ParameterValueStruct * getParameterValue, PAL_INT32_T * soapSize );
PAL_INT32_T add_getparameternames_res_method ( char *sendbuf, ParameterInfoStruct* getParameterNames, PAL_INT32_T * soapSize );
PAL_INT32_T add_setparameterattributes_res_method ( char *sendbuf, PAL_INT32_T * soapSize );
PAL_INT32_T add_getparameterattributes_res_method ( char *sendbuf, ParameterAttributeStruct * getParameterAttribute, PAL_INT32_T * soapSize );
PAL_INT32_T add_addobject_res_method ( char *sendbuf, AddObjectResponseStruct *addObject, PAL_INT32_T * soapSize );
PAL_INT32_T add_deleteobject_res_method ( char *sendbuf, PAL_INT32_T status, PAL_INT32_T * soapSize );
PAL_INT32_T add_getrpcmethods_res_method ( char *sendbuf, PAL_INT32_T * soapSize );
PAL_INT32_T add_download_res_method ( char *sendbuf, DownloadResponseStruct * download, PAL_INT32_T * soapSize );
PAL_INT32_T add_reboot_res_method ( char *sendbuf, PAL_INT32_T * soapSize );
//PAL_INT32_T add_setvouchers_res_method();
//PAL_INT32_T add_getoptions_res_method();
PAL_INT32_T add_getqueuedtransfers_res_method ( char *sendbuf, TransferListStruct *getQueuedTransfer, PAL_INT32_T * soapSize );
PAL_INT32_T add_scheduleinform_res_method ( char *sendbuf, PAL_INT32_T * soapSize );
PAL_INT32_T add_factoryreset_res_method ( char *sendbuf, PAL_INT32_T * soapSize );
PAL_INT32_T add_upload_res_method ( char *sendbuf, UploadResponseStruct *upload, PAL_INT32_T * soapSize );
PAL_INT32_T add_requestdownload_method ( char *sendbuf, RequestDownloadStruct *requestDownload, PAL_INT32_T * soapSize );
char *get_type ( ValueType type );
//PAL_INT32_T add_kicked_method();
#endif

