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
Filename: http_public.h
Author: Gavin
Date: 2009-05-26
Description:
***********************************************************************/

#ifndef _HTTP_PUBLIC_H_
#define _HTTP_PUBLIC_H_

#include "pal.h"
#include "tr69_agent_public.h"

/**************************************************************************
Function Name: http_download_file
Description  : download a file specified by url from a file server , save file
                     data into global buffer .
 Returns      : status indicate download result, 0 indicate success , other no-zero value
                     indicate fault error code.
 Parameter :
                  Input :
                               url :  file server path indicate a file location .
       userName:  used to authenticate with server .
       passWord:   used to authenticate with server .
   Output:
                               fileContent:  download stream
                               fileSize:        download stream size
**************************************************************************/
extern Result  http_download_file ( IN char *url,  IN char *userName, IN char *passWord,
                                      OUT char **fileContent, OUT PAL_INT32_T *fileSize);

/**************************************************************************
 Function Name: http_upload_file
 Description  : upload file into location specified by url .
 Returns      : status indicate upload result, 0 indicate success , other no-zero value
                     indicate fault error code.
 Parameter :
                 Input :
                              url :   file server path indicate a file location .
       userName:    used to authenticate with server .
       passWord:            used to authenticate with server .
       fileContent :  buffer contained file content need be upload .
       fileSize :          file stream size .
   Output:
**************************************************************************/
Result  http_upload_file (IN char *fileType, IN char *url, IN char *userName, IN char *passWord, IN char *fileContent, PAL_UINT32_T fileSize );

/**************************************************************************
 Function Name: init_main_session_socket
 Description  : initialize a main session TCP connection through basic socket or SSL  with the acs server.
 Returns      : status indicate initialization result , 0 indicate success , other no-zero value
                    indicate error reason for failure .
       Error code:
            SUCCESS       0
     SOCK_RW_ERROR   -1
     SSL_LIB_ERROR    -2
     SOCK_FDSET_ERROR   -3
     SOCK_TIMEOUT    -4
 Parameter :
                    Input :
                        acsUrl:  http://172.16.0.1:40000/basic/TR069
                        acsUserName:  used to auth with server digest or basic
                        acsPassword:   used to auth with server digest or basic
             Output: none
**************************************************************************/
Result init_main_session_socket ( IN char * acsUrl);

/**************************************************************************
 Function Name: destroy_main_session_socket
 Description  : destroy a TCP connection through basic socket and/or SSL .
 Returns      : status indicate destroy result , 0 indicate success , -1 indicate error .
       Error code:
           SUCCESS       0
     FAILURE    -1
 Parameter :
                  Input :  none
   Output: none
**************************************************************************/
Result  destroy_main_session_socket ( void );

/**************************************************************************
 Function Name: send_packet_to_acs_server
 Description  : send out http post request with/without soap data ,receive packet and
      do process according to packet data, receiving a non-200/204 OK http packet
      should process internal ; receiving a 200/204 OK http packet should get soap
      data and call soap handle api :
    send packet :
   received a  packet :
   if( http 200 OK)
   {
    get soap data, stop send_and_receive loop ,
    return this soap data to function caller
   }
   else if( http 204 OK)
   {
    get empty packets
    return this soap data to function caller
   else
   {401,etc.
     //process response packet according to http state machine
     continue to  send original packet .
   }

 Returns      : status indicate process result , 0 indicate success , -1 indicate error .
       Error code:
           SUCCESS        0
     SOCK_RW_ERROR   -1
     SSL_LIB_ERROR    -2
     SOCK_FDSET_ERROR   -3
     SOCK_TIMEOUT    -4
     HTTP_AUTH_FAILURE    -5
 Parameter :
                  Input :
                          sBuffer:     sending data, not need to release the memory here
                         sBufferLen:  sending data length
   Output:
                          rBuffer:    receiving buffer, which need malloc here
                          rBufferLen:  receiving data length.
**************************************************************************/
Result send_packet_to_acs_server ( IN char *sBuffer, IN PAL_UINT32_T sBufferLen,
                                   OUT char ** rBufferP, OUT PAL_UINT32_T *rBufferLenPtr );


#endif
