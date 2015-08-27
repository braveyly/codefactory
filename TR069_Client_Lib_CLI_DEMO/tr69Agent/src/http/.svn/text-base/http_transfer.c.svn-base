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
Filename: http_transfer.c
Author: Gavin
Date: 2009-05-26
Description:
***********************************************************************/
#include "pal.h"
#include "http_client.h"
#include "http_public.h"


/***********************************************************************
Function: destroy_session_socket
Description:  destroy the socket and ssl resources.

Return: Result ,
Input: int *sockFdPtr,
       void ** sslP,
       void ** ctxP,
       int sslFlag,
Output:
Others:
************************************************************************/
Result  destroy_session_socket ( int *sockFdPtr, void ** sslP, void ** ctxP, int sslFlag )
{
    Result ret = TCMAGENT_OK;
    //reset scoket
#ifdef USE_SSL
    if ( sslFlag == 1 && * ( ( SSL ** ) sslP ) != NULL && * ( ( SSL_CTX ** ) ctxP ) != NULL )
    {
        SSL_free ( * ( ( SSL ** ) sslP ) );
        SSL_CTX_free ( * ( ( SSL_CTX ** ) ctxP ) );
        *sslP = NULL;
        *ctxP = NULL;
    }
#endif
    close ( *sockFdPtr );
    return ret;
}

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
Result  http_download_file ( IN char *url,  IN char *userName, IN char *passWord,
                             OUT char **fileContent, IN PAL_INT32_T *fileSize )
{

    Result ret = TCMAGENT_OK;
    int sockFd; /* main session socket */
    int sslConn = 0;
    char httpPath[HTTP_PATH_LENGTH];
    char serverName[HOST_LENGTH];
    char serverIp[IP_LENGTH];
    int    serverPort;
    char sendBuf[HTTP_DOWNLOAD_SEND_LENGTH];
    char recvBuf[HTTP_DOWNLOAD_RECV_LENGTH+1];
    int len = 0, recvLen = 0, resp = 0, status = 0, contentLength = -1;
    HttpStatus httpStatus;
    char *temp_ptr = NULL;

#ifdef USE_SSL
    SSL * sockSSL = NULL;
    SSL_CTX * sockCTX = NULL;
#endif

    AuthStatus authStatus = NO_AUTH;
    DigestAuth  digestAuth;

#ifdef USE_SSL
    ret = init_session_socket ( &sockFd, ( void ** ) & sockSSL, ( void ** ) & sockCTX, serverName, serverIp, &serverPort, httpPath, &sslConn, url );
#else
    ret = init_session_socket ( &sockFd, NULL, NULL, serverName, serverIp, &serverPort, httpPath, &sslConn, url );
#endif

    if ( ret != TCMAGENT_OK )
        return ret;

    if ( userName != NULL && passWord != NULL  && strlen ( userName ) > 0 && strlen ( passWord ) > 0 ) /* for get, just use basic as default */
        authStatus = BASIC_AUTH;

    while ( status < HTTP_MAX_TRY_TIMES )   /* let auth pass */
    {
        memset ( sendBuf, 0, sizeof ( sendBuf ) );
        ret = gen_http_header ( tGetMethod, authStatus, &digestAuth, 0, httpPath, serverIp, serverPort, 0, userName, passWord, sendBuf, DOWNLOAD_HEAD );

        if ( ret != TCMAGENT_OK )
        {
            goto download_end;
        }

        len = strlen ( sendBuf );
        /* send out the http packet */
#ifdef USE_SSL
        ret = send_raw ( &sockFd, ( void ** ) ( &sockSSL ), sendBuf, len , sslConn );
#else
        ret = send_raw ( &sockFd, NULL, sendBuf, len, sslConn );
#endif

        if ( ret != TCMAGENT_OK )
        {
            goto download_end;
        }

        len = 0; /* used to statistics the overall recving length */

        if ( *fileSize == 0 ) /* download have not specify the length */
            *fileSize = -1;

        do
        {
            memset ( recvBuf, 0, sizeof ( recvBuf ) );
#ifdef USE_SSL
            ret = recv_raw ( &sockFd, ( void ** ) ( &sockSSL ), recvBuf, HTTP_SOAP_MAX_RECV_LENGTH, sslConn, &recvLen );
#else
            ret = recv_raw ( &sockFd, NULL, recvBuf, HTTP_SOAP_MAX_RECV_LENGTH, sslConn, &recvLen );
#endif

            if ( ret != TCMAGENT_OK )
            {
                if ( *fileContent != NULL )
                {
                    PAL_FREE ( *fileContent );
                    *fileContent = NULL;
                }
                goto download_end;
            }

            if ( resp == 0 )
            {
                httpStatus = parse_http_header ( recvBuf, httpPath, &authStatus, &digestAuth, &contentLength , DOWNLOAD_HEAD );
                switch ( httpStatus )
                {
                    case STATUS_OK:
                        if ( *fileSize == -1 && contentLength != 0 ) /* update the fileSize with contentLength */
                            *fileSize = contentLength;

                        printf ( "\n===download file size [%d],contentLength[%d]==\n", *fileSize, contentLength );
                        //Http body is not NULL, then process it
                        temp_ptr = strstr ( recvBuf, "\r\n\r\n" );
                        if ( temp_ptr == NULL )
                        {
                            ret = HTTP_FORMAT_ERROR;
                            goto download_end;
                        }

                        recvLen = recvLen - ( temp_ptr + 4 - recvBuf );
                        if ( recvLen < 0 )
                        {
                            ret = SOCKET_RW_ERROR;
                            goto download_end;
                        }

                        *fileContent = ( char * ) PAL_MALLOC ( *fileSize );
                        if ( *fileContent == NULL )
                        {
                            printf ( "\n====download malloc memery[%d] failed==\n", *fileSize );
                            ret = MALLOC_FAILURE;
                            goto download_end;
                        }
                        else
                        {
                            printf ( "\n====download malloc memery success=[%d]=\n", *fileSize );
                            memset ( *fileContent, 0, *fileSize );
                            sleep ( 1 );
                        }

                        memcpy ( *fileContent, temp_ptr + 4, recvLen );
                        len += recvLen;

                        status = HTTP_MAX_TRY_TIMES;
                        resp = 1;
                        ret = TCMAGENT_OK;
                        break;
                    case STATUS_NEED_AUTH:
                        status++;/* send again, this time will update the auth information */
                        len = *fileSize + 1;
                        ret = HTTP_AUTH_ERROR;
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "HTTP need auth\n" );
                        break;
                    case UNKNOW_STATUS_CODE:
                    case FORMAT_ERROR:
                    case PARSER_ERROR:
                    default:
                        status++;
                        ret = SOCKET_RW_ERROR;
                        len = *fileSize + 1;
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "HTTP Unknow status [%d]\n", httpStatus );
                        break;

                }
            }
            else
            {
                /* just copy data  */
                if ( ( *fileSize - len ) < recvLen )
                    recvLen = ( *fileSize - len );
                memcpy ( ( *fileContent ) + len, recvBuf, recvLen );
                len += recvLen;
            }
        }
        while ( len < *fileSize );
    }

