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
Filename: act_firewall.h
Author: Ken
Date: 2010-09-01
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#ifndef __ACT_FIREWALL__H
#define __ACT_FIREWALL__H

#include "configact.h"

tcmRet actFirewall_doconfig( int doAct );
tcmRet actFirewall_setting(int action, int pfIndex, char** value);

#endif

