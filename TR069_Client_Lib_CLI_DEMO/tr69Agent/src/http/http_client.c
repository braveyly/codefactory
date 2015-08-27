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
Filename: http_client.c
Author: Gavin
Date: 2009-05-26
Description:

***********************************************************************/
#include "pal.h"
#include "http_public.h"
#include "http_client.h"
#include "mainloop_public.h"
#include "digcalcu.h"
#include "base64.h"

/*
 * Define global variable ....
 */
int gSockFd; /* main session socket */
int gSslConn = 0;
char gHttpPath[HTTP_PATH_LENGTH];
char gServerName[HOST_LENGTH];
char gServerIp[IP_LENGTH];
int  gServerPort;
char gSendBuf[HTTP_SOAP_MAX_SEND_LENGTH];
char gRecvBuf[HTTP_SOAP_MAX_RECV_LENGTH];

char gMethodList[tMaxMethod][8] =
{
    "GET",
    "POST"
};

#ifdef USE_SSL
SSL * gSockSSL;
SSL_CTX * gSockCTX;
int gVerifyPeer = 0;
#endif

AuthStatus gAuthStatus = NO_AUTH;
unsigned short gNonceCount = 0;
unsigned short gCookieFlag = 0;
unsigned short cookieDiscardFlag = 0;
unsigned short gCookieVersion = 1;
SessionCookie sessionCookie[MAX_COOKIE_NUM];
DigestAuth  gDigestAuth;

unsigned short gChunkFlag = 0;

/* name ,ip cache  */
static NameIPCache gNameIPCache =
{
    "", ""
};

/***********************************************************************
Function: parse_url
Description:  parse url and get serverName, server(IP format),serverPort, path and ssl flag information


Return: Result,
Input: IN char *url,http://www.acsserver.com:8080/acs/TR069
       OUT char *serverName,
       OUT char *server,
       OUT int *serverPort,
       OUT char * path,
       OUT int *sslFlagPtr,
************************************************************************/
Result parse_url ( IN char *acsurl, OUT char *serverName, OUT char *server, OUT int *serverPort, OUT char * path, OUT int *sslFlagPtr )
{
    struct hostent *host = NULL;
    char *ptr = NULL, *ptr1 = NULL;
    char **ipP;
    char ip[16];
    char url[256];


    if ( acsurl == NULL || strlen ( acsurl ) < 7 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "ACS URL error in parse url\n" );
        return SOCKET_PARAM_ERROR;
    }

    strcpy ( url, acsurl );

    if ( memcmp ( url, "http://", 7 ) == 0 )
        *sslFlagPtr = 0;
    else if ( memcmp ( url, "https://", 8 ) == 0 )
        *sslFlagPtr = 1;
    else
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "ACS URL error [%s] in parse url\n", url );
        return SOCKET_PARAM_ERROR;
    }

    //remove http:// or https:// header
    ptr = url + 7 + ( *sslFlagPtr );

    //get path
    ptr1 = strstr ( ptr, "/" );

    if ( ptr1 == NULL )
    {
        strcpy ( path, "/" );
    }
    else
    {
        strcpy ( path, ptr1 );
        ptr1[0] = '\0';
    }

    //get serverPort
    ptr1 = strstr ( ptr, ":" );
    if ( ptr1 == NULL )
    {
        if ( ( *sslFlagPtr ) == 1 )
            *serverPort = 443;
        else
            *serverPort = 80;
    }
    else
    {
        *serverPort = atoi ( ptr1 + 1 );
        ptr1[0] = '\0';
    }


    //get serverName server
    if ( ptr == NULL || strlen ( ptr ) == 0 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "not server name in parse url\n" );
        return SOCKET_PARAM_ERROR;
    }

    strcpy ( serverName, ptr );

    if ( strcmp ( serverName, gNameIPCache.name ) != 0 )
    {
        host = gethostbyname ( ptr );
        if ( host == NULL )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "gethostbyname [%s] unsuccessfully.\n", ptr );
            return SOCKET_PARAM_ERROR;
        }
        ipP = host->h_addr_list;

        strcpy ( server, ( char * ) inet_ntop ( host->h_addrtype, *ipP, ip, sizeof ( ip ) ) );

        PAL_HOST_FREE ( host );

        strcpy ( gNameIPCache.name, serverName );
        strcpy ( gNameIPCache.ip, server );
    }
    else
        strcpy ( server, gNameIPCache.ip );

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "tr69 client parse url, server[%s], port[%d]", server, *serverPort );

    return TCMAGENT_OK;
}


#ifdef USE_SSL
char gKeyPasswd[64];// the password of client.key

/***********************************************************************
Function: password_cb
Description:  password callback function

Return:     static int,
Input: char *buf, the buffer for password;
       int size, the buffer size;
       int rwflag, read or write;
       void *userdata,  user's data
Output:
Others:
************************************************************************/
static int password_cb ( OUT char *buf, IN  int size, IN int rwflag, IN void *userdata )
{
    const char *pass = ( const char * ) userdata;
    int pass_len = strlen ( pass );

    if ( pass_len > size )
        return -1;

    strncpy ( buf, pass, pass_len );

    return pass_len;
}

/***********************************************************************
Function: verify_cb
Description:   verify cert callback function

Return:     static int,
Input: int ok, the cert verify is OK or not;
       X509_STORE_CTX *store, the pointer point to the context
Output:
Others:
************************************************************************/
static int verify_cb ( IN int ok, IN  X509_STORE_CTX *store )
{
    char data[256];
    if ( !ok )
    {


        X509 *cert = X509_STORE_CTX_get_current_cert ( store );
        int depth = X509_STORE_CTX_get_error_depth ( store );
        int err = X509_STORE_CTX_get_error ( store );

        //pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "Error with certificate at depth: %i\n", depth );

        X509_NAME_oneline ( X509_get_issuer_name ( cert ), data, 256 );
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, " error issuer = %s\n", data );

        X509_NAME_oneline ( X509_get_subject_name ( cert ), data, 256 );
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "error subject = %s\n", data );

        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "error code %i:%s\n", err, X509_verify_cert_error_string ( err ) );
    }

    return ok;
}


