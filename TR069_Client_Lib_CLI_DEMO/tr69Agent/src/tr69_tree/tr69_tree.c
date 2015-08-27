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
File name: tr69_tree.c
Author: kiffen guan
Date: May 17th. 2009
************************************************************************/

#include "tr69_tree.h"

static Tr069NodeEntry rootNode;
static Tr069NodeEntry cache;   /* the cache record the father of node which is finded by search last time */
static Tr069ValueArg *valueList = NULL; /* the temp value , is only used in internal */
static Tr069NameArg *nameList = NULL;
static Tr069AttributeArg *attributeList = NULL;
static Tr069Notification gNotification;
static Tr069AccessList gAccessList;
static PAL_MUTEX_T    tr069TreeMutex;
static PAL_INT32_T mutexReEnter = 0; /* incidate the re-enter of mutex */
static PAL_UINT32_T tr069TreeSize;  /* record the tree size */
static FILE *instance_file;

char *Tr069TypeInfo[8] =
{
    "Unknown",
    "Int",
    "Unsigned",
    "Bool",
    "String",
    "Instance",
    "Object",
    "MultiObject",
};

char *Tr069NotificationInfo[4] =
{
    "none",
    "passive",
    "active",
    "always",
};

char *Tr069AccessInfo[4] =
{
    "",
    "readOnly",
    "writeOnly",
    "readWrite",
};

char *Tr069AccessListInfo[3] =
{
    "",
    "",
    "subscriber",
};

char *Tr069StyleInfo[3] =
{
    "",
    "configuration",
    "status",
};

/***********************************************************************
Function: is_multi_instance
Description:  check if the name of node is a sub-node of multi-instance.

Return: 0 - no 1 - yes
Input:  name
Output:
Others:
************************************************************************/
PAL_INT32_T is_multi_instance ( char *name )
{
    char nameTmp[MAX_TR069_NAME_LEN + 1];
    PAL_INT32_T instanceID;
    char *ptr1, *ptr2;

    if ( NULL == name )
    {
        return 0;
    }

    strcpy ( nameTmp, name );

    ptr1 = nameTmp;

    while ( NULL != ptr1 )
    {
        if ( NULL != ( ptr2 = strstr ( ptr1, "." ) ) )
        {
            *ptr2 = '\0';

            ptr2 ++;

            if ( string_to_num ( ptr1, &instanceID ) )
            {
                return 1;
            }
        }

        ptr1 = ptr2;
    }

    return 0;
}

/***********************************************************************
Function: walk_through_tr069_all
Description:  walk through the nodes of tree and call the func for every node.

Return: according to the result.
Input:  topName - the topName of the name of tr069 protocal of the node.
           begin - which node is walk through begin from.
           func - callback function which to deal with every node.
Output:
Others:
************************************************************************/
Result walk_through_tr069_all ( char *topName, Tr069NodeEntry *begin, Result ( *func ) ( char *, Tr069NodeEntry * ) )
{

    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry beginTmp;
    char topNameTmp[MAX_TR069_NAME_LEN + 1];

    memset ( &beginTmp, 0, sizeof ( Tr069NodeEntry ) );

    if ( NULL == begin )
    {
        memcpy ( &beginTmp, &rootNode, sizeof ( Tr069NodeEntry ) );
    }
    else
    {
        memcpy ( &beginTmp, begin, sizeof ( Tr069NodeEntry ) );
    }

    if ( NULL == topName )
    {
        strcpy ( topNameTmp, "" );
    }
    else
    {
        strcpy ( topNameTmp, topName );
    }

    /* deal the current node */
    if ( TCMAGENT_OK != ( retTmp = func ( topNameTmp, &beginTmp ) ) )
    {
        endret ( retTmp );
    }

    /* walk through the sub-nodes of current node */
    if ( Instance == beginTmp.type )
    {
        PAL_INT32_T i, num;
        Tr069Instance *node = ( Tr069Instance * ) beginTmp.node;
        Tr069Node *subNode = node->subNode;

        num = node->childNum;
        sprintf ( topNameTmp, "%s%d.", topNameTmp, node->index );

        for ( i = 0; i < num; i++ )
        {
            strcpy ( beginTmp.name, subNode[i].name );
            beginTmp.type = subNode[i].type;
            beginTmp.node = &subNode[i];

            if ( TCMAGENT_OK != ( retTmp = walk_through_tr069_all ( topNameTmp, &beginTmp, func ) ) )
            {
                endret ( retTmp );
            }
        }
    }
    else if ( Object == beginTmp.type )
    {
        PAL_INT32_T i, num;
        Tr069Node *node = ( Tr069Node * ) beginTmp.node;
        Tr069Node *subNode = node->c.subNode;

        num = node->childNum;
        strcat ( topNameTmp, node->name );
        strcat ( topNameTmp, "." );

        for ( i = 0; i < num; i++ )
        {
            strcpy ( beginTmp.name, subNode[i].name );
            beginTmp.type = subNode[i].type;
            beginTmp.node = &subNode[i];

            if ( TCMAGENT_OK != ( retTmp = walk_through_tr069_all ( topNameTmp, &beginTmp, func ) ) )
            {
                endret ( retTmp );
            }
        }
    }
    else if ( MultiObject == beginTmp.type )
    {
        Tr069Node *node = ( Tr069Node * ) beginTmp.node;
        Tr069Node *subDefNodes;
        Tr069Instances *firstSubNode = node->c.instances;
        Tr069Instance  *subNode = firstSubNode->defaultInstance.next;
        char topNameOfDefInstances[MAX_TR069_NAME_LEN + 1];
        PAL_INT32_T i, num;

        strcat ( topNameTmp, node->name );
        strcat ( topNameTmp, "." );

        beginTmp.node = firstSubNode;
        beginTmp.type = DefInstance;
        sprintf ( beginTmp.name, "0" );

        if ( TCMAGENT_OK != ( retTmp = func ( topNameTmp, &beginTmp ) ) )
        {
            endret ( retTmp );
        }

        subDefNodes = firstSubNode->defaultInstance.subNode;
        num = firstSubNode->defaultInstance.childNum;
        sprintf ( topNameOfDefInstances, "%s%d.", topNameTmp, 0 );

        for ( i = 0; i < num; i++ )
        {
            strcpy ( beginTmp.name, subDefNodes[i].name );
            beginTmp.type = subDefNodes[i].type;
            beginTmp.node = &subDefNodes[i];

            if ( TCMAGENT_OK != ( retTmp = walk_through_tr069_all ( topNameOfDefInstances, &beginTmp, func ) ) )
            {
                endret ( retTmp );
            }
        }

        while ( NULL != subNode )
        {
            sprintf ( beginTmp.name, "%d", subNode->index );
            beginTmp.type = subNode->type;
            beginTmp.node = subNode;

            if ( TCMAGENT_OK != ( retTmp = walk_through_tr069_all ( topNameTmp, &beginTmp, func ) ) )
            {
                endret ( retTmp );
            }
            subNode = subNode->next;
        }
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: walk_through_tr069_all
Description:  walk through the nodes of tree except the template node, and call the func for every node.

Return: according to the result.
Input:  topName - the topName of the name of tr069 protocal of the node.
           begin - which node is walk through begin from.
           func - callback function which to deal with every node.
Output:
Others:
************************************************************************/
Result walk_through_tr069 ( char *topName, Tr069NodeEntry *begin, Result ( *func ) ( char *, Tr069NodeEntry * ) )
{

    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry beginTmp;
    char topNameTmp[MAX_TR069_NAME_LEN + 1];

    memset ( &beginTmp, 0, sizeof ( Tr069NodeEntry ) );

    if ( NULL == begin )
    {
        memcpy ( &beginTmp, &rootNode, sizeof ( Tr069NodeEntry ) );
    }
    else
    {
        memcpy ( &beginTmp, begin, sizeof ( Tr069NodeEntry ) );
    }

    if ( NULL == topName )
    {
        strcpy ( topNameTmp, "" );
    }
    else
    {
        strcpy ( topNameTmp, topName );
    }

    /* deal the current node */
    if ( TCMAGENT_OK != ( retTmp = func ( topNameTmp, &beginTmp ) ) )
    {
        endret ( retTmp );
    }

    /* walk through the sub-nodes of current node */
    if ( Instance == beginTmp.type )
    {
        PAL_INT32_T i, num;
        Tr069Instance *node = ( Tr069Instance * ) beginTmp.node;
        Tr069Node *subNode = node->subNode;

        num = node->childNum;
        sprintf ( topNameTmp, "%s%d.", topNameTmp, node->index );

        for ( i = 0; i < num; i++ )
        {
            strcpy ( beginTmp.name, subNode[i].name );
            beginTmp.type = subNode[i].type;
            beginTmp.node = &subNode[i];

            if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( topNameTmp, &beginTmp, func ) ) )
            {
                endret ( retTmp );
            }
        }
    }
    else if ( Object == beginTmp.type )
    {
        PAL_INT32_T i, num;
        Tr069Node *node = ( Tr069Node * ) beginTmp.node;
        Tr069Node *subNode = node->c.subNode;

        num = node->childNum;
        strcat ( topNameTmp, node->name );
        strcat ( topNameTmp, "." );

        for ( i = 0; i < num; i++ )
        {
            strcpy ( beginTmp.name, subNode[i].name );
            beginTmp.type = subNode[i].type;
            beginTmp.node = &subNode[i];

            if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( topNameTmp, &beginTmp, func ) ) )
            {
                endret ( retTmp );
            }
        }
    }
    else if ( MultiObject == beginTmp.type )
    {
        Tr069Node *node = ( Tr069Node * ) beginTmp.node;
        Tr069Instances *firstSubNode = node->c.instances;
        Tr069Instance  *subNode = firstSubNode->defaultInstance.next;

        strcat ( topNameTmp, node->name );
        strcat ( topNameTmp, "." );

        while ( NULL != subNode )
        {
            sprintf ( beginTmp.name, "%d", subNode->index );
            beginTmp.type = subNode->type;
            beginTmp.node = subNode;

            if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( topNameTmp, &beginTmp, func ) ) )
            {
                endret ( retTmp );
            }
            subNode = subNode->next;
        }
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: build_xml_node
Description:  build the xml node from the information of tr069-tree node.

Return: according to the Result.
Input:  entry - tree node
Output: xml - xml node
Others:
************************************************************************/
static Result build_xml_node ( XmlNode *xml, Tr069NodeEntry *entry )
{
#define XML_ELEMENT_ATTRS_NUM(xml) (xml->value.element.num_attrs)
#define XML_ELEMENT_ATTRS(xml) (xml->value.element.attrs)
#define XML_ELEMENT_ATTRS_NAME(xml, i) (xml->value.element.attrs[i].name)
#define XML_ELEMENT_ATTRS_VALUE(xml, i) (xml->value.element.attrs[i].value)

    Result ret = TCMAGENT_OK;
    char tmp[256];

    if ( Object == entry->type || MultiObject == entry->type )
    {
        Tr069Node *node = ( Tr069Node * ) entry->node;

        if ( !IS_LEAF ( node->type ) )
        {
            XML_ELEMENT_ATTRS_NUM ( xml ) = 2;
            XML_ELEMENT_ATTRS ( xml ) = PAL_MALLOC ( sizeof ( XmlAttribute ) * 2 );
        }
        else
        {
            XML_ELEMENT_ATTRS_NUM ( xml ) = 8;
            XML_ELEMENT_ATTRS ( xml ) = PAL_MALLOC ( sizeof ( XmlAttribute ) * 8 );
        }

        /* type */
        dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 0 ), "type" );
        dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 0 ), Tr069TypeInfo[ ( PAL_INT32_T ) node->type] );

        if ( IS_LEAF ( node->type ) )
        {
            /* access type */
            dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 1 ), "access" );
            dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 1 ), Tr069AccessInfo[ ( PAL_INT32_T ) node->accessType] );

            /* notification */
            dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 2 ), "notification" );
            dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 2 ), Tr069NotificationInfo[ ( PAL_INT32_T ) node->notification] );

            /* need reboot */
            dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 3 ), "needReboot" );
            dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 3 ), ( node->needReboot ? "yes" : "no" ) );

            /* access list */
            dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 4 ), "accessList" );
            dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 4 ), Tr069AccessListInfo[ ( PAL_INT32_T ) node->accessModule] );

            /* style */
            dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 5 ), "style" );
            dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 5 ), Tr069StyleInfo[ ( PAL_INT32_T ) node->style] );

            /* range */
            if ( String == node->type )
            {
                dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 6 ), "lengthRange" );
            }
            else
            {
                dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 6 ), "valueRange" );
            }
            sprintf ( tmp, "%d-%d", node->range.min, node->range.max );
            dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 6 ), tmp );

            /* default value */
            dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 7 ), "defaultValue" );

            if ( String == node->type )
            {
                dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 7 ), node->defaultValue.string );
            }
            else
            {
                if ( Int == node->type )
                {
                    sprintf ( tmp, "%d", node->defaultValue.intv );
                }
                else
                {
                    sprintf ( tmp, "%u", node->defaultValue.uintv );
                }
                dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 7 ), tmp );
            }
        }
        else
        {
            /* nextLevelNum */
            sprintf ( tmp, "%d", node->childNum );
            dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 1 ), "nextLevelNum" );
            dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 1 ), tmp );
        }

    }
    else if ( Instance == entry->type )
    {
        Tr069Instances *instances = ( Tr069Instances * ) entry->node;

        XML_ELEMENT_ATTRS_NUM ( xml ) = 3;
        XML_ELEMENT_ATTRS ( xml ) = PAL_MALLOC ( sizeof ( XmlAttribute ) * 3 );

        /* type */
        dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 0 ), "type" );
        dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 0 ), Tr069TypeInfo[ ( PAL_INT32_T ) Instance] );

        /* nextLevelNum */
        sprintf ( tmp, "%d", instances->defaultInstance.childNum );
        dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 1 ), "nextLevelNum" );
        dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 1 ), tmp );

        /* maxNumOfEntries */
        sprintf ( tmp, "%d", instances->maxNumOfEntries );
        dump_string ( XML_ELEMENT_ATTRS_NAME ( xml, 2 ), "maxNumOfEntries" );
        dump_string ( XML_ELEMENT_ATTRS_VALUE ( xml, 2 ), tmp );
    }


