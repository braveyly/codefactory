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
**********************************************************************
File name: crq_handler.c
Author: Hook Guo
Date: Jun 3rd. 2009
Description: Incomplete implementation of http server only for handling
             http digest authentication.
History:
************************************************************************/
#include "crq_handler.h"

static PAL_THREAD_T gCrqTcpSvrTaskHandle;

static char *gHttpCliIP = NULL;
static PAL_UINT16_T gHttpCliPortCurr = -1;
static char *gHttpCliIPCurr = NULL;

static PAL_INT32_T gHttpServFd;
static PAL_INT32_T gHttpCliFd;

/* for blacklist usage */
static BlackList gBlackList;
static PAL_INT32_T gConnTimes = 0;
static PAL_TIME_T gLastTimeStamp;
static PAL_TIME_T gCurrTimeStamp;

/* local function declaration */
static PAL_INT32_T crq_handle_connection_request();
static PAL_INT32_T crq_validate_http_packet ( char *, PAL_INT32_T );
static PAL_INT32_T crq_do_authentication ( char * );
static PAL_INT32_T crq_do_challenge();
static PAL_INT32_T crq_init();
static PAL_INT32_T crq_do_snd_401Unauthorized();
static PAL_INT32_T crq_do_snd_200OK();
static PAL_INT32_T crq_do_snd_503ServiceUnavailable();
static void crq_blacklist_init();
static void crq_blacklist_destroy();
static PAL_INT32_T crq_blacklist_add ( char * );
static PAL_INT32_T crq_blacklist_remove ( char * );
static PAL_INT32_T crq_blacklist_find ( char * );
static void crq_blacklist_refresh();

/*****************************************************************************
 Function Name : __trim
 Description : remove the space at the end and start of the string
 Returns : NULL
 Parameter : @str: string to be trimed
******************************************************************************/
static void __trim ( IN OUT char *str )
{
    char tmp[1024], *p, *q;
    PAL_INT32_T len;

    p = str;

    while ( *p == ' ' && *p != '\0' )
    {
        p++;
    }

    if ( ( q = strchr ( p, ' ' ) ) == NULL )
    {
        strcpy ( tmp, p );
    }
    else
    {
        len = q - p;
        memcpy ( tmp, p, len );
        tmp[len] = '\0';
    }

    len = strlen ( tmp );
    memset ( str, 0, len );
    memcpy ( str, tmp, len );
    str[len] = '\0';
}

/*****************************************************************************
 Function Name : __dequote
 Description : remove the quote of the string
 Returns : NULL
 Parameter : @str: string to be dequote
******************************************************************************/
static void __dequote ( IN OUT char *str )
{
    char *p, tmp[1024];
    PAL_INT32_T len;

    p = str;
    len = strlen ( str );

    if ( *p == '\"' && p[len-1] == '\"' )
    {
        memcpy ( tmp, p + 1, len - 2 );
        memset ( str, 0, len );
        memcpy ( str, tmp, len - 2 );
        str[len - 2] = '\0';
    }
}