/***********************************************************************
Function: init_ssl_socket
Description:  bind socket fd to a ssl object.

Return: Result,
Input: IN int *sockfdPtr,  the http client socket
       OUT SSL **sslP,  ssl object binding with the socket
       OUT SSL_CTX **ctxP, ssl enviroment
Output:
Others:
************************************************************************/
Result init_ssl_socket ( IN int *sockfdPtr, OUT SSL **sslP, OUT SSL_CTX **ctxP )
{
    Result ret = TCMAGENT_OK;
    SSL_load_error_strings();
    if ( SSL_library_init() != 1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "load SSL lib failure.\n" );
        return SSL_LIB_ERROR;
    }

    if ( ( *ctxP = SSL_CTX_new ( SSLv23_method() ) ) == NULL )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "create SSLv23 context failure.\n" );
        return SSL_LIB_ERROR;
    }
    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "create SSLv23 context success.\n" );

    gCB.get_clientkey_passwd ( gKeyPasswd, &gVerifyPeer );
    if ( gVerifyPeer == 1 )
    {
        SSL_CTX_set_verify ( *ctxP, SSL_VERIFY_PEER, verify_cb );
        SSL_CTX_set_verify_depth ( *ctxP, SSL_DEPTH );
        SSL_CTX_set_options ( *ctxP, SSL_OP_NO_SSLv2 );
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "need to verify peer certificate.\n" );
    }

    if ( SSL_CTX_load_verify_locations ( *ctxP, CA_CERTIFICATE_FILE, NULL ) != 1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "load CA certificate failure.\n" );
        return SSL_CERTIFICATE_ERROR;
    }

    if ( strlen ( gKeyPasswd ) > 0 )
    {
        SSL_CTX_set_default_passwd_cb_userdata ( *ctxP, ( void * ) gKeyPasswd );
        SSL_CTX_set_default_passwd_cb ( *ctxP, password_cb );
    }

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "load CA certificate ok.\n" );

    if ( SSL_CTX_use_certificate_file ( *ctxP, CPE_CERTIFICATE_FILE, SSL_FILETYPE_PEM ) != 1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "load CPE certificate failure.\n" );
        return SSL_CERTIFICATE_ERROR;
    }
    else
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "load CPE certificate success.\n" );
    }

    if ( SSL_CTX_use_PrivateKey_file ( *ctxP, CPE_KEY_FILE, SSL_FILETYPE_PEM ) != 1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "load CPE key failure.\n" );
        return SSL_CERTIFICATE_ERROR;
    }
    else
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "load CPE key ok.\n" );
    }

    if ( SSL_CTX_check_private_key ( *ctxP ) != 1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "validate local certificate failure.\n" );
        return SSL_CERTIFICATE_ERROR;
    }
    else
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "validate local certificate success.\n" );
    }

    if ( ( *sslP = SSL_new ( *ctxP ) ) == NULL )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "create SSL object failure.\n" );
        return SSL_CERTIFICATE_ERROR;
    }
    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "create SSL object success.\n" );

    if ( SSL_set_fd ( *sslP, *sockfdPtr ) != 1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "bind SSL socket failure.\n" );
        return SSL_BIND_ERROR;
    }
    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "bind SSL socket success.\n" );

    {
        int  i;
        char stringall[129];
        srand ( time ( NULL ) );
        for ( i = 0; i < 128; i++ )
        {
            stringall[i] = ( rand() % ( 128 ) );
        }

        RAND_seed ( stringall, strlen ( stringall ) );
    }

    if ( SSL_connect ( *sslP ) != 1 )
    {
        ERR_print_errors_fp ( stderr );
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "SSL socket connect failure.\n" );
        return -1;
    }
    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "SSL socket connect success.\n" );

    return ret;
}
#endif


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
             Output: none
**************************************************************************/
Result init_main_session_socket ( IN char * acsUrl )
{
    Result ret = TCMAGENT_OK;
#ifdef USE_SSL
    ret = init_session_socket ( &gSockFd, ( void ** ) & gSockSSL, ( void ** ) & gSockCTX, gServerName, gServerIp, &gServerPort, gHttpPath, &gSslConn, acsUrl );
#else
    ret = init_session_socket ( &gSockFd, NULL, NULL, gServerName, gServerIp, &gServerPort, gHttpPath, &gSslConn, acsUrl );
#endif
    return ret;
}


/***********************************************************************
Function: init_session_socket
Description:  initialize a TCP connection through basic socket or SSL  with the given server ip and port.

Return: Result,
Input: OUT int *sockFdPtr, the created socket.
       OUT SSL **sslP,  if needed, the created ssl binding with the socket.
       OUT SSL_CTX **ctxP, if needed, the ssl environment
       OUT char *serverName, http server name
       OUT char *serverIP, http server IP
       OUT int *serverPort, http server port
       OUT char *path, http requested path
       OUT int *sslFlag, ssl enabled or disabled.
       IN char * acsUrl, the target url ,acs url of ftp/http url.
       IN char *acsUserName, http authenticated username
       IN char *acsPassword,  http authenticated password
Output:
Others:
************************************************************************/
Result init_session_socket ( OUT int *sockFdPtr, OUT void  **sslP, OUT void **ctxP, OUT char *serverName, OUT char *serverIP, OUT int *serverPort, OUT char *path, OUT int *sslFlag, IN char * acsUrl )
{
    Result ret = TCMAGENT_OK;
    struct timeval timeout;
    fd_set fds;
    int errlen, res;
    int error;
    unsigned long fc;
    struct sockaddr_in dest;

    if ( parse_url ( acsUrl, serverName, serverIP, serverPort, path, sslFlag ) != TCMAGENT_OK )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "parse acs url  failed\n" );
        return SOCKET_PARAM_ERROR;
    }

    if ( ( *sockFdPtr = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, " create main session socket failed\n" );
        return SOCKET_CREATE_ERROR;
    }

    bzero ( &dest, sizeof ( &dest ) );
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr ( serverIP );
    dest.sin_port = htons ( *serverPort );

    fc = 1;
    if ( ioctl ( *sockFdPtr, FIONBIO, &fc ) == -1 )
    {
        return SOCKET_IOCTL_ERROR;
    }

    if ( connect ( *sockFdPtr, ( struct sockaddr * ) &dest, sizeof ( struct sockaddr ) ) == -1 )
    {
        errlen = sizeof ( int );
        FD_ZERO ( &fds );
        FD_SET ( *sockFdPtr, &fds );
        timeout.tv_sec = TIME_OUT;
        timeout.tv_usec = 0;

        if ( ( res = select ( *sockFdPtr + 1, NULL, &fds, NULL, &timeout ) ) > 0 )
        {
            getsockopt ( *sockFdPtr, SOL_SOCKET, SO_ERROR, &error, ( socklen_t * ) &errlen );
            if ( error != 0 )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "Connect error: %d\n", error );
                return SOCKET_RW_ERROR;
            }
        }
        else
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "Connect Timeout\n" );
            return SOCKET_TIMEOUT;
        }
    }
    else
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "connected\n" );
    }

    fc = 0;
    if ( ioctl ( *sockFdPtr, FIONBIO, &fc ) == -1 )
    {
        //pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_HTTP, __func__, "ioctl error\n" );
        return SOCKET_IOCTL_ERROR;
    }

