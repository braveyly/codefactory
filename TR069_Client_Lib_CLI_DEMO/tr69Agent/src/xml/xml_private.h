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
File name: xml_private.h
Author: kiffen guan
Date: April 1th. 2009
************************************************************************/

#ifndef _XML_PRIVATE_H_
#define _XML_PRIVATE_H_

#include "xml_public.h"

typedef struct _XmlGlobal
{
    void (*cbError)(const char *);
    int numOfCbEntity;
    int (*cbsEntity[100])(const char *name);
    int wrap;
    cb_xml_custom_load cbCustomLoad;
    cb_xml_custom_save cbCustomSave;
} XmlGlobal;

extern XmlGlobal *xml_global(void);
extern int cb_xml_entity(const char *name);

#endif
