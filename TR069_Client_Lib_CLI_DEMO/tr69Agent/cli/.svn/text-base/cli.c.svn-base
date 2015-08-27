/********************************************************************* 
Copyright (c) 2009 Wuhan Tecom Co., Ltd. 
All Rights Reserved  
No portions of this material may be reproduced in any form without the  
written permission of:  
Wuhan Tecom Co., Ltd. 
#18, Huaguang Road  
Wuhan, PR China 430074  
All information contained in this document is Wuhan Tecom Co., Ltd. 
company private, proprietary, and trade secret. 
********************************************************************** 
File name: cli.c 
Author: Hook Guo 
Date: Jun 12th. 2009
History:
            2009-6-17   change some command line parameters into 
                        lower case (Hook Guo)
            2009-6-25   all the features provided by cli has been tested,
                        modifications have been made especially on 
                        Inter Process Communication (Hook Guo)
************************************************************************/

#include "cli.h"

char *gParamName = NULL;
static CliAction gCliAction = CA_NONE;
PAL_MSG_T gMsgUser[MAX_MODULE];

void cli_help()
{
    fprintf(stdout, "Usage: tcmAgentCli [option] ...\n");
    fprintf(stdout, "-h                                          show help\n");
    fprintf(stdout, "-g tr069_tree -o [filename]                 redirect tree\n");
    fprintf(stdout, "-g param -n [param_name]                    get tree parameter value\n");
    fprintf(stdout, "-g log_level                                get current log level\n");
    fprintf(stdout, "-g log_mode                                 get current log mode\n");
    fprintf(stdout, "-s log_level -a [log_level_value]           set current log level\n");
    fprintf(stdout, "-s log_mode -a [log_mode_value]             set current log mode\n");
    fprintf(stdout, "-s param -n [param_name] -a [param_value]   set tree parameter value\n");
    fprintf(stdout, "log level can be set to 0-3, representing T_LOG_INFO, T_LOG_WARNING, T_LOG_ERROR and T_LOG_EMERGENCY respectly\n");
    fprintf(stdout, "log mode can be set to 0-3, representing LOG_DISABLE, LOG_CONSOLE, LOG_SYS_LOG, LOG_BOTH respectly\n");
}

static void cli_msg_init()
{
    PAL_INT32_T i;

    for (i = 0; i < MAX_MODULE; i++)
    {
        gMsgUser[i] = pal_create_msg ( i );
    }
}

static PAL_INT32_T cli_msg_snd(Module dest, Module src, char *buf
                              , PAL_SIZE_T bufLen)
{
    return pal_send_msg (gMsgUser[(PAL_INT32_T)dest]
                        , gMsgUser[(PAL_INT32_T)src]
                        , buf, bufLen, 0);
}

static PAL_INT32_T cli_msg_recv(Module dest, Module *src, char *buf
                               , PAL_SIZE_T bufLen)
{
    PAL_INT32_T ret;
    PAL_UINT32_T eventType;
    PAL_INT32_T i;

    ret = pal_recv_msg(gMsgUser[(PAL_INT32_T)dest]
        , &eventType, buf, bufLen, 0);

    for (i = 0; i < MAX_MODULE; i++)
    {
        if (gMsgUser[i] == eventType)
        {
            *src = i;
        }
    }

    return ret;
}