#ifdef USE_SSL
    if ( *sslFlag == 1 )
        ret = init_ssl_socket ( sockFdPtr, ( SSL ** ) sslP, ( SSL_CTX ** ) ctxP );
#else
    if ( *sslFlag == 1 )
        ret = SSL_NOT_SUPPORT;
#endif

    return ret;
}

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
Result  destroy_main_session_socket ( void )
{
    Result ret = TCMAGENT_OK;
    //reset scoket
#ifdef USE_SSL
    if ( gSslConn == 1 )
    {
        SSL_free ( gSockSSL );
        SSL_CTX_free ( gSockCTX );
    }
#endif
    close ( gSockFd );
    return ret;
}

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
                                   OUT char ** rBufferP, OUT PAL_UINT32_T *rBufferLenPtr )
{
    Result ret = TCMAGENT_OK, status = 0;
    int len = 0, recvLen = 0;
    HttpStatus httpStatus;
    char *p = NULL, *rcvPtr = NULL;
    int contentLength = -1;
    int chunk_size, total_chunk_size;
    char *http_body = NULL, *chunk_body = NULL, *chunk_start = NULL;
    int recv_body_len = 0;
    char tmprecvbuf[HTTP_SOAP_MAX_RECV_LENGTH];
    int tmp_recv_len = 0;
    char *str = NULL;
    char tmp_buf[30];

#if 0
    printf ( "\n======send packet[%d]=", sBufferLen );
    if ( sBufferLen<32000 && sBufferLen >0 )
        printf ( "====content[%s]==\n", sBuffer );
#endif

    while ( status < HTTP_MAX_TRY_TIMES )
    {
        memset ( gSendBuf, 0, sizeof ( gSendBuf ) );

        /* generate the http packet */
        ret = gen_http_header ( tPostMethod, gAuthStatus, &gDigestAuth, gCookieFlag, gHttpPath, gServerName, gServerPort, sBufferLen, gManagementServer.acsUsername, gManagementServer.acsPassword, gSendBuf, SOAP_HEAD );

        if ( ret != TCMAGENT_OK || gSendBuf == NULL || sBufferLen < 0 )
            return SOCKET_RW_ERROR;

        len = strlen ( gSendBuf );

        if ( sBufferLen > 0 )
            memcpy ( gSendBuf + len, sBuffer, sBufferLen );

        len += sBufferLen;
        /* send out the http packet */
#ifdef USE_SSL
        ret = send_raw ( &gSockFd, ( void ** ) ( &gSockSSL ), gSendBuf, len , gSslConn );
#else
        ret = send_raw ( &gSockFd, NULL, gSendBuf, len, gSslConn );
#endif

        if ( ret != TCMAGENT_OK )
        {
            status++;
            continue;
        }

        memset ( gRecvBuf, 0, sizeof ( gRecvBuf ) );
        rcvPtr = gRecvBuf;

continue_recv_tag:
        memset ( tmprecvbuf, 0, sizeof ( tmprecvbuf ) );

#ifdef USE_SSL
        ret = recv_raw ( &gSockFd, ( void ** ) ( &gSockSSL ), tmprecvbuf, HTTP_SOAP_MAX_RECV_LENGTH, gSslConn, &tmp_recv_len );
#else
        ret = recv_raw ( &gSockFd, NULL, tmprecvbuf, HTTP_SOAP_MAX_RECV_LENGTH, gSslConn, &tmp_recv_len );
#endif
        // printf("\n==== recv packet from server[%s]=ret[%d]=,recvLen[%d]=\n",gRecvBuf,ret,recvLen);
        if ( ret != TCMAGENT_OK || tmp_recv_len <= 0 )
            return ret;

        if ( recvLen + tmp_recv_len >= sizeof ( gRecvBuf ) )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "http extend recv buf\n" );
            return HTTP_FORMAT_ERROR;
        }

        memcpy ( rcvPtr + recvLen,  tmprecvbuf, tmp_recv_len );
        recvLen += tmp_recv_len;


        httpStatus = parse_http_header ( gRecvBuf, gHttpPath, &gAuthStatus, &gDigestAuth, &contentLength, SOAP_HEAD );

        switch ( httpStatus )
        {
            case STATUS_OK:
                //Http body is not NULL, then process it

                if ( contentLength > 0 )
                {
                    http_body = strstr ( gRecvBuf, "\r\n\r\n" );
                    if ( http_body == NULL ) /* cannot find http body */
                        return HTTP_FORMAT_ERROR;
                    http_body += 4;

                    recv_body_len = recvLen - ( http_body - gRecvBuf );
                    if ( recv_body_len < contentLength )
                    {
                        goto continue_recv_tag;
                    }

                    *rBufferLenPtr = contentLength;

                    *rBufferP = ( char * ) PAL_MALLOC ( ( *rBufferLenPtr ) + 1 );
                    if ( *rBufferP != NULL )
                    {
                        memset ( *rBufferP, 0, ( *rBufferLenPtr ) + 1 );
                        memcpy ( *rBufferP, http_body, contentLength );
                    }
                    else
                        return MALLOC_FAILURE;

                }
                else if ( gChunkFlag )
                {
                    http_body = strstr ( gRecvBuf, "\r\n\r\n" );
                    if ( http_body == NULL ) /* cannot find http body */
                        return HTTP_FORMAT_ERROR;
                    http_body += 4;


                    str = strstr ( http_body, "\r\n\r\n" ); /*to find http chunk end flag*/
                    if ( !str )
                    {
                        goto continue_recv_tag;
                    }

                    chunk_size = 0;
                    total_chunk_size = 0;
                    chunk_start = http_body;
                    memset ( tmprecvbuf, 0, sizeof ( tmprecvbuf ) );
                    rcvPtr = tmprecvbuf;
                    do
                    {
                        str = strstr ( chunk_start, "\r\n" );
                        if ( str )
                        {
                            memset ( tmp_buf, 0, sizeof ( tmp_buf ) );
                            memcpy ( tmp_buf, chunk_start, str - chunk_start );
                            chunk_size = strtol ( tmp_buf, NULL, 16 );

                            if ( chunk_size < 0 )
                            {
                                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "chunk size parse error: %d\n", chunk_size );
                                return HTTP_FORMAT_ERROR;
                            }

                            chunk_body = str + 2;
                            if ( total_chunk_size + chunk_size > recvLen )
                            {
                                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "chunk size extend error: current size %d, total size%d, recvlen %d\n", chunk_size, total_chunk_size, recvLen );
                                return HTTP_FORMAT_ERROR;
                            }

                            memcpy ( rcvPtr, chunk_body, chunk_size );
                            rcvPtr += chunk_size;

                            chunk_start = chunk_body + chunk_size + 2; /*2 is chunk boundary*/
                            total_chunk_size += chunk_size;

                        }
                        else
                        {
                            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "chunk format parse error\n" );
                            return HTTP_FORMAT_ERROR;
                        }
                    }
                    while ( chunk_size );


                    if ( total_chunk_size )
                    {
                        *rBufferLenPtr = total_chunk_size;

                        *rBufferP = ( char * ) PAL_MALLOC ( ( *rBufferLenPtr ) + 1 );
                        if ( *rBufferP != NULL )
                        {
                            memset ( *rBufferP, 0, ( *rBufferLenPtr ) + 1 );
                            memcpy ( *rBufferP, tmprecvbuf,  total_chunk_size );
                        }
                        else
                        {
                            return MALLOC_FAILURE;
                        }

                    }
                    else
                    {
                        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "total chunk size is error: %d\n", total_chunk_size );
                        return HTTP_FORMAT_ERROR;
                    }

                }
                else
                {
                    *rBufferLenPtr = 0;
                    return HTTP_FORMAT_ERROR;
                }

