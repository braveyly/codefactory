/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************/

#ifndef __TCM_UTIL_H__
#define __TCM_UTIL_H__

#include "tcm_log.h"
#include "tcm_mem.h"
#include "tcm_ast.h"
#include "tcm_tmr.h"
#include "tcm_tms.h"
#include "tcm_strconv.h"
#include "tcm_net.h"
#include "tcm_fil.h"
#include "tcm_dlist.h"
#include "tcm_image.h"
#include "tcm_msg.h"

typedef enum
{
    RULE_ADD = 1,
    RULE_DELETE,
}tcmActTalbesID;

void tcm_doSystemAction(const char* from, char *cmd);
UBOOL8 tcm_getIfSubnet(const char *ipaddr, const char *netmask, char* ipSubnet);
void tcm_toUpperCase(char * string);
void tcm_toLowerCase(char * string);

#endif /* __TCM_UTIL_H__ */