static void cli_msg_info_fetch(IN char *buf
                              , IN PAL_INT32_T bufsiz
                              , OUT MsgType *msgTypePtr
                              , OUT char *msgName
                              , OUT MsgValueType *msgValueTypePtr
                              , OUT Tr069ValueArg *msgValuePtr
                              , OUT PAL_INT32_T *msgValueLenPtr)
{
    PAL_INT32_T _msgTotalLen, _msgNameLen;
    MsgType _msgType;
    MsgValueType _msgValueType;
    char _msgName[MSG_NAME_MAX_LEN + 1];

    char *p = buf;
    _msgTotalLen = *((PAL_INT32_T *)p);
    p += sizeof (PAL_INT32_T);
    _msgType = *((MsgType *)p);
    p += sizeof (MsgType);
    _msgNameLen = *((PAL_INT32_T *)p);
    p += sizeof (PAL_INT32_T);
    memcpy(_msgName, p, _msgNameLen);
    p += _msgNameLen;
    _msgName[_msgNameLen] = '\0';

    if ((p - buf) >= _msgTotalLen)
    {
        /* no message value in the buffer */
        goto parseEnd_tag;
    }

    if (msgValueTypePtr == NULL && msgValuePtr == NULL)
    {
        /* no need to parse message value */
        goto parseEnd_tag;
    }
    
    /* parse message value */
    *msgValueTypePtr = *((MsgValueType *)p);
    p += sizeof (MsgValueType);

    switch (*msgValueTypePtr)
    {
        case MSG_INT:
            msgValuePtr->value.intv = *((PAL_INT32_T *)p);
            *msgValueLenPtr = sizeof (PAL_INT32_T);
            break;
        case MSG_UINT:
            msgValuePtr->value.uintv = *((PAL_UINT32_T *)p);
            *msgValueLenPtr = sizeof (PAL_UINT32_T);
            break;
        case MSG_BOOL:
            msgValuePtr->value.boolv = *((PAL_BOOL_T *)p);
            *msgValueLenPtr = sizeof (PAL_BOOL_T);
            break;
        case MSG_STRING:
            msgValuePtr->value.string = strdup((char *)p);
            *msgValueLenPtr = strlen((char *)p);
            break;
        default:
            break;
    }

parseEnd_tag:
    if (msgTypePtr != NULL)
    {
        *msgTypePtr = _msgType;
    }

    if (msgName != NULL)
    {
        strcpy(msgName, _msgName);
    }
}

static PAL_INT32_T cli_do_msg_handle(PAL_UINT8_T *buf, PAL_INT32_T bufsiz)
{
    PAL_INT32_T msgLen;
    MsgType msgType;
    PAL_INT32_T msgNameLen;
    char msgName[MAX_TR069_NAME_LEN + 1];
    MsgValueType msgValueType;
    PAL_INT32_T ret;
    Tr069ValueArg msgValue;
    PAL_INT32_T msgValueLen;

    /* parse msg information */
    cli_msg_info_fetch(buf 
                      , bufsiz
                      , &msgType
                      , msgName
                      , &msgValueType
                      , &msgValue
                      , &msgValueLen);

    if (msgType != MSG_OK)
    {
        fprintf(stderr, "command failed\n");
        return CLI_ERR_FAILED;
    }

    /* do corresponding things */
    if ((gCliAction == CA_GET_TR069_TREE_SIZE) 
	   && (strcmp (msgName, MN_GET_TR069_TREE_SIZE) == 0))
    {
        printf ("current tr069 tree size: %d bytes\n", msgValue.value.intv);
    }
    if ((gCliAction == CA_SET_LOG_LEVEL)
       && (strcmp(msgName, MN_SET_LOG_LEVEL) == 0))
    {

    }
    else if ((gCliAction == CA_SET_LOG_MODE)
            && (strcmp(msgName, MN_SET_LOG_MODE) == 0))
    {

    }
    else if ((gCliAction == CA_GET_TR069_TREE)
            && (strcmp(msgName, MN_GET_TR069_TREE) == 0))
    {
        
    }
    else if ((gCliAction == CA_GET_LOG_LEVEL)
            && (strcmp(msgName, MN_GET_LOG_LEVEL) == 0))
    {
        switch (msgValue.value.intv)
        {
            case T_LOG_INFO:
                printf("current log level: T_LOG_INFO\n");
                break;
            case T_LOG_WARNING:
                printf("current log level: T_LOG_WARNING\n");
                break;
            case T_LOG_ERROR:
                printf("current log level: T_LOG_ERROR\n");
                break;
            case T_LOG_EMERGENCY:
                printf("current log level: T_LOG_EMERGENCY\n");
                break;
            default:
                printf("failed to get current log level\n");
                break;
        }
    }
    else if ((gCliAction == CA_GET_LOG_MODE) 
            && (strcmp(msgName, MN_GET_LOG_MODE) == 0))
    {
        switch (msgValue.value.intv)
        {
            case LOG_DISABLE:
                printf("current log mode: LOG_DISABLE\n");
                break;
            case LOG_CONSOLE:
                printf("current log mode: LOG_CONSOLE\n");
                break;
            case LOG_SYS_LOG:
                printf("current log mode: LOG_SYS_LOG\n");
                break;
            case LOG_BOTH:
                printf("current log mode: LOG_BOTH\n");
                break;
            default:
                printf("failed to get current log mode\n");
                break;
        }
    }
    else if ((gCliAction == CA_GET_TREE_PARAM_VALUE)
            && (strcmp(gParamName, msgName) == 0))
    {
        printf("param-name: %s, param-value: %s\n"
              , msgName, msgValue.value.string);
        return CLI_OK;
    }
    else if ((gCliAction == CA_SET_TREE_PARAM_VALUE)
            && (strcmp(gParamName, msgName) == 0))
    {

    }
    return CLI_OK;
}