#if 0
                if ( *rBufferP != NULL )
                    printf ( "\n===gavin_debug, http recv buf[%s]===\n", *rBufferP );
#endif

                status = HTTP_MAX_TRY_TIMES;
                ret = TCMAGENT_OK;
                break;
            case STATUS_NO_CONTENT:
                *rBufferLenPtr = 0;
                status = HTTP_MAX_TRY_TIMES;
                ret = TCMAGENT_OK;
                break;
            case STATUS_NEED_AUTH:
                status++;/* send again, this time will update the auth information */
                ret = HTTP_AUTH_ERROR;
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "HTTP need auth\n" );

                //Http body is not NULL, then process it
                p = strstr ( gRecvBuf, "\r\n\r\n" );
                if ( p == NULL ) /* cannot find the head */
                {
                    printf ( "\n===cannot find the auth packet head==\n" );
                    return HTTP_FORMAT_ERROR;
                }

                recvLen = recvLen - ( p + 4 - gRecvBuf );
                // printf("\n===receive len is [%d]=,contentLength is [%d]=\n",recvLen,contentLength);
                if ( recvLen < 0 )
                {
                    return SOCKET_RW_ERROR;
                }

                while ( recvLen < contentLength )
                {
                    contentLength = contentLength - recvLen;
#ifdef USE_SSL
                    ret = recv_raw ( &gSockFd, ( void ** ) ( &gSockSSL ), gRecvBuf, HTTP_SOAP_MAX_RECV_LENGTH, gSslConn, &recvLen );
#else
                    ret = recv_raw ( &gSockFd, NULL, gRecvBuf, HTTP_SOAP_MAX_RECV_LENGTH, gSslConn, &recvLen );
#endif
                    if ( ret != TCMAGENT_OK || recvLen > contentLength || recvLen <= 0 )
                    {
                        recvLen = 0;
                        contentLength = -1;
                    }
                }
                break;
            case UNKNOW_STATUS_CODE:
            case FORMAT_ERROR:
            case PARSER_ERROR:
            default:
                status++;
                ret = SOCKET_RW_ERROR;
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "HTTP Unknow status [%d]\n", httpStatus );
                break;

        }
    }

    return ret;
}

/***********************************************************************
Function: send_raw
Description:  send raw data through the given socket or ssl.

Return: Result ,
Input: IN int *sockFdPtr, the given socket
       IN void **sslP,  the given ssl
       IN char * sendBuf,  ready to send data
       IN int sendLength, sending data length
       IN int sslFlag,  ssl enable or disable
Output:
Others:
************************************************************************/
Result  send_raw ( IN int *sockFdPtr, IN void **sslP, IN char * sendBuf, IN int sendLength, IN int sslFlag )
{
    Result ret;
    int send_ret_value;
    fd_set sendfds;
    struct timeval timeout;

    FD_ZERO ( &sendfds );
    FD_SET ( *sockFdPtr, &sendfds );
    timeout.tv_sec = TIME_OUT;
    timeout.tv_usec = 0;

    if ( sendBuf == NULL || strlen ( sendBuf ) < 1 )
        return TCMAGENT_OK;

    if ( select ( *sockFdPtr + 1, NULL, &sendfds, NULL, &timeout ) > 0 )
    {
        if ( FD_ISSET ( *sockFdPtr, &sendfds ) )
        {
            if ( sslFlag == 1 )
            {
#ifdef USE_SSL
                send_ret_value = SSL_write ( * ( ( SSL ** ) sslP ), sendBuf, sendLength );
                //printf("\n=====ssl send packet real len[%d], task len[%d]==\n",send_ret_value,sendLength);
                if ( send_ret_value < sendLength )
                {
                    ret = SOCKET_RW_ERROR;
                }
                else
                {
                    ret = TCMAGENT_OK;
                }
#else
                ret = SSL_LIB_ERROR;
#endif

            }
            else
            {
                send_ret_value = send ( *sockFdPtr, sendBuf, sendLength, MSG_NOSIGNAL );

                // printf("\n===gavin_test, send length is [%d]==\n",send_ret_value);

                if ( send_ret_value < sendLength )
                {
                    ret = SOCKET_RW_ERROR;
                }
                else
                {
                    ret = TCMAGENT_OK;
                }
            }
        }
        else
        {
            ret = SOCKET_FDSET_ERROR;
        }
    }
    else
    {
        ret = SOCKET_TIMEOUT;
    }

    return ret;
}

