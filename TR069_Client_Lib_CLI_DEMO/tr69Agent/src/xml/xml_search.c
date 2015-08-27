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
File name: xml_search.c
Author: kiffen guan
Date: April 1th. 2009
************************************************************************/

#include "xml_public.h"

XmlNode * xml_walk_next(XmlNode *node, XmlNode *top, int  descend)
{
    if (!node)
    {
        return (NULL);
    }
    else if (node->child && descend)
    {
        return (node->child);
    }
    else if (node == top)
    {
        return (NULL);
    }
    else if (node->next)
    {
        return (node->next);
    }
    else if (node->parent && node->parent != top)
    {
        node = node->parent;

        while (!node->next)
        {
            if (node->parent == top || !node->parent)
            {
                return (NULL);
            }
            else
            {
                node = node->parent;
            }
        }

        return (node->next);
    }
    else
    {
        return (NULL);
    }
}

XmlNode * xml_walk_prev(XmlNode *node, XmlNode *top, int  descend)
{
    if (!node || node == top)
    {
        return (NULL);
    }
    else if (node->prev)
    {
        if (node->prev->lastChild && descend)
        {
            node = node->prev->lastChild;

            while (node->lastChild)
            {
                node = node->lastChild;
            }

            return (node);
        }
        else
        {
            return (node->prev);
        }
    }
    else if (node->parent != top)
    {
        return (node->parent);
    }
    else
    {
        return (NULL);
    }
}

