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
Filename: ral_iptables.h
Author: Ken
Date: 2010-08-25
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#ifndef __RAL_IPTABLES__H
#define __RAL_IPTABLES__H

#include <tcm.h>

tcmRet ralIptables_initNat(const char* extIf, const char* intIf);
tcmRet ralIptables_initSroute(const char* extIf, const char* intIf);
tcmRet ralIptables_setMss();
tcmRet ralIptables_setBridge(int action, const char* extIf, const char* intIf, const char* briIf, const char* bridgeIp, int maskbit);
tcmRet ralIptables_setPortForwarding(int action, const char* deviceName, const char* proto, int iStartPort, int iEndPort, int fStartPort, int fEndPort, const char* serverIp);
tcmRet ralIptables_setFirewall(int action, const char* deviceName, const char* proto, const char* srcip, const char* srcmask, const char* srcport, const char* dstip, const char* dstmask, const char* dstport, const char* policy);
tcmRet ralIptables_setDmz(int action, const char* dmzIp, const char* device);

#endif

