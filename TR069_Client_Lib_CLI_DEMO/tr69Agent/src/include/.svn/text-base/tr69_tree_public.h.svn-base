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
File name: tr69_tree_public.h
Author: kiffen guan
Date: May 17th. 2009
************************************************************************/

#ifndef  _TR69_TREE_PUBLIC_H_
#define _TR69_TREE_PUBLIC_H_

#include "pal.h"
#include "tr69_agent_public.h"

#define IS_VALID_MODULE(x) (ACS_MODULE <= x && x <= WEB_MODULE)
#define IS_VALID_NOTIFICATION(x) (NotificationNone <= x && x <= NotificationActive)
#define IS_VALID_ACCESSLIST(x) (ACS <= x && x <= ALL)
#define dump_string(dst, src)  do { \
    if (NULL == (dst = PAL_MALLOC (strlen (src) + 1)))  \
    {  \
        endret (MALLOC_FAILURE);  \
    }  \
    strcpy (dst, src);  \
} while(0)

typedef union
{
    char *string;
    PAL_UINT32_T uintv;
    PAL_INT32_T intv;
    PAL_INT32_T boolv;
} Tr069Value;

/* if you change the enum of Tr069TypeArg, please modify the Tr069Type at the same time */
typedef enum
{
    tInt = 1,
    tUnsigned,
    tBool,
    tString
} Tr069TypeArg;

/* Access Type, value of read, write, or read | write */
typedef enum
{
    Read =1,
    Write =2
} Tr069AccessType;

/* ALL means subscribe in tr069 protocol */
typedef enum
{
    ACS = 1,
    ALL = 2
} Tr069AccessList;

typedef struct _Tr069ValueArg
{
    Tr069Value value;
    Tr069TypeArg   type;
    struct _Tr069ValueArg *next;
} Tr069ValueArg;

typedef enum
{
    NotificationNone = 0,
    NotificationPassive = 1,
    NotificationActive = 2,
} Tr069Notification;

typedef struct _Tr069NameArg
{
    char name[MAX_TR069_NAME_LEN + 1];
    Tr069AccessType accessType;
    struct _Tr069NameArg *next;
} Tr069NameArg;

typedef struct _Tr069AttributeArg
{
    Tr069Notification notification;
    Tr069AccessList accessList;
    struct _Tr069AttributeArg *next;
} Tr069AttributeArg;

/*
  * Notice: If you want to modify the enum Tr069AccessModule, pleare review whether IS_VALID_MODULE()
  * need modified.
  */
typedef enum
{
    ACS_MODULE = 1,
    WEB_MODULE = 2,
} Tr069AccessModule;


//**************************************************************************
// Function Name: init_tr069_tree
// Description  : read the user template file, and build the tr069 node tree.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           configFile :  The complete dir name of template file .
//**************************************************************************
extern Result init_tr069_tree ( char *configFile );

//**************************************************************************
// Function Name: set_tr069_attribute
// Description  : set the attribute of the tr069 node.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           name :     tr069 node name according to the protocol of tr069.
//                           setNotification:    if 0, then don't set the notification, other then set the notification.
//                           notification:
//                           setAccessList:     if 0, then don't set the accessList, other then set the accessList.
//                           accessList:
//**************************************************************************
extern Result set_tr069_attribute ( char *name, PAL_INT32_T setNotification, Tr069Notification notification, PAL_INT32_T setAccessList, Tr069AccessList accessList );

//**************************************************************************
// Function Name: get_tr069_attribute
// Description  : get the attribute of the tr069 node.
// Returns       : according to the Result.
// Notice:        : caller must free the retName and retAttribute.
// Parameter :
//                  Input :
//                           name :     tr069 node name according to the protocol of tr069.
//                           retName :     tr069 node names and access types according to the protocol of tr069.
//                           retAttribute:        tr069 attributes of tr069.
//                  Output:
//                           notification:
//                           accessList:
//**************************************************************************
extern Result get_tr069_attribute ( char *name, OUT Tr069NameArg **retName, OUT Tr069AttributeArg ** retAttribute );