/*****************************************************************************
 Function Name : crq_handle_connection_request
 Description : handle ACE connection request, which is in the form of HTTP
               digest request. The crq_lite server decide the validaty of the
               HTTP request, and return HTTP response according to both the
               content of HTTP request and the tr069 main loop status.
               NOTE: only 200 OK, 401 Unauthorized and 503 Service Unavailable
               can be replied to the remote host.
 Returns : return OK if failed, CRQ_ERR_SOCK_ERROR if error related to
           socket api occur, CRQ_ERR_SOCK_TIMEOUT if select timeout,
           CRQ_ERR_FAILED if other error exclusive of the above occured.
 Parameter : NULL
******************************************************************************/
static PAL_INT32_T crq_handle_connection_request()
{
    fd_set rset;
    struct timeval timeout;
    PAL_INT32_T retval;
    char buf[BUFSIZ];
    PAL_INT32_T nbytes;
    char line[LINELEN];
    char *p, *q;
    PAL_INT32_T hasAuthInfo =  FALSE;

    timeout.tv_sec = CRQ_ACCEPT_TIMEOUT;
    timeout.tv_usec = 0;

    FD_ZERO ( &rset );
    FD_SET ( gHttpCliFd, &rset );

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "in function crq_handler_connection_request\n" );

    while ( 1 )
    {
        nbytes = recv ( gHttpCliFd, buf, BUFSIZ, MSG_NOSIGNAL );

        if ( nbytes == -1 )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "%s\n", strerror ( errno ) );
            return CRQ_ERR_SOCK_ERROR;
        }

        if ( crq_validate_http_packet ( buf, nbytes ) == CRQ_ERR_INVALID )
        {
            pal_logger ( T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "invalid http packet\n" );
            return CRQ_ERR_FAILED;
        }

        p = buf;
        q = buf;

        while ( p != ( buf + nbytes ) )
        {
            if ( *p == '\r' && * ( p + 1 ) == '\n' )
            {
                PAL_INT32_T len;

                len = p -  q;
                memcpy ( line, q, len );
                line[len] = '\0';

                p += 2; /* skip \r */
                q = p;

                if ( strstr ( line, "Authorization" ) != NULL )
                {
                    hasAuthInfo = TRUE;
                    break;
                }
                else
                {
                    continue;
                }
            }

            p++;
        }

        if ( hasAuthInfo == TRUE )
        {
            if ( crq_do_authentication ( line ) == TCMAGENT_OK )
            {
                /* authentication succeed */
                pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                             , "client: %s, authentication succeed\n"
                             , gHttpCliIPCurr );

                if ( check_tr069_main_loop_status() == TCMAGENT_OK )
                {
                    InformArg *informArg = NULL;

                    retval = crq_do_snd_200OK();
                    informArg = ( InformArg * ) PAL_MALLOC ( sizeof ( informArg ) );

                    if ( informArg == NULL )
                    {
                        pal_logger ( T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                                     , "%s\n", strerror ( errno ) );
                        return CRQ_ERR_FAILED;
                    }

                    informArg->event = EVENT_CONNECTION_REQUEST;
                    add_event ( NOW, INFORM, INFORM_EVENT_NAME, informArg );
                    return retval;
                }
                else
                {
                    return crq_do_snd_503ServiceUnavailable();
                }
            }
            else
            {
                pal_logger ( T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                             , "authentication failed\n" );
                if ( crq_do_snd_401Unauthorized() != TCMAGENT_OK )
                {
                    return CRQ_ERR_FAILED;
                }
            }
        }
        else
        {
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "do digest authentication\n" );
            if ( crq_do_challenge() != TCMAGENT_OK )
            {
                return CRQ_ERR_FAILED;
            }
        }
    } /* end of while (1) */

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "=====end of while(1)\n" );
}

/*****************************************************************************
 Function Name : crq_validate_http_packet
 Description : validate HTTP packet, only HTTP request with 'GET' method
               can be accepted.
 Returns : return OK if succeed, or CRQ_ERR_INVALID if failed
 Parameter : @buf: HTTP request content
             @bufsiz: HTTP request content length
******************************************************************************/
static PAL_INT32_T crq_validate_http_packet ( IN char *buf
        , IN PAL_INT32_T bufsiz )
{
    /* HTTP GET should look like:
       GET http://www.tecomtech.com HTTP/1.1 */

    char *p, *q;
    PAL_INT32_T len;
    char msgType[CRQ_HTTP_PARAM_LEN];
    char method[CRQ_HTTP_PARAM_LEN];
    char url[CRQ_HTTP_PARAM_LEN];
    char version[CRQ_HTTP_PARAM_LEN];

    pal_logger ( T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "in function crq_validate_http_packet\n" );
    if ( ( p = strchr ( buf, '\r' ) ) == NULL )
    {
        goto failed;
    }

    len = p - buf;
    memcpy ( msgType, buf, len );
    msgType[len] = '\0';

    memcpy ( method, msgType, 3 );
    method[3] = '\0';

    if ( ( p = strchr ( msgType, ' ' ) ) == NULL )
    {
        goto failed;
    }

    p = p + 1;

    if ( ( q = strchr ( p, ' ' ) ) == NULL )
    {
        goto failed;
    }

    len = q - p;
    memcpy ( url, p, len );
    url[len] = '\0';

    strcpy ( version, q + 1 );

    /* FIXME: url should also be invalidated */
    if ( strcasecmp ( method, "GET" ) == 0 && strcasecmp ( version, "HTTP/1.1" ) == 0 )
    {
        return TCMAGENT_OK;
    }

failed:
    return CRQ_ERR_INVALID;
}

