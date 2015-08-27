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
File name: stun_public.h
Author: Hook Guo
Date: May 15th. 2009
************************************************************************/

#ifndef _STUN_PUBLIC_H
#define _STUN_PUBLIC_H

#include "tr69_agent_public.h"

/**************************************************************************
 Function Name: stun_client_start
 Description: initiate the tr111 module
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
Result stun_client_start();
void stun_wakeup();
#endif /* _STUN_PUBLIC_H */

