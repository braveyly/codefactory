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
Filename: act_dmz.c
Author: Ken
Date: 2010-09-01
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#include "act_dmz.h"
#include "act_wan.h"

tcmRet actDmz_configDmz(int ruleAction)
{
    char dmzIp[BUFLEN_18];
    
    if ( config_api_read_parameter(ID_DMZ, PARA_DMZ_IP, dmzIp) )
    {
        tcmLog_error("Fail to read the dmz configuration");
        return TCMRET_CONFLIB_READ_ERROR;
    }

    if( ralIptables_setDmz(ruleAction, dmzIp, INTERFACENAME_WAN)!= TCMRET_SUCCESS )
    {
        tcmLog_error("Fail to config dmz");
        return TCMRET_INTERNAL_ERROR;
    }
    
    return TCMRET_SUCCESS;
}


tcmRet actDmz_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        case APP_START:
        {
            ret = actDmz_configDmz( RULE_ADD );
        }
        break;

        case APP_STOP:
        {
            ret = actDmz_configDmz( RULE_DELETE );
        }
        break;

        default:
        {
            tcmLog_error("Unsupport action: [%d]", doAct);
            ret = TCMRET_INVALID_ARGUMENTS;
        }
        break;
    }
    
    return ret;
}