/*****************************************************************************
 Function Name : crq_do_authentication
 Description : authenticate the comming HTTP digest authentication
 Returns : return OK if authentication succeed, return CRQ_ERR_INVALID
           if authentication information provided by HTTP request is invalid,
           or return CRQ_ERR_FAILED if other errors.
 Parameter : @authinfo: string contains the information for the authentication
******************************************************************************/
static PAL_INT32_T crq_do_authentication ( IN char *authinfo )
{
    PAL_INT32_T i, arraylen;
    PAL_INT32_T len;
    char *p, *q;
    char username[CRQ_AUTH_PARAM_LEN];
    char realm[CRQ_AUTH_PARAM_LEN];
    char nonce[CRQ_AUTH_PARAM_LEN];
    char uri[CRQ_AUTH_PARAM_LEN];
    char qop[CRQ_AUTH_PARAM_LEN];
    char nc[CRQ_AUTH_PARAM_LEN];
    char cnonce[CRQ_AUTH_PARAM_LEN];
    char opaque[CRQ_AUTH_PARAM_LEN];
    char response[CRQ_AUTH_PARAM_LEN];
    char *paramNameArray[] = {"username", "realm", "nonce", "uri"
                              , "qop", "nc", "cnonce", "opaque", "response"
                             };
    char *paramValueArray[] = {username, realm, nonce, uri, qop
                               , nc, cnonce, opaque, response
                              };
    HashHex ha1;
    HashHex ha2 = "";
    HashHex calcuResponse;

    arraylen = sizeof ( paramNameArray ) / sizeof ( paramNameArray[0] );

    for ( i = 0; i < arraylen; i++ )
    {
        /* 'nc', 'nonce' and 'cnonce' share the same string 'nc' */
        if ( strcmp ( "nc", paramNameArray[i] ) == 0 )
        {
            p = authinfo;
again:
            if ( ( p = strstr ( p, "nc" ) ) == NULL )
            {
                continue;
            }

            if ( * ( p + 2 ) == '=' || * ( p + 2 ) == ' ' )
            {
                /* 'nc' found */
                goto found;
            }
            else
            {
                p++;
                goto again;
            }
        }

        if ( ( p = strstr ( authinfo, paramNameArray[i] ) ) != NULL )
        {
found:
            if ( ( p = strchr ( p, '=' ) ) == NULL )
            {
                return CRQ_ERR_INVALID;
            }

            if ( ( q = strchr ( p, ',' ) ) == NULL )
            {
                /* last param to parse */
                strcpy ( paramValueArray[i], p + 1 );
                __trim ( paramValueArray[i] );
                __dequote ( paramValueArray[i] );
            }
            else
            {
                len = q - p - 1;
                memcpy ( paramValueArray[i], p + 1, len );
                paramValueArray[i][len] = '\0';
                __trim ( paramValueArray[i] );
                __dequote ( paramValueArray[i] );
            }
        }
    }

    if ( strcmp ( gManagementServer.connectionRequestUsername, username ) != 0 )
    {
        /* invalid username */
        return CRQ_ERR_FAILED;
    }

    digest_calc_ha1 ( "md5", username, realm
                      , gManagementServer.connectionRequestPassword
                      , nonce, cnonce, ha1 );
    digest_calc_response ( ha1, nonce, nc, cnonce, qop
                           , "GET", uri, ha2, calcuResponse );

    if ( strcmp ( response, calcuResponse ) == 0 )
    {
        return TCMAGENT_OK;
    }
    else
    {
        /* authentication failed */
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "gHttpCliIP: %s, gHttpCliIPCurr: %s\n"
                     , gHttpCliIP, gHttpCliIPCurr );

        if ( strcmp ( gHttpCliIP, gHttpCliIPCurr ) == 0 )
        {
            if ( gConnTimes++ > MAX_CONN_TIME )
            {
                crq_blacklist_add ( gHttpCliIPCurr );
                gConnTimes = 0; /* reset connection time */
            }
        }
        return CRQ_ERR_FAILED;
    }
}