download_end:
#ifdef USE_SSL
    destroy_session_socket ( &sockFd, ( void ** ) &sockSSL, ( void ** ) &sockCTX, sslConn );
#else
    destroy_session_socket ( &sockFd, NULL, NULL, sslConn );
#endif
    return ret;
}


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
Result  http_upload_file ( IN char *fileType, IN char *url, IN char *userName, IN char *passWord,
                           IN char *fileContent, PAL_UINT32_T fileSize )
{
    Result ret = TCMAGENT_OK;
    int sockFd; /* main session socket */
    int sslConn = 0;
    char httpPath[HTTP_PATH_LENGTH];
    char serverName[HOST_LENGTH];
    char serverIp[IP_LENGTH];
    int  serverPort;
    char sendBuf[HTTP_UPLOAD_SEND_LENGTH];
    char recvBuf[HTTP_UPLOAD_RECV_LENGTH];
    int len = 0, recvLen = 0, status = 0, contentLength = -1;
    HttpStatus httpStatus;
    char *p = NULL;
    char packetHead[PACKET_HEAD_LENGTH];
    char packetTail[PACKET_TAIL_LENGTH];
    char fileName[FILE_NAME_LEGNTH];
    char timestr[TIME_STR_LENGTH];

#ifdef USE_SSL
    SSL * sockSSL = NULL;
    SSL_CTX * sockCTX = NULL;
#endif

    AuthStatus authStatus = NO_AUTH;
    DigestAuth  digestAuth;

    if ( fileContent == NULL || fileSize > ( HTTP_SOAP_MAX_SEND_LENGTH - 1024 ) )
    {
        printf ( "\n==error, the file is null or fileSize is too large [%d]===\n", fileSize );
        return UPLOAD_FAILURE; /*the send buf is HTTP_SOAP_MAX_SEND_LENGTH currently */
    }

#ifdef USE_SSL
    ret = init_session_socket ( &sockFd, ( void ** ) & sockSSL, ( void ** ) & sockCTX, serverName, serverIp, &serverPort, httpPath, &sslConn, url );
#else
    ret = init_session_socket ( &sockFd, NULL, NULL, serverName, serverIp, &serverPort, httpPath, &sslConn, url );
#endif

    if ( ret != TCMAGENT_OK )
        return ret;

    /*judge the path  /upload/ /upload/tt.img*/
    len = strlen ( httpPath );
    if ( httpPath[len-1] == '/' )
    {
        memset ( fileName, 0, sizeof ( fileName ) );
        pal_datetime ( timestr );//use time to diff name
        if ( strlen ( timestr ) > 10 )
            strncpy ( fileName, timestr, 10 );
        else
            strcpy ( fileName, timestr );

        /* not name specified */
        if ( strcmp ( fileType, UPLOAD_CONFIG_TYPE ) == 0 )
            strcat ( fileName, DEFAULT_CONFIG_NAME );
        else if ( strcmp ( fileType, UPLOAD_LOG_TYPE ) == 0 )
            strcat ( fileName, "log.txt" );
        else
            strcat ( fileName, DEFAULT_X_NAME );

        //pal_datetime(timestr);//use time to diff name
        //strcat(fileName,timestr);
    }
    else
    {
        p = strrchr ( httpPath, '/' );
        strcpy ( fileName, p + 1 );
        * ( p + 1 ) = 0;
    }

    /* compose the head */
    memset ( packetHead, 0, sizeof ( packetHead ) );
    strcpy ( packetHead, "--" );
    strcat ( packetHead, BOUNDARY );
    strcat ( packetHead, "\r\n" );
    strcat ( packetHead, "Content-Disposition:form-data;" );
    strcat ( packetHead, "name=\"" );
    strcat ( packetHead, fileName );
    strcat ( packetHead, "\";" );
    strcat ( packetHead, "filename=\"" );
    strcat ( packetHead, fileName ); /*filename */
    strcat ( packetHead, "\"\r\n" );
    if ( strcmp ( fileType, UPLOAD_CONFIG_TYPE ) == 0 )
        strcat ( packetHead, "Content-Type: application/zip" );
    else
        strcat ( packetHead, "Content-Type: text/plain" );
    strcat ( packetHead, "\r\n\r\n" );

    /*compose the tail */
    memset ( packetTail, 0, sizeof ( packetTail ) );
    strcpy ( packetTail, "\r\n--" );
    strcat ( packetTail, BOUNDARY );
    strcat ( packetTail, "\r\n" );

    strcat ( packetTail, "Content-Disposition:form-data; name=\"file2\";filename=\"\"\r\n" );
    strcat ( packetTail, "application/octet-stream\r\n\r\n" );

    strcat ( packetTail, "\r\n--" );
    strcat ( packetTail, BOUNDARY );
    strcat ( packetTail, "--\r\n" );

    contentLength = fileSize + strlen ( packetHead ) + strlen ( packetTail );

    while ( status < HTTP_MAX_TRY_TIMES )   /* let auth pass */
    {
        memset ( sendBuf, 0, sizeof ( sendBuf ) );
        ret = gen_http_header ( tPostMethod, authStatus, &digestAuth, 0, httpPath, serverIp, serverPort, contentLength, userName, passWord, sendBuf, UPLOAD_HEAD );

        if ( ret != TCMAGENT_OK )
        {
            goto upload_end;
        }

        //printf("\n====fileContent[%s],fileSize[%d]==\n",fileContent,fileSize);
        strcat ( sendBuf, packetHead );
        len = strlen ( sendBuf );
        memcpy ( sendBuf + len, fileContent, fileSize );
        memcpy ( sendBuf + len + fileSize, packetTail, strlen ( packetTail ) );
        len = len + fileSize + strlen ( packetTail );

        /* send out the http packet */
#ifdef USE_SSL
        ret = send_raw ( &sockFd, ( void ** ) ( &sockSSL ), sendBuf, len , sslConn );
#else
        ret = send_raw ( &sockFd, NULL, sendBuf, len, sslConn );
#endif

        if ( ret != TCMAGENT_OK )
        {
            goto upload_end;
        }

        memset ( recvBuf, 0, sizeof ( recvBuf ) );
        recvLen = 0;

#ifdef USE_SSL
        ret = recv_raw ( &sockFd, ( void ** ) ( &sockSSL ), recvBuf, HTTP_SOAP_MAX_RECV_LENGTH, sslConn, &recvLen );
#else
        ret = recv_raw ( &sockFd, NULL, recvBuf, HTTP_SOAP_MAX_RECV_LENGTH, sslConn, &recvLen );
#endif

        if ( ret != TCMAGENT_OK )
        {
            goto upload_end;
        }

        httpStatus = parse_http_header ( recvBuf, httpPath, &authStatus, &digestAuth, &contentLength , UPLOAD_HEAD );
        switch ( httpStatus )
        {
            case STATUS_OK:
                //Http body is not NULL, then process it
                p = strstr ( recvBuf, "\r\n\r\n" );
                if ( p == NULL )
                {
                    ret = HTTP_FORMAT_ERROR;
                    goto upload_end;
                }

                status = HTTP_MAX_TRY_TIMES;
                ret = TCMAGENT_OK;
                break;
            case STATUS_NEED_AUTH:
                status++;/* send again, this time will update the auth information */
                ret = HTTP_AUTH_ERROR;
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "HTTP need auth\n" );
                break;
            case UNKNOW_STATUS_CODE:
            case FORMAT_ERROR:
            case PARSER_ERROR:
            default:
                status++;
                ret = SOCKET_RW_ERROR;

                if ( PARSER_ERROR == httpStatus )
                {
                    status = HTTP_MAX_TRY_TIMES;
                }

                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "HTTP Unknow status [%d]\n", httpStatus );
                break;
        }
    }

upload_end:
#ifdef USE_SSL
    destroy_session_socket ( &sockFd, ( void ** ) &sockSSL, ( void ** ) &sockCTX, sslConn );
#else
    destroy_session_socket ( &sockFd, NULL, NULL, sslConn );
#endif
    return ret;
}