#undef XML_ELEMENT_ATTRS_NUM
#undef XML_ELEMENT_ATTRS
#undef XML_ELEMENT_ATTRS_NAME
#undef XML_ELEMENT_ATTRS_VALUE

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: write_xml_file
Description:  build the xml-tree thourgh the information of tr069-tree.

Return: XmlNode - the root node of xml-tree.
Input:  father - the father xml node of current node.
           topName - the topName of the tr069 node name of current node.
           beginNodes - the node which is walk through from.
           nodesNum - the num of current nodes.
Output:
Others:
************************************************************************/
static XmlNode *write_xml_file ( XmlNode *father, char *topName, Tr069NodeEntry *beginNodes, PAL_INT32_T nodesNum )
{
    XmlNode *xml;
    char topNameTmp[MAX_TR069_NAME_LEN + 1];
    Tr069NodeEntry nodes;
    Tr069NodeEntry entry;

    if ( NULL == beginNodes )
    {
        memcpy ( &nodes, &rootNode, sizeof ( Tr069NodeEntry ) );
    }
    else
    {
        memcpy ( &nodes, beginNodes, sizeof ( Tr069NodeEntry ) );
    }

    if ( NULL == topName )
    {
        strcpy ( topNameTmp, "" );
    }
    else
    {
        strcpy ( topNameTmp, topName );
    }

    if ( Object == nodes.type )
    {
        PAL_INT32_T i, num;
        Tr069Node *node = ( Tr069Node * ) nodes.node;

        for ( i = 0; i < nodesNum; i++ )
        {
            PAL_INT32_T subNodeNum;
            char tmp[MAX_TR069_NAME_LEN + 1];

            strcpy ( tmp, topNameTmp );
            strcat ( tmp, node[i].name );

            if ( !IS_LEAF ( node[i].type ) )
                strcat ( tmp, "." );

            if ( NULL == ( xml = xml_new_element ( father, tmp ) ) )
            {
                return NULL;
            }

            if ( MultiObject == node[i].type )
            {
                entry.type = MultiObject;
            }
            else
            {
                entry.type = Object;
            }

            entry.node = &node[i];

            if ( TCMAGENT_OK != build_xml_node ( xml, &entry ) )
            {
                return NULL;
            }

            if ( !IS_LEAF ( node[i].type ) )
            {
                subNodeNum = node[i].childNum;
                if ( MultiObject == node[i].type )
                {
                    entry.type = Instance;
                    entry.node = node[i].c.instances;
                }
                else
                {
                    entry.type = Object;
                    entry.node = node[i].c.subNode;
                }

                if ( NULL == write_xml_file ( xml, tmp, &entry, subNodeNum ) )
                {
                    return NULL;
                }
            }
        }
    }
    else if ( Instance == nodes.type )
    {
        PAL_INT32_T subNodeNum;

        strcat ( topNameTmp, "0." );

        if ( NULL == ( xml = xml_new_element ( father, topNameTmp ) ) )
        {
            return NULL;
        }

        entry.type = Instance;
        entry.node = nodes.node;

        if ( TCMAGENT_OK != build_xml_node ( xml, &entry ) )
        {
            return NULL;
        }

        entry.type = Object;
        entry.node = ( ( Tr069Instances * ) nodes.node )->defaultInstance.subNode;
        subNodeNum = ( ( Tr069Instances * ) nodes.node )->defaultInstance.childNum;

        if ( NULL == write_xml_file ( xml, topNameTmp, &entry, subNodeNum ) )
        {
            return NULL;
        }
    }

    return xml;
}

const char * tr069_whitespace_cb ( XmlNode *node, int where )
{
    const char *name;
    static char format[128];

    strcpy ( format, "\n" );

    name = node->value.element.name;

    if ( where == XML_WS_BEFORE_OPEN || where == XML_WS_AFTER_CLOSE )
    {
        static PAL_INT32_T begin_line = 1;

        if ( begin_line )
        {
            begin_line = 0;
            return ( NULL );
        }
        else
        {
            char *tmp = name;

            tmp = strchr ( tmp, '.' );

            if ( NULL != tmp )
            {
                tmp ++;

                while ( NULL != ( tmp = strchr ( tmp, '.' ) ) )
                {
                    tmp ++;
                    strcat ( format, "\t" );
                }

                if ( '.' != name[strlen ( name ) -1] )
                {
                    strcat ( format, "\t" );
                }
            }

            if ( format[0] != '\0' )
            {
                return format;
            }
            else
                return ( NULL );
        }
    }
    else
    {
        char *tmp = name;

        tmp = strchr ( tmp, '.' );

        if ( NULL != tmp )
        {
            tmp ++;

            while ( NULL != ( tmp = strchr ( tmp, '.' ) ) )
            {
                tmp ++;
                strcat ( format, "\t" );
            }

            if ( '.' != name[strlen ( name ) -1] )
            {
                strcat ( format, "\t" );
            }
        }

        if ( format[0] != '\0' )
        {
            return format;
        }
        else
            return ( NULL );
    }

    return ( NULL );
}

