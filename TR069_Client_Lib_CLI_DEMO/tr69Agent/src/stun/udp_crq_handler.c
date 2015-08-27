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
#include "stun.h"


/* local function declaration */
PAL_INT32_T udp_crq_handle_connection_request(char *buf, int buf_len);
static PAL_INT32_T udp_crq_validate_http_packet(char *, PAL_INT32_T);
static PAL_INT32_T udp_crq_do_authentication(char *);
static PAL_INT32_T udp_crq_do_challenge();
static PAL_INT32_T udp_crq_do_snd_401Unauthorized();
static PAL_INT32_T udp_crq_do_snd_200OK();


/*****************************************************************************
 Function Name : __trim
 Description : remove the space at the end and start of the string
 Returns : NULL
 Parameter : @str: string to be trimed
******************************************************************************/
static void udp___trim(IN OUT char *str)
{
    char tmp[1024], *p, *q;
    PAL_INT32_T len;

    p = str;

    while (*p == ' ' && *p != '\0')
    {
        p++;
    }

    if ((q = strchr(p, ' ')) == NULL)
    {
        strcpy(tmp, p);
    }
    else
    {
        len = q - p;
        memcpy(tmp, p, len);
        tmp[len] = '\0';
    }

    len = strlen(tmp);
    memset(str, 0, len);
    memcpy(str, tmp, len);
    str[len] = '\0';
}

/*****************************************************************************
 Function Name : __dequote
 Description : remove the quote of the string
 Returns : NULL
 Parameter : @str: string to be dequote
******************************************************************************/
static void udp___dequote(IN OUT char *str)
{
    char *p, tmp[1024];
    PAL_INT32_T len;

    p = str;
    len = strlen(str);

    if (*p == '\"' && p[len-1] == '\"')
    {
        memcpy(tmp, p + 1, len - 2);
        memset(str, 0, len);
        memcpy(str, tmp, len - 2);
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
PAL_INT32_T udp_crq_handle_connection_request(char *buf, int buf_len)
{
    char line[LINELEN];
    char *p, *q;
    PAL_INT32_T hasAuthInfo =  FALSE;


    pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
               , "in function udp_crq_handle_connection_request\n");


    if (udp_crq_validate_http_packet(buf, buf_len) == CRQ_ERR_INVALID)
    {
        pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                   , "invalid http packet\n");
        return CRQ_ERR_FAILED;
    }

    p = buf;
    q = buf;
    memset(line, 0, sizeof(line));

    while (p != (buf + buf_len))
    {
        if (*p == '\r' && *(p + 1) == '\n')
        {
            PAL_INT32_T len;

            len = p -  q;
            memcpy(line, q, len);
            line[len] = '\0';

            p += 2; /* skip \r */
            q = p;

            if (strstr(line, "Authorization") != NULL)
            {
                hasAuthInfo = TRUE;
                break;
            }
            else
            {
                memset(line, 0, sizeof(line));
                continue;
            }
        }

        p++;
    }

    if (hasAuthInfo == TRUE)
    {
        if (udp_crq_do_authentication(line) == TCMAGENT_OK)
        {
            /* authentication succeed */
            pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                       , "authentication succeed\n");

            return  udp_crq_do_snd_200OK();
        }
        else
        {
            pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                       , "authentication failed\n");
            udp_crq_do_snd_401Unauthorized();
        }
    }
    else
    {
        pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                   , "do digest authentication\n");
        udp_crq_do_challenge();
    }

    return STUN_ERR_UNAUTH;
}

