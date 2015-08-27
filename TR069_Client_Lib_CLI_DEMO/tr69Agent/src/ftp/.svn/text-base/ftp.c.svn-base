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
File name: ftp.c
Author: Hook Guo
Date: May 27th. 2009
Description: ftp download & upload client
History:
************************************************************************/

#include "ftp.h"

static char gFtpServerStrIP[INET_ADDRSTRLEN];
static unsigned long gFtpServerPort;
static char gFilename[FILENAME_MAX];
static PAL_INT32_T gFilenameLen;
static char gPasvIPAddr[INET_ADDRSTRLEN + 1];
static PAL_INT32_T gPasvIPAddrLen;
static PAL_UINT16_T gPasvPort;
static char gPasvStrPort[8];

static PAL_INT32_T ftp_sock_wait(PAL_INT32_T, PAL_INT32_T);
static PAL_INT32_T ftp_snd_req(PAL_INT32_T, PAL_UINT8_T *, PAL_INT32_T);
static PAL_INT32_T ftp_parse_url(char *);
static PAL_INT32_T ftp_sock_auth(PAL_INT32_T, char*, char*);
static PAL_INT32_T ftp_commit_cmd(PAL_INT32_T, char *, void *arg);
static PAL_INT32_T ftp_handle_cmd_resp(PAL_INT32_T, char *, PAL_INT32_T, char
                                       , void *, PAL_INT32_T *);
static PAL_INT32_T ftp_readline(IN PAL_INT32_T sockfd
                                , IN    PAL_INT32_T  resplength
                                , OUT PAL_UINT8_T *resplinePtr);

static PAL_INT32_T __msleep(PAL_INT32_T msec)
{
    PAL_INT32_T retval;
    struct timeval tv;

    tv.tv_sec = msec / 1000;
    tv.tv_usec = msec % 1000;

    retval = select(STDOUT_FILENO + 1, NULL, NULL, NULL, &tv);
    if (retval == 0)
    {
        /* timeout */
        return FTP_ERR_TIMEOUT;
    }
    else if (retval == -1)
    {
        return INTERNAL_ERROR; /* is this error matter ? */
    }

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : ftp_commit_cmd
 Description : build and send ftp request command from ftp control channel
 Returns : FTP_ERR_FAILED if failed or count of bytes sent
 Parameter : @sockfd: the socket file descriptor of ftp control channel
             @cmd: ftp command
             @arg: ftp command arguments
******************************************************************************/
static PAL_INT32_T ftp_commit_cmd(IN PAL_INT32_T sockfd, IN char *cmd
                                  , IN void *arg)
{
    char req[FTP_REQ_BUF_LEN];

    if (strcmp(cmd, FTP_CMD_USER) == 0)
    {
        char *username = (char *)arg;
        sprintf(req, "%s %s\r\n", cmd, username);
    }
    else if (strcmp(cmd, FTP_CMD_PASS) == 0)
    {
        char *password = (char *)arg;
        sprintf(req, "%s %s\r\n", cmd, password);
    }
    else if (strcmp(cmd, FTP_CMD_PASV) == 0)
    {
        sprintf(req, "%s \r\n", cmd);
    }
    else if (strcmp(cmd, FTP_CMD_TYPE) == 0)
    {
        sprintf(req, "%s I\r\n", cmd);
    }
    else if (strcmp(cmd, FTP_CMD_RETR) == 0)
    {
        char *filename = (char *)arg;
        sprintf(req, "%s %s\r\n", cmd, filename);
    }
    else if (strcmp(cmd, FTP_CMD_CWD) == 0)
    {
        char *cwd = (char *)arg;
        sprintf(req, "%s %s\r\n", cmd, cwd);
    }
    else if (strcmp(cmd, FTP_CMD_PWD) == 0)
    {
        sprintf(req, "%s \r\n", cmd);
    }
    else if (strcmp(cmd, FTP_CMD_STOR) == 0)
    {
        char *filename = (char *)arg;
        sprintf(req, "%s %s\r\n", cmd, filename);
    }
    else if (strcmp(cmd, FTP_CMD_PORT) == 0)
    {
        char *port = (char *)arg;
        sprintf(req, "%s %s\r\n", cmd, port);
    }
    else if (strcmp(cmd, FTP_CMD_MKD) == 0)
    {
        char *cwd = (char *)arg;
        sprintf(req, "%s %s\r\n", cmd, cwd);
    }
    else if (strcmp(cmd, FTP_CMD_SIZE) == 0)
    {
        char *filename = (char *)arg;
        sprintf(req, "%s %s \r\n", cmd, filename);
    }
    else
    {
        pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "unknown command\n");
        return FTP_ERR_FAILED;
    }

    pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
               , "%s", req);

    return ftp_snd_req(sockfd, (PAL_UINT8_T *)req, strlen(req));
}