/***********************************************************************
Function: write config file
Description:  build the xml-tree through the information of tr69-tree, and write to the configuration file.

Return: according to the Result.
Input:
Output:
Others:
************************************************************************/
static Result write_config_file ( void )
{
    Result ret = TCMAGENT_OK;
    XmlNode *xml;
    FILE *file;

    if ( NULL == ( file = fopen ( PAL_CONFIG_FILE, "w" ) ) )
    {
        endret ( CONFIG_FILE_NOT_FOUND );
    }

    xml_set_wrap_margin ( MAX_CONFIG_LINE_LEN );

    /* build the xml tree through the tr69-tree */
    if ( NULL == ( xml = write_xml_file ( XML_NO_PARENT, NULL, NULL, 1 ) ) )
    {
        endret ( CONFIG_FILE_WRITE_ERROR );
    }

    /* write the xml-tree to configuration file */
    if ( xml_save_file ( xml, file, tr069_whitespace_cb ) )
    {
        endret ( CONFIG_FILE_WRITE_ERROR );
    }

end:

    if ( NULL != file )
    {
        fclose ( file );
    }

    if ( NULL != xml )
    {
        /* free the xml-tree */
        xml_delete ( xml );
    }

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: init_tr069_node
Description:  build the tr069 node through the information of xml node of configuration file.

Return: according to the Result.
Input:  nodeName - node name
           attrNum -the num of attributes of xml-node.
           attr - the array of attributes of xml.
Output: entry - tr69-tree node.
Others:
************************************************************************/
static Result init_tr069_node ( char *nodeName, PAL_INT32_T attrNum, XmlAttribute *attr, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    PAL_INT32_T index;
    char name[MAX_TR069_NAME_LEN + 1];
    /* the initial value is the default value of attribute of tr69-node */
    Tr069Type type = Unknown;
    Tr069AccessType accessType = Write | Read;
    Tr069Notification notification = NotificationNone;
    PAL_INT32_T needReboot = 0;
    Tr069AccessList accessModule = ALL;
    Tr069Style style = CONFIGURATION;
    PAL_INT32_T lengthRange = 0;
    PAL_INT32_T valueRange = 0;
    PAL_INT32_T min = 0;
    PAL_INT32_T max = 0;
    PAL_INT32_T nextLevelNum = 0;
    PAL_UINT32_T maxNumOfEntries;
    void *defaultValue = NULL;

    if ( NULL == name || strlen ( name ) > MAX_TR069_NAME_LEN )
    {
        endret ( NODE_NAME_ERROR );
    }

    strcpy ( name, nodeName );

    for ( index = 0; index < attrNum; index++ )
    {
        if ( 0 == strcmp ( attr->name, "type" ) )
        {
            if ( 0 == strcmp ( attr->value, "String" ) )
            {
                type = String;
            }
            else if ( 0 == strcmp ( attr->value, "Int" ) )
            {
                type = Int;
            }
            else if ( 0 == strcmp ( attr->value, "Unsigned" ) )
            {
                type = Unsigned;
            }
            else if ( 0 == strcmp ( attr->value, "Bool" ) )
            {
                type = Bool;
            }
            else if ( 0 == strcmp ( attr->value, "Instance" ) )
            {
                type = Instance;
            }
            else if ( 0 == strcmp ( attr->value, "Object" ) )
            {
                type = Object;
            }
            else if ( 0 == strcmp ( attr->value, "MultiObject" ) )
            {
                type = MultiObject;
            }
            else
            {
                endret ( ATTRIBUTE_TYPE_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "access" ) )
        {
            accessType = 0;

            if ( 0 == strcmp ( attr->value, "readOnly" ) )
            {
                accessType |= Read;
            }
            else if ( 0 == strcmp ( attr->value, "writeOnly" ) )
            {
                accessType |= Write;
            }
            else if ( 0 == strcmp ( attr->value, "readWrite" ) )
            {
                accessType |= ( Read | Write );
            }
            else
            {
                endret ( ATTRIBUTE_ACCESS_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "notification" ) )
        {

            if ( 0 == strcmp ( attr->value, "none" ) )
            {
                notification = NotificationNone;
            }
            else if ( 0 == strcmp ( attr->value, "passive" ) )
            {
                notification = NotificationPassive;
            }
            else if ( 0 == strcmp ( attr->value, "active" ) )
            {
                notification = NotificationActive;
            }
            else
            {
                endret ( ATTRIBUTE_NOTIFICATION_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "needReboot" ) )
        {

            if ( 0 == strcmp ( attr->value, "yes" ) )
            {
                needReboot = 1;
            }
            else if ( 0 == strcmp ( attr->value, "no" ) )
            {
                needReboot = 0;
            }
            else
            {
                endret ( ATTRIBUTE_NEEDREBOOT_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "nextLevelNum" ) )
        {

            if ( !string_to_num ( attr->value, &nextLevelNum ) || nextLevelNum < 0 )
            {
                endret ( ATTRIBUTE_NEXTLEVELNUM_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "valueRange" ) || 0 == strcmp ( attr->name, "lengthRange" ) )
        {
            char *ptr1, *ptr2, *ptr3;

            ptr1 = strchr ( attr->value, '-' );

            if ( NULL != ptr1 )
            {
                ptr1++;

                ptr2 = strchr ( ptr1, '-' );

                if ( NULL != ptr2 )
                {
                    ptr2++;

                    ptr3 = strchr ( ptr2, '-' );

                    if ( NULL != ptr3 || '-' == attr->value[0] )
                    {
                        * ( ptr2 - 1 ) = '\0';
                        ptr1 = ptr2;
                    }
                    else
                    {
                        * ( ptr1 - 1 ) = '\0';
                    }
                }
                else
                {
                    * ( ptr1 - 1 ) = '\0';
                }
            }
            else
            {
                endret ( ATTRIBUTE_RANGE_ERROR );
            }

            if ( !string_to_num ( attr->value, &min ) )
            {
                endret ( ATTRIBUTE_RANGE_ERROR );
            }

            if ( !string_to_num ( ptr1, &max ) )
            {
                endret ( ATTRIBUTE_RANGE_ERROR );
            }

            if ( min > max )
            {
                endret ( ATTRIBUTE_RANGE_ERROR );
            }

            if ( 0 == strcmp ( attr->name, "lengthRange" ) )
            {
                lengthRange = 1;

                if ( min < 0 || max < 0 )
                {
                    endret ( ATTRIBUTE_RANGE_ERROR );
                }
            }
            else
            {
                valueRange = 1;
            }
        }
        else if ( 0 == strcmp ( attr->name, "style" ) )
        {

            if ( 0 == strcmp ( attr->value, "configuration" ) )
            {
                style = CONFIGURATION;
            }
            else if ( 0 == strcmp ( attr->value, "status" ) )
            {
                style = TREE_STATUS;
            }
            else
            {
                endret ( ATTRIBUTE_STYLE_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "accessList" ) )
        {

            if ( 0 == strcmp ( attr->value, "" ) )
            {
                accessModule = ACS;
            }
            else if ( 0 == strcmp ( attr->value, "subscriber" ) )
            {
                accessModule = ALL;
            }
            else
            {
                endret ( ATTRIBUTE_ACCESSLIST_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "maxNumOfEntries" ) )
        {

            if ( !string_to_num ( attr->value, ( PAL_INT32_T * ) &maxNumOfEntries ) || maxNumOfEntries < 0 )
            {
                endret ( ATTRIBUTE_MAXNUMOFENTRIES_ERROR );
            }
        }
        else if ( 0 == strcmp ( attr->name, "defaultValue" ) )
        {
            defaultValue = attr->value;
        }
        else
        {
            endret ( ATTRIBUTE_NAME_ERROR );
        }

        attr++;
    }

    switch ( type )
    {

        case Instance:
            if ( maxNumOfEntries < 0 )
            {
                endret ( ATTRIBUTE_MAXNUMOFENTRIES_ERROR );
            }

        case MultiObject:
        case Object:
            if ( nextLevelNum <= 0 )
            {
                endret ( ATTRIBUTE_NEXTLEVELNUM_ERROR );
            }
            break;

        case String:
            if ( !lengthRange )
            {
                endret ( ATTRIBUTE_RANGE_ERROR );
            }
            if ( NULL != defaultValue )
            {
                if ( strlen ( defaultValue ) < min || strlen ( defaultValue ) > max )
                {
                    endret ( ATTRIBUTE_DEFAULTVALLUE_ERROR );
                }
            }
            break;

        case Int:
        case Unsigned:
        {
            PAL_INT32_T value;

            if ( !valueRange )
            {
                endret ( ATTRIBUTE_RANGE_ERROR );
            }

            if ( NULL != defaultValue )
            {
                if ( !string_to_num ( defaultValue, &value ) )
                {
                    endret ( ATTRIBUTE_DEFAULTVALLUE_ERROR );
                }

                if ( value < min || value > max )
                {
                    endret ( ATTRIBUTE_DEFAULTVALLUE_ERROR );
                }
            }
            break;
        }

        case Bool:
            break;

        default:
            endret ( ATTRIBUTE_TYPE_ERROR );
    }

    if ( Instance == type )
    {
        Tr069Instances *tmpNode = ( Tr069Instances * ) PAL_MALLOC ( sizeof ( Tr069Instances ) );
        memset ( tmpNode, 0, sizeof ( Tr069Instances ) );

        tmpNode->freeIndex = 1;
        tmpNode->maxNumOfEntries = maxNumOfEntries;
        tmpNode->defaultInstance.type = type;
        tmpNode->defaultInstance.index = 0;
        tmpNode->defaultInstance.accessType = accessType;
        tmpNode->defaultInstance.notification = notification;
        tmpNode->defaultInstance.childNum = nextLevelNum;
        tmpNode->defaultInstance.accessModule = accessModule;
        tmpNode->defaultInstance.next = NULL;
        tmpNode->defaultInstance.prev = NULL;
        tmpNode->defaultInstance.subNode = ( Tr069Node * ) PAL_MALLOC ( sizeof ( Tr069Node ) * tmpNode->defaultInstance.childNum );

        memset ( tmpNode->defaultInstance.subNode, 0, sizeof ( Tr069Node ) *tmpNode->defaultInstance.childNum );

        ( *entry ).node = tmpNode;

    }
    else
    {
        char *ptr;
        char tmp[MAX_TR069_NAME_LEN + 1];
        Tr069Value value;
        Tr069Node *tmpNode = ( Tr069Node * ) PAL_MALLOC ( sizeof ( Tr069Node ) );

        memset ( tmpNode, 0, sizeof ( Tr069Node ) );

        strcpy ( tmp, name );

        if ( '.' == tmp[strlen ( tmp ) - 1] )
        {
            tmp[strlen ( tmp ) - 1] = '\0';
        }

        if ( NULL == ( ptr = strrchr ( tmp, '.' ) ) )
        {
            strcpy ( tmpNode->name, tmp );
        }
        else
        {
            strcpy ( tmpNode->name, ptr + 1 );
        }

        tmpNode->type = type;

        if ( IS_TEMPLATE ( nodeName ) )
        {
            tmpNode->accessModule = ALL;
            tmpNode->notification = NotificationNone;
        }
        else
        {
            tmpNode->accessModule = accessModule;
            tmpNode->notification = notification;
        }

        tmpNode->accessType = accessType;
        tmpNode->needReboot = needReboot;
        tmpNode->isChanged = 0;
        tmpNode->style = style;
        tmpNode->range.min = min;
        tmpNode->range.max = max;
        tmpNode->childNum = nextLevelNum;

        if ( NULL != defaultValue && Object != type && MultiObject != type )
        {
            switch ( type )
            {

                case String:
                    value.string = PAL_MALLOC ( strlen ( defaultValue ) + 1 );
                    strcpy ( value.string, defaultValue );
                    break;

                case Bool:
                    value.boolv = ( 0 == atoi ( defaultValue ) ? 0 : 1 );
                    break;

                case Int:
                    string_to_num ( defaultValue, &value.intv );
                    break;

                case Unsigned:
                    string_to_num ( defaultValue, ( PAL_INT32_T * ) &value.uintv );
                    break;

                default: /* it's impossible here, only cut warning */
                    break;
            }

            memcpy ( &tmpNode->c.value, &value, sizeof ( Tr069Value ) );
            if ( String == type )
            {
                value.string = PAL_MALLOC ( strlen ( defaultValue ) + 1 );
                strcpy ( value.string, defaultValue );
            }
            memcpy ( &tmpNode->defaultValue, &value, sizeof ( Tr069Value ) );
        }
        else if ( Object == type )
        {
            tmpNode->c.subNode = ( Tr069Node * ) PAL_MALLOC ( sizeof ( Tr069Node ) * tmpNode->childNum );
            memset ( tmpNode->c.subNode, 0, sizeof ( Tr069Node ) * tmpNode->childNum );
        }
        else if ( MultiObject == type )
        {
            tmpNode->c.instances = ( Tr069Instances * ) PAL_MALLOC ( sizeof ( Tr069Instances ) );
            memset ( tmpNode->c.instances, 0, sizeof ( Tr069Instances ) );
            tmpNode->c.instances->defaultInstance.index = -1;
        }


        ( *entry ).node = tmpNode;
    }

    ( *entry ).type = type;
    strcpy ( ( *entry ).name, name );

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " node name: %s error: %d\n", name, ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: get_tr069_father_name
Description:  get the father name of the name according to the tr069 procotol name.

Return: according to the Result.
Input:   name - the name of node.
Output: fatherName - the name of father node.
Others:
************************************************************************/
static Result get_tr069_father_name ( char *name, OUT char *fatherName )
{
    Result ret = TCMAGENT_OK;
    char nameTmp[MAX_TR069_NAME_LEN + 1];
    char *tmp;

    if ( NULL == name || NULL == fatherName || '.' == name[0] )
    {
        endret ( NODE_NAME_ERROR );
    }

    strcpy ( nameTmp, name );

    /* if the name is end with '.', cut the '.' */
    if ( '.' == nameTmp[strlen ( nameTmp ) -1] )
    {
        nameTmp[strlen ( nameTmp ) - 1] = '\0';
    }

    if ( NULL != ( tmp = strrchr ( nameTmp, '.' ) ) )
    {
        * ( tmp + 1 ) = '\0';
    }
    else
    {
        nameTmp[0] = '\0';
    }

    strcpy ( fatherName, nameTmp );

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );

}

/***********************************************************************
Function: get_tr069_node_by_name
Description:  search the tr69-tree node by name.

Return: according to the Result.
Input:   name - the name of node.
Output: entry - the searched node.
Others:
************************************************************************/
static Result get_tr069_node_by_name ( char *name, OUT Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    char tmpName[MAX_TR069_NAME_LEN + 1];
    char *ptr1, *ptr2;
    Tr069NodeEntry dest, next, cacheTmp;
    PAL_INT32_T num = 1;

    if ( NULL == name || 0 == strlen ( name ) || NULL == entry )
    {
        endret ( NODE_NOT_FOUND );
    }

    memset ( &dest, 0, sizeof ( Tr069NodeEntry ) );
    memset ( &next, 0, sizeof ( Tr069NodeEntry ) );
    memset ( &cacheTmp, 0, sizeof ( Tr069NodeEntry ) );

    memcpy ( &next, &rootNode, sizeof ( Tr069NodeEntry ) );

    strcpy ( tmpName, name );
    ptr1 = ptr2 = tmpName;

    /* if the name is end with '.', cut the '.' */
    if ( '.' == tmpName[strlen ( tmpName ) - 1] )
    {
        tmpName[strlen ( tmpName ) - 1] = '\0';
    }

    /* if cache is valid, find in it first, else find from the root node */
    if ( 0 != strlen ( cache.name ) )
    {
        if ( 0 == strncmp ( cache.name, tmpName, strlen ( cache.name ) ) )
        {
            memcpy ( &cacheTmp, &cache, sizeof ( Tr069NodeEntry ) );

            if ( strlen ( cache.name ) == strlen ( tmpName ) )
            {
                memcpy ( entry, &cache, sizeof ( Tr069NodeEntry ) );
                endret ( TCMAGENT_OK );
            }

            if ( IS_LEAF ( cache.type ) )
            {
                endret ( CACHE_TYPE_ERROR );
            }

            if ( MultiObject == cache.type )
            {
                next.node = & ( ( ( Tr069Node * ) cache.node )->c.instances->defaultInstance );
                next.type = Instance;

            }
            else if ( Object == cache.type )
            {
                next.node = ( ( Tr069Node * ) cache.node )->c.subNode;
                num = ( ( Tr069Node * ) cache.node )->childNum;
                next.type = Object;
            }
            else if ( Instance == cache.type )
            {
                next.node = ( ( Tr069Instance * ) cache.node )->subNode;
                num = ( ( Tr069Instance * ) cache.node )->childNum;
                next.type = Object;
            }
            else
            {
                endret ( CACHE_TYPE_ERROR );
            }

            tmpName[strlen ( cache.name ) - 1] = '\0';
            ptr1 = ptr2 = tmpName + strlen ( cache.name );
        }
    }
    else
    {
        memcpy ( &cacheTmp, &rootNode, sizeof ( Tr069NodeEntry ) );
    }

    do
    {
        ptr2 = strchr ( ptr1, '.' );

        if ( NULL == next.node )
        {
            endret ( NODE_NOT_FOUND );
        }

        if ( NULL == ptr2 )
        {
            strcpy ( tmpName, ptr1 );

            if ( NULL != dest.node )
            {
                cacheTmp.node = dest.node;
                cacheTmp.type = dest.type;
            }

        }
        else
        {
            *ptr2 = '\0';
            ptr2++;

            strcpy ( tmpName, ptr1 );
            strcat ( cacheTmp.name, tmpName );
            strcat ( cacheTmp.name, "." );
        }

        if ( Object == next.type )
        {
            PAL_INT32_T index;
            PAL_INT32_T begin = 0;
            PAL_INT32_T end = num;
            Tr069Node *tmp = ( Tr069Node * ) next.node;

            /* seach the name in the way of bisearch */
            if ( strcmp ( tmpName, tmp[ ( num+1 ) /2 - 1].name ) <= 0 )
            {
                end = ( num + 1 ) / 2;
            }
            else
            {
                begin = ( num + 1 ) / 2 - 1;
            }

            for ( index = begin; index < end; index++ )
            {
                if ( 0 == strcmp ( tmpName, tmp[index].name ) )
                {
                    dest.node = &tmp[index];
                    dest.type = tmp[index].type;

                    if ( MultiObject == dest.type )
                    {
                        next.node = & ( ( ( Tr069Node * ) dest.node )->c.instances->defaultInstance );
                        next.type = Instance;

                    }
                    else if ( Object == dest.type )
                    {
                        next.node = ( ( Tr069Node * ) dest.node )->c.subNode;

                        next.type = Object;
                        num = ( ( Tr069Node * ) dest.node )->childNum;

                    }
                    else
                    {
                        next.node = NULL;
                    }
                    break;
                }
            }

            if ( index == end )
            {
                endret ( NODE_NOT_FOUND );
            }
        }
        else if ( Instance == next.type )
        {
            PAL_INT32_T index;
            Tr069Instance *tmp = ( Tr069Instance * ) next.node;

            if ( !string_to_num ( tmpName, &index ) )
            {
                endret ( NODE_NOT_FOUND );
            }

            while ( NULL != tmp )
            {
                if ( tmp->index == index )
                {
                    dest.node = tmp;
                    dest.type = Instance;

                    next.node = tmp->subNode;
                    next.type = Object;
                    num = tmp->childNum;

                    break;
                }

                tmp = tmp->next;
            }

            if ( NULL == tmp )
            {
                endret ( NODE_NOT_FOUND );
            }
        }

        ptr1 = ptr2;
    }
    while ( NULL != ptr1 );

    entry->node = dest.node;
    entry->type = dest.type;

end:

    if ( TCMAGENT_OK == ret )
    {
        /* if search success, update the cache */
        entry->node = dest.node;
        entry->type = dest.type;
        memcpy ( &cache, &cacheTmp, sizeof ( Tr069NodeEntry ) );
    }
    else
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "node name: %s error: %d\n", name, ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: add_tr069_node_internal
Description:  add the node to tr69-tree when initial.

Return: according to the Result.
Input:   entry - the inserted node entry.
            fatherEntry - the father node entry of inserted node.
Output:
Others:
************************************************************************/
static Result add_tr069_node_internal ( Tr069NodeEntry *entry, Tr069NodeEntry *fatherEntry )
{
    Result ret = TCMAGENT_OK;
    PAL_INT32_T num, i, j, destIndex = 0, haveFree = 0;

    if ( NULL == entry || NULL == fatherEntry || NULL == entry->node || NULL == fatherEntry->node )
    {
        endret ( NODE_NOT_FOUND );
    }

    if ( MultiObject == fatherEntry->type )
    {
        Tr069Node *father = ( Tr069Node * ) fatherEntry->node;
        Tr069Instances *node;

        if ( Instance != entry->type )
        {
            endret ( NODE_TYPE_ERROR );
        }
        node = ( Tr069Instances * ) entry->node;
        memcpy ( father->c.instances, node, sizeof ( Tr069Instances ) );

    }
    else if ( Object == fatherEntry->type || Instance == fatherEntry->type )
    {
        Tr069Node *subNode;
        Tr069Node *node;

        if ( Instance == entry->type )
        {
            endret ( NODE_TYPE_ERROR );
        }

        node = ( Tr069Node * ) entry->node;

        if ( Object == fatherEntry->type )
        {
            Tr069Node *father = ( Tr069Node * ) fatherEntry->node;
            subNode = father->c.subNode;
            num = father->childNum;
        }
        else
        {
            Tr069Instances *father = ( Tr069Instances * ) fatherEntry->node;
            subNode = father->defaultInstance.subNode;
            num = father->defaultInstance.childNum;
        }

        if ( 0 != strlen ( subNode[0].name ) )
        {
            endret ( NODE_NUM_ERROR );
        }

        for ( i = 0; i < num; i++ )
        {
            if ( 0 == strlen ( subNode[i].name ) )
            {
                haveFree = 1;
            }

            if ( i == num - 1 && !haveFree )
            {
                endret ( NODE_NUM_ERROR );
            }

            if ( strcmp ( node->name, subNode[i].name ) < 0 )
            {
                destIndex = i - 1;
                break;
            }
        }

        if ( i == num )
        {
            destIndex = num - 1;
        }

        for ( j = 1; j < i; j++ )
        {
            memcpy ( &subNode[j-1], &subNode[j], sizeof ( Tr069Node ) );
        }
        memcpy ( &subNode[destIndex], node, sizeof ( Tr069Node ) );

    }
    else
    {
        endret ( NODE_TYPE_ERROR );
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: add_tr069_node
Description:  build tr69-tree node through the information of xml-node, and add it to tr69-tree.

Return: according to the Result.
Input:   name - node name
            attrNum - the num of attributes of xml node.
            attr - the attributes of xml node.
Output:
Others:
************************************************************************/
static Result add_tr069_node ( char *name, PAL_INT32_T attrNum, XmlAttribute *attr )
{
    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry entry;
    char fatherName[MAX_TR069_NAME_LEN + 1];
    Tr069NodeEntry fatherEntry;

    /* build tr69-tree node through the xml node */
    if ( TCMAGENT_OK != ( retTmp = init_tr069_node ( name, attrNum, attr, &entry ) ) )
    {
        endret ( retTmp );
    }

    if ( TCMAGENT_OK != ( retTmp = get_tr069_father_name ( entry.name, fatherName ) ) )
    {
        endret ( NODE_NAME_ERROR );
    }

    if ( 0 == strlen ( fatherName ) ) /* root node */
    {
        if ( NULL != rootNode.node )
        {
            endret ( NODE_NAME_ERROR );
        }

        memcpy ( &rootNode, &entry, sizeof ( Tr069NodeEntry ) );
        endret ( TCMAGENT_OK );
    }

    /* search the father node */
    if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( fatherName, &fatherEntry ) ) )
    {
        endret ( retTmp );
    }

    /* add the node to the sub-nodes of father node */
    if ( TCMAGENT_OK != ( retTmp = add_tr069_node_internal ( &entry, &fatherEntry ) ) )
    {
        endret ( retTmp );
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " name: %s error: %d\n", name, ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: get_value
Description:  get the value of node of entry.

Return: according to the Result.
Input:   entry - the node entry.
Output: value - the value.
Others:
************************************************************************/
static Result get_value ( Tr069NodeEntry *entry, Tr069ValueArg *value )
{
    Result ret = TCMAGENT_OK;
    Tr069Node *node;

    if ( NULL == entry || NULL == value || NULL == entry->node )
    {
        endret ( NODE_VALUE_ERROR );
    }

    if ( !IS_LEAF ( entry->type ) )
    {
        endret ( NODE_TYPE_ERROR );
    }

    node = ( Tr069Node * ) entry->node;

    value->type = ( Tr069TypeArg ) node->type;

    switch ( node->type )
    {

        case Int:
            value->value.intv = node->c.value.intv;
            break;

        case Unsigned:
            value->value.uintv = node->c.value.uintv;
            break;

        case Bool:
            value->value.boolv = node->c.value.boolv;
            break;

        case String:
            value->value.string = PAL_MALLOC ( strlen ( node->c.value.string ) + 1 );
            strcpy ( value->value.string , node->c.value.string );
            break;

        default: /* it's impossible here, only cut warning */
            break;
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );

}

/***********************************************************************
Function: set_value
Description:  set the value of node of entry.

Return: according to the Result.
Input:   entry - the node entry.
            value -
Output:
Others:
************************************************************************/
static Result set_value ( Tr069NodeEntry *entry, Tr069ValueArg *valueArg )
{
    Result ret = TCMAGENT_OK;
    Tr069Node *node;
    Tr069ValueArg value;

    if ( NULL == entry || NULL == valueArg || NULL == entry->node )
    {
        endret ( NODE_VALUE_ERROR );
    }

    memcpy ( &value, valueArg, sizeof ( Tr069ValueArg ) );

    if ( !IS_LEAF ( entry->type ) )
    {
        endret ( NODE_TYPE_ERROR );
    }

    node = ( Tr069Node * ) entry->node;

    /*  for compatible with ACS server, comment the type error. */
    if ( ( PAL_INT32_T ) valueArg->type != ( PAL_INT32_T ) entry->type )
    {
        if ( tString == valueArg->type )
        {
            value.type = entry->type;

            if ( 0 == strcasecmp ( valueArg->value.string, "true" ) )
            {
                value.value.intv = 1;
            }
            else if ( 0 == strcasecmp ( valueArg->value.string, "false" ) )
            {
                value.value.intv = 0;
            }
            else if ( !string_to_num ( valueArg->value.string, &value.value.intv ) )
            {
                endret ( NODE_VALUE_ERROR );
            }
        }
        else if ( tString == entry->type )
        {
            value.type = entry->type;
            value.value.string = PAL_MALLOC ( MAX_NUM_LEN );
            sprintf ( value.value.string, "%d", valueArg->value.intv );
        }
        else
        {
            value.type = entry->type;
        }
    }

    switch ( node->type )
    {

        case Int:
            if ( value.value.intv < node->range.min || value.value.intv > node->range.max )
            {
                endret ( NODE_VALUE_ERROR );
            }

            node->c.value.intv = value.value.intv;
            break;

        case Unsigned:
            if ( value.value.uintv < node->range.min || value.value.uintv > node->range.max )
            {
                endret ( NODE_VALUE_ERROR );
            }

            node->c.value.uintv = value.value.uintv;
            break;

        case Bool:
            if ( value.value.boolv < node->range.min || value.value.boolv > node->range.max )
            {
                endret ( NODE_VALUE_ERROR );
            }
            node->c.value.boolv = value.value.boolv;
            break;

        case String:
            if ( ( strlen ( value.value.string ) ) < node->range.min || ( strlen ( value.value.string ) ) > node->range.max )
            {
                endret ( NODE_VALUE_ERROR );
            }

            if ( NULL != node->c.value.string )
            {
                if ( strlen ( value.value.string ) > strlen ( node->c.value.string ) )
                {
                    PAL_FREE ( node->c.value.string );
                    node->c.value.string = PAL_MALLOC ( strlen ( value.value.string ) + 1 );
                }
            }
            else
            {
                node->c.value.string = PAL_MALLOC ( strlen ( value.value.string ) + 1 );
            }

            strcpy ( node->c.value.string, value.value.string );
            break;

        default: /* it's impossible here, only cut warning */
            break;
    }

end:
    if ( ( PAL_INT32_T ) valueArg->type != ( PAL_INT32_T ) entry->type )
    {
        if ( tString == entry->type )
        {
            if ( NULL != value.value.string )
            {
                PAL_FREE ( value.value.string );
            }
        }
    }


    return ( ret );

}

/***********************************************************************
Function: get_name
Description:  get the name and readable attribute of node of entry.

Return: according to the Result.
Input:   topName -
            entry -
Others:
************************************************************************/
static Result get_name ( char *topName, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    Tr069NameArg *nameArg;

    if ( NULL == ( nameArg = PAL_MALLOC ( sizeof ( Tr069NameArg ) ) ) )
    {
        endret ( MALLOC_FAILURE );
    }

    memset ( nameArg, 0, sizeof ( Tr069NodeEntry ) );

    if ( NULL == topName )
    {
        strcpy ( nameArg->name, "" );
    }
    else
    {
        strcpy ( nameArg->name, topName );
    }

    if ( Instance  == entry->type )
    {
        Tr069Instance *instance = ( Tr069Instance * ) entry->node;

        sprintf ( nameArg->name, "%s%d", nameArg->name, instance->index );
        strcat ( nameArg->name, "." );
        nameArg->accessType = instance->accessType;
    }
    else
    {
        Tr069Node *node = ( Tr069Node * ) entry->node;

        strcat ( nameArg->name, node->name );

        if ( !IS_LEAF ( node->type ) )
        {
            strcat ( nameArg->name, "." );
        }

        nameArg->accessType = node->accessType;
    }

    /* insert the nameList and return to the caller */
    if ( NULL == nameList )
    {
        nameList = nameArg;
    }
    else
    {
        nameArg->next = nameList;
        nameList = nameArg;
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: get_nextLevel_names
Description:  get the name and readable attribute of node and it's sub-nodes.

Return: according to the Result.
Input:   topName -
            entry -
Others:
************************************************************************/
static Result get_nextLevel_names ( char *topName, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK, retTmp;

    if ( NULL == topName || NULL == entry )
    {
        endret ( NODE_NAME_ERROR );
    }

    if ( TCMAGENT_OK != ( retTmp = get_name ( topName, entry ) ) )
    {
        endret ( retTmp );
    }

    if ( Object == entry->type || Instance == entry->type )
    {
        PAL_INT32_T index, childNum;
        Tr069Node *subNode;

        if ( Object == entry->type )
        {
            subNode = ( ( Tr069Node * ) entry->node )->c.subNode;
            childNum = ( ( Tr069Node * ) entry->node )->childNum;
            strcat ( topName, ( ( Tr069Node * ) entry->node )->name );
        }
        else
        {
            subNode = ( ( Tr069Instance * ) entry->node )->subNode;
            childNum = ( ( Tr069Instance * ) entry->node )->childNum;
            sprintf ( topName, "%s%d", topName, ( ( Tr069Instance * ) entry->node )->index );
        }
        strcat ( topName, "." );

        for ( index = 0; index < childNum; index++ )
        {
            entry->type = Object;
            entry->node = &subNode[index];
            if ( TCMAGENT_OK != ( retTmp = get_name ( topName, entry ) ) )
            {
                endret ( retTmp );
            }
        }

    }
    else if ( MultiObject == entry->type )
    {
        Tr069Instance *instance;
        Tr069Instances *instances;

        instances = ( ( Tr069Node * ) entry->node )->c.instances;
        instance = instances->defaultInstance.next;

        strcat ( topName, ( ( Tr069Node * ) entry->node )->name );
        strcat ( topName, "." );

        while ( NULL != instance )
        {
            entry->type = Instance;
            entry->node = instance;

            if ( TCMAGENT_OK != ( retTmp = get_name ( topName, entry ) ) )
            {
                endret ( retTmp );
            }

            instance = instance->next;
        }
    }
    else
    {
        endret ( NODE_TYPE_ERROR );
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );

}

/***********************************************************************
Function: get_values
Description:  get the value of node.

Return: according to the Result.
Input:   topName -
            entry -
Others:
************************************************************************/
static Result get_values ( char *topName, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK, retTmp;
    Tr069ValueArg  *value;
    Tr069NameArg *name;
    Tr069Node *node;

    if ( !IS_LEAF ( entry->type ) )
    {
        endret ( TCMAGENT_OK );
    }

    if ( NULL == ( value = PAL_MALLOC ( sizeof ( Tr069ValueArg ) ) ) )
    {
        endret ( MALLOC_FAILURE );
    }

    memset ( value, 0, sizeof ( Tr069ValueArg ) );

    if ( NULL == ( name = PAL_MALLOC ( sizeof ( Tr069NameArg ) ) ) )
    {
        endret ( MALLOC_FAILURE );
    }

    memset ( name, 0, sizeof ( Tr069NameArg ) );

    /* only leaf */
    if ( NULL == topName )
    {
        strcpy ( name->name, "" );
    }
    else
    {
        strcpy ( name->name, topName );
    }

    node = ( Tr069Node * ) entry->node;

    strcat ( name->name, node->name );
    name->accessType = node->accessType;

    if ( 1/* Read == (node->accessType & Read) */ )
    {
        value->type = node->type;

        if ( CONFIGURATION != node->style )
        {
            if ( NULL != gCB.get_value )
            {

                if ( String == node->type )
                {
                    char *valueBuf;

                    if ( NULL == ( valueBuf = PAL_MALLOC ( node->range.max ) ) )
                    {
                        PAL_FREE ( valueBuf );
                        endret ( MALLOC_FAILURE );
                    }

                    if ( 0 != gCB.get_value ( name->name, valueBuf ) )
                    {
                        value->value.string = PAL_MALLOC ( 1 );
                        value->value.string[0] = '\0';
                    }
                    else
                    {
                        value->value.string = PAL_MALLOC ( strlen ( valueBuf ) + 1 );
                        strcpy ( value->value.string, valueBuf );
                    }

                    PAL_FREE ( valueBuf );
                }
                else
                {
                    PAL_INT32_T valueBuf;
                    char valueStr[MAX_NUM_LEN + 1];

                    if ( 0 != gCB.get_value ( name->name, valueStr ) )
                    {
                        value->value.intv = 0;
                    }
                    else
                    {
                        if ( !string_to_num ( valueStr, &valueBuf ) )
                        {
                            endret ( NODE_VALUE_ERROR );
                        }

                        value->value.intv = valueBuf;
                    }
                }
            }
            else
            {
                if ( String == node->type )
                {
                    value->value.string = PAL_MALLOC ( 1 );
                    value->value.string[0] = '\0';
                }
                else
                {
                    value->value.intv = 0;
                }
            }
        }
        else
        {
            if ( TCMAGENT_OK != ( retTmp = get_value ( entry, value ) ) )
            {
                endret ( retTmp );
            }
        }
    }
    else
    {
        if ( String == node->type )
        {
            value->value.string = PAL_MALLOC ( 1 );
            value->value.string[0] = '\0';
        }
        else
        {
            value->value.intv = 0;
        }
    }

    name->next = nameList;
    nameList = name;

    value->next = valueList;
    valueList = value;

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}


/***********************************************************************
Function: get_attributes
Description:  get the attributes of node.

Return: according to the Result.
Input:   topName -
            entry -
Others:
************************************************************************/
static Result get_attributes ( char *topName, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    Tr069AttributeArg  *attribute;
    Tr069NameArg *name;
    Tr069Node *node;

    if ( !IS_LEAF ( entry->type ) )
    {
        endret ( TCMAGENT_OK );
    }

    if ( NULL == ( attribute = PAL_MALLOC ( sizeof ( Tr069AttributeArg ) ) ) )
    {
        endret ( MALLOC_FAILURE );
    }

    memset ( attribute, 0, sizeof ( Tr069AttributeArg ) );

    if ( NULL == ( name = PAL_MALLOC ( sizeof ( Tr069NameArg ) ) ) )
    {
        endret ( MALLOC_FAILURE );
    }

    memset ( name, 0, sizeof ( Tr069NameArg ) );

    /* only leaf */
    if ( NULL == topName )
    {
        strcpy ( name->name, "" );
    }
    else
    {
        strcpy ( name->name, topName );
    }

    node = ( Tr069Node * ) entry->node;

    strcat ( name->name, node->name );
    name->accessType = node->accessType;

    attribute->accessList = node->accessModule;
    attribute->notification = node->notification;

    name->next = nameList;
    nameList = name;

    attribute->next = attributeList;
    attributeList = attribute;

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: write_instance_attribute
Description:  write the attribute of instance to file.

Return: according to the Result.
Input:   topName -
            entry -
Others:
************************************************************************/
static Result write_instance_attribute ( char *topName, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    Tr069Node *node;
    char allName[MAX_TR069_NAME_LEN + 1];

    if ( !IS_LEAF ( entry->type ) )
    {
        endret ( TCMAGENT_OK );
    }

    node = ( Tr069Node * ) entry->node;

    if ( NULL == topName )
    {
        strcpy ( allName, "" );
    }
    else
    {
        strcpy ( allName, topName );
    }

    strcat ( allName, node->name );

    if ( !is_multi_instance ( allName ) )
    {
        endret ( TCMAGENT_OK );
    }

    if ( NULL != instance_file )
    {
        fprintf ( instance_file, "%s %d %d\n", allName, node->notification, node->accessModule );
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: set_attributes
Description:  set the attributes of node.

Return: according to the Result.
Input:   topName -
            entry -
Others:
************************************************************************/
static Result set_attributes ( char *topName, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    Tr069Node *node;

    if ( !IS_LEAF ( entry->type ) )
    {
        endret ( TCMAGENT_OK );
    }

    node = ( Tr069Node * ) entry->node;

    if ( INVALID_VALUE != gAccessList )
    {
        node->accessModule = gAccessList;
    }

    if ( INVALID_VALUE != gNotification )
    {
        node->notification = gNotification;
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: write_instance_attributes
Description:  write the attributes of instance to the file.

Return: according to the Result.
Input:
Output:
Others:
************************************************************************/
static Result write_instance_attributes ( void )
{
    Result ret = TCMAGENT_OK, retTmp;

    remove ( PAL_INSTANCE_FILE );

    if ( NULL == ( instance_file = fopen ( PAL_INSTANCE_FILE, "a" ) ) )
    {
        endret ( INSTANCE_FILE_NOT_FOUND );
    }

    if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( NULL, NULL, write_instance_attribute ) ) )
    {
        endret ( retTmp );
    }

end:

    if ( NULL != instance_file )
    {
        fclose ( instance_file );
    }

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: copy_tr069_nodes
Description:  copy the tr069 nodes from the src nodes.

Return: according to the Result.
Input:   entryPtr - the src nodes which is copied from
            numOfNode - the num of array the src nodes.
Others:
************************************************************************/
static Result copy_tr069_node ( void **result, Tr069NodeEntry *entryPtr, PAL_INT32_T numOfNode )
{
    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry entry;

    if ( NULL == entryPtr )
    {
        endret ( NODE_NOT_FOUND );
    }

    /* copy the object, it maybe a array of many nodes */
    if ( Object == entryPtr->type )
    {
        Tr069Node *nodes = ( Tr069Node * ) entryPtr->node;
        Tr069Node *retNodes;
        PAL_UINT32_T i;

        retNodes = PAL_MALLOC ( numOfNode * sizeof ( Tr069Node ) );
        memcpy ( retNodes, nodes, numOfNode * sizeof ( Tr069Node ) );

        for ( i = 0; i < numOfNode; i++ )
        {
            Tr069Node *node = &retNodes[i];
            Tr069Node *fromNode = &nodes[i];

            if ( !IS_LEAF ( node->type ) )
            {
                if ( MultiObject == node->type )
                {
                    Tr069Instances *instances = fromNode->c.instances;

                    node->c.instances = PAL_MALLOC ( sizeof ( Tr069Instances ) );
                    memset ( node->c.instances, 0, sizeof ( Tr069Instances ) );
                    memcpy ( node->c.instances, instances, sizeof ( Tr069Instances ) );

                    /* I think this copy only is from default template, so don't consider the instance */

                    entry.type = Object;
                    entry.node = instances->defaultInstance.subNode;

                    if ( TCMAGENT_OK != ( retTmp = copy_tr069_node ( ( void ** ) ( & ( node->c.instances->defaultInstance.subNode ) ), &entry, instances->defaultInstance.childNum ) ) )
                    {
                        endret ( retTmp );
                    }
                }
                else   /* Object only */
                {
                    entry.type = Object;
                    entry.node = fromNode->c.subNode;

                    if ( TCMAGENT_OK != ( retTmp = copy_tr069_node ( ( void ** ) ( &node->c.subNode ), &entry, fromNode->childNum ) ) )
                    {
                        endret ( retTmp );
                    }
                }
            }
            else if ( String == node->type )
            {
                node->c.value.string = PAL_MALLOC ( strlen ( fromNode->c.value.string ) + 1 );
                strcpy ( node->c.value.string, fromNode->c.value.string );
            }
        }

        *result = retNodes;
    }
    else if ( Instance == entryPtr->type )
    {
        Tr069Instance *instance = ( Tr069Instance * ) entryPtr->node;
        Tr069Node *child = instance->subNode;

        *result = PAL_MALLOC ( sizeof ( Tr069Instance ) );
        memcpy ( *result, instance, sizeof ( Tr069Instance ) );

        entry.type = Object;
        entry.node = child;

        if ( TCMAGENT_OK != ( retTmp = copy_tr069_node ( ( void ** ) ( & ( ( Tr069Instance* ) * result )->subNode ), &entry, instance->childNum ) ) )
        {
            endret ( retTmp );
        }
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ret;
}

/***********************************************************************
Function: copy_from_template
Description:  copy the template instance.

Return: according to the Result.
Input:   defaultInstance
Output: newInstance
Others:
************************************************************************/
static Result copy_from_template ( Tr069Instance *defaultInstance, OUT Tr069Instance **newInstance )
{
    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry entry;

    entry.type = Instance;
    entry.node = defaultInstance;

    if ( TCMAGENT_OK != ( retTmp = copy_tr069_node ( ( void ** ) newInstance, &entry, 1 ) ) )
    {
        endret ( retTmp );
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );

}

/***********************************************************************
Function: delete_tr069_node
Description:  free the tr69-tree nodes.

Return: according to the Result.
Input:   entryPtr - the tr069 node
            numOfNode - the num of tr69 nodes.
Output:
Others:
************************************************************************/
static Result delete_tr069_node ( Tr069NodeEntry *entryPtr, PAL_INT32_T numOfNode )
{
    Result ret = TCMAGENT_OK, retTmp;

    if ( NULL == entryPtr )
    {
        endret ( NODE_NOT_FOUND );
    }

    if ( Object == entryPtr->type )
    {
        PAL_UINT32_T i;
        Tr069Node *nodes = ( Tr069Node * ) entryPtr->node;
        Tr069NodeEntry entry;

        for ( i = 0; i < numOfNode; i++ )
        {
            Tr069Node *node = &nodes[i];

            if ( !IS_LEAF ( node->type ) )
            {
                if ( MultiObject == node->type )
                {
                    Tr069Instances *instances = node->c.instances;
                    Tr069Instance  *instance = instances->defaultInstance.next;

                    while ( NULL != instance )
                    {
                        instance->prev->next = instance->next;
                        if ( NULL != instance->next )
                        {
                            instance->next->prev = instance->prev;
                        }
                        instance->next = NULL;
                        instance->prev = NULL;

                        entry.type = Instance;
                        entry.node = instance;

                        if ( TCMAGENT_OK != ( retTmp = delete_tr069_node ( &entry, 0 ) ) )
                        {
                            endret ( retTmp );
                        }

                        instance = instances->defaultInstance.next;
                    }

                    entry.type = Object;
                    entry.node = instances->defaultInstance.subNode;

                    if ( TCMAGENT_OK != ( retTmp = delete_tr069_node ( &entry, instances->defaultInstance.childNum ) ) )
                    {
                        endret ( retTmp );
                    }

                    PAL_FREE ( instances );
                }
                else   /* only Object */
                {
                    entry.type = Object;
                    entry.node = node->c.subNode;

                    if ( TCMAGENT_OK != ( retTmp = delete_tr069_node ( &entry, node->childNum ) ) )
                    {
                        endret ( retTmp );
                    }
                }
            }
            else if ( String == node->type )
            {
                if ( NULL != node->c.value.string )
                {
                    PAL_FREE ( node->c.value.string );
                }
            }
        }

        PAL_FREE ( nodes );
    }
    else if ( Instance == entryPtr->type )
    {
        Tr069Instance *instance = ( Tr069Instance * ) entryPtr->node;
        Tr069Node *child = instance->subNode;
        PAL_UINT32_T childNum = instance->childNum;
        Tr069NodeEntry entry;

        entry.type = Object;
        entry.node = child;

        if ( TCMAGENT_OK != ( retTmp = delete_tr069_node ( &entry, childNum ) ) )
        {
            endret ( retTmp );
        }

        PAL_FREE ( instance );
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );

}

/***********************************************************************
Function: delete_instance
Description:  delete the instance from the tr069-tree.

Return: according to the Result.
Input:   instance
Output:
Others:
************************************************************************/
static Result delete_instance ( Tr069Instance *instance )
{
    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry node;

    memset ( &node, 0, sizeof ( node ) );

    node.type = Instance;
    node.node = instance;

    if ( TCMAGENT_OK != ( retTmp = delete_tr069_node ( &node, 1 ) ) )
    {
        endret ( retTmp );
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );

}

/***********************************************************************
Function: get_notification_node
Description:  get the nodes from tr69-tree which is need to notify to acs.

Return: according to the Result.
Input:   topName
            entry
Output:
Others:
************************************************************************/
static Result get_notification_node ( char *topName, Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    Tr069Node *node;
    Tr069NameArg *nameArg;
    Tr069ValueArg *valueArg;

    if ( !IS_LEAF ( entry->type ) )
    {
        endret ( TCMAGENT_OK );
    }

    node = entry->node;

    if ( !node->isChanged )
    {
        endret ( TCMAGENT_OK );
    }
    else
    {
        /* gavin added to restore the ischanged status */
        node->isChanged = 0;
    }

    if ( NULL == ( nameArg = PAL_MALLOC ( sizeof ( Tr069NameArg ) ) ) )
    {
        endret ( MALLOC_FAILURE );
    }

    if ( NULL == ( valueArg = PAL_MALLOC ( sizeof ( Tr069ValueArg ) ) ) )
    {
        endret ( MALLOC_FAILURE );
    }

    memset ( nameArg, 0, sizeof ( *nameArg ) );
    memset ( valueArg, 0, sizeof ( *valueArg ) );

    if ( NULL == topName )
    {
        strcpy ( nameArg->name, "" );
    }
    else
    {
        strcpy ( nameArg->name, topName );
    }

    strcat ( nameArg->name, node->name );
    nameArg->accessType = node->accessType;

    valueArg->type = ( Tr069TypeArg ) node->type;

    if ( String == node->type )
    {
        valueArg->value.string = PAL_MALLOC ( strlen ( node->c.value.string ) + 1 );
        strcpy ( valueArg->value.string, node->c.value.string );
    }
    else if ( Int == node->type )
    {
        valueArg->value.intv = node->c.value.intv;
    }
    else if ( Unsigned == node->type )
    {
        valueArg->value.uintv = node->c.value.uintv;
    }
    else if ( Bool == node->type )
    {
        valueArg->value.boolv = node->c.value.boolv;
    }


    if ( NULL == nameList )
    {
        nameList = nameArg;
    }
    else
    {
        nameArg->next = nameList;
        nameList = nameArg;
    }

    if ( NULL == valueList )
    {
        valueList = valueArg;
    }
    else
    {
        valueArg->next = valueList;
        valueList = valueArg;
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

/***********************************************************************
Function: get_num_of_entries_name
Description:  get the XXXNumOfEntries name of multi-node.

Return: according to the Result.
Input:   nodeName
Output: nodeName
Others:
************************************************************************/
void get_num_of_entries_name ( char *nodeName )
{
    if ( 0 == strcmp ( nodeName, "Forwarding" ) )
    {
        strcpy ( nodeName, "Forward" );
    }
    else if ( 0 == strcmp ( nodeName, "LANEthernetInterfaceConfig" ) )
    {
        strcpy ( nodeName, "LANEthernetInterface" );
    }
    else if ( 0 == strcmp ( nodeName, "LANUSBInterfaceConfig" ) )
    {
        strcpy ( nodeName, "LANUSBInterface" );
    }
    else if ( 0 == strcmp ( nodeName, "WLANConfiguration" ) )
    {
        strcpy ( nodeName, "LANWLANConfiguration" );
    }
    else if ( 0 == strcmp ( nodeName, "Vendor-ConfigFile" ) )
    {
        strcpy ( nodeName, "VendorConfigFile" );
    }
    else if ( 0 == strcmp ( nodeName, "WANConnectionDevice" ) )
    {
        strcpy ( nodeName, "WANConnection" );
    }

    if ( 0 == strcmp ( nodeName, "WEPKey" ) || 0 == strcmp ( nodeName, "X_CISCO_COM_Key" ) || 0 == strcmp ( nodeName, "PreSharedKey" ) )
    {
        nodeName[0] = '\0';
    }
    else if ( 0 == strcmp ( nodeName, "Line" ) )
    {
        strcpy ( nodeName, "NumberOfLines" );
    }
    else
    {
        strcat ( nodeName, NUMBER_OF_ENTRIES_STR );
    }
}

/* ------------------  extern user interface ---------------------- */

//**************************************************************************
// Function Name: init_tr069_tree
// Description  : read the user template file, and build the tr069 node tree.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           configFile :  The complete dir name of template file .
//**************************************************************************
Result init_tr069_tree ( char *configFile )
{
    FILE *fp;
    Result ret = TCMAGENT_OK;
    char line[MAX_CONFIG_LINE_LEN];

    XmlNode *xml = NULL, *searchNode = NULL;

    if ( pal_mutex_init ( &tr069TreeMutex ) )
    {
        endret ( MUTEX_INIT_FAILURE );
    }

    //pal_set_logger ( LOG_TR069_TREE, T_LOG_INFO, LOG_CONSOLE, 1024,  "tr69-tree" );

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " tr69-tree init start\n" );

    tr069_tree_lock();

    if ( NULL == configFile )
    {
        endret ( CONFIG_FILE_NOT_FOUND );
    }

    fp = fopen ( configFile, "r" );

    if ( NULL == fp )
    {
        endret ( CONFIG_FILE_NOT_FOUND );
    }

    memset ( &cache, 0, sizeof ( Tr069NodeEntry ) );
    memset ( &rootNode, 0, sizeof ( Tr069NodeEntry ) );

    while ( 0 < fgets ( line, MAX_CONFIG_LINE_LEN, fp ) )
    {
        if ( NULL != strstr ( line, "<" ) )
        {
            fseek ( fp, - ( strlen ( line ) ), SEEK_CUR );
            break;
        }
    }

    xml = xml_load_file ( NULL, fp, NULL );

    searchNode = xml;

    if ( NULL != searchNode )
    {
        do
        {
            Result retTmp;

            if ( searchNode->type == XML_ELEMENT )
            {
                if ( TCMAGENT_OK != ( retTmp = add_tr069_node ( searchNode->value.element.name, searchNode->value.element.num_attrs, searchNode->value.element.attrs ) ) )
                {
                    endret ( retTmp );
                }
            }

            searchNode = xml_walk_next ( searchNode, xml, XML_DESCEND );
        }
        while ( searchNode != NULL );
    }
    else
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " configuration file is blank!\n" );
    }

end:

    if ( NULL != xml )
    {
        xml_delete ( xml );
    }

    tr069_tree_unlock();

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " tr69-tree init end\n" );

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

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
Result get_tr069_name ( char *beginName, OUT Tr069NameArg **retName, PAL_INT32_T nextlevel )
{
    Result ret = TCMAGENT_OK, retTmp;
    char topName[MAX_TR069_NAME_LEN + 1];
    char *topNameArg = topName;
    Tr069NodeEntry entry;
    Tr069NodeEntry *entryArg;
    Tr069NameArg *nameListTmp1;
    Tr069NameArg *nameListTmp2;

    tr069_tree_lock();

    nameList = NULL;

    if ( NULL == beginName || NULL == retName )
    {
        endret ( NODE_NAME_ERROR );
    }

    *retName = NULL;

    if ( 0 == strlen ( beginName ) )
    {
        Tr069Node *node = ( Tr069Node * ) rootNode.node;

        if ( NULL == node )
        {
            endret ( NODE_NAME_ERROR );
        }
        else
        {
            get_name ( NULL, &rootNode );
        }
    }
    else if ( '.' != beginName[strlen ( beginName ) - 1] )
    {
        if ( TCMAGENT_OK != ( retTmp = get_tr069_father_name ( beginName, topName ) ) )
        {
            endret ( retTmp );
        }

        if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( beginName, &entry ) ) )
        {
            endret ( retTmp );
        }

        if ( TCMAGENT_OK != ( retTmp = get_name ( topName, &entry ) ) )
        {
            endret ( retTmp );
        }

    }
    else if ( !nextlevel )
    {
        if ( 0 != strcmp ( beginName, "." ) )
        {
            if ( TCMAGENT_OK != ( retTmp = get_tr069_father_name ( beginName, topName ) ) )
            {
                endret ( retTmp );
            }

            if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( beginName, &entry ) ) )
            {
                endret ( retTmp );
            }

            entryArg = &entry;
        }
        else
        {
            entryArg = NULL;
            topNameArg = NULL;
        }

        if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( topNameArg, entryArg, get_name ) ) )
        {
            endret ( retTmp );
        }
    }
    else
    {
        if ( 0 == strcmp ( beginName, "." ) )
        {
            strcpy ( topName, "" );
            memcpy ( &entry, &rootNode, sizeof ( rootNode ) );
        }
        else
        {
            if ( TCMAGENT_OK != ( retTmp = get_tr069_father_name ( beginName, topName ) ) )
            {
                endret ( retTmp );
            }

            if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( beginName, &entry ) ) )
            {
                endret ( retTmp );
            }
        }

        if ( TCMAGENT_OK != ( retTmp = get_nextLevel_names ( topName, &entry ) ) )
        {
            endret ( retTmp );
        }
    }

    *retName = nameList;
    nameList = NULL;

    nameListTmp1 = *retName;
    nameListTmp2 = nameListTmp1;

    /* delete the beginName node , maybe it could be improve in future */
    if ( strlen ( beginName ) == 0 || '.' == beginName[strlen ( beginName ) - 1] )
    {
        while ( NULL != nameListTmp1 )
        {
            if ( 0 == strcmp ( beginName, nameListTmp1->name ) )
            {
                if ( *retName == nameListTmp1 )
                {
                    *retName = nameListTmp1->next;
                    nameListTmp1->next = NULL;
                    free_tr69_name_arg ( &nameListTmp1 );
                }
                else
                {
                    nameListTmp2->next = nameListTmp1->next;
                    nameListTmp1->next = NULL;
                    free_tr69_name_arg ( &nameListTmp1 );
                }

                break;
            }

            nameListTmp2 = nameListTmp1;
            nameListTmp1 = nameListTmp1->next;
        }
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        if ( NULL != nameList )
        {
            free_tr69_name_arg ( &nameList );
        }

        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "fatherName: %s error: %d\n", ( NULL == beginName ? "NULL" : beginName ), ( PAL_INT32_T ) ret );
    }

    tr069_tree_unlock();

    return ( ret );
}

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
Result get_tr069_value ( char *name, OUT Tr069NameArg **retName, OUT Tr069ValueArg **retValue )
{
    Result ret = TCMAGENT_OK, retTmp;
    char topName[MAX_TR069_NAME_LEN + 1];
    Tr069NodeEntry entry;

    tr069_tree_lock();

    valueList = NULL;
    nameList = NULL;

    if ( NULL == name || NULL == retName || NULL == retValue )
    {
        endret ( NODE_NAME_ERROR );
    }

    *retValue = NULL;
    *retName = NULL;

    if ( '.' == name[strlen ( name ) -1] || 0 == strlen ( name ) )
    {
        char *topNameArg;
        Tr069NodeEntry *entryArg;

        if ( 0 != strcmp ( name, "." ) && 0 != strlen ( name ) )
        {
            if ( TCMAGENT_OK != ( retTmp = get_tr069_father_name ( name, topName ) ) )
            {
                endret ( retTmp );
            }

            if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
            {
                endret ( retTmp );
            }

            topNameArg = topName;
            entryArg = &entry;
        }
        else
        {
            topNameArg = NULL;
            entryArg = NULL;
        }
        if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( topNameArg, entryArg, get_values ) ) )
        {
            endret ( retTmp );
        }

        *retValue = valueList;
        *retName = nameList;

    }
    else
    {
        Tr069ValueArg *valueTmp;
        Tr069NameArg *nameTmp;
        Tr069Node *node;

        if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
        {
            endret ( retTmp );
        }

        if ( NULL == ( valueTmp = ( Tr069ValueArg * ) PAL_MALLOC ( sizeof ( Tr069ValueArg ) ) ) )
        {
            endret ( MALLOC_FAILURE );
        }

        valueTmp->next = NULL;

        if ( !IS_LEAF ( ( ( Tr069Node * ) entry.node )->type ) )
        {
            endret ( NODE_TYPE_ERROR );
        }

        node = ( Tr069Node * ) entry.node;

        if ( /*Read == (node->accessType & Read) */ 1 )
        {
            valueTmp->type = node->type;

            if ( CONFIGURATION != node->style )
            {
                if ( NULL != gCB.get_value )
                {
                    if ( String == node->type )
                    {
                        char *value;

                        if ( NULL == ( value = PAL_MALLOC ( node->range.max ) ) )
                        {
                            PAL_FREE ( valueTmp );
                            endret ( MALLOC_FAILURE );
                        }

                        if ( 0 != gCB.get_value ( name, value ) )
                        {
                            valueTmp->value.string = PAL_MALLOC ( 1 );
                            valueTmp->value.string[0] = '\0';
                        }
                        else
                        {
                            valueTmp->value.string = PAL_MALLOC ( strlen ( value ) + 1 );
                            strcpy ( valueTmp->value.string, value );
                        }

                        PAL_FREE ( value );
                    }
                    else
                    {
                        PAL_INT32_T value;
                        char valueStr[MAX_NUM_LEN + 1];

                        if ( 0 != gCB.get_value ( name, valueStr ) )
                        {
                            valueTmp->value.intv = 0;
                        }
                        else
                        {
                            if ( !string_to_num ( valueStr, &value ) )
                            {
                                endret ( NODE_VALUE_ERROR );
                            }

                            valueTmp->value.intv = value;
                        }
                    }
                }
                else
                {
                    if ( String == node->type )
                    {
                        valueTmp->value.string = PAL_MALLOC ( 1 );
                        valueTmp->value.string[0] = '\0';
                    }
                    else
                    {
                        valueTmp->value.intv = 0;
                    }
                }
            }
            else
            {
                if ( TCMAGENT_OK != ( retTmp = get_value ( &entry, valueTmp ) ) )
                {
                    endret ( retTmp );
                }
            }
        }
        else
        {
            if ( String == node->type )
            {
                valueTmp->value.string = PAL_MALLOC ( 1 );
                valueTmp->value.string[0] = '\0';
            }
            else
            {
                valueTmp->value.intv = 0;
            }
        }

        *retValue = valueTmp;

        nameTmp = ( Tr069NameArg * ) PAL_MALLOC ( sizeof ( Tr069NameArg ) );
        nameTmp->accessType = node->accessType;
        strcpy ( nameTmp->name, name );
        nameTmp->next = NULL;

        *retName = nameTmp;
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        if ( NULL != nameList )
        {
            free_tr69_name_arg ( &nameList );
        }
        if ( NULL != valueList )
        {
            free_tr69_value_arg ( &valueList );
        }

        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "node name: %s error: %d\n", ( NULL == name ? "NULL" : name ), ( PAL_INT32_T ) ret );
    }

    tr069_tree_unlock();

    return ( ret );
}

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
Result set_tr069_value ( Tr069AccessModule module, char *name , Tr069ValueArg *value )
{
    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry entry;
    Tr069Node *node;

    tr069_tree_lock();

    if ( NULL == name || NULL == value )
    {
        endret ( NODE_NOT_FOUND );
    }

    if ( !IS_VALID_MODULE ( module ) )
    {
        endret ( NODE_ACCESSLIST_CHECK_ERROR );
    }

    if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
    {
        endret ( retTmp );
    }

    if ( !IS_LEAF ( entry.type ) )
    {
        endret ( NODE_TYPE_ERROR );
    }

    node = ( Tr069Node * ) entry.node;

    if ( ALL != node->accessModule && ( PAL_INT32_T ) node->accessModule != ( PAL_INT32_T ) module )
    {
        endret ( NODE_ACCESSLIST_CHECK_ERROR );
    }

    if ( ACS == module && ! ( node->accessType & Write ) )
    {
        endret ( NODE_ACCESS_ERROR );
    }

    if ( TCMAGENT_OK != ( retTmp = set_value ( &entry, value ) ) )
    {
        endret ( retTmp );
    }

    if ( WEB_MODULE == module )
    {
        if ( NotificationNone != ( ( Tr069Node * ) ( entry.node ) )->notification )
        {
            if ( gInitSuccess )
            {
                ( ( Tr069Node * ) ( entry.node ) )->isChanged = 1;
            }
        }

        if ( NotificationActive == ( ( Tr069Node * ) ( entry.node ) )->notification )
        {
            endret ( NODE_NEED_NOTIFICATION );
        }

        if ( NotificationPassive == ( ( Tr069Node * ) ( entry.node ) )->notification )
        {
            endret ( NODE_NEED_NOTIFICATION_PASSIVE );
        }
    }

end:

    tr069_tree_unlock();

    if ( TCMAGENT_OK != ret && NODE_NEED_NOTIFICATION != ret && NODE_NEED_NOTIFICATION_PASSIVE  != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " module: %d node name: %s error: %d\n", ( PAL_INT32_T ) module, ( NULL == name ? "NULL" : name ), ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

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
Result get_tr069_attribute ( char *name, OUT Tr069NameArg **retName, OUT Tr069AttributeArg ** retAttribute )
{
    Result ret = TCMAGENT_OK, retTmp;
    char topName[MAX_TR069_NAME_LEN + 1];
    Tr069NodeEntry entry;

    tr069_tree_lock();

    attributeList = NULL;
    nameList = NULL;

    if ( NULL == name || NULL == retName || NULL == retAttribute )
    {
        endret ( NODE_NAME_ERROR );
    }

    *retAttribute = NULL;
    *retName = NULL;

    if ( '.' == name[strlen ( name ) -1] || 0 == strlen ( name ) )
    {
        char *topNameArg;
        Tr069NodeEntry *entryArg;

        if ( 0 != strcmp ( name, "." ) && 0 != strlen ( name ) )
        {
            if ( TCMAGENT_OK != ( retTmp = get_tr069_father_name ( name, topName ) ) )
            {
                endret ( retTmp );
            }

            if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
            {
                endret ( retTmp );
            }

            entryArg = &entry;
            topNameArg = topName;
        }
        else
        {
            topNameArg = NULL;
            entryArg = NULL;
        }

        if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( topNameArg, entryArg, get_attributes ) ) )
        {
            endret ( retTmp );
        }

        *retAttribute = attributeList;
        *retName = nameList;

    }
    else
    {
        Tr069AttributeArg *attributeTmp;
        Tr069NameArg *nameTmp;
        Tr069Node *node;

        if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
        {
            endret ( retTmp );
        }

        if ( NULL == ( attributeTmp = ( Tr069AttributeArg * ) PAL_MALLOC ( sizeof ( Tr069AttributeArg ) ) ) )
        {
            endret ( MALLOC_FAILURE );
        }

        attributeTmp->next = NULL;

        if ( !IS_LEAF ( entry.type ) )
        {
            endret ( NODE_TYPE_ERROR );
        }

        node = ( Tr069Node * ) entry.node;

        attributeTmp->accessList = node->accessModule;
        attributeTmp->notification = node->notification;

        *retAttribute = attributeTmp;

        nameTmp = ( Tr069NameArg * ) PAL_MALLOC ( sizeof ( Tr069NameArg ) );
        nameTmp->accessType = node->accessType;
        strcpy ( nameTmp->name, name );
        nameTmp->next = NULL;

        *retName = nameTmp;
    }

end:

    if ( TCMAGENT_OK != ret )
    {
        if ( NULL != nameList )
        {
            free_tr69_name_arg ( &nameList );
        }
        if ( NULL != attributeList )
        {
            free_tr69_attribute_arg ( &attributeList );
        }

        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "node name: %s error: %d\n", ( NULL == name ? "NULL" : name ), ( PAL_INT32_T ) ret );
    }

    tr069_tree_unlock();

    return ( ret );
}

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
Result set_tr069_attribute ( char *name, PAL_INT32_T setNotification, Tr069Notification notification, PAL_INT32_T setAccessList, Tr069AccessList accessList )
{
    Result ret = TCMAGENT_OK, retTmp;
    Tr069NodeEntry entry;
    char topName[MAX_TR069_NAME_LEN + 1];
    Tr069Notification *notificationPtr;
    Tr069AccessList *accessListPtr;
    PAL_INT32_T *isChanged;
    PAL_INT32_T save_instance_attributes = 0;

    tr069_tree_lock();

    if ( NULL == name )
    {
        endret ( NODE_NOT_FOUND );
    }

    if ( !setNotification && !setAccessList )
    {
        endret ( TCMAGENT_OK );
    }

    if ( setNotification && !IS_VALID_NOTIFICATION ( notification ) )
    {
        endret ( ATTRIBUTE_NOTIFICATION_ERROR );
    }

    if ( setAccessList && !IS_VALID_ACCESSLIST ( accessList ) )
    {
        endret ( ATTRIBUTE_ACCESSLIST_ERROR );
    }

    gAccessList = INVALID_VALUE;
    gNotification = INVALID_VALUE;

    if ( setAccessList )
    {
        gAccessList = accessList;
    }

    if ( setNotification )
    {
        gNotification = notification;
    }

    if ( '.' == name[strlen ( name ) -1] || 0 == strlen ( name ) )
    {
        char *topNameArg;
        Tr069NodeEntry *entryArg;

        if ( 0 != strcmp ( name, "." ) && 0 != strlen ( name ) )
        {
            if ( TCMAGENT_OK != ( retTmp = get_tr069_father_name ( name, topName ) ) )
            {
                endret ( retTmp );
            }

            if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
            {
                endret ( retTmp );
            }

            entryArg = &entry;
            topNameArg = topName;
        }
        else
        {
            topNameArg = NULL;
            entryArg = NULL;
        }

        save_instance_attributes = 1;

        if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( topNameArg, entryArg, set_attributes ) ) )
        {
            endret ( retTmp );
        }
    }
    else
    {
        if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
        {
            endret ( retTmp );
        }


        if ( Instance == entry.type )
        {
            Tr069Instance *node = ( Tr069Instance * ) entry.node;

            notificationPtr = & ( node->notification );
            accessListPtr = & ( node->accessModule );

        }
        else
        {
            Tr069Node *node = ( Tr069Node * ) entry.node;

            notificationPtr = & ( node->notification );
            accessListPtr = & ( node->accessModule );
            isChanged = & ( node->isChanged );
        }

        if ( setNotification )
        {
            *notificationPtr = notification;
            *isChanged = 0;
        }

        if ( setAccessList )
        {
            *accessListPtr = accessList;
        }

        if ( is_multi_instance ( name ) )
        {
            save_instance_attributes = 1;
        }
    }

    if ( gInitSuccess )
    {
        write_config_file();

        if ( save_instance_attributes )
        {
            write_instance_attributes();
        }
    }

end:

    tr069_tree_unlock();

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "node name: %s, setNotification: %d, notification: %d,  setAccessList: %d, accessList: %d\n",
                     ( ( NULL == name ) ? "NULL" : name ), setNotification, ( PAL_INT32_T ) notification, setAccessList, ( PAL_INT32_T ) accessList );
    }

    return ( ret );
}

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
Result add_tr069_object ( char *objName, OUT PAL_UINT32_T *index )
{
    Result ret = TCMAGENT_OK, retTmp;
    char fatherName[MAX_TR069_NAME_LEN + 1];
    char nodeName[MAX_TR069_NAME_LEN + 1];
    char name[MAX_TR069_NAME_LEN + 1];
    char *strPtr;
    Tr069ValueArg value;
    PAL_INT32_T instanceID;
    Tr069NodeEntry entry;
    Tr069Node *node;
    Tr069Instances *defaultInstance;
    Tr069Instance *instance, *newInstance, *tmp;
    int needAddNumber = 1;

    tr069_tree_lock();

    /* accessList is only valid in leaf.
    if (!IS_VALID_MODULE(module))
    {
        endret (NODE_ACCESSLIST_CHECK_ERROR);
    }
      */

    if ( NULL == objName || NULL == index )
    {
        endret ( NODE_NOT_FOUND );
    }
    else
    {
        char *tmp1, *tmp2;
        strcpy ( name, objName );

        if ( name[strlen ( name ) -1] != '.' )
        {
            endret ( NODE_NAME_ERROR );
        }

        tmp1 = &name[strlen ( name ) -1];
        *tmp1 = '\0';

        instanceID = 0;

        if ( NULL != ( tmp2 = strrchr ( name, '.' ) ) )
        {
            tmp2 ++;

            if ( !string_to_num ( tmp2, ( PAL_INT32_T* ) &instanceID ) )
            {
                *tmp1 = '.';
                instanceID = 0;
            }
            else
            {
                *tmp2 = '\0';
                if ( instanceID <= 0 )
                    instanceID = 0;
            }
        }
    }

    if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
    {
        endret ( retTmp );
    }

    if ( MultiObject != entry.type )
    {
        endret ( NODE_NAME_ERROR );
    }

    node = ( Tr069Node * ) entry.node;

    /* accessList is only valid in leaf.
    if (ALL != node->accessModule && (PAL_INT32_T)node->accessModule != (PAL_INT32_T)module)
    {
        endret (NODE_ACCESSLIST_CHECK_ERROR);
    }
      */

    defaultInstance = ( Tr069Instances * ) node->c.instances;
    tmp = instance = defaultInstance->defaultInstance.next;

    if ( 0 != instanceID )
    {
        while ( NULL != tmp )
        {
            if ( tmp->index == instanceID )
            {
                endret ( TCMAGENT_OK );
            }

            tmp = tmp->next;
        }
    }

    if ( defaultInstance->numOfEntries >= defaultInstance->maxNumOfEntries )
    {
        endret ( INSTANCES_IS_FULL );
    }

    if ( instanceID )
    {
        instance = defaultInstance->defaultInstance.next;

        while ( NULL != instance )
        {
            if ( instanceID == instance->index )
            {
                endret ( TCMAGENT_OK );
            }

            instance = instance->next;
        }
    }
    else
    {
        instanceID = defaultInstance->freeIndex;
    }

    if ( TCMAGENT_OK != ( retTmp = copy_from_template ( &defaultInstance->defaultInstance, &newInstance ) ) )
    {
        endret ( retTmp );
    }

    newInstance->next = newInstance->prev = NULL;
    newInstance->index = instanceID;

    instance = &defaultInstance->defaultInstance;

    while ( NULL != instance )
    {
        if ( newInstance->index < instance->index )
        {
            instance->prev->next = newInstance; /* instance->prev must be not NULL */
            newInstance->prev = instance->prev;
            instance->prev = newInstance;
            newInstance->next = instance;
            break;
        }
        else if ( NULL == instance->next )
        {
            newInstance->next = instance->next;
            instance->next = newInstance;
            newInstance->prev = instance;
            break;
        }

        instance = instance->next;
    }

    *index = instanceID;

    /* freeIndex recalculated */
    defaultInstance->freeIndex++;

    if ( defaultInstance->freeIndex > 65535 )
    {
        defaultInstance->freeIndex = 1;
    }


    /* XXXNumOfEntries */
    defaultInstance->numOfEntries++;

    /* ignore any error, it's impossible to get fatherName failure, it is guaranteed by the correct template file */
    if ( TCMAGENT_OK != get_tr069_father_name ( name, fatherName ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "node name: %s, index: %d, result: %d\n", ( NULL == name ? "NULL" : name ), *index, ( PAL_INT32_T ) ret );
    }

    strPtr = strrchr ( name, '.' );
    *strPtr = '\0';
    strPtr = strrchr ( name, '.' );
    strPtr++;

    strcpy ( nodeName, strPtr );
    *strPtr = '\0';

    get_num_of_entries_name ( nodeName );

    if ( 0 == strlen ( nodeName ) )
    {
        needAddNumber = 0;
    }

    strcat ( name, nodeName );

    value.type = Unsigned;
    value.value.uintv = defaultInstance->numOfEntries;

    /* ignore any error, it's impossible to get fatherName failure, it is guaranteed by the correct template file */
    mutexReEnter = 1;

    if ( needAddNumber )
    {
        if ( TCMAGENT_OK != ( retTmp = set_tr069_value ( WEB_MODULE, name, &value ) ) && NODE_NEED_NOTIFICATION != retTmp )
        {
        }
    }

    mutexReEnter = 0;
end:

    tr069_tree_unlock();

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "node name: %s, index: %d, result: %d\n", ( NULL == name ? "NULL" : name ), *index, ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

//**************************************************************************
// Function Name: delete_tr069_object
// Description  : delete a instance of object from tr069-tree.
// Returns       : according to the Result;
// Parameter :
//                  Input :
//                           name :          tr069 node name according to the protocol of tr069.It must be end with a instance id
//**************************************************************************
Result delete_tr069_object ( char *objName )
{
    Result ret = TCMAGENT_OK, retTmp;
    char fatherName[MAX_TR069_NAME_LEN + 1];
    char nodeName[MAX_TR069_NAME_LEN + 1];
    char name[MAX_TR069_NAME_LEN + 1];
    char cacheName[MAX_TR069_NAME_LEN + 1];
    char *strPtr;
    Tr069ValueArg value;
    PAL_UINT32_T instanceID;
    Tr069NodeEntry entry;
    Tr069Node *node;
    Tr069Instances *defaultInstance;
    Tr069Instance *instance;
    int needAddNumber = 1;

    tr069_tree_lock();

    /*
    if (!IS_VALID_MODULE(module))
    {
        endret (NODE_ACCESSLIST_CHECK_ERROR);
    }
      */

    if ( NULL == objName )
    {
        endret ( NODE_NOT_FOUND );
    }
    else
    {
        char *tmp1, *tmp2;
        strcpy ( name, objName );

        if ( name[strlen ( name ) -1] != '.' )
        {
            endret ( NODE_NAME_ERROR );
        }

        tmp1 = &name[strlen ( name ) -1];
        *tmp1 = '\0';

        instanceID = 0;

        if ( NULL != ( tmp2 = strrchr ( name, '.' ) ) )
        {
            tmp2 ++;

            if ( !string_to_num ( tmp2, ( PAL_INT32_T* ) &instanceID ) || instanceID <= 0 )
            {
                endret ( NODE_NAME_ERROR );
            }
            else
            {
                *tmp2 = '\0';
            }
        }
        else
        {
            endret ( NODE_NAME_ERROR );
        }
    }

    if ( TCMAGENT_OK != ( retTmp = get_tr069_node_by_name ( name, &entry ) ) )
    {
        endret ( retTmp );
    }

    if ( MultiObject != entry.type )
    {
        endret ( NODE_NAME_ERROR );
    }

    node = ( Tr069Node * ) entry.node;

    /* accessList is only valid in leaf.
    if (ALL != node->accessModule && (PAL_INT32_T)node->accessModule != (PAL_INT32_T)module)
    {
        endret (NODE_ACCESSLIST_CHECK_ERROR);
    }
     */

    defaultInstance = ( Tr069Instances * ) node->c.instances;
    instance = defaultInstance->defaultInstance.next;

    while ( NULL != instance )
    {
        if ( instanceID == instance->index )
        {
            break;
        }
        instance = instance->next;
    }

    if ( NULL == instance )
    {
        endret ( TCMAGENT_OK );
    }

    /* down from the list */
    instance->prev->next = instance->next;

    if ( NULL != instance->next )
    {
        instance->next->prev = instance->prev;
    }

    instance->next = NULL;
    instance->prev = NULL;

    if ( TCMAGENT_OK != ( retTmp = delete_instance ( instance ) ) )
    {
        endret ( retTmp );
    }

    /* cache update */
    strcpy ( cacheName, objName );
    cacheName[strlen ( cacheName ) ] = '\0';
    if ( strstr ( cache.name, cacheName ) )
    {
        memcpy ( &cache, &rootNode, sizeof ( cache ) );
    }

    /* XXXNumOfEntries */
    defaultInstance->numOfEntries--;

    /* ignore any error, it's impossible to get fatherName failure, it is guaranteed by the correct template file */
    if ( TCMAGENT_OK != get_tr069_father_name ( name, fatherName ) )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " It's impossible to here." );
    }

    strPtr = strrchr ( name, '.' );
    *strPtr = '\0';
    strPtr = strrchr ( name, '.' );
    strPtr++;

    strcpy ( nodeName, strPtr );
    *strPtr = '\0';

    get_num_of_entries_name ( nodeName );

    if ( 0 == strlen ( nodeName ) )
    {
        needAddNumber = 0;
    }

    strcat ( name, nodeName );

    value.type = Unsigned;
    value.value.uintv = defaultInstance->numOfEntries;

    /* ignore any error, it's impossible to get fatherName failure, it is guaranteed by the correct template file */
    mutexReEnter = 1;

    if ( needAddNumber )
    {
        if ( TCMAGENT_OK != set_tr069_value ( WEB_MODULE, name, &value ) )
        {
        }
    }
    mutexReEnter = 0;