//**************************************************************************
// Function Name: set_tr069_value
// Description  : set the value of the tr069 node.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           name :     tr069 node name according to the protocol of tr069.
//                           module:              who visit the tr069-tree.
//                           value:
//**************************************************************************
extern Result set_tr069_value ( Tr069AccessModule module, char *name , Tr069ValueArg *value );

//**************************************************************************
// Function Name: get_tr069_value
// Description  : get the value of the tr069 node.
// Returns       : according to the Result;
// Notice:        : caller must free the retName.
// Parameter :
//                  Input :
//                           name :     tr069 node name according to the protocol of tr069.
//                           retName :     tr069 node names and access types according to the protocol of tr069.
//                           retValue:
//**************************************************************************
extern Result get_tr069_value ( char *name, OUT Tr069NameArg **retName, OUT Tr069ValueArg **retValue );


//**************************************************************************
// Function Name: get_tr069_name
// Description  : get the names and access types of the tr069 nodes from tr069 tree.
// Returns       : according to the Result;
// Notice         : the caller must free the retName.
// Parameter :
//                  Input :
//                           beginName :       which node do you want to get name and access type from.
//                  Output
//                           retName:           the names and access type of the nodes.
//                           nextLevel:          if 0 then get the name and access type from all the sub-node of beginName,
//                                                    other then get the name and access type from first level of sub-node of beginName
//**************************************************************************
extern Result get_tr069_name ( char *beginName, OUT Tr069NameArg **retName, PAL_INT32_T nextlevel );

//**************************************************************************
// Function Name: add_tr069_object
// Description  : add a instance of object to tr069-tree.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           name :          tr069 node name according to the protocol of tr069, if the name is end with the instance index, then
//                                                   use this index as the instance id of new instance, other then the tr069-tree will alloc a new index.
//                  Output
//                           index:               the index is return the instances id of new instance alloc by tr069-tree.
//**************************************************************************
extern Result add_tr069_object ( char *objName, OUT PAL_UINT32_T *index );

//**************************************************************************
// Function Name: delete_tr069_object
// Description  : delete a instance of object from tr069-tree.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           name :          tr069 node name according to the protocol of tr069.It must be end with a instance id
//**************************************************************************
extern Result delete_tr069_object ( char *objName );

//**************************************************************************
// Function Name: get_tr069_notification_node
// Description  : get all the nodes which is need to notified to ACS.
// Returns       : according to the Result;
// Notice         : the caller must free the retName and retValue.
// Parameter :
//                  Input :
//                  Output :
//                           retName :          the names and access type of the node which is need to notified to acs.
//                           retValue :
//**************************************************************************
extern Result get_tr069_notification_node ( OUT Tr069NameArg **retName, OUT Tr069ValueArg **retValue );

/***********************************************************************
Function: free_tr69_name_arg
Description: free the memory of Tr069NameArg
************************************************************************/
extern Result free_tr69_name_arg ( Tr069NameArg **nameP );

/***********************************************************************
Function: free_tr69_value_arg
Description: free the memory of Tr069ValueArg
************************************************************************/
extern Result free_tr69_value_arg ( Tr069ValueArg **valueP );

/***********************************************************************
Function: free_tr69_attribute_arg
Description: free the memory of Tr069AttributeArg
************************************************************************/
extern Result free_tr69_attribute_arg ( Tr069AttributeArg * * attributeP );

/***********************************************************************
Function: print_tr069_nodes
Description: print all the tr069 nodes of tree except the template node.
************************************************************************/
extern void print_tr069_nodes ( void );

/***********************************************************************
Function: get_tr069_tree_size
Description: get the size of tr069 tree in the unit of byte.
************************************************************************/
extern PAL_UINT32_T get_tr069_tree_size ( void );

/***********************************************************************
Function: load_instance_attributes
Description: load the attrbutes of instance from file to tr069-tree.
************************************************************************/
extern void load_instance_attributes (void);

#endif /* _TR69_TREE_PUBLIC_H_ */