/*****************************************************************************
 Function Name : ftp_handle_cmd_resp
 Description : handle the ftp response from the ftp control channel, some
               useful information may be acquired from the response content
 Returns : return FTP_ERR_SOCK_ERROR if socket failure occur, FTP_ERR_TIMEOUT
           if ftp control channel is not readable within the given period of
           timeout, FTP_ERR_ERROR_RESP if ftp 4xx of 5xx error response
           received, FTP_ERR_RESP_UNEXPECTED if the expected response is not
           received, OK if the expected response is received
 Parameter : @sockfd: the socket descriptor of ftp control channel
             @cmd: ftp command to which the response if for
             @timeout: timeout value in microseconds
             @c: the value expected in ftp response
             @arg: useful information may be acquired from response content
             @argLenPtr: length of arg
******************************************************************************/
static PAL_INT32_T ftp_handle_cmd_resp(IN PAL_INT32_T sockfd
                                       , IN char *cmd
                                       , IN PAL_INT32_T timeout
                                       , IN char c
                                       , OUT void *arg
                                       , OUT PAL_INT32_T *argLenPtr)
{
    PAL_INT32_T retval;
    char *p, *q;
    char respline[FTP_RESP_BUF_LEN];
    PAL_INT32_T *filesizePtr;
    char fileStrSize[16];

    retval = ftp_sock_wait(sockfd, timeout);
    if (retval == FTP_ERR_SOCK_ERROR || retval == FTP_ERR_TIMEOUT)
    {
        return retval;
    }

readline:
    retval = ftp_readline(sockfd, sizeof(respline), (PAL_UINT8_T *)respline);
    if (retval == INTERNAL_ERROR || retval == FTP_ERR_FAILED)
    {
        return INTERNAL_ERROR;
    }

    pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
               , "%s\n", respline);

    if (respline[3] == '-')
    {
        /* multilines */
        goto readline;
    }
    else if (respline[0] == '4' || respline[0] == '5')
    {
        /* ftp error rely */
        return FTP_ERR_ERROR_RESP;
    }
    else if (respline[0] == c)
    {
        /* expected ftp reply arrived */
        goto fetch;
    }
    else
    {
        /* unexpected non-error ftp reply */
        goto readline;
    }

