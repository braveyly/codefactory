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
Filename: act_lan.c
Author: Ken
Date: 2010-08-16
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 

#include "act_lan.h"
#include <ral_lan.h>

tcmRet actLan_configLan()
{
    char lanIp[BUFLEN_18];
    char lanMask[BUFLEN_18];
    
    if ( config_api_read_parameter(ID_LAN, PARA_LAN_IP, lanIp) 
      || config_api_read_parameter(ID_LAN, PARA_LAN_MASK, lanMask))
    {
        tcmLog_error("Fail to read the lan configuration");
        return TCMRET_CONFLIB_READ_ERROR;
    }

    if( ralLan_doConfig(lanIp, lanMask) != TCMRET_SUCCESS )
    {
        tcmLog_error("Fail to config lan");
        return TCMRET_INTERNAL_ERROR;
    }
    
    return TCMRET_SUCCESS;
}


tcmRet actLan_doConfig( int doAct )
{
    tcmRet ret = TCMRET_SUCCESS;
    switch ( doAct )
    {
        case APP_WRITE_RESTART:
        case APP_RESTART:
        case APP_START:
        {
            ret = actLan_configLan();
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
