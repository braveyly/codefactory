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
File name: tr69_tree.h
Author: kiffen guan
Date: May 17th. 2009
************************************************************************/

#ifndef  _TR69_TREE_H_
#define _TR69_TREE_H_

#include <stdio.h>
#include "xml_public.h"
#include "tr69_tree_public.h"
#include "host_handler_public.h"
#include "tr69_agent.h"

#define NUMBER_OF_ENTRIES_STR "NumberOfEntries"

#define INVALID_VALUE -1
#define MAX_TR069_NAME_LEN 256
#define MAX_CONFIG_LINE_LEN 1024

#define IS_TEMPLATE(x) (NULL != strstr (x, ".0."))

#define endret(x) { ret = x; goto end; }

#define tr069_tree_lock() { if (!mutexReEnter) pal_mutex_lock (&tr069TreeMutex); }
#define tr069_tree_unlock() { if (!mutexReEnter) pal_mutex_unlock (&tr069TreeMutex); }

#define IS_LEAF(x) ((x == String) || (x == Int) || (x == Unsigned) || (x == Bool))

typedef enum
{
    CONFIGURATION = 1,
    TREE_STATUS = 2,
} Tr069Style;

/* the order of String, Int, Unsigned, Bool can't be changed, and String must be 1 according to Tr069TypeArg */
typedef enum
{
    Unknown = 0,
    Int = 1,
    Unsigned,
    Bool,
    String,
    Instance,
    Object,
    MultiObject,
    DefInstance, /* Only for debug */
} Tr069Type;

struct _Tr069Instances;

typedef struct _Tr069Node
{
    char name[MAX_TR069_NAME_LEN + 1];
    Tr069Type type;
    Tr069AccessType accessType;
    Tr069Notification notification;
    PAL_INT32_T needReboot;
    Tr069AccessList accessModule;
    PAL_INT32_T isChanged;
    Tr069Style style;
    PAL_INT32_T childNum; /* the num of nodes of next level */
    Tr069Value defaultValue;
    struct
    {
        PAL_INT32_T min;
        PAL_INT32_T max;
    } range; /* min-max means minValue-maxValue, or minLength-maxLength.*/

    union
    {
        struct _Tr069Node *subNode;     /* the node of next level seperator by '.' */
        struct _Tr069Instances *instances;  /* the template node of multi-node */
        Tr069Value value;
    } c;
} Tr069Node;

typedef struct _Tr069Instance
{
    Tr069Type type;
    PAL_INT32_T index;
    Tr069AccessType accessType;
    Tr069Notification notification;
    Tr069AccessList accessModule;
    PAL_INT32_T childNum;
    struct _Tr069Instance *next;
    struct _Tr069Instance *prev;
    Tr069Node *subNode;
} Tr069Instance;

typedef struct _Tr069Instances
{
    /* may be need expansion */
    Tr069Instance defaultInstance;/* the template instance of multi-node */
    PAL_INT32_T freeIndex; /* the next valid instance ID of multi-node */
    PAL_UINT32_T maxNumOfEntries; /* the max num of instances of multi-node will be supported */
    PAL_UINT32_T numOfEntries; /* the num of instance of multi-node */
} Tr069Instances;

typedef struct _Tr069NodeEntry
{
    char name[MAX_TR069_NAME_LEN + 1];
    Tr069Type type;
    void *node;
} Tr069NodeEntry;

/***********************************************************************
Function: walk_through_tr069
Description:  walk through the nodes of tree except the template node, and call the func for every node.
************************************************************************/
extern Result walk_through_tr069(char *topName, Tr069NodeEntry *begin, Result(*func)(char *, Tr069NodeEntry *));

/***********************************************************************
Function: walk_through_tr069_all
Description:  walk through the nodes of tree and call the func for every node.
************************************************************************/
extern Result walk_through_tr069_all(char *topName, Tr069NodeEntry *begin, Result(*func)(char *, Tr069NodeEntry *));

/***********************************************************************
Function: print_node
Description:  print the information of node of tree.
************************************************************************/
extern Result print_node(char *topName , Tr069NodeEntry *entry);

/***********************************************************************
Function: get_node_size
Description:  get the size of node of tree.
************************************************************************/
extern Result get_node_size(char *topName, Tr069NodeEntry *entry);
#endif /* _TR69_TREE_H_ */