fetch:
    /* fetch the useful information from response line */
    if (strcmp(cmd, FTP_CMD_PASV) == 0)
    {
        char *tmpPtr1, *tmpPtr2;
        PAL_INT32_T count;

        tmpPtr1 = strchr(respline, '(');
        tmpPtr2 = strchr(respline, ')');
        count = 0;
        for (p = tmpPtr1; p != tmpPtr2; p++)
        {
            if (*p == ',')
            {
                count++;
                if (count == 4)
                {
                    break;
                }
            }

        }

        gPasvIPAddrLen = p - tmpPtr1 - 1;
        memcpy(gPasvIPAddr, tmpPtr1 + 1, gPasvIPAddrLen);
        gPasvIPAddr[gPasvIPAddrLen] = '\0';
        for (q = gPasvIPAddr; *q != '\0'; q++)
        {
            if (*q == ',')
            {
                *q = '.';
            }
        }

        memcpy(gPasvStrPort, p + 1, tmpPtr2 - p - 1);
        gPasvStrPort[tmpPtr2 - p - 1] = '\0';
        for (q = gPasvStrPort; *q != '\0'; q++)
        {
            char strHigh[4];
            char strLow[4];
            PAL_INT32_T len;

            if (*q == ',')
            {
                memcpy(strHigh, gPasvStrPort, q - gPasvStrPort);
                strHigh[q - gPasvStrPort] = '\0';
                len = strlen(gPasvStrPort) - strlen(strHigh) - 1;
                memcpy(strLow, q + 1, len);
                strLow[len] = '\0';
            }
            gPasvPort = atoi(strHigh) * 256 + atoi(strLow);
        }
    }
    else if (strcmp(cmd, FTP_CMD_RETR) == 0)
    {
        /* some ftp server such as FileZilla may not return file size info,
         * in that case, 'SIZE' command should be explicitly used */
        filesizePtr = (PAL_INT32_T *)arg;
        p = strchr(respline, '(');
        if (p == NULL)
        {
            /* should try 'SIZE' command */
            *filesizePtr = -1;
            return TCMAGENT_OK;
        }
        p = p + 1;
        q = strchr(p, ' ');
        memcpy(fileStrSize, p, q - p);
        fileStrSize[q - p] = '\0';
        *filesizePtr = atoi(fileStrSize);
    }
    else if (strcmp(cmd, FTP_CMD_SIZE) == 0)
    {
        filesizePtr = (PAL_INT32_T *)arg;
        p = strchr(respline, ' ');
        strcpy(fileStrSize, p + 1);
        *filesizePtr = atoi(fileStrSize);
    }

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : ftp_readline
 Description : read a line ended with CRLF in ftp control channel
 Returns : return OK is success or FTP_ERR_FAILED if failed
 Parameter : @sockfd: the socket file descriptor of ftp control channel
             @resplinePtr: the ASCII string encoded line read from
                           ftp control channel
******************************************************************************/
static PAL_INT32_T ftp_readline(IN PAL_INT32_T sockfd
                                , IN    PAL_INT32_T  resplength
                                , OUT PAL_UINT8_T *resplinePtr)
{
    PAL_UINT8_T p, *q;
    PAL_INT32_T i = 0;


    if (resplinePtr)
        memset(resplinePtr, 0, resplength);
    else
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "resplinePtr is null\n");
        return FTP_ERR_FAILED;
    }

    q = &resplinePtr[0];

    do
    {
        if (read(sockfd, &p, 1) == -1)
        {
            pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "%s\n", strerror(errno));
            return INTERNAL_ERROR;
        }

        if (++i >= resplength)
        {
            pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "extend line buf len, read len %d\n", i);
            return FTP_ERR_FAILED;
        }

        /* reach \r\n */
        if (p == '\r')
        {
            if (read(sockfd, &p, 1) == -1)
            {
                pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                           , "%S\n", strerror(errno));
                return INTERNAL_ERROR;
            }

            if (p == '\n')
            {
                PAL_INT32_T len;

                len = q - &resplinePtr[0];
                resplinePtr[len] = '\0';
                return TCMAGENT_OK;
            }
            else
            {
                /* not CLRF ended */
                return FTP_ERR_FAILED;
            }
        }

        memcpy(q, &p, 1);
        q += 1;
    }
    while (1);
}