/*****************************************************************************
 Function Name : crq_do_challenge
 Description : send HTTP response with challenge information
 Returns : return OK if succeed, return CRQ_ERR_SOCK_ERROR if error
           related to socket occur, or return CRQ_ERR_FAILED if other error.
 Parameter : NULL
******************************************************************************/
static PAL_INT32_T crq_do_challenge()
{
    gHttpCliIP = strdup ( gHttpCliIPCurr );

    return crq_do_snd_401Unauthorized();
}

/*****************************************************************************
 Function Name : crq_init
 Description : initiate socket
 Returns : return OK if succeed, or return CRQ_ERR_SOCK_ERROR if error
           related to socket occur.
 Parameter : NULL
******************************************************************************/
static PAL_INT32_T crq_init()
{
    struct sockaddr_in httpServerAddr;
    PAL_INT32_T optval, retval;
    char httpServerIP[INET_ADDRSTRLEN];

    if ( ( gHttpServFd = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "%s\n", strerror ( errno ) );
        return CRQ_ERR_SOCK_ERROR;
    }

    optval = 1;
    retval = setsockopt ( gHttpServFd, SOL_SOCKET, SO_REUSEADDR
                          , ( void * ) & optval, sizeof ( optval ) );
    if ( retval == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "%s\n", strerror ( errno ) );
        return CRQ_ERR_SOCK_ERROR;
    }

    memset ( &httpServerAddr, 0, sizeof ( struct sockaddr ) );
    httpServerAddr.sin_family = AF_INET;
    httpServerAddr.sin_port = htons ( CPE_CRQ_PORT );
    if ( gCB.get_wan_ipaddress ( httpServerIP ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "failed to acquire wan ip address\n" );
        return CRQ_ERR_FAILED;
    }

    httpServerAddr.sin_addr.s_addr = INADDR_ANY;

    retval = bind ( gHttpServFd, ( struct sockaddr * ) & httpServerAddr
                    , sizeof ( struct sockaddr ) );
    if ( retval == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "%s\n", strerror ( errno ) );
        return CRQ_ERR_SOCK_ERROR;
    }

    retval = listen ( gHttpServFd, 10 );
    if ( retval == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "%s\n", strerror ( errno ) );
        return CRQ_ERR_SOCK_ERROR;
    }

    return TCMAGENT_OK;
}