end:
    if ( TCMAGENT_OK == ret )
    {
        write_instance_attributes();
    }

    tr069_tree_unlock();

    if ( TCMAGENT_OK != ret )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, "node name: %s error: %d\n", ( NULL == name ? "NULL" : name ), ( PAL_INT32_T ) ret );
    }

    return ( ret );
}

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
Result get_tr069_notification_node ( OUT Tr069NameArg **retName, OUT Tr069ValueArg **retValue )
{
    Result ret = TCMAGENT_OK, retTmp;

    tr069_tree_lock();

    nameList = NULL;
    valueList = NULL;


    if ( NULL == retName || NULL == retValue )
    {
        endret ( NODE_NAME_ERROR );
    }

    *retName = NULL;
    *retValue = NULL;

    if ( TCMAGENT_OK != ( retTmp = walk_through_tr069 ( NULL, NULL, get_notification_node ) ) )
    {
        endret ( retTmp );
    }

    *retName = nameList;
    *retValue = valueList;
    nameList = NULL;
    valueList = NULL;

end:

    if ( TCMAGENT_OK != ret )
    {
        if ( NULL != nameList )
        {
            free_tr69_name_arg ( &nameList );
        }

        if ( NULL != valueList )
        {
            free_tr69_value_arg ( &valueList );
        }

        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_TR069_TREE, __func__, " error: %d\n", ( PAL_INT32_T ) ret );
    }

    tr069_tree_unlock();

    return ( ret );
}