/*****************************************************************************
 Function Name : ftp_parse_url
 Description : parse ftp server ip/port and filename
 Returns : return INTERNAL_ERROR if failed to parse the url or
           OK is success
 Parameter : @url: url to be parsed
******************************************************************************/
static PAL_INT32_T ftp_parse_url(IN char *url)
{
    char *p, *q;
    const char *urlPrefix = "ftp://";
    char serverURI[FTP_URL_PARAM_LEN];
    char serverName[FTP_URL_PARAM_LEN];
    char serverStrPort[16];
    PAL_UINT16_T serverPort;
    struct in_addr serverAddr;
    struct hostent *hePtr;

    /* FIXME: case should be considered */
    if ((p = strstr(url, urlPrefix)) != NULL)
    {
        /* has 'ftp://' prefix */
        p = url + strlen(urlPrefix);
    }
    else
    {
        p = url;
    }

    if ((q = strchr(p, '/')) != NULL)
    {
        PAL_INT32_T serverURILen;

        serverURILen = q - p;
        memcpy(serverURI, p, serverURILen);
        serverURI[serverURILen] = '\0';
        memcpy(gFilename, q, strlen(url) - serverURILen);
    }
    else
    {
        return INTERNAL_ERROR;
    }

    if ((p = strchr(serverURI, ':')) != NULL)
    {
        PAL_INT32_T serverNameLen;
        PAL_INT32_T serverStrPortLen;

        serverNameLen = p - serverURI;
        serverStrPortLen = strlen(serverURI) - serverNameLen;
        memcpy(serverName, serverURI, serverNameLen);
        memcpy(serverStrPort, p + 1, serverStrPortLen);
        serverStrPort[serverStrPortLen] = '\0';
        serverPort = atoi(serverStrPort);
    }
    else
    {
        serverPort = 21;
        memcpy(serverName, serverURI, strlen(serverURI) + 1);
    }

    if (inet_aton(serverName, &serverAddr) == 0)
    {
        hePtr = gethostbyname(serverName);
        if (hePtr != NULL)
        {
            unsigned long resolveIP;
            struct in_addr tmpaddr;

            resolveIP = *(unsigned long *)(hePtr->h_addr_list[0]);
            tmpaddr.s_addr = resolveIP;
            strcpy(gFtpServerStrIP, inet_ntoa(tmpaddr));
        }
        else
        {
            pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "%s\n", strerror(errno));
            return INTERNAL_ERROR;
        }
    }
    else
    {
        strcpy(gFtpServerStrIP, serverName);
    }

    gFtpServerPort = serverPort;
    pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
               , "FTP Server: %s:%d\n", gFtpServerStrIP, gFtpServerPort);

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : ftp_snd_req
 Description : send request to ftp server from the ftp control channel
 Returns : return FTP_ERR_SOCK_ERROR if fatal error occur of count of bytes
           that has been sent
 Parameter : @sockCtrlFd: the socket descriptor of the ftp control channel
             @reqPtr: request content
             @reqLen: request content length
******************************************************************************/
static PAL_INT32_T ftp_snd_req(IN PAL_INT32_T sockCtrlFd
                               , IN PAL_UINT8_T *reqPtr
                               , IN PAL_INT32_T reqLen)
{
    PAL_INT32_T nbytes;

    if ((nbytes = write(sockCtrlFd, reqPtr, reqLen)) == -1)
    {
        pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return FTP_ERR_SOCK_ERROR;
    }
    else
    {
        return nbytes;
    }
}

