#ifndef TR069_WRITE_H
#define TR069_WRITE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#define _GNU_SOURCE
#include <getopt.h>
#include <unistd.h>
#include "pal.h"
#include "tr69_agent.h"
#include "tr69_agent_public.h"
#include "tr69_tree_public.h"
#include "host_handler_public.h"
#include "tcm.h"
#include "tcm_eid.h"
#include "tcm_log.h"

#define MAX_NUM_OF_ROW_PARA 8
#define MAX_LEN_OF_VALUE 640

#define  TCMAGENTAPI_DEBUG(args...) tcmLog_debug(args)
#define  TCMAGENTAPI_NOTICE(args...) tcmLog_notice(args)
#define  TCMAGENTAPI_ERROR(args...) tcmLog_error(args)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
extern tcmRet tcmAgent_api_set_value_by_index(tcmEntityId appID, int rowID, int parID, char* value);
extern tcmRet tcmAgent_api_add_object_by_index(tcmEntityId appID, int rowID, char** value);
extern tcmRet tcmAgent_api_del_object_by_index(tcmEntityId appID, int rowID);
#endif