/***********************************************************************
Function: get_tr069_tree_size
Description: get the size of tr069 tree in the unit of byte.
************************************************************************/
PAL_UINT32_T get_tr069_tree_size ( void )
{
    tr069_tree_lock();
    tr069TreeSize = 0;
    walk_through_tr069_all ( NULL, NULL, get_node_size );
    tr069_tree_unlock();

    return tr069TreeSize;
}

/***********************************************************************
Function: get_node_size
Description:  get the size of node of tree.
************************************************************************/
Result get_node_size ( char *topName , Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;

    if ( Object == entry->type || MultiObject == entry->type )
    {
        tr069TreeSize += sizeof ( Tr069Node );
    }
    else if ( Instance == entry->type )
    {
        tr069TreeSize += sizeof ( Tr069Instance );
    }
    else if ( DefInstance == entry->type )
    {
        tr069TreeSize += sizeof ( Tr069Instances );
    }
    else
    {
        Tr069Node *node = ( Tr069Node * ) entry->node;

        tr069TreeSize += sizeof ( Tr069Node );

        if ( String == node->type )
        {
            tr069TreeSize += ( strlen ( node->c.value.string ) + 1 );
        }
    }

    return TCMAGENT_OK;
}

Result print_node_simple ( char *, Tr069NodeEntry * );