void *crq_http_server_task ( IN void *param )
{
    PAL_INT32_T retval;
    PAL_UINT32_T sockaddrlen;
    struct sockaddr_in httpCliAddr;
    struct timeval timeout;
    fd_set rset;

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "crq http server task thread launched\n" );
    retval = crq_init ( gHttpServFd );
    if ( retval != TCMAGENT_OK )
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "crq init failed\n" );
        return NULL;
    }

    gLastTimeStamp = pal_time_current ( NULL );

    /* initiate black list */
    crq_blacklist_init();

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "crq http server mainloop start\n" );
    while ( 1 )
    {
        timeout.tv_sec = CRQ_ACCEPT_TIMEOUT;
        timeout.tv_usec = 0;

        FD_ZERO ( &rset );
        FD_SET ( gHttpServFd, &rset );

        retval = select ( gHttpServFd + 1, &rset, NULL, NULL, &timeout );
        if ( retval == -1 )
        {
            pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "%s\n", strerror ( errno ) );
            return NULL;
        }
        else if ( retval > 0 )
        {
            sockaddrlen = sizeof ( struct sockaddr );

            if ( ( gHttpCliFd = accept ( gHttpServFd
                                         , ( struct sockaddr * ) & httpCliAddr, &sockaddrlen ) ) == -1 )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                             , "%s\n", strerror ( errno ) );
                return NULL;
            }
            gHttpCliIPCurr = inet_ntoa ( httpCliAddr.sin_addr );
            gHttpCliPortCurr = ntohs ( httpCliAddr.sin_port );

            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "host: %s connected\n", gHttpCliIPCurr );

#if 0
            /* see if the new connection is black listed */
            if ( crq_blacklist_find ( gHttpCliIPCurr ) >= 0 )
            {
                pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                             , "host: %s is in the black list\n", gHttpCliIPCurr );
                close ( gHttpCliFd );
                continue;
            }
#endif
            /* handle the http request */
            crq_handle_connection_request();
            close ( gHttpCliFd );
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "end of while loop\n" );
        }

        /* refresh black list */
#if 0
        gCurrTimeStamp = pal_time_current ( NULL );
        if ( ( gCurrTimeStamp - gLastTimeStamp ) >= BLACKLIST_REFRESH_FREQUENCY )
        {
            crq_blacklist_refresh();
            gLastTimeStamp = gCurrTimeStamp;
        }
#endif
    } /* end of while (1) */

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "crq http server mainloop terminated\n" );

    close ( gHttpServFd );
}

/*****************************************************************************
 Function Name : crq_tcp_main
 Description : crq_lite server main loop start routine
 Returns : return OK if succeed, return CRQ_ERR_SOCK_ERROR if errors
           related to socket occur, or return CRQ_ERR_FAILED on other errors
 Parameter : NULL
******************************************************************************/
Result crq_http_server_start()
{
    PAL_THREAD_ATTR_T a;
    //pal_set_logger(LOG_CRQ_MODULE, T_LOG_INFO, LOG_CONSOLE, 0, NULL);

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "crq http server started\n" );

    if ( pal_thread_create ( &gCrqTcpSvrTaskHandle
                             , &crq_http_server_task, NULL, NULL ) != 0 )
    {
        return CRQ_ERR_FAILED;
    }

    return TCMAGENT_OK;
}

Result crq_http_server_stop()
{
    if ( pal_thread_cancel ( gCrqTcpSvrTaskHandle ) != 0 )
    {
        return CRQ_ERR_FAILED;
    }

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "crq http server stoped\n" );

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : crq_do_snd_200OK
 Description : send HTTP response with '200 OK' reply
 Returns : return OK if succeed, return CRQ_ERR_SOCK_ERROR if errors
           related to socket occur, or return CRQ_ERR_FAILED on other errors.
 Parameter : NULL
******************************************************************************/
PAL_INT32_T crq_do_snd_200OK()
{
    char *msgType = "HTTP/1.1 200 OK\r\n";
    char *msgContentLength = "Content-Length: 0\r\n";
    char buf[256];
    PAL_INT32_T nbytes, bufsiz;

    sprintf ( buf, "%s%s\r\n", msgType, msgContentLength );
    bufsiz = strlen ( buf );

    nbytes = send ( gHttpCliFd, buf, bufsiz, MSG_NOSIGNAL );

    if ( nbytes == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "%s\n", strerror ( errno ) );
        return CRQ_ERR_SOCK_ERROR;
    }

    return ( bufsiz == nbytes ) ? TCMAGENT_OK : CRQ_ERR_FAILED;
}