/***********************************************************************
Function: recv_raw
Description:  receive data through given socket or ssl

Return: Result,
Input: IN int *sockFdPtr, the pointer point to the socket
       IN void **sslP,  the pointer point to  the pointer point to the ssl
       OUT char *recvBuf, the pointer point to the receiving data
       IN int length, max recv length length
       IN int sslFlag, ssl enable
       OUT int *recvLenPtr, real recv length
Output:
Others:
************************************************************************/
Result recv_raw ( IN int *sockFdPtr, IN void **sslP, OUT char *recvBuf, IN  int length, IN  int sslFlag, OUT int *recvLenPtr )
{
    Result ret = TCMAGENT_OK;
    int recv_ret_value;
    fd_set recvfds;
    struct timeval timeout;

    FD_ZERO ( &recvfds );
    FD_SET ( *sockFdPtr, &recvfds );
    timeout.tv_sec = TIME_OUT;
    timeout.tv_usec = 0;
    //printf("\n====gavin_debug,recv raw===\n");
    if ( select ( *sockFdPtr + 1, &recvfds, NULL, NULL, &timeout ) > 0 )
    {
        if ( FD_ISSET ( *sockFdPtr, &recvfds ) )
        {
            if ( sslFlag == 1 )
            {
#ifdef USE_SSL
                //printf("\n===gavin_debug ,before ssl_read===\n");
                recv_ret_value = SSL_read ( * ( ( SSL ** ) sslP ), recvBuf, length );
                //printf("\n====ssl read length ,real[%d], buf[%d]==\n",recv_ret_value,length);

                if ( recv_ret_value < 0 )
                {
                    ret = SOCKET_RW_ERROR;
                }
                else
                {
                    if ( recv_ret_value > length )
                    {
                        recvBuf[0] = '\0';
                        ret = SOCKET_RW_ERROR;
                    }
                    else
                    {
                        *recvLenPtr = recv_ret_value;
                        recvBuf[recv_ret_value] = '\0';
                    }
                }
#else
                ret = SSL_LIB_ERROR;
#endif
            }
            else
            {
                recv_ret_value = recv ( *sockFdPtr, recvBuf, length, MSG_NOSIGNAL );

                if ( recv_ret_value < 0 )
                {
                    ret = SOCKET_RW_ERROR;
                }
                else
                {
                    if ( recv_ret_value > length )
                    {
                        recvBuf[0] = '\0';
                        ret = SOCKET_RW_ERROR;
                    }
                    else
                    {
                        *recvLenPtr = recv_ret_value;
                        recvBuf[recv_ret_value] = '\0';
                    }
                }
            }
        }
        else
        {
            ret = SOCKET_IOCTL_ERROR;
        }
    }
    else
    {
        ret = SOCKET_TIMEOUT;
    }

    return ret;
}


/***********************************************************************
Function: gen_http_header
Description:  Generate http header for http package

Return: static Result,
Input: IN HttpMethod httpMethod,
       IN AuthStatus authStatus,
       IN DigestAuth * digestAuthPtr,
       IN PAL_INT32_T cookieFlag,
       IN char * httpPath,
       IN char * server,
       IN int serverPort,
       IN PAL_INT32_T contentLength,
       IN char *username,
       IN char *password,
       OUT char *sendbuf,  point to the http header content
       IN HeadType *headType, which type of packet, soap,download request,upload request.
Output:
Others:
************************************************************************/
Result gen_http_header ( IN HttpMethod httpMethod, IN AuthStatus authStatus, IN DigestAuth * digestAuthPtr, IN PAL_INT32_T cookieFlag, IN char * httpPath, IN char * server, IN  int serverPort, IN PAL_INT32_T contentLength, IN char *username, IN char *password, OUT char *sendbuf, IN HeadType headType )
{
    Result ret = TCMAGENT_OK;
    char tmp[32];

    //Add POST value to http head
    sprintf ( sendbuf, "%s %s %s\r\n", gMethodList[httpMethod], httpPath, HTTP_VERSION );

    //Add DOWNLOAD ACCEPT ETC INFORMATION
    if ( headType == DOWNLOAD_HEAD )
    {
        strcat ( sendbuf, "Accept:" );
        strcat ( sendbuf, "*/*" );
        strcat ( sendbuf, "\r\n" );
    }

    //Add User_agent value to http head
    strcat ( sendbuf, "User-Agent:" );
    strcat ( sendbuf, USER_AGENT );
    strcat ( sendbuf, "\r\n" );

    //Add HOST value to http head
    strcat ( sendbuf, "Host:" );
    strcat ( sendbuf, server );
    sprintf ( tmp, ":%d", serverPort );
    strcat ( sendbuf, tmp );
    strcat ( sendbuf, "\r\n" );

    //Add Content type value to http head
    if ( headType != DOWNLOAD_HEAD )
    {
        strcat ( sendbuf, "Content-Type:" );
        if ( headType == SOAP_HEAD )
            strcat ( sendbuf, "text/xml; charset=utf-8" );
        else  /*upload */
        {
            strcat ( sendbuf, "multipart/form-data;boundary=" );
            strcat ( sendbuf, BOUNDARY );
        }
        strcat ( sendbuf, "\r\n" );

        //Add Content length value to http head
        strcat ( sendbuf, "Content-Length:" );
        sprintf ( tmp, "%d", contentLength );
        strcat ( sendbuf, tmp );
        strcat ( sendbuf, "\r\n" );
    }

    //Add Connection value to http head
    strcat ( sendbuf, "Connection:" );
    strcat ( sendbuf, CONNECTION );
    strcat ( sendbuf, "\r\n" );

    //Add Cookie
    if ( headType == SOAP_HEAD && cookieFlag == 1 )
    {
        gen_cookie_info ( sendbuf + strlen ( sendbuf ) );
        strcat ( sendbuf, "\r\n" );
    }

    //Add auth head
    if ( authStatus != NO_AUTH )
    {
        if ( gen_auth_info ( sendbuf + strlen ( sendbuf ), authStatus, digestAuthPtr, username, password ) != TCMAGENT_OK )
        {
            return HTTP_AUTH_ERROR;
        }
        strcat ( sendbuf, "\r\n" );
    }
    else if ( headType != SOAP_HEAD && username != NULL && strlen ( username ) > 0 && password != NULL && strlen ( password ) > 0 )
    {
        //upload and download default use basic authentication method
        if ( gen_auth_info ( sendbuf + strlen ( sendbuf ), BASIC_AUTH, digestAuthPtr, username, password ) != TCMAGENT_OK )
        {
            return HTTP_AUTH_ERROR;
        }
        strcat ( sendbuf, "\r\n" );
    }

    strcat ( sendbuf, "\r\n" );


    return ret;
}

