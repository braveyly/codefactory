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
File name: xml_public.h
Author: kiffen guan
Date: April 1th. 2009
************************************************************************/

#ifndef _XML_H_
#define _XML_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>


#define XML_TAB  8
#define XML_NO_CALLBACK 0
#define XML_INTEGER_CALLBACK xml_integer_cb
#define XML_OPAQUE_CALLBACK xml_opaque_cb
#define XML_REAL_CALLBACK xml_real_cb
#define XML_TEXT_CALLBACK 0
#define XML_IGNORE_CALLBACK xml_ignore_cb

#define XML_NO_PARENT 0

#define XML_DESCEND  1
#define XML_NO_DESCEND 0
#define XML_DESCEND_FIRST -1

#define XML_WS_BEFORE_OPEN 0
#define XML_WS_AFTER_OPEN 1
#define XML_WS_BEFORE_CLOSE 2
#define XML_WS_AFTER_CLOSE 3

#define XML_ADD_BEFORE 0
#define XML_ADD_AFTER 1
#define XML_ADD_TO_PARENT NULL

typedef enum _XmlSaxEvent
{
    XML_SAX_CDATA,
    XML_SAX_COMMENT,
    XML_SAX_DATA,
    XML_SAX_DIRECTIVE,
    XML_SAX_ELEMENT_CLOSE,
    XML_SAX_ELEMENT_OPEN,
} XmlSaxEvent;

typedef enum _XmlType
{
    XML_IGNORE = -1,
    XML_ELEMENT,
    XML_INTEGER,
    XML_OPAQUE,
    XML_REAL,
    XML_TEXT,
    XML_CUSTOM,
} XmlType;

typedef void (*cb_xml_custom_destroy)(void *);
typedef void (*cb_xml_error)(const char *);

typedef struct _XmlAttribute
{
    char *name;
    char *value;
} XmlAttribute;

typedef struct _XmlElement
{
    char *name;
    int num_attrs;
    XmlAttribute *attrs;
} XmlElement;

typedef struct _XmlText
{
    int whitespace;
    char *string;
} XmlText;

typedef struct _XmlCustom
{
    void *data;
    cb_xml_custom_destroy destroy;
} XmlCustom;

typedef union _XmlValue
{
    XmlElement element;
    int integer;
    char *opaque;
    double real;
    XmlText text;
    XmlCustom custom;
} XmlValue;

typedef struct _XmlNode
{
    XmlType type;
    struct _XmlNode *next;
    struct _XmlNode *prev;
    struct _XmlNode *parent;
    struct _XmlNode *child;
    struct _XmlNode *lastChild;
    XmlValue value;
    int refCount;
    void *data;
} XmlNode;

typedef struct _XmlIndex
{
    char *attr;
    int num_nodes;
    int alloc_nodes;
    int cur_node;
    XmlNode **nodes;
} XmlIndex;

typedef int (*cb_xml_custom_load)(XmlNode *, const char *);
typedef char *(*cb_xml_custom_save)(XmlNode *);
typedef XmlType(*cb_xml_load)(XmlNode *);
typedef const char *(*cb_xml_save)(XmlNode *, int);
typedef void (*cb_xml_sax)(XmlNode *, XmlSaxEvent, void *);

extern void  xml_add(XmlNode *parent, int where, XmlNode *child, XmlNode *node);
extern void  xml_delete(XmlNode *node);

extern const char *xml_element_get_attr(XmlNode *node, const char *name);
extern void  xml_element_set_attr(XmlNode *node, const char *name,
                                      const char *value);


extern const char *xml_entity_get_name(int val);

extern int  xml_entity_get_value(const char *name);


extern XmlNode *xml_load_file(XmlNode *top, FILE *fp,
                                  XmlType(*cb)(XmlNode *));
extern XmlNode *xml_load_string(XmlNode *top, const char *s,
                                    XmlType(*cb)(XmlNode *));
extern XmlNode *xml_new_custom(XmlNode *parent, void *data,
                                   cb_xml_custom_destroy destroy);
extern XmlNode *xml_new_element(XmlNode *parent, const char *name);
extern XmlNode *xml_new_integer(XmlNode *parent, int integer);
extern XmlNode *xml_new_opaque(XmlNode *parent, const char *opaque);
extern XmlNode *xml_new_real(XmlNode *parent, double real);
extern XmlNode *xml_new_text(XmlNode *parent, int whitespace,
                                 const char *string);


extern int  xml_release(XmlNode *node);
extern void xml_remove(XmlNode *node);

extern int  xml_save_file(XmlNode *node, FILE *fp,
                              cb_xml_save cb);


extern void  xml_set_wrap_margin(int column);
extern XmlNode *xml_walk_next(XmlNode *node, XmlNode *top,
                                  int descend);
extern XmlNode *xml_walk_prev(XmlNode *node, XmlNode *top,
                                  int descend);

extern void  xml_error(const char *format, ...);
extern XmlType xml_ignore_cb(XmlNode *node);
extern XmlType xml_integer_cb(XmlNode *node);
extern XmlType xml_opaque_cb(XmlNode *node);
extern XmlType xml_real_cb(XmlNode *node);

#endif
