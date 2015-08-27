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
Filename: ral_wan.h
Author: Ken
Date: 2010-08-25
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#ifndef __RAL_WAN__H
#define __RAL_WAN__H

#include <tcm.h>

tcmRet ralWan_doConfig(int action, const char* wanName, const char* wanIp, const char* wanMask, const char* wanGateway, const char* wanMtu);
tcmRet ralWan_setVlan(int action, const char* infName, int vlanID, int skbPrio, int vlanPrio);

#endif