/***********************************************************************
Function: print_tr069_nodes
Description: print all the tr069 nodes of tree except the template node.
************************************************************************/
void print_tr069_nodes ( void )
{
    tr069_tree_lock();
    walk_through_tr069 ( NULL, NULL, print_node_simple );
    tr069_tree_unlock();
}

/***********************************************************************
Function: print_node
Description:  print the information of node of tree.
************************************************************************/
Result print_node ( char *topName , Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    char tmpName[MAX_TR069_NAME_LEN + 1];
    FILE *fp;
    char _tmpStr[BUFSIZ];

    if ( NULL == topName )
    {
        strcpy ( tmpName, "" );
    }
    else
    {
        strcpy ( tmpName, topName );
    }

    if ( strlen ( gOutputDIR ) == 0 )
    {
        fprintf ( stderr, "no output directory specified\n" );
        return -1;
    }

    if ( ( fp = fopen ( gOutputDIR, "a" ) ) == NULL )
    {
        fprintf ( stderr, "failed to open %s\n", CLI_SHOW_TREE_TMP_FILE );
        return -1;
    }

    if ( Object == entry->type || MultiObject == entry->type )
    {
        Tr069Node *node = ( Tr069Node * ) entry->node;

        strcat ( tmpName, node->name );
        strcat ( tmpName, "." );

        fprintf ( fp, "node name: %s type: %s notification: %s, accessType: %s,"
                  "needReboot: %d, accessList: %s, isChanged: %d, style: %s,"
                  "childNum: %d, min: %d, max: %d\n"
                  , tmpName
                  , Tr069TypeInfo[ ( PAL_INT32_T ) node->type]
                  , Tr069NotificationInfo[ ( PAL_INT32_T ) node->notification]
                  , Tr069AccessInfo[ ( PAL_INT32_T ) node->accessType]
                  , node->needReboot
                  , Tr069AccessListInfo[ ( PAL_INT32_T ) node->accessModule]
                  , node->isChanged
                  , Tr069StyleInfo[ ( PAL_INT32_T ) node->style]
                  , node->childNum
                  , node->range.min
                  , node->range.max );
    }
    else if ( Instance == entry->type )
    {
        Tr069Instance *node = ( Tr069Instance * ) ( & ( ( Tr069Instances * ) entry->node )->defaultInstance );

        sprintf ( tmpName, "%s%d.", tmpName, node->index );

        fprintf ( fp, "node name: %s type: %s notification: %s, accessTyCLI_SHOW_TREE_TMP_FILEpe: %s,"
                  "accessList: %s, childNum: %d\n"
                  , tmpName
                  , Tr069TypeInfo[ ( PAL_INT32_T ) node->type]
                  , Tr069NotificationInfo[ ( PAL_INT32_T ) node->notification]
                  , Tr069AccessInfo[ ( PAL_INT32_T ) node->accessType]
                  , Tr069AccessListInfo[ ( PAL_INT32_T ) node->accessModule]
                  , node->childNum );
    }
    else
    {
        Tr069Node *node = ( Tr069Node * ) entry->node;

        strcat ( tmpName, node->name );

        fprintf ( fp, "node name: %s type: %s notification: %s, accessType: %s, "
                  "needReboot: %d, accessList: %s, isChanged: %d, style: %s,"
                  "childNum: %d, min: %d, max: %d\n"
                  , tmpName
                  , Tr069TypeInfo[ ( PAL_INT32_T ) node->type]
                  , Tr069NotificationInfo[ ( PAL_INT32_T ) node->notification]
                  , Tr069AccessInfo[ ( PAL_INT32_T ) node->accessType]
                  , node->needReboot
                  , Tr069AccessListInfo[ ( PAL_INT32_T ) node->accessModule]
                  , node->isChanged
                  , Tr069StyleInfo[ ( PAL_INT32_T ) node->style]
                  , node->childNum
                  , node->range.min
                  , node->range.max );

        switch ( node->type )
        {
            case Int:
                fprintf ( fp, "value: %d\n", node->c.value.intv );
                break;
            case Unsigned:
                fprintf ( fp, "value: %d\n", node->c.value.intv );
                break;
            case Bool:
                fprintf ( fp, "value: %d\n", node->c.value.intv );
                break;
            case String:
                fprintf ( fp, "value: %s\n", node->c.value.string );
                break;
            default :
                break;
        }
    }

    fclose ( fp );
    return TCMAGENT_OK;
}


