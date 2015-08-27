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
Filename: http_client.h
Author: Gavin
Date: 2009-05-26
Description:
***********************************************************************/

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "pal.h"
#include "tr69_agent_public.h"

#ifdef USE_SSL
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/ssl23.h>
#include <openssl/ssl2.h>

#define SSL_DEPTH 1
#endif

#define HTTP_DOWNLOAD_SEND_LENGTH 10000
#define HTTP_DOWNLOAD_RECV_LENGTH 32000
#define HTTP_UPLOAD_SEND_LENGTH 200000
#define HTTP_UPLOAD_RECV_LENGTH 10000

#define HTTP_SOAP_MAX_SEND_LENGTH 400000  //for not crash when sending big packets
#define HTTP_SOAP_MAX_RECV_LENGTH 32000
#define IP_LENGTH 16
#define HOST_LENGTH 128
#define HTTP_PATH_LENGTH 64
#define FILE_PATH_LENGTH 256
#define FILE_NAME_LEGNTH 64
#define TIME_OUT 60    /* socket connect timeout */
#define HTTP_MAX_TRY_TIMES 3
#define DIGEST_BIG_LENGTH 256
#define DIGEST_MIDDLE_LENGTH 32
#define DIGEST_SMALL_LENGTH 8
/*
 * Define http header value
 */
#define HTTP_VERSION    "HTTP/1.1"
#define USER_AGENT      "TcmAgent/1.0"
#define CONNECTION      "Keep-Alive"

#define BOUNDARY  "-----------------------------7dd923456ccef"
#define PACKET_HEAD_LENGTH 256
#define PACKET_TAIL_LENGTH 512
/*
 * Define global constants ......
 */
#define NC_VALUE_LEN       8

#define HTTP_HEAD_LINE_LEN     512
#define MAX_COOKIE_NUM 4

#define COOKIE_NAME_LEN    64
#define COOKIE_VALUE_LEN   128
#define COOKIE_DOMAIN_LEN  64
#define COOKIE_PATH_LEN    32
#define COOKIE_PORT_LEN    32


typedef enum
{
    SOAP_HEAD,
    DOWNLOAD_HEAD,
    UPLOAD_HEAD
}HeadType;

typedef enum
{
    tGetMethod = 0,
    tPostMethod,
    tMaxMethod
}HttpMethod;

typedef enum
{
    NO_AUTH = 0,
    BASIC_AUTH,
    DIGEST_AUTH
}AuthStatus;


typedef struct
{
    char name[COOKIE_NAME_LEN];
    char value[COOKIE_VALUE_LEN];
    char path[COOKIE_PATH_LEN];
    char domain[COOKIE_DOMAIN_LEN];
    char port[COOKIE_PORT_LEN];
}SessionCookie;

typedef enum
{
    STATUS_OK = 0,
    STATUS_NEED_AUTH,
    STATUS_NEED_REDIRECTD,
    STATUS_NO_CONTENT,
    CONTENT_NOT_SUPPORT,
    PROTOCOL_NOT_SUPPORT,
    UNKNOW_STATUS_CODE,
    FORMAT_ERROR,
    PARSER_ERROR
}HttpStatus;

typedef struct
{
    char name[HOST_LENGTH];
    char ip[IP_LENGTH];
}NameIPCache;

typedef struct _DigestAuth
{
    char username[DIGEST_BIG_LENGTH+1];
    char realm[DIGEST_MIDDLE_LENGTH+1];
    char password[DIGEST_MIDDLE_LENGTH+1];
    char uri[DIGEST_MIDDLE_LENGTH+1];
    char nonce[DIGEST_MIDDLE_LENGTH+1];
    char nonceCount[DIGEST_SMALL_LENGTH+1];
    char cNonce[DIGEST_MIDDLE_LENGTH+1];
    char qop[DIGEST_MIDDLE_LENGTH+1];
    char alg[DIGEST_MIDDLE_LENGTH+1];
    char method[DIGEST_SMALL_LENGTH+1];
}DigestAuth;

/* extern variable or functions */

extern char gMethodList[][8];
extern unsigned short gNonceCount;

/***********************************************************************
Function: parse_url
Description:  parse url and get serverName, server(IP format),serverPort, path and ssl flag information

Return: Result,
Input: IN char *url,
       OUT char *serverName,
       OUT char *server,
       OUT int *serverPort,
       OUT char * path,
       OUT int *sslFlagPtr,
************************************************************************/
extern Result parse_url ( IN char *url, OUT char *serverName, OUT char *server, OUT int *serverPort, OUT char * path, OUT int *sslFlagPtr );


#ifdef USE_SSL
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
Result init_ssl_socket ( IN int *sockfdPtr, OUT SSL **sslP, OUT SSL_CTX **ctxP );
#endif

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
Result init_session_socket ( OUT int *sockFdPtr, OUT void  **sslP, OUT void **ctxP, OUT char *serverName, OUT char *serverIP, OUT int *serverPort, OUT char *path, OUT int *sslFlag, IN char * acsUrl );

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
Result  send_raw ( IN int *sockFdPtr, IN void **sslP, IN char * sendBuf, IN int sendLength, IN int sslFlag );

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
Result recv_raw ( IN int *sockFdPtr, IN void **sslP, OUT char *recvBuf, IN  int length, IN  int sslFlag, OUT int *recvLenPtr );


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
Output:
Others:
************************************************************************/
Result gen_http_header ( IN HttpMethod httpMethod, IN AuthStatus authStatus, IN DigestAuth * digestAuthPtr, IN PAL_INT32_T cookieFlag, IN char * httpPath, IN char * server, IN  int serverPort, IN PAL_INT32_T contentLength, IN char *username, IN char *password, OUT char *sendbuf, IN HeadType headType );

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
Result gen_auth_info ( OUT char *auth, IN AuthStatus authStatus, IN DigestAuth * digestAuthPtr, IN char *username, IN char *password );

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
Result get_basic_auth_info ( OUT char *auth_head, IN char *username, IN char *password );

/***********************************************************************
Function: get_digest_auth_info
Description:  Get the string of digest authorization header field which will be put in HTTP head.

Return: Result,
Input: DigestAuth *digestAuthPtr, point to a DigestAuth structure which stores the information need to generate the digest response value.
       char *auth_head, the string of digest authorization header field which will be put in HTTP head.
Output:
Others:

************************************************************************/
Result get_digest_auth_info ( DigestAuth *digestAuthPtr, char *auth_head );

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
HttpStatus parse_http_header ( IN char *recvBuf, IN char *path, OUT  AuthStatus *authStatusPtr, OUT DigestAuth *digestAuthPtr, OUT int *contentLengthPtr , IN HeadType headType );


/***********************************************************************
Function: parse_cookie
Description:  Parse http cookie "Set-Cookie2"

Return: static HttpStatus,
Input: IN char *cookieBuf,  pointer to cookie infomation
Output:
Others:

************************************************************************/
static HttpStatus parse_cookie ( IN char *cookieBuf );

/***********************************************************************
Function: gen_cookie_info
Description:  generate session cookie header for http request

Return: static void,
Input: char *sessionCookieBuf, used to store cookie information
Output:
Others:

************************************************************************/
static void gen_cookie_info ( char *sessionCookieBuf );


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
int get_line ( OUT char *s, IN int size, IN char *stream );

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
static HttpStatus parse_auth_info ( IN char *auth_buf, IN char *path, OUT AuthStatus *authStatusPtr, OUT DigestAuth *digestAuthPtr );

#endif

