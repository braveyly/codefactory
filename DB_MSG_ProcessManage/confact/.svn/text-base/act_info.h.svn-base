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
Filename: act_info.h
Author: Ken
Date: 2010-08-17
Description: 
 
Histroy: 
    1.  Modification: 
        Author: 
        Date: 
  
    2.  
***********************************************************************/
 
#ifndef __ACT_INFO__H
#define __ACT_INFO__H

#include <tcm_util.h>

typedef struct
{
   tcmEntityId eid; /**< TcmEntityId of the user of this library. */
   void *msgHandle; /**< Message Handle of the user.*/
}actInfo;

tcmRet actLib_init(tcmEntityId myId, void* msgHandle);
void actLib_cleanup();

#endif