Result print_node_simple ( char *topName , Tr069NodeEntry *entry )
{
    Result ret = TCMAGENT_OK;
    char tmpName[MAX_TR069_NAME_LEN + 1];
    FILE *fp;
    char _tmpStr[BUFSIZ];

    if ( NULL == topName )
    {
        strcpy ( tmpName, "" );
    }
    else
    {
        strcpy ( tmpName, topName );
    }

    if ( strlen ( gOutputDIR ) == 0 )
    {
        fprintf ( stderr, "no output directory specified\n" );
        return -1;
    }

    if ( ( fp = fopen ( gOutputDIR, "a" ) ) == NULL )
    {
        fprintf ( stderr, "failed to open %s\n", CLI_SHOW_TREE_TMP_FILE );
        return -1;
    }

    if ( Object == entry->type || MultiObject == entry->type )
    {
        Tr069Node *node = ( Tr069Node * ) entry->node;

        strcat ( tmpName, node->name );
        strcat ( tmpName, "." );

        fprintf ( fp, "node name: %s\n", tmpName );
    }
    else if ( Instance == entry->type )
    {
        Tr069Instance *node = ( Tr069Instance * ) ( & ( ( Tr069Instances * ) entry->node )->defaultInstance );

        sprintf ( tmpName, "%s%d.", tmpName, node->index );

        fprintf ( fp, "node name: %s\n", tmpName );
    }
    else
    {
        Tr069Node *node = ( Tr069Node * ) entry->node;

        strcat ( tmpName, node->name );

        fprintf ( fp, "node name: %s\t", tmpName );

        switch ( node->type )
        {
            case Int:
                fprintf ( fp, "value: %d\n", node->c.value.intv );
                break;
            case Unsigned:
                fprintf ( fp, "value: %d\n", node->c.value.intv );
                break;
            case Bool:
                fprintf ( fp, "value: %d\n", node->c.value.intv );
                break;
            case String:
                fprintf ( fp, "value: %s\n", node->c.value.string );
                break;
            default :
                break;
        }
    }

    fclose ( fp );
    return TCMAGENT_OK;
}