/***********************************************************************
Function: gen_auth_info
Description:  Generate basic or digest authnication information

Return: static Result,
Input: OUT char *auth,
       IN AuthStatus authStatus,
       IN DigestAuth * digestAuthPtr,
       IN char *username,
       IN char *password,
Output:
Others:

************************************************************************/
Result gen_auth_info ( OUT char *auth, IN AuthStatus authStatus, IN DigestAuth * digestAuthPtr, IN char *username, IN char *password )
{
    Result ret = TCMAGENT_OK;
    char nc_value[NC_VALUE_LEN + 1];

    if ( authStatus == BASIC_AUTH )    //check auth type
    {
        if ( get_basic_auth_info ( auth, username, password ) != 0 )
        {
            return HTTP_AUTH_ERROR;
        }
    }
    else
    {
        memset ( digestAuthPtr->nonceCount, '0', sizeof ( digestAuthPtr->nonceCount ) );

        strcpy ( digestAuthPtr->username, username );
        strcpy ( digestAuthPtr->password, password );

        gNonceCount++;
        sprintf ( nc_value, "%x", gNonceCount );
        strcpy ( ( digestAuthPtr->nonceCount + ( NC_VALUE_LEN - strlen ( nc_value ) ) ), nc_value );
        //calculate the auth information
        if ( get_digest_auth_info ( digestAuthPtr, auth ) != TCMAGENT_OK )
        {
            return HTTP_AUTH_ERROR;
        }
    }

    return ret;
}

/***********************************************************************
Function: get_basic_auth_info
Description:   Get the string of basic authorization header field which will be put in HTTP head.

Return: Result,
Input: char *auth_head, the string of basic authorization header field which will be put in HTTP head
       char *username,
       char *password,
Output:
Others:
************************************************************************/
Result get_basic_auth_info ( OUT char *auth_head, IN char *username, IN  char *password )
{
    char input[128];
    char output[128];
    int length;

    sprintf ( input, "%s:%s", username, password );

    /*Encode "username:password" using base64*/
    length = base64_encrypt ( ( unsigned char * ) output, ( const unsigned char * ) input, strlen ( input ) );

    sprintf ( auth_head, "Authorization: Basic %s", output );

    return TCMAGENT_OK;
}


/***********************************************************************
Function: get_digest_auth_info
Description:  Get the string of digest authorization header field which will be put in HTTP head.

Return: Result,
Input: DigestAuth *digestAuthPtr, point to a DigestAuth structure which stores the information need to generate the digest response value.
       char *auth_head, the string of digest authorization header field which will be put in HTTP head.
Output:
Others:

************************************************************************/
Result get_digest_auth_info ( DigestAuth *digestAuthPtr, char *auth_head )
{
    Result ret = TCMAGENT_OK;
    HashHex HA1;
    HashHex HA2 = "";
    HashHex Response;

    /*calculate the digest response*/
    digest_calc_ha1 ( digestAuthPtr->alg, digestAuthPtr->username, digestAuthPtr->realm, digestAuthPtr->password, digestAuthPtr->nonce, digestAuthPtr->cNonce, HA1 );
    digest_calc_response ( HA1, digestAuthPtr->nonce, digestAuthPtr->nonceCount, digestAuthPtr->cNonce, digestAuthPtr->qop, digestAuthPtr->method, digestAuthPtr->uri, HA2, Response );

    sprintf ( auth_head, "Authorization: Digest username=\"%s\", realm=\"%s\", qop=\"%s\", algorithm=\"%s\", uri=\"%s\", nonce=\"%s\", nc=%s, cnonce=\"%s\", response=\"%s\"", digestAuthPtr->username, digestAuthPtr->realm, digestAuthPtr->qop, digestAuthPtr->alg, digestAuthPtr->uri, digestAuthPtr->nonce, digestAuthPtr->nonceCount, digestAuthPtr->cNonce, Response );

    return ret;
}

