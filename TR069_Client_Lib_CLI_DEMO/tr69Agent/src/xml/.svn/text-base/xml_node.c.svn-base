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
File name: xml_node.c
Author: kiffen guan
Date: April 1th. 2009
************************************************************************/

#include "xml_public.h"

static XmlNode *xml_new(XmlNode *parent, XmlType type);

void
xml_add(XmlNode *parent, int where, XmlNode *child,  XmlNode *node)
{
    if (!parent || !node)
    {
        return;
    }

    if (node->parent)
    {
        xml_remove(node);
    }

    node->parent = parent;

    switch (where)
    {
        case XML_ADD_BEFORE :
            if (!child || child == parent->child || child->parent != parent)
            {
                node->next = parent->child;

                if (parent->child)
                {
                    parent->child->prev = node;
                }
                else
                {
                    parent->lastChild = node;
                }

                parent->child = node;
            }
            else
            {
                node->next = child;
                node->prev = child->prev;

                if (child->prev)
                {
                    child->prev->next = node;
                }
                else
                {
                    parent->child = node;
                }

                child->prev = node;
            }
            break;

        case XML_ADD_AFTER :
            if (!child || child == parent->lastChild || child->parent != parent)
            {
                node->parent = parent;
                node->prev   = parent->lastChild;

                if (parent->lastChild)
                {
                    parent->lastChild->next = node;
                }
                else
                {
                    parent->child = node;
                }

                parent->lastChild = node;
            }
            else
            {
                node->prev = child;
                node->next = child->next;

                if (child->next)
                {
                    child->next->prev = node;
                }
                else
                {
                    parent->lastChild = node;
                }

                child->next = node;
            }
            break;
    }
}

void xml_delete(XmlNode *node)
{
    int i;

    if (!node)
    {
        return;
    }

    xml_remove(node);

    while (node->child)
    {
        xml_delete(node->child);
    }

    switch (node->type)
    {
        case XML_ELEMENT :
            if (node->value.element.name)
            {
                free(node->value.element.name);
            }

            if (node->value.element.num_attrs)
            {
                for (i = 0; i < node->value.element.num_attrs; i ++)
                {
                    if (node->value.element.attrs[i].name)
                    {
                        free(node->value.element.attrs[i].name);
                    }
                    if (node->value.element.attrs[i].value)
                    {
                        free(node->value.element.attrs[i].value);
                    }
                }

                free(node->value.element.attrs);
            }
            break;
        case XML_INTEGER :
            /* Nothing to do */
            break;
        case XML_OPAQUE :
            if (node->value.opaque)
            {
                free(node->value.opaque);
            }
            break;
        case XML_REAL :
            /* Nothing to do */
            break;
        case XML_TEXT :
            if (node->value.text.string)
            {
                free(node->value.text.string);
            }
            break;
        case XML_CUSTOM :
            if (node->value.custom.data &&
                    node->value.custom.destroy)
            {
                (*(node->value.custom.destroy))(node->value.custom.data);
            }
            break;
        default :
            break;
    }

    free(node);
}

XmlNode * xml_new_custom(XmlNode *parent, void *data, cb_xml_custom_destroy destroy)
{
    XmlNode *node;

    if ((node = xml_new(parent, XML_CUSTOM)) != NULL)
    {
        node->value.custom.data    = data;
        node->value.custom.destroy = destroy;
    }

    return (node);
}

XmlNode *  xml_new_element(XmlNode *parent, const char  *name)
{
    XmlNode *node;

    if (!name)
    {
        return (NULL);
    }

    if ((node = xml_new(parent, XML_ELEMENT)) != NULL)
    {
        node->value.element.name = strdup(name);
    }

    return (node);
}

XmlNode *xml_new_integer(XmlNode *parent, int  integer)
{
    XmlNode *node;

    if ((node = xml_new(parent, XML_INTEGER)) != NULL)
    {
        node->value.integer = integer;
    }

    return (node);
}

XmlNode * xml_new_opaque(XmlNode *parent, const char  *opaque)
{
    XmlNode *node;

    if (!opaque)
    {
        return (NULL);
    }

    if ((node = xml_new(parent, XML_OPAQUE)) != NULL)
    {
        node->value.opaque = strdup(opaque);
    }

    return (node);
}

XmlNode * xml_new_real(XmlNode *parent, double real)
{
    XmlNode *node;

    if ((node = xml_new(parent, XML_REAL)) != NULL)
    {
        node->value.real = real;
    }

    return (node);
}

XmlNode * xml_new_text(XmlNode *parent, int  whitespace, const char  *string)
{
    XmlNode *node;

    if (!string)
    {
        return (NULL);
    }

    if ((node = xml_new(parent, XML_TEXT)) != NULL)
    {
        node->value.text.whitespace = whitespace;
        node->value.text.string     = strdup(string);
    }

    return (node);
}

void xml_remove(XmlNode *node)
{
    if (!node || !node->parent)
    {
        return;
    }

    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        node->parent->child = node->next;
    }

    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        node->parent->lastChild = node->prev;
    }

    node->parent = NULL;
    node->prev   = NULL;
    node->next   = NULL;
}

int xml_release(XmlNode *node)
{
    if (node)
    {
        if ((-- node->refCount) <= 0)
        {
            xml_delete(node);
            return (0);
        }
        else
        {
            return (node->refCount);
        }
    }
    else
        return (-1);
}

static XmlNode *xml_new(XmlNode *parent, XmlType type)
{
    XmlNode *node;

    if ((node = calloc(1, sizeof(XmlNode))) == NULL)
    {
        return (NULL);
    }

    node->type      = type;
    node->refCount = 1;

    if (parent)
    {
        xml_add(parent, XML_ADD_AFTER, XML_ADD_TO_PARENT, node);
    }

    return (node);
}
