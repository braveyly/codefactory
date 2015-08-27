#ifndef _FTP_PUBLIC_H_
#define _FTP_PUBLIC_H_

#include "pal.h"
#include "tr69_agent_public.h"

/**************************************************************************
 Function Name: ftp_download_file
 Description  : download a file specified by url/filename from a file 
                server , save file data into global buffer .
 Returns      : status indicate download result, 0 indicate success, other
                no-zero value indicate fault error code.
                Fault is classified in the following case:
                  9001, Request denied
                  9002, Internal error (Such as malloc failed,etc.)
                  9010, Download failure (network error)
                  9012, File transfer server authentication failure
                    
 Parameter :
                Input:
                  url: file server path indicating a file location .
				  userName: used to authenticate with server .
				  passWord: used to authenticate with server .

			    Output:
                  fileContent: download file data stream, It's malloced 
                               in the function, and the function caller 
                               release it.
                  fileSize: download file data length
**************************************************************************/
PAL_INT32_T  ftp_download_file(IN char *url
                              , IN char *username
                              , IN char *passwoid
							  , OUT char **fileContent
                              , OUT PAL_UINT32_T *fileSizePtr);




/**************************************************************************
 Function Name: ftp_upload_file
 Description: upload file into location specified by url .
 Returns: status indicate upload result, 0 indicate success , other
                no-zero value indicate fault error code.
                Fault is classified in the following case:
                  9001, Request denied
                  9002, Internal error (Such as malloc failed,etc.)
                  9011, Upload failure (network error)
                  9012, File transfer server authentication failure
 Parameter :
                Input:
                  url: file server path indicate a file location.
				  fileContent: buffer contained file content need be
				                 uploaded . It's malloced and released
				                 by caller.
                  fileSize: file content length
				  userName: used to authenticate with server.
				  passWord: used to authenticate with server.

			    Output:
**************************************************************************/
PAL_INT32_T ftp_upload_file(IN char *fileType
                           , IN char *url
                           , IN char *username
                           , IN char *password
                           , IN char *fileContent
                           , IN PAL_UINT32_T fileSize);

#endif /* _FTP_PUBLIC_H_ */