/*****************************************************************************
 Function Name : ftp_sock_wait
 Description : wait socket until it is readable or timeout
 Returns : return FTP_ERR_TIMEOUT if socket timeout, FTP_ERR_SOCK_ERROR if
           fatal error occur or OK if the sockCtrlFd is readable
 Parameter : @sockCtrlFd: the socket descriptor of the ftp control channel
             @usec: timeout in microseconds
******************************************************************************/
static PAL_INT32_T ftp_sock_wait(IN PAL_INT32_T sockCtrlFd
                                 , IN PAL_INT32_T usec)
{
    struct timeval timeout;
    PAL_INT32_T retval;
    fd_set rset;

    FD_ZERO(&rset);
    FD_SET(sockCtrlFd, &rset);

    timeout.tv_sec = usec / 1000;
    timeout.tv_usec = usec % 1000;

    retval = select(sockCtrlFd + 1, &rset, NULL, NULL, &timeout);

    if (retval == 0)
    {
        return FTP_ERR_TIMEOUT;
    }
    else if (retval == -1)
    {
        return FTP_ERR_SOCK_ERROR;
    }
    else if (retval > 0)
    {
        return TCMAGENT_OK;
    }

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : ftp_sock_auth
 Description : authenticate with the server.
 Returns : return FILE_TRANSFER_AUTHFAILURE on authentication failure or OK on
           authentication success
 Parameter : @sockfd: the socket descriptor of ftp control channel
             @username: used to authenticate with server
             @password: used to authenticate with server
******************************************************************************/
static PAL_INT32_T ftp_sock_auth(IN PAL_INT32_T sockfd, IN char *username
                                 , IN char *password)
{
    PAL_INT32_T retval;

    /* USER */
    ftp_commit_cmd(sockfd, FTP_CMD_USER, (void *)username);
    retval = ftp_handle_cmd_resp(sockfd, FTP_CMD_USER
                                 , FTP_RCV_RESP_TIMEOUT, '3', NULL, NULL);

    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* TODO: 4xx or 5xx ftp error response */
        return FILE_TRANSFER_AUTHFAILURE;
    }

    /* PASS */
    ftp_commit_cmd(sockfd, FTP_CMD_PASS, (void *)password);
    retval = ftp_handle_cmd_resp(sockfd, FTP_CMD_PASS
                                 , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);

    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* TODO: 4xx or 5xx ftp error response */
        return FILE_TRANSFER_AUTHFAILURE;
    }

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : ftp_download_file
 Description : download a file specified by url/filename from a file server,
               save file data into global buffer.
 Returns : status indicate download result, 0 indicate success , other no-zero
           value indicate fault error code.
           Fault is classified in the following case:
                9001, Request denied
                9002, Internal error (Such as malloc failed,etc.)
                9010, Download failure (network error)
                9012, File transfer server authentication failure
 Parameter : @url: file server path indicating a file location
             @username: used to authenticate with server
             @password: used to authenticate with server
             @fileContent: download file data stream, It's malloced in the
                           function, and the function caller release it
             @filesizePtr: download file data length