static PAL_INT32_T cli_do_msg_interchange(PAL_UINT8_T *sndBufPtr
                                         , PAL_INT32_T sndBufLen
                                         , PAL_UINT8_T *recvBufPtr
                                         , PAL_INT32_T *recvBufLenPtr)
{
    Result ret;
    Module srcModule;
    PAL_INT32_T nbytes;

    if ((ret = cli_msg_snd(HOST_HANDLER, CLI, sndBufPtr, sndBufLen)) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return CLI_ERR_FAILED;
    }

    nbytes = cli_msg_recv(CLI, &srcModule, recvBufPtr, *recvBufLenPtr);
    *recvBufLenPtr = nbytes;

    if (nbytes < 0 || srcModule != HOST_HANDLER)
    {
        return CLI_ERR_FAILED;
    }

    return CLI_OK;
}

static void cli_msg_build(IN PAL_UINT8_T *buf
                         , IN OUT PAL_INT32_T *bufsiz
                         , IN MsgType msgType
                         , IN PAL_INT32_T msgNameLen 
                         , IN char *msgName
                         , IN MsgValueType msgValueType
                         , IN void *msgValue
                         , IN PAL_INT32_T msgValueLen)
{
    PAL_INT32_T msgLen;
    PAL_UINT8_T *p;

    p = buf;
    p += sizeof(PAL_INT32_T); /* skip msgLen */
    *((PAL_INT32_T *)p) = msgType; /* msg type */
    p += sizeof(PAL_INT32_T);
    *((PAL_INT32_T *)p) = msgNameLen; /* msg name len */
    p += sizeof(PAL_INT32_T);
    memcpy(p, msgName, strlen(msgName)); /* msg name */
    p += strlen(msgName);
    *p++ = '\0';
    
    if (msgValue == NULL)
    {
        goto skipValue_tag;
    }
    
    *((MsgValueType *)p) = msgValueType; /* msg value type */
    p += sizeof(MsgValueType);
    memcpy(p, msgValue, msgValueLen); /* msg value */
    p += msgValueLen;

skipValue_tag:
    msgLen = p - buf;
    *((PAL_INT32_T *)buf) = msgLen;
    *bufsiz = msgLen;
}

