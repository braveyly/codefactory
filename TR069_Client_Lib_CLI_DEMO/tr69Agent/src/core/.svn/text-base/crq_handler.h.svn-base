#ifndef _CRQ_HANDLER_H_
#define _CRQ_HANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "digcalcu.h"
#include "global.h"
#include "md5.h"
#include "pal.h"
#include "tr69_agent_public.h"
#include "tr69_agent.h"
#include "mainloop_public.h"
#include "host_handler_public.h"
#include "event_public.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define STATUS_CODE_200 200
#define STATUS_CODE_200_REASON "OK"
#define STATUS_CODE_401 401
#define STATUS_CODE_401_REASON "Unauthorized"
#define STATUS_CODE_503 503
#define STATUS_CODE_503_REASON "Service Unavailable"

#define CRQ_AUTH_PARAM_LEN 128
#define CRQ_HTTP_PARAM_LEN 128
#define CRQ_HTTP_AUTH_INFO_LEN 1024

#define BLACKLIST_CAPACITY 1024
#define BLACKLIST_STATUS_EMPTY 0
#define BLACKLIST_STATUS_OCUPIED 1

typedef struct _BlackListItem
{
    char bannedIP[INET_ADDRSTRLEN + 1];
    PAL_TIME_T ts;
    PAL_INT32_T status;
}BlackListItem;

typedef struct _BlackList
{
    BlackListItem banned[BLACKLIST_CAPACITY];
    PAL_INT32_T size;
}BlackList;

#ifndef BUFSIZ
#define BUFSIZ 8192
#endif

#define LINELEN 1024

#define MAX_CONN_TIME 5
#define BAN_COST_TIME 30 * 60
#define BLACKLIST_REFRESH_FREQUENCY 60
#define CRQ_ACCEPT_TIMEOUT 30

Result init_tcp_crq_handler();

#endif /* _CRQ_HANDLER_H_ */