******************************************************************************/
PAL_INT32_T ftp_download_file(IN char *url
                              , IN char *username
                              , IN char *password
                              , OUT char **fileContent
                              , OUT PAL_UINT32_T *filesizePtr)
{

    PAL_UINT8_T *p;
    struct sockaddr_in ftpServerSockAddr;
    struct sockaddr_in ftpServerSockDataAddr;
    PAL_INT32_T sockCtrlFd;
    PAL_INT32_T sockDataFd;
    PAL_INT32_T optval;
    PAL_INT32_T retval;
    PAL_INT32_T nbytes;
    PAL_INT32_T filesize;
    PAL_UINT8_T buf[FTP_RCV_BUF_SIZE];
    char *_fileContent;
    PAL_INT32_T downCount;

    //pal_set_logger(LOG_FTP_MODULE, T_LOG_INFO, LOG_CONSOLE, 0, NULL);

    if (ftp_parse_url(url) == INTERNAL_ERROR)
    {
        return INTERNAL_ERROR;
    }

    /* initiate socket */
    if ((sockCtrlFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    /* reuse ip/port */
    optval = 1;
    retval = setsockopt(sockCtrlFd, SOL_SOCKET, SO_REUSEADDR
                        , (void *) & optval, sizeof(optval));
    if (retval == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    memset(&ftpServerSockAddr, 0, sizeof(struct sockaddr));
    ftpServerSockAddr.sin_family = AF_INET;
    ftpServerSockAddr.sin_port = htons(gFtpServerPort);
    inet_aton(gFtpServerStrIP, &ftpServerSockAddr.sin_addr);

    retval = connect(sockCtrlFd, (struct sockaddr *) & ftpServerSockAddr
                     , sizeof(struct sockaddr));
    if (retval == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    /* ftp login */
    if (username == NULL && password == NULL)
    {
        /* anonymous login */
        retval = ftp_sock_auth(sockCtrlFd, "anonymous", "anonymous");
        if (retval == FILE_TRANSFER_AUTHFAILURE)
        {
            pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "authentication failed\n");
            return FILE_TRANSFER_AUTHFAILURE;
        }
    }
    else if (username == NULL || password == NULL)
    {
        pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "authentication failed\n");
        return FILE_TRANSFER_AUTHFAILURE;
    }
    else
    {
        retval = ftp_sock_auth(sockCtrlFd, username, password);
        if (retval == FILE_TRANSFER_AUTHFAILURE)
        {
            pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "authentication failed\n");
            return FILE_TRANSFER_AUTHFAILURE;
        }
    }

    /* PASV mode */
    ftp_commit_cmd(sockCtrlFd, FTP_CMD_PASV, NULL);
    retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_PASV
                                 , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);
    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* ftp 5xx or 4xx error response */
        return REQUEST_DENIED ;
    }

    /* TYPE I */
    ftp_commit_cmd(sockCtrlFd, FTP_CMD_TYPE, NULL);
    retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_TYPE
                                 , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);
    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* ftp 5xx or 4xx error response */
        return REQUEST_DENIED ;
    }

    /* connect to data ip/port */
    memset(&ftpServerSockDataAddr, 0, sizeof(struct sockaddr));
    ftpServerSockDataAddr.sin_family = AF_INET;
    ftpServerSockDataAddr.sin_port = htons(gPasvPort);
    inet_aton((char *)gPasvIPAddr, &ftpServerSockDataAddr.sin_addr);

    sockDataFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockDataFd == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    retval = connect(sockDataFd, (struct sockaddr *) & ftpServerSockDataAddr
                     , sizeof(struct sockaddr));
    if (retval == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    /* RETR */
    ftp_commit_cmd(sockCtrlFd, FTP_CMD_RETR, gFilename);
    retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_RETR
                                 , FTP_RCV_RESP_TIMEOUT, '1', &filesize, NULL);
    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* ftp 5xx or 4xx error response */
        return REQUEST_DENIED ;
    }

    /* should commit SIZE commmand to retrieve the file size */
    if (filesize == -1)
    {
        /* SIZE */
        ftp_commit_cmd(sockCtrlFd, FTP_CMD_SIZE, gFilename);
        retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_SIZE
                                     , FTP_RCV_RESP_TIMEOUT, '2', &filesize, NULL);
        if (retval == FTP_ERR_ERROR_RESP)
        {
            /* ftp 5xx or 4xx error response */
            return REQUEST_DENIED ;
        }
    }

    *filesizePtr = filesize;

    _fileContent = (char *)PAL_MALLOC(sizeof(char) * filesize);
    if (_fileContent == NULL)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    p = (PAL_UINT8_T *)_fileContent;
    downCount = 0;

    while (1)
    {
        nbytes = read(sockDataFd, buf, 1460);
        if (nbytes == 0)
        {
            break;
        }
        else if (nbytes == -1)
        {
            shutdown(sockDataFd, SHUT_RDWR);
            shutdown(sockCtrlFd, SHUT_RDWR);

            pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "%s\n", strerror(errno));
            return FTP_ERR_FAILED;
        }
        else
        {
            memcpy(p, buf, nbytes);
            p = p + nbytes;
            downCount += nbytes;
        }
    }

    /* shutdown socket */
    shutdown(sockDataFd, SHUT_RDWR);
    shutdown(sockCtrlFd, SHUT_RDWR);

    if (downCount == filesize)
    {
        /* NOTE: caller should take the responsiblity to free fileContent */
        *fileContent = _fileContent;
        return TCMAGENT_OK;
    }
    else
    {
        PAL_FREE(_fileContent);
        return DOWNLOAD_FAILURE ;
    }
}

