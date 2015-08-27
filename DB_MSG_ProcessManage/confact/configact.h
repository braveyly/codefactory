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
Filename: configact.h
Author: Ken
Date: 2010-08-16
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/

#ifndef __CONFIG_ACT__H
#define __CONFIG_ACT__H

#include <conflib.h>
#include <tcm_util.h>
#include <act_util.h>
#include <act_info.h>

typedef enum
{
    APP_WRITE = 1,
    APP_WRITE_RESTART,
    APP_START,
    APP_STOP,
    APP_RESTART,

    /* Must be the last one */
    APP_NO_ACTION,
} tcmActAppID;

tcmRet config_act_refresh_apps( tcmEntityId APP_ID, tcmActAppID APP_ACTION );
tcmRet config_act_refresh_tables( tcmEntityId APP_ID, tcmActTalbesID TABLES_ACTION, int tabIndex, char** value);

#endif