/***********************************************************************
Function: parse_http_header
Description:  parse http head get the head information

Return: HttpStatus,
Input: IN char *recvBuf, point to the string of recv buffer
       IN char *path,
       OUT AuthStatus *authStatusPtr,
       OUT DigestAuth *digestAuthPtr,
Output:
Others:
************************************************************************/
HttpStatus parse_http_header ( IN char *recvBuf, IN char *path, OUT  AuthStatus *authStatusPtr, OUT DigestAuth *digestAuthPtr, OUT int *contentLengthPtr , IN HeadType headType )
{
    char line[HTTP_HEAD_LINE_LEN], protocol[6], status[4], title[10];
    int statusCode, cl;
    HttpStatus ret = PARSER_ERROR;

    //init variable
    memset ( line, 0, sizeof ( line ) );
    memset ( protocol, 0, sizeof ( protocol ) );
    memset ( status, 0, sizeof ( status ) );
    memset ( title, 0, sizeof ( title ) );

    gChunkFlag = 0;


    //parse the first line for http response
    if ( get_line ( line, sizeof ( line ), recvBuf ) == 0 )
    {
        return ret;
    }

    //gavin add to fix http receive error packet
    if ( memcmp ( line, "HTTP", 4 )  != 0 )
        return UNKNOW_STATUS_CODE;
    //printf("\n====line is [%s]==\n",line);
    if ( sscanf ( line, "%s %d %s", protocol, &statusCode, title ) != 3 )
    {
        return ret;
    }

    //printf("\n====gavin debug parse http status code [%d]=[%s  %s]=\n",statusCode,protocol,title);
    //change the pointer
    recvBuf = strchr ( recvBuf, '\n' );
    if ( recvBuf == NULL )
    {
        return ret;
    }
    recvBuf++;

    //parse the rest of header
    switch ( statusCode )
    {
        case 200:
            ret = STATUS_OK;
            while ( get_line ( line, sizeof ( line ), recvBuf ) != 0 )
            {
                if ( strncasecmp ( line, "Content-Type:", 13 ) == 0 )
                {
                    if ( headType == SOAP_HEAD && strstr ( line, "text/xml" ) == NULL )
                    {
                        ret = CONTENT_NOT_SUPPORT;
                        printf ( "\n====format not match==\n" );
                        break;
                    }
                }
                else if ( strncasecmp ( line, "Content-Length:", 15 ) == 0 )
                {
                    cl = atoi ( line + 15 );
                    *contentLengthPtr = cl;
                    if ( cl == 0 )
                    {
                        ret = STATUS_NO_CONTENT;
                        break;
                    }
                }
                else if ( strncasecmp ( line, "Set-Cookie", 10 ) == 0 )
                {
                    gCookieFlag = 1;
                    if ( parse_cookie ( line + 12 ) != 0 )
                    {
                        ret = FORMAT_ERROR;
                        break;
                    }
                }
                else if ( ( strncasecmp ( line, "Transfer-Encoding", 17 ) == 0 ) && ( strcasestr ( line, "chunked" ) ) )
                {
                    gChunkFlag = 1;
                }


                recvBuf = strchr ( recvBuf, '\n' );
                if ( recvBuf == NULL )
                {
                    ret = PARSER_ERROR;
                    break;
                }
                recvBuf++;
            }
            break;
        case 204:
            ret = STATUS_NO_CONTENT;
            break;
        case 401:
		case 400:
            while ( get_line ( line, sizeof ( line ), recvBuf ) != 0 )
            {

                if ( strncasecmp ( line, "Content-Length:", 15 ) == 0 )
                {
                    cl = atoi ( line + 15 );
                    *contentLengthPtr = cl;
                    if ( cl == 0 )
                    {
                        ret = STATUS_NO_CONTENT;
                        break;
                    }
                }

                if ( strncasecmp ( line, "WWW-Authenticate:", 17 ) == 0 )
                {
                    memset ( digestAuthPtr, 0, sizeof ( DigestAuth ) );
                    ret = parse_auth_info ( line + 17, path, authStatusPtr, digestAuthPtr );
                    break;
                }
                recvBuf = strchr ( recvBuf, '\n' );
                if ( recvBuf == NULL )
                {
                    ret = PARSER_ERROR;
                    break;
                }
                recvBuf++;
            }
            break;
        default:
            ret = UNKNOW_STATUS_CODE;
            break;
    }

    return ret;
}


/***********************************************************************
Function: parse_cookie
Description:  Parse http cookie "Set-Cookie2"

Return: static HttpStatus,
Input: IN char *cookieBuf,  pointer to cookie infomation
Output:
Others:

************************************************************************/
static HttpStatus parse_cookie ( IN char *cookieBuf )
{
    char *p;
    SessionCookie cookie;
    HttpStatus ret = FORMAT_ERROR;
    char version[4];
    int i;

    //init valiable
    memset ( &cookie, 0, sizeof ( SessionCookie ) );

    while ( *cookieBuf == ' ' )
    {
        cookieBuf++;
    }

    p = strchr ( cookieBuf, '=' );
    if ( p == NULL )
    {
        return ret;
    }
    strncpy ( cookie.name, cookieBuf, p - cookieBuf );
    cookieBuf = ++p;
    p = strchr ( cookieBuf, ';' );
    if ( p == NULL )
    {
        return ret;
    }
    else
    {
        strncpy ( cookie.value, cookieBuf, p - cookieBuf );
        cookieBuf = p;
    }

    //get cookie av path / domain / port /discard / Max-Age / version /secure
    while ( *cookieBuf != '\0' )
    {
        cookieBuf++;
        if ( *cookieBuf == ' ' )
        {
            cookieBuf++;
            continue;
        }
        p = strchr ( cookieBuf, ';' );
        if ( p == NULL )
        {
            p = strchr ( cookieBuf, '\0' );
        }
        if ( memcmp ( cookieBuf, "Version=", 8 ) == 0 )
        {
            cookieBuf += 8;
            strncpy ( version, cookieBuf, p - cookieBuf );
            gCookieVersion = atoi ( version );
        }
        else if ( memcmp ( cookieBuf, "Domain=", 7 ) == 0 )
        {
            cookieBuf += 7;
            strncpy ( cookie.domain, cookieBuf, p - cookieBuf );
        }
        else if ( memcmp ( cookieBuf, "Path=", 5 ) == 0 )
        {
            cookieBuf += 5;
            strncpy ( cookie.path, cookieBuf, p - cookieBuf );
        }
        else if ( memcmp ( cookieBuf, "Port=", 5 ) == 0 )
        {
            cookieBuf += 5;
            strncpy ( cookie.port, cookieBuf, p - cookieBuf );
        }
        else if ( memcmp ( cookieBuf, "Discard", 7 ) == 0 )
        {
            cookieDiscardFlag = 1;
        }
        cookieBuf = p;

    }

    //compare the name domain path , store the info to cache
    for ( i = 0; i < MAX_COOKIE_NUM; i++ )
    {
        if ( !strcmp ( sessionCookie[i].name, cookie.name )
                && !strcmp ( sessionCookie[i].domain, cookie.domain )
                && !strcasecmp ( sessionCookie[i].path, cookie.path ) )
        {
            //then new cookie supersedes the old
            break;
        }
        else if ( !strcmp ( sessionCookie[i].name, "" ) )
        {
            break;
        }
    }
    //then store the information
    if ( i < MAX_COOKIE_NUM )
    {
        strcpy ( sessionCookie[i].name, cookie.name );
        strcpy ( sessionCookie[i].value, cookie.value );
        strcpy ( sessionCookie[i].domain, cookie.domain );
        strcpy ( sessionCookie[i].path, cookie.path );
        strcpy ( sessionCookie[i].port, cookie.port );
        ret = STATUS_OK;
    }
    else
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_HTTP, __func__, "session cookie is overflow\n" );
    }

    return ret;
}

