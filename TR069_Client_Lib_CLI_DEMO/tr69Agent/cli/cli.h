/********************************************************************** 
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
File name: cli.h 
Author: Hook Guo 
Date: Jun 12th. 2009
************************************************************************/

#ifndef _CLI_H_
#define _CLI_H_

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

/* error number */
#define CLI_OK 0
#define CLI_ERR_FAILED -1

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define TMP_MEM_TR69_TREE_PATH "../../demo/tmp/conf/mem/tr69_tree.$$$$"

typedef enum _CliAction {
    CA_NONE = 0,
    CA_SHOW_HELP,
    CA_SET_LOG_LEVEL,
    CA_GET_LOG_LEVEL,
    CA_SET_LOG_MODE,
    CA_GET_LOG_MODE,
    CA_GET_TREE_PARAM_VALUE,
    CA_SET_TREE_PARAM_VALUE,
    CA_GET_TR069_TREE,
    CA_GET_TR069_TREE_SIZE,
} CliAction;

#endif /* _CLI_H_ */