/*****************************************************************************
 Function Name : crq_do_snd_401Unauthorized
 Description : send HTTP response with '401 Unauthorized' reply
 Returns : return OK if succeed, return CRQ_ERR_SOCK_ERROR if errors
           related to socket occur, or return CRQ_ERR_FAILED on other errors.
 Parameter : NULL
******************************************************************************/
PAL_INT32_T crq_do_snd_401Unauthorized()
{
    char *type = "HTTP/1.1 401 Unauthorized\r\n";
    char *contentLength = "Content-Length: 0\r\n";
    char auth[128];
    char *server = "Server: tcmAgent/alpha\r\n";
    char *contentType = "Content-Type: text/xml; charset=utf-8\r\n";
    char *connection = "Connection: keep-alive\r\n";
    PAL_INT32_T nbytes, bufsiz;
    char buf[512];
    PAL_TIME_T timestamp;

    pal_logger ( T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "in function crq_do_snd_401Unauthorized\n" );

    timestamp = pal_time_current ( NULL );

    sprintf ( auth, "WWW-Authenticate: Digest realm=\"cpeAuthentication\", "
              "domain=\"/\", nonce=\"%d\", algorithm=MD5, "
              "qop=\"auth\"\r\n", ( PAL_INT32_T ) timestamp );

    sprintf ( buf, "%s%s%s%s%s%s\r\n", type, auth, server, contentType, contentLength, connection );

    bufsiz = strlen ( buf );

    nbytes = send ( gHttpCliFd, buf, bufsiz, MSG_NOSIGNAL );

    if ( nbytes == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "%s\n", strerror ( errno ) );
        return CRQ_ERR_SOCK_ERROR;
    }

    pal_logger ( T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                 , "end of function crq_do_snd_401Unauthorized\n" );

    return ( nbytes == bufsiz ) ? TCMAGENT_OK : CRQ_ERR_FAILED;
}

/*****************************************************************************
 Function Name : crq_do_snd_503ServiceUnavailable
 Description : send HTTP response with '503 Service Unavailable' reply
 Returns : return OK if succeed, return CRQ_ERR_SOCK_ERROR if errors
           related to socket occur, or return CRQ_ERR_FAILED on other errors.
 Parameter : NULL
******************************************************************************/
PAL_INT32_T crq_do_snd_503ServiceUnavailable()
{
    char *msgType = "HTTP/1.1 503 Service Unavailable\r\n";
    char *msgServer = "Server: crq_lite\r\n";
    char msgContentLength[128];
    char *msgContentType = "Content-Type: text/html; charset=iso-8859-1\r\n";
    char *payload = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
                    "<html><head><title>503 Service Unavailable</title></head>"
                    "<body>503 Service Unavailable</body></html>";
    PAL_INT32_T len, nbytes, bufsiz;
    char buf[BUFSIZ];

    len = strlen ( payload );
    sprintf ( msgContentLength, "Content-Length: %d\r\n", len );

    sprintf ( buf, "%s%s%s%s\r\n%s", msgType, msgServer, msgContentType
              , msgContentLength, payload );
    bufsiz = strlen ( buf );

    nbytes = send ( gHttpCliFd, buf, bufsiz, MSG_NOSIGNAL );

    if ( nbytes == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                     , "%s\n", strerror ( errno ) );
        return CRQ_ERR_SOCK_ERROR;
    }

    return ( nbytes == bufsiz ) ? TCMAGENT_OK : CRQ_ERR_FAILED;
}

/*****************************************************************************
 Function Name : crq_blacklist_init
 Description : initiate crq_lite server black list
 Returns : NULL
 Parameter : NULL
******************************************************************************/
static void crq_blacklist_init()
{
    PAL_INT32_T i;

    /* initiate blacklist */
    gBlackList.size = 0;
    for ( i = 0; i < BLACKLIST_CAPACITY; i++ )
    {
        gBlackList.banned[i].status = BLACKLIST_STATUS_EMPTY;
    }
}

