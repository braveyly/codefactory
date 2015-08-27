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
Filename: mainloop.h
Author: Gavin
Date: 2009-05-19
Description:

***********************************************************************/

#ifndef _MAINLOOP_H_
#define _MAINLOOP_H_

#include "pal.h"
#include "mainloop_public.h"
#include "tr69_tree_public.h"
#include "stun_public.h"
#include "soap_public.h"
#include "event_public.h"
#include "crq_handler_public.h"
#include "http_public.h"
#include "tr69_agent_public.h"
#include "tr69_agent.h"
#include "host_handler_public.h"

/***********************************************************************
Function: wait_wake_up_main_session
Description:  the main session will block here and wait wake up semaphore from other threads.

************************************************************************/
void wait_wake_up_main_session ( void );

/***********************************************************************
Function: free_inform_parameter_list
Description:free inform parameter list
************************************************************************/
extern void free_inform_parameter_list ( IN OUT InformInfoStruct * informPtr ,IN PAL_UINT16_T paramListFlag);

/***********************************************************************
Function: free_transfer_complete_list
Description:free transfer complete list
************************************************************************/
void free_transfer_complete_list ( IN OUT TransferCompleteList **transferCompleteP );

/***********************************************************************
Function: check_transfer_done_event
Description: scan the PAL_TMP_RESULT_DIR,Return transfer done event numbers
************************************************************************/
PAL_INT32_T check_transfer_done_event ( void );

/***********************************************************************
Function: do_upload_immediate
Description:  call http or ftp to upload file immediately, and store the result to uploadResponsePtr

************************************************************************/
Result do_upload_immediate ( IN UploadStruct *uploadPtr, OUT UploadResponseStruct *uploadResponsePtr );

/***********************************************************************
Function: do_download_immediate
Description:  call http or ftp to download file immediately, and store the result to downloadResponsePtr
************************************************************************/
Result do_download_immediate ( IN DownloadStruct *downloadPtr, OUT  DownloadResponseStruct *downloadResponsePtr );

/***********************************************************************
Function: get_local_parameter_value
Description:get parameter value from local configuration tree
************************************************************************/
void get_local_parameter_value ( IN char *name, OUT char ** value );
/***********************************************************************
Function: get_transfer_list
Description:  get Transfer list for soap
************************************************************************/
void get_transfer_list ( OUT TransferListStruct * transferListP );

/***********************************************************************
Function: convert_schedule_to_local_event
Description:  convert soap schedule inform structure to local schedule file

************************************************************************/
void convert_schedule_to_local_event ( IN ScheduleInformStruct * schInformPtr, OUT char * evtFileName, OUT ScheduleArg * localSchedulePtr );

/***********************************************************************
Function: convert_upload_to_local
Description:  convert soap upload structure to local upload task file
************************************************************************/
void convert_upload_to_local ( IN UploadStruct * uploadPtr, OUT UploadArg *uploadTaskPtr );

/***********************************************************************
Function: convert_dowload_to_local
Description:  convert soap download structure to local download task file
************************************************************************/
void convert_dowload_to_local ( IN DownloadStruct * downloadPtr, OUT DownloadArg * downloadTaskPtr );
/***********************************************************************
Function: store_schedule_event
Description:  store schedule event to file
************************************************************************/
void store_schedule_event ( IN  ScheduleArg * localSchedulePtr );

/***********************************************************************
Function: store_upload_event
Description:  store upload event to file
************************************************************************/
void store_upload_event ( IN UploadArg * uploadTaskPtr );

/***********************************************************************
Function: store_download_event
Description:  store download event to file
************************************************************************/
void store_download_event ( IN DownloadArg  *downloadTaskPtr );

/***********************************************************************
Function: local_type_to_soap
Description:  convert tree type to soap type
************************************************************************/
ValueType local_type_to_soap ( IN Tr069TypeArg type );

/***********************************************************************
Function: get_inform_parameter_list
Description:create inform parameter list
************************************************************************/
Result get_inform_parameter_list ( OUT InformInfoStruct * informContentPtr );
/***********************************************************************
Function: update_transfer_eventcode
Description: check transfer directory, then update related commandkey and eventcode
************************************************************************/
void update_transfer_eventcode ( void );
void update_mreboot_eventcode();
/***********************************************************************
Function: update_inform_content
Description:  it will read parameters from agent's configuration tree and system environment. and generate the
informContent, which will be used in later soap encapsulation packet.
************************************************************************/
Result update_inform_content ( OUT InformInfoStruct * informContentPtr );
/***********************************************************************
Function: update_transfer_complete_content
Description:  According to the acs server's Maximum envelopes number ,it add transferComplete event to the transferCmpList for
soap function to encapsulate packets.
************************************************************************/
Result update_transfer_complete_content ( OUT TransferCompleteList ** transferCmpListP, IN PAL_UINT32_T  maxMaxEnvelopes );

/***********************************************************************
Function: remove_transfer_done_event
Description: when a transferComplete Response received, the related transfer record list should be clear in this function.
************************************************************************/
Result remove_transfer_done_event ( IN OUT TransferCompleteList * transferCmpList );

/***********************************************************************
Function: result_to_fault
Description: convert the ret value to tr69 soap fault code.
************************************************************************/
FaultType result_to_fault ( Result ret );

/***********************************************************************
Function: access_string_to_type
Description: convert the access List string to tr069 tree access type
************************************************************************/
Tr069AccessList access_string_to_type ( IN char * accessList );

/***********************************************************************
Function: convert_soap_to_local_value
Description: convert soap parameter value to tr069 tree value structure
************************************************************************/
void convert_soap_to_local_value ( IN ParameterValueStruct *paramValuePtr, OUT Tr069ValueArg **treeValueP );

/***********************************************************************
Function: convert_local_name_to_soap
Description:  convert data to soap struct ,notice that *paramNameP maybe be a valid pointer,
  notice that , *paramNameP return the head pointer of list,
************************************************************************/
void convert_local_name_to_soap ( IN Tr069NameArg ** treeNameP, OUT ParameterInfoStruct ** paramNameP );


/***********************************************************************
Function: convert_soap_to_local_value
Description:  convert data to soap struct ,notice that *paramValueP maybe be a valid pointer,
  notice that , *paramValueP return the head pointer of list,
************************************************************************/
void convert_local_value_to_soap ( IN Tr069NameArg ** treeNameP, IN Tr069ValueArg ** treeValueP, IN OUT ParameterValueStruct ** paramValueP );


/***********************************************************************
Function: access_string_to_type
Description: convert data to soap struct,notice that , parameterAttributePtr return the head pointer of list
  notice that , parameterAttributePtr return the head pointer of list,
************************************************************************/
void convert_local_attribute_to_soap ( IN Tr069NameArg ** treeNameArgP, IN Tr069AttributeArg ** treeAttributeArgP, OUT ParameterAttributeStruct ** parameterAttributeP );
/***********************************************************************
Function: handle_server_rpc_commands
Description:  main rpc commands handling function. it will get command and paramter from soapInfo,
and execute the command to get the result. after that, it will call generate_soap_packet function to generate
a soap buffer which ready to sent to remote acs server.
************************************************************************/
Result handle_server_rpc_commands ( IN SOAP * soapInfo, OUT char  **sendBuffer, OUT PAL_UINT32_T * sendBufferLen );

#endif
