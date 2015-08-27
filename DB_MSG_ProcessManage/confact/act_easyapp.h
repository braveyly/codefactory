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
Filename: act_easyapp.h
Author: Ken
Date: 2010-08-27
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#ifndef __ACT_EASYAPP__H
#define __ACT_EASYAPP__H

#include "configact.h"

tcmRet actSyslog_doConfig( int doAct );
tcmRet actInetd_doConfig( int doAct );
tcmRet actPcscd_doConfig( int doAct );
tcmRet actSdkd_doConfig( int doAct );
tcmRet actAutocm_doConfig( int doAct );
#endif