/*****************************************************************************
 Function Name : crq_validate_http_packet
 Description : validate HTTP packet, only HTTP request with 'GET' method
               can be accepted.
 Returns : return OK if succeed, or CRQ_ERR_INVALID if failed
 Parameter : @buf: HTTP request content
             @bufsiz: HTTP request content length
******************************************************************************/
static PAL_INT32_T udp_crq_validate_http_packet(IN char *buf
        , IN PAL_INT32_T bufsiz)
{
    /* HTTP GET should look like:
       GET http://www.tecomtech.com HTTP/1.1 */

    char *p, *q;
    PAL_INT32_T len;
    char msgType[CRQ_HTTP_PARAM_LEN];
    char method[CRQ_HTTP_PARAM_LEN];
    char url[CRQ_HTTP_PARAM_LEN];
    char version[CRQ_HTTP_PARAM_LEN];

    pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
               , "in function crq_validate_http_packet\n");
    if ((p = strchr(buf, '\r')) == NULL)
    {
        goto failed;
    }

    len = p - buf;
    memcpy(msgType, buf, len);
    msgType[len] = '\0';

    memcpy(method, msgType, 3);
    method[3] = '\0';

    if ((p = strchr(msgType, ' ')) == NULL)
    {
        goto failed;
    }

    p = p + 1;

    if ((q = strchr(p, ' ')) == NULL)
    {
        goto failed;
    }

    len = q - p;
    memcpy(url, p, len);
    url[len] = '\0';

    strcpy(version, q + 1);

    /* FIXME: url should also be invalidated */
    if (strcasecmp(method, "GET") == 0 && strcasecmp(version, "HTTP/1.1") == 0)
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
static PAL_INT32_T udp_crq_do_authentication(IN char *authinfo)
{
    PAL_INT32_T i, arraylen;
    PAL_INT32_T len;
    char *p, *q;
    char timestamp[CRQ_AUTH_PARAM_LEN];
    char messageid[CRQ_AUTH_PARAM_LEN];
    char username[CRQ_AUTH_PARAM_LEN];
    char cnonce[CRQ_AUTH_PARAM_LEN];
    char signature[CRQ_AUTH_PARAM_LEN];
    char *paramNameArray[] = {"ts", "id", "un", "cn", "sig"};
    char *paramValueArray[] = {timestamp, messageid, username,
                               cnonce, signature
                              };
    char calcusignature[CRQ_AUTH_PARAM_LEN];
    char *ctx;
    PAL_INT32_T ctxLen;
    PAL_INT32_T sha1_result[5];

    arraylen = sizeof(paramNameArray) / sizeof(paramNameArray[0]);
    memset(timestamp, 0, sizeof(timestamp));
    memset(messageid, 0, sizeof(messageid));
    memset(username, 0, sizeof(username));
    memset(cnonce, 0, sizeof(cnonce));
    memset(signature, 0, sizeof(signature));
    memset(calcusignature, 0, sizeof(calcusignature));

    for (i = 0; i < arraylen; i++)
    {
        /* 'nc', 'nonce' and 'cnonce' share the same string 'nc' */
        if (strcmp("nc", paramNameArray[i]) == 0)
        {
            p = authinfo;
again:
            if ((p = strstr(p, "nc")) == NULL)
            {
                continue;
            }

            if (*(p + 2) == '=' || *(p + 2) == ' ')
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

        if ((p = strstr(authinfo, paramNameArray[i])) != NULL)
        {
found:
            if ((p = strchr(p, '=')) == NULL)
            {
                return CRQ_ERR_INVALID;
            }

            if ((q = strchr(p, ',')) == NULL)
            {
                /* last param to parse */
                strcpy(paramValueArray[i], p + 1);
                udp___trim(paramValueArray[i]);
                udp___dequote(paramValueArray[i]);
            }
            else
            {
                len = q - p - 1;
                memcpy(paramValueArray[i], p + 1, len);
                paramValueArray[i][len] = '\0';
                udp___trim(paramValueArray[i]);
                udp___dequote(paramValueArray[i]);
            }
        }
    }

    if (strcmp(gManagementServer.connectionRequestUsername, username) != 0)
    {
        /* invalid username */
        return CRQ_ERR_FAILED;
    }


    /* compute local HMAC-SHA1 */
    ctxLen = strlen(timestamp) + strlen(messageid) + strlen(username) + strlen(cnonce);
    ctx = (PAL_INT8_T *)PAL_MALLOC((ctxLen + 1) * sizeof(PAL_INT8_T));
    if (ctx == NULL)
    {
        return CRQ_ERR_FAILED;
    }
    ctx = strcat(ctx, timestamp);
    ctx = strcat(ctx, messageid);
    ctx = strcat(ctx, username);
    ctx = strcat(ctx, cnonce);


    hmac_sha1(gManagementServer.connectionRequestPassword, ctx, ctxLen, (char *)&sha1_result[0]);
    sprintf(calcusignature, "%x%x%x%x%x", sha1_result[0]
            , sha1_result[1]
            , sha1_result[2]
            , sha1_result[3]
            , sha1_result[4]
           );
    if (strncmp(signature, calcusignature, 40) == 0)
    {
        return TCMAGENT_OK;
    }
    else
    {
        /* authentication failed */
        pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
                   , "authentication failed\n");
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
static PAL_INT32_T udp_crq_do_challenge()
{
    return udp_crq_do_snd_401Unauthorized();
}




/*****************************************************************************
 Function Name : crq_do_snd_200OK
 Description : send HTTP response with '200 OK' reply
 Returns : return OK if succeed, return CRQ_ERR_SOCK_ERROR if errors
           related to socket occur, or return CRQ_ERR_FAILED on other errors.
 Parameter : NULL
******************************************************************************/
PAL_INT32_T udp_crq_do_snd_200OK()
{
    char *msgType = "HTTP/1.1 200 OK\r\n";
    char *msgContentLength = "Content-Length: 0\r\n";
    char buf[256];
    PAL_INT32_T ret, bufsiz;

    sprintf(buf, "%s%s\r\n", msgType, msgContentLength);
    bufsiz = strlen(buf);

    ret = stun_snd_binding_req(buf, bufsiz, BINDING_REQ_MAIN_PORT);

    return (ret == TCMAGENT_OK) ? TCMAGENT_OK : CRQ_ERR_FAILED;
}

/*****************************************************************************
 Function Name : crq_do_snd_401Unauthorized
 Description : send HTTP response with '401 Unauthorized' reply
 Returns : return OK if succeed, return CRQ_ERR_SOCK_ERROR if errors
           related to socket occur, or return CRQ_ERR_FAILED on other errors.
 Parameter : NULL
******************************************************************************/
PAL_INT32_T udp_crq_do_snd_401Unauthorized()
{
    char *type = "HTTP/1.1 401 Unauthorized\r\n";
    char *contentLength = "Content-Length: 0\r\n";
    char auth[128];
    char *server = "Server: tcmAgent/alpha\r\n";
    char *contentType = "Content-Type: text/xml; charset=utf-8\r\n";
    char *connection = "Connection: keep-alive\r\n";
    PAL_INT32_T  bufsiz, ret;
    char buf[512];
    PAL_TIME_T timestamp;

    pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_CRQ_MODULE, __func__
               , "in function crq_do_snd_401Unauthorized\n");

    timestamp = pal_time_current(NULL);

    sprintf(auth, "WWW-Authenticate: Digest realm=\"cpeAuthentication\", "
            "domain=\"/\", nonce=\"%d\", algorithm=MD5, "
            "qop=\"auth\"\r\n", (PAL_INT32_T)timestamp);

    sprintf(buf, "%s%s%s%s%s%s\r\n", type, auth, server, contentType, contentLength, connection);

    bufsiz = strlen(buf);

    ret = stun_snd_binding_req(buf, bufsiz, BINDING_REQ_MAIN_PORT);

    return (ret == TCMAGENT_OK) ? TCMAGENT_OK : CRQ_ERR_FAILED;
}


