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
File name: xml_attr.c
Author: kiffen guan
Date: April 1th. 2009
************************************************************************/

#include "xml_public.h"

static int xml_set_attr(XmlNode *node, const char *name, char *value);

const char *xml_element_get_attr(XmlNode *node,  const char  *name)
{
    int  i;
    XmlAttribute *attr;

    if (!node || node->type != XML_ELEMENT || !name)
    {
        return (NULL);
    }

    for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i --, attr ++)
    {
        if (!strcmp(attr->name, name))
        {
            return (attr->value);
        }
    }

    return (NULL);
}

void xml_element_set_attr(XmlNode *node,  const char  *name, const char  *value)
{
    char *valuec;

    if (!node || node->type != XML_ELEMENT || !name)
    {
        return;
    }

    if (value)
    {
        valuec = strdup(value);
    }
    else
    {
        valuec = NULL;
    }

    if (xml_set_attr(node, name, valuec))
    {
        free(valuec);
    }
}

static int xml_set_attr(XmlNode *node,  const char  *name, char *value)
{
    int  i;
    XmlAttribute *attr;

    for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i --, attr ++)
    {
        if (!strcmp(attr->name, name))
        {
            if (attr->value)
            {
                free(attr->value);
            }

            attr->value = value;

            return (0);
        }
    }

    if (node->value.element.num_attrs == 0)
    {
        attr = malloc(sizeof(XmlAttribute));
    }
    else
    {
        attr = realloc(node->value.element.attrs,
                       (node->value.element.num_attrs + 1) * sizeof(XmlAttribute));
    }

    if (!attr)
    {
        xml_error("Unable to allocate memory for attribute '%s' in element %s!", name, node->value.element.name);
        return (-1);
    }

    node->value.element.attrs = attr;
    attr += node->value.element.num_attrs;

    if ((attr->name = strdup(name)) == NULL)
    {
        xml_error("Unable to allocate memory for attribute '%s' in element %s!",  name, node->value.element.name);
        return (-1);
    }

    attr->value = value;

    node->value.element.num_attrs ++;

    return (0);
}