/*****************************************************************************
 Function Name : ftp_upload_file
 Description : upload file into location specified by url.
 Returns : status indicate download result, 0 indicate success , other no-zero
           value indicate fault error code.
           Fault is classified in the following case:
                9001, Request denied
                9002, Internal error (Such as malloc failed,etc.)
                9011, Upload failure (network error)
                9012, File transfer server authentication failure
 Parameter : @url: file server path indicating a file location
             @username: used to authenticate with ftp server
             @password: used to authenticate with ftp server
             @filesize: fileContent length
             @fileContent: buffer contained file content need be uploaded.
                           It's allocated and released by caller
******************************************************************************/
PAL_INT32_T  ftp_upload_file(IN char *fileType
                             , IN char *url
                             , IN char *username
                             , IN char *password
                             , IN char *fileContent
                             , IN PAL_UINT32_T filesize)
{
    PAL_INT32_T retval;
    PAL_INT32_T n, left;
    PAL_INT32_T optval;
    char *p, *beginWith;
    char _filename[FILENAME_MAX];
    PAL_INT32_T _len;
    PAL_INT32_T sockCtrlFd;
    PAL_INT32_T sockDataFd;
    struct sockaddr_in ftpServerSockAddr;
    struct sockaddr_in ftpServerDataAddr;
    PAL_INT32_T nsent;
    char timestr[TIME_STR_LENGTH];

    //pal_set_logger(LOG_FTP_MODULE, T_LOG_INFO, LOG_CONSOLE, 0, NULL);

    /* parse ftp server ip/port */
    if (ftp_parse_url(url) == INTERNAL_ERROR)
    {
        return INTERNAL_ERROR;
    }

    /* use default value if filename is not specified in url */
    if (url[strlen(url)-1] == '/')
    {
        if (strcmp(fileType, UPLOAD_CONFIG_TYPE) == 0)
        {
            strcat(gFilename, DEFAULT_CONFIG_NAME);
        }
        else if (strcmp(fileType, UPLOAD_LOG_TYPE) == 0)
        {
            strcat(gFilename, DEFAULT_LOG_NAME);
        }
        else
        {
            pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "unknown file type\n");
            return INTERNAL_ERROR;
        }

        //pal_datetime(timestr);//use time to diff name
        //strcat(gFilename,timestr);
    }

    pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
               , "gFilename: %s\n", gFilename);

    /* initiate socket */
    if ((sockCtrlFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    optval = 1;
    retval = setsockopt(sockCtrlFd, SOL_SOCKET, SO_REUSEADDR
                        , (void *) & optval, sizeof(optval));
    if (retval == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    memset(&ftpServerSockAddr, 0, sizeof(struct sockaddr));
    ftpServerSockAddr.sin_family = AF_INET;
    ftpServerSockAddr.sin_port = htons(gFtpServerPort);
    inet_aton(gFtpServerStrIP, &ftpServerSockAddr.sin_addr);

    retval = connect(sockCtrlFd, (struct sockaddr *) & ftpServerSockAddr
                     , sizeof(struct sockaddr));
    if (retval == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    /* ftp login */
    if (username == NULL && password == NULL)
    {
        /* user anonymous login */
        if (ftp_sock_auth(sockCtrlFd, "anonymous", "anonymous")
                == FILE_TRANSFER_AUTHFAILURE)
        {
            pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "authentication failed\n");
            return FILE_TRANSFER_AUTHFAILURE;
        }
    }
    else if (username == NULL || password == NULL)
    {
        pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "username or password needed\n");
        return FILE_TRANSFER_AUTHFAILURE;
    }
    else
    {
        retval = ftp_sock_auth(sockCtrlFd, username, password);
        if (retval == FILE_TRANSFER_AUTHFAILURE)
        {
            pal_logger(T_LOG_WARNING, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "authentication failed\n");
            return FILE_TRANSFER_AUTHFAILURE;
        }
    }

    /* create the directory structure if it does not exist */
    /* gFilename may looks like '/upload/hook/doc/TR-111.pdf' */
    beginWith = gFilename + 1;
    gFilenameLen = strlen(gFilename);

    while (1)
    {
        char dirname[64];
        PAL_INT32_T dirlen;
        char *slash;

        slash = strchr(beginWith, '/');
        if (slash == NULL)
        {
            break;
        }
        dirlen = slash - beginWith;
        memcpy(dirname, beginWith, dirlen);
        dirname[dirlen] = '\0';

        ftp_commit_cmd(sockCtrlFd, FTP_CMD_CWD, dirname);
        retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_CWD
                                     , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);

        if (retval == FTP_ERR_ERROR_RESP)
        {
            /* directory does no exist, create it */
            ftp_commit_cmd(sockCtrlFd, FTP_CMD_MKD, dirname);
            retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_MKD
                                         , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);
            if (retval == FTP_ERR_ERROR_RESP)
            {
                /* unable to mkdir */
                return REQUEST_DENIED ;
            }

            /* change directory */
            ftp_commit_cmd(sockCtrlFd, FTP_CMD_CWD, dirname);
            retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_CWD
                                         , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);
            if (retval == FTP_ERR_ERROR_RESP)
            {
                /* unable to change dir */
                return REQUEST_DENIED ;
            }
        }

        beginWith = beginWith + strlen(dirname) + 1;

    } /* end of while(1) */

    /* PASV */
    ftp_commit_cmd(sockCtrlFd, FTP_CMD_PASV, NULL);
    retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_PASV
                                 , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);
    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* ftp 4xx or 5xx error response */
        return REQUEST_DENIED ;
    }

    /* TYPE I */
    ftp_commit_cmd(sockCtrlFd, FTP_CMD_TYPE, NULL);
    retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_TYPE
                                 , FTP_RCV_RESP_TIMEOUT, '2', NULL, NULL);
    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* ftp 4xx or 5xx error response */
        return REQUEST_DENIED ;
    }

    /* initiate data socket */
    if ((sockDataFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    /* reuse ip/port */
    optval = 1;
    retval = setsockopt(sockCtrlFd, SOL_SOCKET, SO_REUSEADDR
                        , (void *) & optval, sizeof(optval));
    if (retval == -1)
    {
        return INTERNAL_ERROR;
    }

    memset(&ftpServerDataAddr, 0, sizeof(struct sockaddr));
    ftpServerDataAddr.sin_family = AF_INET;
    ftpServerDataAddr.sin_port = htons(gPasvPort);
    inet_aton((char *)gPasvIPAddr, &ftpServerDataAddr.sin_addr);

    retval = connect(sockDataFd, (struct sockaddr *) & ftpServerDataAddr
                     , sizeof(struct sockaddr));
    if (retval == -1)
    {
        pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                   , "%s\n", strerror(errno));
        return INTERNAL_ERROR;
    }

    /* get absolute filename without path */
    p = strrchr(gFilename, '/');
    _len = strlen(gFilename) - (p - gFilename);
    memcpy(_filename, p + 1, _len);
    _filename[_len] = '\0';


    /* STOR */
    ftp_commit_cmd(sockCtrlFd, FTP_CMD_STOR, _filename);
    retval = ftp_handle_cmd_resp(sockCtrlFd, FTP_CMD_STOR
                                 , FTP_RCV_RESP_TIMEOUT, '1', NULL, NULL);
    if (retval == FTP_ERR_ERROR_RESP)
    {
        /* ftp 4xx or 5xx error response */
        return REQUEST_DENIED ;
    }

    p = fileContent;
    left = filesize;
    nsent = 0;

    while (left > 0)
    {
        PAL_INT32_T bufsize;

        bufsize = (left > 1460) ? 1460 : left;
        n = write(sockDataFd, p, bufsize);
        __msleep(100);

        if (n == -1)
        {
            pal_logger(T_LOG_ERROR, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
                       , "%s\n", strerror(errno));
            close(sockDataFd);
            close(sockCtrlFd);
            return -1;
        }
        else if (n == 0)
        {
            break;
        }
        else if (n > 0)
        {
            left -= n;
            p += n;
            nsent += n;
        }
    }

    pal_logger(T_LOG_INFO, __FILE__, __LINE__, LOG_FTP_MODULE, __func__
               , "filesize: %d, sent: %d\n", filesize, nsent);

    shutdown(sockDataFd, SHUT_RDWR);
    shutdown(sockCtrlFd, SHUT_RDWR);

    return (left == 0) ? TCMAGENT_OK : UPLOAD_FAILURE;
}
