/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************
* Filename:    launcher.h
* Author:      Daniel Gao
* Date:        2009-03-19
* Description:
*
* Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/
#ifndef __LAUNCHER__H
#define __LAUNCHER__H

#include <tcm.h>
#include <conflib.h>
#include <dls.h>

extern UBOOL8 keepLooping;

void sendMessageByAppState(DlsInfo *dInfo, tcmMsgHeader **msg);

/**************************************************************
* Name: app_launcher                                          *
* Usage: Launch the network apps need boot up at start up     *
* Return: tcmRet                                              *
***************************************************************/
tcmRet app_launcher(UINT32 stage);


tcmRet init_launcher();


void clean_launcher();

#endif