PAL_INT32_T main(PAL_INT32_T argc, char *argv[])
{
    PAL_INT32_T c;
    char *options;

    PAL_UINT8_T sndBufPtr[MAX_TCM_SEND_MSG_LEN];
    PAL_INT32_T sndBufLen;
    PAL_UINT8_T recvBufPtr[MAX_TCM_SEND_MSG_LEN];
    PAL_INT32_T recvBufLen = MAX_TCM_SEND_MSG_LEN;

    MsgType msgType;
    MsgValueType msgValueType;
    char msgName[MAX_TR069_NAME_LEN + 1];
    PAL_INT32_T msgNameLen;
    PAL_INT32_T ret;
    void *msgValue = NULL;
    PAL_INT32_T msgValueLen;

    char *paramName = NULL;
    void *paramValue = NULL;
    PAL_INT32_T setValueFlag = FALSE;
    PAL_INT32_T getValueFlag = FALSE;

    char outputDIR[128];
    char *pwd;

    msgName[0] = '\0';
    outputDIR[0] = '\0';
    cli_msg_init();

    if (argc <= 1)
    {
        /* no command line argument provided */
        fprintf(stderr, "no input, use './tcmAgentCli -h' for help\n");
        return CLI_ERR_FAILED;
    }

    while (1)
    {
        PAL_INT32_T optionIdx = 0;
        
        options = "hs:g:n:a:o:p";
        c = getopt(argc, argv, options);

        if (c == -1)
        {
            /* parsing complete */
            break;
        }

        switch (c)
        {
	        case 'p':
	        {
                gCliAction = CA_GET_TR069_TREE_SIZE;
	            break; 
	        }			
            case 'h':
            {
                cli_help();
                return CLI_OK;
            }
            case 'o':
            {
                if (((char *)optarg)[0] != '/')
                {
                    /* relative directory */
                    strcpy(outputDIR, PAL_CONFIG_BASE);
                    strcat(outputDIR, (char *)optarg);
                }
                else
                {
                    strcpy(outputDIR, (char *)optarg);
                }
                
                break;
            }
            case 's':
            {
                char *choice = (char *)optarg;

                if (strcmp(choice, "param") == 0)
                {
                    gCliAction = CA_SET_TREE_PARAM_VALUE;
                }
                else if (strcmp(choice, "log_level") == 0)
                {
                    gCliAction = CA_SET_LOG_LEVEL;
                }
                else if (strcmp(choice, "log_mode") == 0)
                {
                    gCliAction = CA_SET_LOG_MODE;
                }
                else
                {
                    fprintf(stderr, "invalid option value, use './tcmAgentCli -h' for help\n");
                    return CLI_ERR_FAILED;
                }
                break;
            }
            case 'g':
            {
                char *choice = (char *)optarg;
                
                if (strcmp(choice, "param") == 0)
                {
                    gCliAction = CA_GET_TREE_PARAM_VALUE;
                }
                else if (strcmp(choice, "tr069_tree") == 0)
                {
                    gCliAction = CA_GET_TR069_TREE;
                }
                else if (strcmp(choice, "log_level") == 0)
                {
                    gCliAction = CA_GET_LOG_LEVEL;
                }
                else if (strcmp(choice, "log_mode") == 0)
                {
                    gCliAction = CA_GET_LOG_MODE;
                }
                else 
                {
                    fprintf(stderr, "invalid option value, use './tcmAgentCli -h' for help\n");
                    return CLI_ERR_FAILED;
                }
                break;
            }
            case 'n':
                paramName = (char *)optarg;
                gParamName = strdup(paramName);
                break;

            case 'a':
                paramValue = optarg;
                break;

            default:
                return CLI_ERR_FAILED;
        } /* end of switch */
    } /* end of while (1) */
   
    /* commit cli command */
    switch (gCliAction)
    {
        case CA_GET_TR069_TREE:
        {
            if (strlen(outputDIR) == 0)
            {
                /* no output directory specified */
                fprintf(stderr, "no output directory specified\n");
                return CLI_ERR_FAILED;
            }
            msgType = GET_TR069_TREE;
            strcpy(msgName, MN_GET_TR069_TREE);
            msgNameLen = strlen(msgName) + 1;
            msgValueType = MSG_STRING;
            msgValue = (char *)&outputDIR[0];
            msgValueLen = strlen(outputDIR) + 1;
            break;
        }
        case CA_GET_LOG_LEVEL:
        {
            msgType = GET_LOG_LEVEL;
            strcpy(msgName, MN_GET_LOG_LEVEL);
            msgNameLen = strlen(msgName) + 1;
            msgValue = NULL;
            break;
        }
        case CA_SET_LOG_LEVEL:
        {
            PAL_INT32_T tmpint;
            Pal_LogLevel level;

            if (paramValue == NULL)
            {
                /* no log level specified */
                fprintf(stderr, "no log level specified\n");
                return CLI_ERR_FAILED;
            }
            else if (isdigit(*((char *)paramValue)) == 0)
            {
                /* not a digit value */
                fprintf(stderr, "not a valid option value\n");
                return CLI_ERR_FAILED;
            }

            tmpint = atoi((char *)paramValue);
            switch (tmpint)
            {
                case 0: 
                    level = T_LOG_INFO; break;
                case 1:
                    level = T_LOG_WARNING; break;
                case 2:
                    level = T_LOG_ERROR; break;
                case 3: 
                    level = T_LOG_EMERGENCY; break;
                default:
                        fprintf(stderr, "not a valid log level value\n");
                        break;
            }

            msgType = SET_LOG_LEVEL;
            strcpy(msgName, MN_SET_LOG_LEVEL);
            msgNameLen = strlen(msgName) + 1;
            msgValueType = MSG_INT;
            msgValue = (void *)&tmpint;
            msgValueLen = sizeof (PAL_INT32_T);
            break;
        }
        case CA_GET_LOG_MODE:
        {
            msgType = GET_LOG_MODE;
            strcpy(msgName, MN_GET_LOG_MODE);
            msgNameLen = strlen(msgName) + 1;
            msgValue = NULL;
            break;
        }
        case CA_SET_LOG_MODE:
        {
            PAL_INT32_T tmpint;
            LogMode mode;

            if (paramValue == NULL)
            {
                /* no log mode specified */
                fprintf(stderr, "no log mode specified\n");
                return CLI_ERR_FAILED;
            }   
            else if (isdigit(*((char *)paramValue)) == 0)
            {
                /* not a digit value */
                fprintf(stderr, "not a valid option value\n");
                return CLI_ERR_FAILED;
            } 

            tmpint = atoi((char *)paramValue);
            switch (tmpint)
            {
                case 0: 
                    mode = LOG_DISABLE; break;
                case 1:
                    mode = LOG_CONSOLE; break;
                case 2:
                    mode = LOG_SYS_LOG; break;
                case 3:
                    mode = LOG_BOTH; break;
                default:
                    fprintf(stderr, "not a valid log mode value\n");
                    return CLI_ERR_FAILED;
            }

            msgType = SET_LOG_MODE;
            strcpy(msgName, MN_SET_LOG_MODE);
            msgNameLen = strlen(msgName) + 1;
            msgValueType = MSG_INT;
            msgValue = (void *)&tmpint;
            msgValueLen = sizeof (PAL_INT32_T);
            break;
        }
        case CA_GET_TREE_PARAM_VALUE:
        {
            if (paramName == NULL)
            {
                fprintf(stderr, "no tr069 tree parameter name specified\n");
                return CLI_ERR_FAILED;
            }
            msgType = GET_VALUE;
            strcpy(msgName, paramName);
            msgNameLen = strlen(msgName) + 1;
            msgValue = NULL;
            break;
        }
        case CA_SET_TREE_PARAM_VALUE:
        {
            if (paramName == NULL || paramValue == NULL)
            {
                fprintf(stderr, "tr069 tree parameter value/name not specified\n");
                return CLI_ERR_FAILED;
            }
            msgType = SET_VALUE_EX;
            strcpy(msgName, paramName);
            msgNameLen = strlen(msgName) + 1;
            msgValueType = MSG_STRING;
            msgValue = paramValue;
            msgValueLen = strlen((char *)paramValue) + 1;
            break;
        }
        case CA_GET_TR069_TREE_SIZE:
        {
            msgType = GET_TR069_TREE_SIZE;
	        strcpy (msgName, "");
	        msgNameLen = 0;
	        msgValueType = 0;
            msgValue = NULL;
            msgValueLen = 0;
	        break;
        }
        default:
            return CLI_ERR_FAILED;
    }

    cli_msg_build(sndBufPtr
                 , &sndBufLen
                 , msgType
                 , msgNameLen
                 , msgName
                 , msgValueType
                 , msgValue
                 , msgValueLen);

    ret = cli_do_msg_interchange(sndBufPtr, sndBufLen, recvBufPtr, &recvBufLen);
    if (ret != CLI_OK)
    {
        fprintf(stderr, "function cli_do_msg_interchange failed\n");
        return CLI_ERR_FAILED;
    }
    
    cli_do_msg_handle(recvBufPtr, recvBufLen);

end:
    return CLI_OK;
}
