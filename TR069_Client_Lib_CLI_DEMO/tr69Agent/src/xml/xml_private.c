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
File name: xml_private.c
Author: kiffen guan
Date: April 1th. 2009
************************************************************************/

#include "xml_private.h"

void xml_error(const char *format, ...)
{
    va_list ap;
    char  s[1024];
    XmlGlobal *global = xml_global();

    if (!format)
    {
        return;
    }

    va_start(ap, format);

    vsnprintf(s, sizeof(s), format, ap);

    va_end(ap);

    if (global->cbError)
    {
        (*global->cbError)(s);
    }
    else
    {
        fprintf(stderr, "xml: %s\n", s);
    }
}

XmlType  xml_ignore_cb(XmlNode *node)
{
    (void)node;

    return (XML_IGNORE);
}

XmlType xml_integer_cb(XmlNode *node)
{
    (void)node;

    return (XML_INTEGER);
}

XmlType xml_opaque_cb(XmlNode *node)
{
    (void)node;

    return (XML_OPAQUE);
}

XmlType xml_real_cb(XmlNode *node)
{
    (void)node;

    return (XML_REAL);
}

XmlGlobal *xml_global(void)
{
    static XmlGlobal global =
    {
        NULL,
        1,
        { cb_xml_entity },
        72,
        NULL,
        NULL
    };

    return (&global);
}