/***********************************************************************
Function: free_tr69_name_arg
Description: free the memory of Tr069NameArg
************************************************************************/
Result free_tr69_name_arg ( Tr069NameArg **nameP )
{
    Tr069NameArg *tmp1 = *nameP;
    Tr069NameArg *tmp2;

    while ( NULL != tmp1 )
    {
        tmp2 = tmp1->next;

        PAL_FREE ( tmp1 );

        tmp1 = tmp2;
    }

    return TCMAGENT_OK;
}

/***********************************************************************
Function: free_tr69_value_arg
Description: free the memory of Tr069ValueArg
************************************************************************/
Result free_tr69_value_arg ( Tr069ValueArg **valueP )
{
    Tr069ValueArg *tmp1 = *valueP;
    Tr069ValueArg *tmp2;

    while ( NULL != tmp1 )
    {
        tmp2 = tmp1->next;

        if ( tString == tmp1->type )
        {
            if ( tmp1->value.string != NULL )
            {
                PAL_FREE ( tmp1->value.string );
            }
        }
        PAL_FREE ( tmp1 );

        tmp1 = tmp2;
    }

    return TCMAGENT_OK;
}

/***********************************************************************
Function: free_tr69_attribute_arg
Description: free the memory of Tr069AttributeArg
************************************************************************/
Result free_tr69_attribute_arg ( Tr069AttributeArg * * attributeP )
{
    Tr069AttributeArg *tmp1 = *attributeP;
    Tr069AttributeArg *tmp2;

    while ( NULL != tmp1 )
    {
        tmp2 = tmp1->next;

        PAL_FREE ( tmp1 );

        tmp1 = tmp2;
    }

    return TCMAGENT_OK;
}

/***********************************************************************
Function: load_instance_attributes
Description: load the attrbutes of instance from file to tr069-tree.
************************************************************************/
void load_instance_attributes ( void )
{
    FILE *file;
    char line[MAX_TR069_NAME_LEN + 1 + 4];
    char name[MAX_TR069_NAME_LEN + 1];
    Tr069Notification notification;
    Tr069AccessList accessList;

    if ( NULL == ( file = fopen ( PAL_INSTANCE_FILE, "r" ) ) )
    {
        return;
    }

    while ( 0 < fgets ( line, MAX_TR069_NAME_LEN + 5, file ) )
    {
        if ( 0 == strlen ( line ) )
        {
            continue;
        }

        if ( 3 != sscanf ( line, "%s %d %d\n", name, &notification, &accessList ) )
        {
            continue;
        }

        set_tr069_attribute ( name, 1, notification, 1, accessList );
    }

    if ( NULL != file )
    {
        fclose ( file );
    }

    return ;
}

