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
Filename: configdata.c
Author: Ken
Date: 2009-08-18
Description:

Histroy:
    1.  Modification:
        Author:
        Date:

    2.
***********************************************************************/

#include <stdio.h>

#include "conflib.h"
#include "configapi.h"
#include <act_util.h>

extern char gRecordWriteID[ID_END - 1];

/***********************************************************************
Function: update_config_file
Description: update the config file

Return: int,
Input: 
Output:
Others:

Histroy:
    1.  Modification: Created.
        Author: Ken
        Date: 2009-08-20

    2.  Modification:
        Author:
        Date:
************************************************************************/
int update_config_file ( void )
{
    int i = 0;
    for ( i = 0; i < ID_END - 1; i++ )
    {
        if ( gRecordWriteID[i] == '1' )
        {
            switch( i + 1 )
            {
                case ID_DHCPD:
                {
                    actUtil_updateDhcpdCfg();
                }
                break;

                case ID_DNSMASQ:
                {
                    actUtil_updateResolveCfg();
                }
                break;

                default:
                {
                    break;
                }
            }
        }
    }

    memset(gRecordWriteID, 0, sizeof(gRecordWriteID));

    return 0;
}


