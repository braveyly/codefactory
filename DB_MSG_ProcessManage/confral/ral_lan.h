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
Filename: ral_lan.h
Author: Ken
Date: 2010-08-16
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#ifndef __RAL_LAN__H
#define __RAL_LAN__H

#include <tcm.h>

tcmRet ralLan_doConfig(const char* lanIp, const char* lanMask);

#endif