/***********************************************************************
Function: gen_cookie_info
Description:  generate session cookie header for http request

Return: static void,
Input: char *sessionCookieBuf, used to store cookie information
Output:
Others:

************************************************************************/
static void gen_cookie_info ( char *sessionCookieBuf )
{
    int i;

    //generate the cookie information
    sprintf ( sessionCookieBuf, "Cookie: $Version=\"%u\"", gCookieVersion );

    for ( i = 0; i < MAX_COOKIE_NUM; i++ )
    {
        if ( !strcmp ( sessionCookie[i].name, "" ) )
        {
            break;
        }
        //add cookie name
        strcat ( sessionCookieBuf, ";" );
        strcat ( sessionCookieBuf, sessionCookie[i].name );
        strcat ( sessionCookieBuf, "=" );
        strcat ( sessionCookieBuf, sessionCookie[i].value );

        //add cookie path
        if ( strcmp ( sessionCookie[i].path, "" ) )
        {
            strcat ( sessionCookieBuf, ";" );
            strcat ( sessionCookieBuf, "$Path=" );
            strcat ( sessionCookieBuf, sessionCookie[i].path );
        }

        //add cookie domain
        if ( strcmp ( sessionCookie[i].domain, "" ) )
        {
            strcat ( sessionCookieBuf, ";" );
            strcat ( sessionCookieBuf, "$Domain=" );
            strcat ( sessionCookieBuf, sessionCookie[i].domain );
        }
        //add cookie port
        if ( strcmp ( sessionCookie[i].port, "" ) )
        {
            strcat ( sessionCookieBuf, ";" );
            strcat ( sessionCookieBuf, "$Port=" );
            strcat ( sessionCookieBuf, sessionCookie[i].port );
        }
    }

    return;
}


/***********************************************************************
Function: get_line
Description:   get a line from a string stream

Return: int, the length of string s
Input: OUT char *s, used to store string line
       IN int size,  the max length of sting which will be copy
       IN char *stream, the source string
Output:
Others:
************************************************************************/
int get_line ( OUT char *s, IN int size, IN char *stream )
{
    int i = 0;
    char *p;

    p = s;
    for ( ;; )
    {
        if ( size == 0 )
        {
            *p = '\0';
            break;
        }
        size--;
        if ( stream[i] == '\r' || stream[i] == '\n' )
        {
            *p = '\0';
            break;
        }
        * ( p++ ) = stream[i];
        i++;
    }

    return strlen ( s );
}

/***********************************************************************
Function: parse_auth_info
Description:  get authcation information from http response

Return: static HttpStatus,
Input: IN char *auth_buf,
       IN char *path,
       OUT AuthStatus *authStatusPtr,
       OUT DigestAuth *digestAuthPtr,
Output:
Others:
************************************************************************/
static HttpStatus parse_auth_info ( IN char *auth_buf, IN char *path, OUT AuthStatus *authStatusPtr, OUT DigestAuth *digestAuthPtr )
{
    char *p;
    HttpStatus ret = PARSER_ERROR;

    //delete space
    while ( *auth_buf == ' ' )
    {
        auth_buf++;
    }

    //get auth type
    if ( memcmp ( auth_buf, "Digest", 6 ) == 0 )
    {
        *authStatusPtr = DIGEST_AUTH;
        auth_buf += 6;
    }
    else if ( memcmp ( auth_buf, "Basic", 5 ) == 0 )
    {
        *authStatusPtr = BASIC_AUTH;
        ret = STATUS_NEED_AUTH;
        return ret;
    }
    else
    {
        return ret;
    }

    //get auth info from http response(realm/qop/nonce/algorithm)
    while ( *auth_buf != '\0' )
    {

        //delete space
        if ( *auth_buf == ' ' )
        {
            auth_buf++;
            continue;
        }

        p = strchr ( auth_buf, ',' );
        if ( p == NULL )
        {
            p = strchr ( auth_buf, '\0' );
        }
        if ( memcmp ( auth_buf, "realm=\"", 7 ) == 0 )
        {
            auth_buf += 7;
            strncpy ( digestAuthPtr->realm, auth_buf, p - 1 - auth_buf );
        }
        else if ( memcmp ( auth_buf, "qop=\"", 5 ) == 0 )
        {
            auth_buf += 5;
            strncpy ( digestAuthPtr->qop, auth_buf, p - 1 - auth_buf );
        }
        else if ( memcmp ( auth_buf, "nonce=\"", 7 ) == 0 )
        {
            auth_buf += 7;
            strncpy ( digestAuthPtr->nonce, auth_buf, p - 1 - auth_buf );
        }
        else if ( memcmp ( auth_buf, "algorithm=\"", 11 ) == 0 )
        {
            auth_buf += 11;
            strncpy ( digestAuthPtr->alg, auth_buf, p - 1 - auth_buf );
        }

        if ( *p == '\0' )
        {
            break;
        }
        auth_buf = ++p;

    }
    //get cNonce value
    strcpy ( digestAuthPtr->cNonce, "d1234512345123" );

    //get nonceCount value
    strcpy ( digestAuthPtr->nonceCount, "00000001" );

    //get method value
    strcpy ( digestAuthPtr->method, "POST" );

    //get uri value
    strcpy ( digestAuthPtr->uri, path );

    ret = STATUS_NEED_AUTH;

    return ret;
}