/*****************************************************************************
 Function Name : crq_blacklist_destroy
 Description : destroy crq_lite server black list
 Returns : NULL
 Parameter : NULL
******************************************************************************/
static void crq_blacklist_destroy()
{
    return;
}

/*****************************************************************************
 Function Name : crq_blacklist_add
 Description : add a ip address to crq_lite server black list
 Returns : return OK if succeed, or return CRQ_ERR_FAILED on failure
 Parameter : @ipaddr: ip address to be added
******************************************************************************/
static PAL_INT32_T crq_blacklist_add ( IN char *ipaddr )
{
    PAL_INT32_T i;
    PAL_TIME_T timestamp;

    if ( gBlackList.size == BLACKLIST_CAPACITY )
    {
        return CRQ_ERR_FAILED;
    }

    for ( i = 0; i < BLACKLIST_CAPACITY; i++ )
    {
        if ( gBlackList.banned[i].status == BLACKLIST_STATUS_EMPTY )
        {
            strcpy ( gBlackList.banned[i].bannedIP, ipaddr );
            gBlackList.banned[i].status = BLACKLIST_STATUS_OCUPIED;
            timestamp = pal_time_current ( NULL );
            gBlackList.banned[i].ts = timestamp;
            gBlackList.size++;

            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "host %s has been black listed\n", ipaddr );

            return TCMAGENT_OK;
        }
    }

    return CRQ_ERR_FAILED;
}

/*****************************************************************************
 Function Name : crq_blacklist_remove
 Description : remove a ip address from crq_lite server black list
 Returns : return OK on succeed, or return CRQ_ERR_FAILED on failure
 Parameter : @ipaddr: ip address to be removed
******************************************************************************/
static PAL_INT32_T crq_blacklist_remove ( IN char *ipaddr )
{
    PAL_INT32_T idx;

    if ( ( idx = crq_blacklist_find ( ipaddr ) ) == CRQ_ERR_NOTFOUND )
    {
        return CRQ_ERR_FAILED;
    }

    gBlackList.banned[idx].status = BLACKLIST_STATUS_EMPTY;
    gBlackList.size--;

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : crq_blacklist_find
 Description : find if the give ip address is in crq_lite black list
 Returns : return index of the ip address in the crq_lite black list if
           succeed, or return CRQ_ERR_NOTFOUND on failed to find ipaddr from
           crq_lite black list.
 Parameter : @ipaddr: ip address to be searching for
******************************************************************************/
static PAL_INT32_T crq_blacklist_find ( IN char *ipaddr )
{
    PAL_INT32_T i;

    for ( i = 0; i < BLACKLIST_CAPACITY; i++ )
    {
        if ( gBlackList.banned[i].status == BLACKLIST_STATUS_OCUPIED
                && strcmp ( gBlackList.banned[i].bannedIP, ipaddr ) == 0 )
        {
            return i;
        }
    }

    return CRQ_ERR_NOTFOUND;
}

/*****************************************************************************
 Function Name : crq_blacklist_refresh
 Description : refresh crq_lite server to find if there are expired banned
               ip address.
 Returns : NULL
 Parameter : NULL
******************************************************************************/
static void crq_blacklist_refresh()
{
    PAL_INT32_T i;
    PAL_TIME_T currTS;

    currTS = pal_time_current ( NULL );

    for ( i = 0; i < BLACKLIST_CAPACITY; i++ )
    {
        if ( ( currTS - gBlackList.banned[i].ts ) > BAN_COST_TIME
                && gBlackList.banned[i].status == BLACKLIST_STATUS_OCUPIED )
        {
            /* remove the expired ipaddr from black list */
            gBlackList.banned[i].status = BLACKLIST_STATUS_EMPTY;
            gBlackList.size--;

            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                         , "host %s has been removed from black list\n"
                         , gBlackList.banned[i].bannedIP );
        }
    }
}
