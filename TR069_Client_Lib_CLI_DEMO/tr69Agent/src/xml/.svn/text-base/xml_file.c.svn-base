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
File name: xml_file.c
Author: kiffen guan
Date: April 1th. 2009
************************************************************************/



#include "xml_private.h"
#include <unistd.h>

#define ENCODE_UTF8 0
#define ENCODE_UTF16BE 1
#define ENCODE_UTF16LE 2

#define xml_bad_char(ch) ((ch) < ' ' && (ch) != '\n' && (ch) != '\r' && (ch) != '\t')

typedef int (*cb_xml_getc)(void *, int *);
typedef int (*cb_xml_putc)(int, void *);

typedef struct _XmlFdBuf
{
    int fd;
    unsigned char *current;
    unsigned char *end;
    unsigned char buffer[8192];
} XmlFdBuf;

static int  xml_add_char(int ch, char **ptr, char **buffer, int *bufsize);
static int  xml_fd_getc(void *p, int *encoding);
static int  xml_fd_putc(int ch, void *p);
static int  xml_fd_read(XmlFdBuf *buf);
static int  xml_fd_write(XmlFdBuf *buf);
static int  xml_file_getc(void *p, int *encoding);
static int  xml_file_putc(int ch, void *p);
static int  xml_get_entity(XmlNode *parent, void *p, int *encoding, cb_xml_getc getc_cb);

static  int xml_isspace(int ch)
{
    return (ch == ' ' || ch == '\t' || ch == '\r' ||
            ch == '\n');
}

static XmlNode *xml_load_data(XmlNode *top, void *p, cb_xml_load cb, cb_xml_getc getc_cb, cb_xml_sax sax_cb, void *sax_data);
static int  xml_parse_element(XmlNode *node, void *p, int *encoding, cb_xml_getc getc_cb);
static int  xml_string_getc(void *p, int *encoding);
static int  xml_string_putc(int ch, void *p);
static int  xml_write_name(const char *s, void *p, cb_xml_putc putc_cb);
static int  xml_write_node(XmlNode *node, void *p, cb_xml_save cb, int col, cb_xml_putc putc_cb, XmlGlobal *global);
static int  xml_write_string(const char *s, void *p, cb_xml_putc putc_cb);
static int  xml_write_ws(XmlNode *node, void *p, cb_xml_save cb, int ws, int col, cb_xml_putc putc_cb);

XmlNode *xml_load_file(XmlNode *top, FILE *fp,  cb_xml_load cb)
{
    return (xml_load_data(top, fp, cb, xml_file_getc, XML_NO_CALLBACK, NULL));
}

XmlNode *xml_load_string(XmlNode *top, const char *s, cb_xml_load cb)
{
    return (xml_load_data(top, (void *)&s, cb, xml_string_getc, XML_NO_CALLBACK, NULL));
}

int xml_save_file(XmlNode *node, FILE *fp,  cb_xml_save cb)
{
    int col;
    XmlGlobal *global = xml_global();

    if ((col = xml_write_node(node, fp, cb, 0, xml_file_putc, global)) < 0)
    {
        return (-1);
    }

    if (col > 0)
    {
        if (putc('\n', fp) < 0)
        {
            return (-1);
        }
    }

    return (0);
}

void xml_set_wrap_margin(int column)
{
    XmlGlobal *global = xml_global();

    if (column <= 0)
        global->wrap = 2147483647;
    else
        global->wrap = column;
}

static int xml_add_char(int  ch, char **bufptr, char **buffer, int  *bufsize)
{
    char *newbuffer;

    if (*bufptr >= (*buffer + *bufsize - 4))
    {
        if (*bufsize < 1024)
        {
            (*bufsize) *= 2;
        }
        else
        {
            (*bufsize) += 1024;
        }

        if ((newbuffer = realloc(*buffer, *bufsize)) == NULL)
        {
            free(*buffer);

            xml_error("Unable to expand string buffer to %d bytes!", *bufsize);

            return (-1);
        }

        *bufptr = newbuffer + (*bufptr - *buffer);
        *buffer = newbuffer;
    }

    if (ch < 0x80)
    {
        *(*bufptr)++ = ch;
    }
    else if (ch < 0x800)
    {
        *(*bufptr)++ = 0xc0 | (ch >> 6);
        *(*bufptr)++ = 0x80 | (ch & 0x3f);
    }
    else if (ch < 0x10000)
    {
        *(*bufptr)++ = 0xe0 | (ch >> 12);
        *(*bufptr)++ = 0x80 | ((ch >> 6) & 0x3f);
        *(*bufptr)++ = 0x80 | (ch & 0x3f);
    }
    else
    {
        *(*bufptr)++ = 0xf0 | (ch >> 18);
        *(*bufptr)++ = 0x80 | ((ch >> 12) & 0x3f);
        *(*bufptr)++ = 0x80 | ((ch >> 6) & 0x3f);
        *(*bufptr)++ = 0x80 | (ch & 0x3f);
    }

    return (0);
}

static int xml_fd_getc(void *p, int  *encoding)
{
    XmlFdBuf *buf;
    int  ch, temp;

    buf = (XmlFdBuf *)p;

    if (buf->current >= buf->end)
    {
        if (xml_fd_read(buf) < 0)
        {
            return (EOF);
        }
    }

    ch = *(buf->current)++;

    switch (*encoding)
    {
        case ENCODE_UTF8 :
            if (!(ch & 0x80))
            {
                if (xml_bad_char(ch))
                {
                    xml_error("Bad control character 0x%02x not allowed by XML standard!", ch);

                    return (EOF);
                }

                return (ch);
            }
            else if (ch == 0xfe)
            {
                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                ch = *(buf->current)++;

                if (ch != 0xff)
                {
                    return (EOF);
                }

                *encoding = ENCODE_UTF16BE;

                return (xml_fd_getc(p, encoding));
            }
            else if (ch == 0xff)
            {
                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                ch = *(buf->current)++;

                if (ch != 0xfe)
                {
                    return (EOF);
                }

                *encoding = ENCODE_UTF16LE;

                return (xml_fd_getc(p, encoding));
            }
            else if ((ch & 0xe0) == 0xc0)
            {
                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                if ((temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = ((ch & 0x1f) << 6) | (temp & 0x3f);

                if (ch < 0x80)
                {
                    return (EOF);
                }
            }
            else if ((ch & 0xf0) == 0xe0)
            {
                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                if ((temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = ((ch & 0x0f) << 6) | (temp & 0x3f);

                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                if ((temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = (ch << 6) | (temp & 0x3f);

                if (ch < 0x800)
                {
                    return (EOF);
                }
            }
            else if ((ch & 0xf8) == 0xf0)
            {
                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                if ((temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = ((ch & 0x07) << 6) | (temp & 0x3f);

                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                if ((temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = (ch << 6) | (temp & 0x3f);

                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                if ((temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = (ch << 6) | (temp & 0x3f);

                if (ch < 0x10000)
                {
                    return (EOF);
                }
            }
            else
            {
                return (EOF);
            }
            break;

        case ENCODE_UTF16BE :
            if (buf->current >= buf->end)
            {
                if (xml_fd_read(buf) < 0)
                {
                    return (EOF);
                }
            }

            temp = *(buf->current)++;

            ch = (ch << 8) | temp;

            if (xml_bad_char(ch))
            {
                xml_error("Bad control character 0x%02x not allowed by XML standard!", ch);
                return (EOF);
            }
            else if (ch >= 0xd800 && ch <= 0xdbff)
            {
                int lch;

                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                lch = *(buf->current)++;

                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                lch = (lch << 8) | temp;

                if (lch < 0xdc00 || lch >= 0xdfff)
                {
                    return (EOF);
                }

                ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
            }
            break;

        case ENCODE_UTF16LE :
            if (buf->current >= buf->end)
            {
                if (xml_fd_read(buf) < 0)
                {
                    return (EOF);
                }
            }

            temp = *(buf->current)++;

            ch |= (temp << 8);

            if (xml_bad_char(ch))
            {
                xml_error("Bad control character 0x%02x not allowed by XML standard!", ch);
                return (EOF);
            }
            else if (ch >= 0xd800 && ch <= 0xdbff)
            {
                int lch;

                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                lch = *(buf->current)++;

                if (buf->current >= buf->end)
                {
                    if (xml_fd_read(buf) < 0)
                    {
                        return (EOF);
                    }
                }

                temp = *(buf->current)++;

                lch |= (temp << 8);

                if (lch < 0xdc00 || lch >= 0xdfff)
                {
                    return (EOF);
                }

                ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
            }
            break;
    }

    return (ch);
}

static int xml_fd_putc(int  ch, void *p)
{
    XmlFdBuf *buf;

    buf = (XmlFdBuf *)p;

    if (buf->current >= buf->end)
    {
        if (xml_fd_write(buf) < 0)
        {
            return (-1);
        }
    }

    if (ch < 0x80)
    {
        *(buf->current)++ = ch;
    }
    else if (ch < 0x800)
    {
        *(buf->current)++ = 0xc0 | (ch >> 6);
        *(buf->current)++ = 0x80 | (ch & 0x3f);
    }
    else if (ch < 0x10000)
    {
        *(buf->current)++ = 0xe0 | (ch >> 12);
        *(buf->current)++ = 0x80 | ((ch >> 6) & 0x3f);
        *(buf->current)++ = 0x80 | (ch & 0x3f);
    }
    else
    {
        *(buf->current)++ = 0xf0 | (ch >> 18);
        *(buf->current)++ = 0x80 | ((ch >> 12) & 0x3f);
        *(buf->current)++ = 0x80 | ((ch >> 6) & 0x3f);
        *(buf->current)++ = 0x80 | (ch & 0x3f);
    }

    return (0);
}

static int xml_fd_read(XmlFdBuf *buf)
{
    int bytes;

    if (!buf)
    {
        return (-1);
    }

    while ((bytes = read(buf->fd, buf->buffer, sizeof(buf->buffer))) < 0)
#ifdef EINTR
        if (errno != EAGAIN && errno != EINTR)
#else
        if (errno != EAGAIN)
#endif
            return (-1);

    if (bytes == 0)
    {
        return (-1);
    }

    buf->current = buf->buffer;
    buf->end     = buf->buffer + bytes;

    return (0);
}


static int  xml_fd_write(XmlFdBuf *buf)
{
    int  bytes;
    unsigned char *ptr;

    if (!buf)
    {
        return (-1);
    }

    if (buf->current == buf->buffer)
    {
        return (0);
    }

    for (ptr = buf->buffer; ptr < buf->current; ptr += bytes)
    {
        if ((bytes = write(buf->fd, ptr, buf->current - ptr)) < 0)
        {
            return (-1);
        }
    }

    buf->current = buf->buffer;

    return (0);
}

static int  xml_file_getc(void *p, int  *encoding)
{
    int ch,
    temp;
    FILE *fp;

    fp = (FILE *)p;
    ch = getc(fp);

    if (ch == EOF)
    {
        return (EOF);
    }

    switch (*encoding)
    {
        case ENCODE_UTF8 :
            if (!(ch & 0x80))
            {
                if (xml_bad_char(ch))
                {
                    xml_error("Bad control character 0x%02x not allowed by XML standard!", ch);
                    return (EOF);
                }

                return (ch);
            }
            else if (ch == 0xfe)
            {
                ch = getc(fp);
                if (ch != 0xff)
                {
                    return (EOF);
                }

                *encoding = ENCODE_UTF16BE;

                return (xml_file_getc(p, encoding));
            }
            else if (ch == 0xff)
            {
                ch = getc(fp);
                if (ch != 0xfe)
                {
                    return (EOF);
                }

                *encoding = ENCODE_UTF16LE;

                return (xml_file_getc(p, encoding));
            }
            else if ((ch & 0xe0) == 0xc0)
            {
                if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = ((ch & 0x1f) << 6) | (temp & 0x3f);

                if (ch < 0x80)
                {
                    return (EOF);
                }
            }
            else if ((ch & 0xf0) == 0xe0)
            {
                if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = ((ch & 0x0f) << 6) | (temp & 0x3f);

                if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = (ch << 6) | (temp & 0x3f);

                if (ch < 0x800)
                {
                    return (EOF);
                }
            }
            else if ((ch & 0xf8) == 0xf0)
            {
                if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = ((ch & 0x07) << 6) | (temp & 0x3f);

                if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = (ch << 6) | (temp & 0x3f);

                if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
                {
                    return (EOF);
                }

                ch = (ch << 6) | (temp & 0x3f);

                if (ch < 0x10000)
                {
                    return (EOF);
                }
            }
            else
            {
                return (EOF);
            }
            break;

        case ENCODE_UTF16BE :
            ch = (ch << 8) | getc(fp);

            if (xml_bad_char(ch))
            {
                xml_error("Bad control character 0x%02x not allowed by XML standard!", ch);

                return (EOF);
            }
            else if (ch >= 0xd800 && ch <= 0xdbff)
            {
                int lch = (getc(fp) << 8) | getc(fp);

                if (lch < 0xdc00 || lch >= 0xdfff)
                {
                    return (EOF);
                }

                ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
            }
            break;

        case ENCODE_UTF16LE :
            ch |= (getc(fp) << 8);

            if (xml_bad_char(ch))
            {
                xml_error("Bad control character 0x%02x not allowed by XML standard!",
                          ch);
                return (EOF);
            }
            else if (ch >= 0xd800 && ch <= 0xdbff)
            {
                int lch = getc(fp) | (getc(fp) << 8);

                if (lch < 0xdc00 || lch >= 0xdfff)
                {
                    return (EOF);
                }

                ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
            }
            break;
    }

    return (ch);
}

static int xml_file_putc(int  ch, void *p)
{
    char buffer[4],
    *bufptr;
    int buflen;

    if (ch < 0x80)
    {
        return (putc(ch, (FILE *)p) == EOF ? -1 : 0);
    }

    bufptr = buffer;

    if (ch < 0x800)
    {
        *bufptr++ = 0xc0 | (ch >> 6);
        *bufptr++ = 0x80 | (ch & 0x3f);
    }
    else if (ch < 0x10000)
    {
        *bufptr++ = 0xe0 | (ch >> 12);
        *bufptr++ = 0x80 | ((ch >> 6) & 0x3f);
        *bufptr++ = 0x80 | (ch & 0x3f);
    }
    else
    {
        *bufptr++ = 0xf0 | (ch >> 18);
        *bufptr++ = 0x80 | ((ch >> 12) & 0x3f);
        *bufptr++ = 0x80 | ((ch >> 6) & 0x3f);
        *bufptr++ = 0x80 | (ch & 0x3f);
    }

    buflen = bufptr - buffer;

    return (fwrite(buffer, 1, buflen, (FILE *)p) < buflen ? -1 : 0);
}

static int xml_get_entity(XmlNode *parent, void *p, int *encoding, int (*getc_cb)(void *, int *))
{
    int ch;
    char entity[64],  *entptr;

    entptr = entity;

    while ((ch = (*getc_cb)(p, encoding)) != EOF)
    {
        if (ch > 126 || (!isalnum(ch) && ch != '#'))
        {
            break;
        }
        else if (entptr < (entity + sizeof(entity) - 1))
        {
            *entptr++ = ch;
        }
        else
        {
            xml_error("Entity name too long under parent <%s>!",
                      parent ? parent->value.element.name : "null");
            break;
        }
    }

    *entptr = '\0';

    if (ch != ';')
    {
        xml_error("Character entity \"%s\" not terminated under parent <%s>!", entity, parent ? parent->value.element.name : "null");
        return (EOF);
    }

    if (entity[0] == '#')
    {
        if (entity[1] == 'x')
        {
            ch = strtol(entity + 2, NULL, 16);
        }
        else
        {
            ch = strtol(entity + 1, NULL, 10);
        }
    }
    else if ((ch = xml_entity_get_value(entity)) < 0)
    {
        xml_error("Entity name \"%s;\" not supported under parent <%s>!", entity, parent ? parent->value.element.name : "null");
    }

    if (xml_bad_char(ch))
    {
        xml_error("Bad control character 0x%02x under parent <%s> not allowed by XML standard!", ch, parent ? parent->value.element.name : "null");

        return (EOF);
    }

    return (ch);
}

static XmlNode *xml_load_data(XmlNode *top, void *p, cb_xml_load  cb,  cb_xml_getc getc_cb,  cb_xml_sax   sax_cb,  void *sax_data)
{
    XmlNode *node, *first, *parent;
    int  ch , whitespace;
    char  *buffer, *bufptr;
    int  bufsize;
    XmlType type;
    int  encoding;
    XmlGlobal *global = xml_global();

    static const char * const types[] =
    {
        "XML_ELEMENT",
        "XML_INTEGER",
        "XML_OPAQUE",
        "XML_REAL",
        "XML_TEXT",
        "XML_CUSTOM"
    };

    if ((buffer = malloc(64)) == NULL)
    {
        xml_error("Unable to allocate string buffer!");
        return (NULL);
    }

    bufsize    = 64;
    bufptr     = buffer;
    parent     = top;
    first      = NULL;
    whitespace = 0;
    encoding   = ENCODE_UTF8;

    if (cb && parent)
    {
        type = (*cb)(parent);
    }
    else
    {
        type = XML_TEXT;
    }

    while ((ch = (*getc_cb)(p, &encoding)) != EOF)
    {
        if ((ch == '<' || (xml_isspace(ch) && type != XML_OPAQUE && type != XML_CUSTOM)) &&
                bufptr > buffer)
        {
            *bufptr = '\0';

            switch (type)
            {
                case XML_INTEGER :
                    node = xml_new_integer(parent, strtol(buffer, &bufptr, 0));
                    break;

                case XML_OPAQUE :
                    node = xml_new_opaque(parent, buffer);
                    break;

                case XML_REAL :
                    node = xml_new_real(parent, strtod(buffer, &bufptr));
                    break;

                case XML_TEXT :
                    node = xml_new_text(parent, whitespace, buffer);
                    break;

                case XML_CUSTOM :
                    if (global->cbCustomLoad)
                    {
                        node = xml_new_custom(parent, NULL, NULL);

                        if ((*global->cbCustomLoad)(node, buffer))
                        {
                            xml_error("Bad custom value '%s' in parent <%s>!", buffer, parent ? parent->value.element.name : "null");
                            xml_delete(node);
                            node = NULL;
                        }
                        break;
                    }

                default :
                    node = NULL;
                    break;
            }

            if (*bufptr)
            {
                xml_error("Bad %s value '%s' in parent <%s>!", type == XML_INTEGER ? "integer" : "real", buffer,
                          parent ? parent->value.element.name : "null");
                break;
            }

            bufptr     = buffer;
            whitespace = xml_isspace(ch) && type == XML_TEXT;

            if (!node && type != XML_IGNORE)
            {
                xml_error("Unable to add value node of type %s to parent <%s>!", types[type], parent ? parent->value.element.name : "null");
                goto error;
            }

            if (sax_cb)
            {
                (*sax_cb)(node, XML_SAX_COMMENT, sax_data);

                if (!xml_release(node))
                {
                    node = NULL;
                }
            }

            if (!first && node)
            {
                first = node;
            }
        }
        else if (xml_isspace(ch) && type == XML_TEXT)
        {
            whitespace = 1;
        }

        if (ch == '<' && whitespace && type == XML_TEXT)
        {
            node = xml_new_text(parent, whitespace, "");

            if (sax_cb)
            {
                (*sax_cb)(node, XML_SAX_COMMENT, sax_data);

                if (!xml_release(node))
                {
                    node = NULL;
                }
            }

            if (!first && node)
            {
                first = node;
            }

            whitespace = 0;
        }

        if (ch == '<')
        {
            bufptr = buffer;

            while ((ch = (*getc_cb)(p, &encoding)) != EOF)
            {
                if (xml_isspace(ch) || ch == '>' || (ch == '/' && bufptr > buffer))
                {
                    break;
                }
                else if (ch == '<')
                {
                    xml_error("Bare < in element!");
                    goto error;
                }
                else if (ch == '&')
                {
                    if ((ch = xml_get_entity(parent, p, &encoding, getc_cb)) == EOF)
                    {
                        goto error;
                    }

                    if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
                    {
                        goto error;
                    }
                }
                else if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
                {
                    goto error;
                }
                else if (((bufptr - buffer) == 1 && buffer[0] == '?') ||
                         ((bufptr - buffer) == 3 && !strncmp(buffer, "!--", 3)) ||
                         ((bufptr - buffer) == 8 && !strncmp(buffer, "![CDATA[", 8)))
                {
                    break;
                }
            }

            *bufptr = '\0';

            if (!strcmp(buffer, "!--"))
            {
                while ((ch = (*getc_cb)(p, &encoding)) != EOF)
                {
                    if (ch == '>' && bufptr > (buffer + 4) &&
                            bufptr[-3] != '-' && bufptr[-2] == '-' && bufptr[-1] == '-')
                    {
                        break;
                    }
                    else if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
                    {
                        goto error;
                    }
                }

                if (ch != '>')
                {
                    xml_error("Early EOF in comment node!");
                    goto error;
                }

                *bufptr = '\0';

                if ((node = xml_new_element(parent, buffer)) == NULL)
                {
                    xml_error("Unable to add comment node to parent <%s>!",
                              parent ? parent->value.element.name : "null");
                    break;
                }

                if (sax_cb)
                {
                    (*sax_cb)(node, XML_SAX_COMMENT, sax_data);

                    if (!xml_release(node))
                    {
                        node = NULL;
                    }
                }

                if (node && !first)
                {
                    first = node;
                }
            }
            else if (!strcmp(buffer, "![CDATA["))
            {
                while ((ch = (*getc_cb)(p, &encoding)) != EOF)
                {
                    if (ch == '>' && !strncmp(bufptr - 2, "]]", 2))
                    {
                        break;
                    }
                    else if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
                    {
                        goto error;
                    }
                }

                if (ch != '>')
                {
                    xml_error("Early EOF in CDATA node!");
                    goto error;
                }

                *bufptr = '\0';

                if ((node = xml_new_element(parent, buffer)) == NULL)
                {
                    xml_error("Unable to add CDATA node to parent <%s>!",
                              parent ? parent->value.element.name : "null");
                    goto error;
                }

                if (sax_cb)
                {
                    (*sax_cb)(node, XML_SAX_CDATA, sax_data);

                    if (!xml_release(node))
                    {
                        node = NULL;
                    }
                }

                if (node && !first)
                {
                    first = node;
                }
            }
            else if (buffer[0] == '?')
            {
                while ((ch = (*getc_cb)(p, &encoding)) != EOF)
                {
                    if (ch == '>' && bufptr > buffer && bufptr[-1] == '?')
                    {
                        break;
                    }
                    else if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
                    {
                        goto error;
                    }
                }

                if (ch != '>')
                {
                    xml_error("Early EOF in processing instruction node!");
                    goto error;
                }

                *bufptr = '\0';

                if ((node = xml_new_element(parent, buffer)) == NULL)
                {
                    xml_error("Unable to add processing instruction node to parent <%s>!",
                              parent ? parent->value.element.name : "null");
                    goto error;
                }

                if (sax_cb)
                {
                    (*sax_cb)(node, XML_SAX_DIRECTIVE, sax_data);

                    if (!xml_release(node))
                    {
                        node = NULL;
                    }
                }

                if (node)
                {
                    if (!first)
                    {
                        first = node;
                    }

                    if (!parent)
                    {
                        parent = node;

                        if (cb)
                        {
                            type = (*cb)(parent);
                        }
                    }
                }
            }
            else if (buffer[0] == '!')
            {
                do
                {
                    if (ch == '>')
                    {
                        break;
                    }
                    else
                    {
                        if (ch == '&')
                        {
                            if ((ch = xml_get_entity(parent, p, &encoding, getc_cb)) == EOF)
                            {
                                goto error;
                            }
                        }

                        if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
                        {
                            goto error;
                        }
                    }
                }
                while ((ch = (*getc_cb)(p, &encoding)) != EOF);

                if (ch != '>')
                {
                    xml_error("Early EOF in declaration node!");
                    goto error;
                }

                *bufptr = '\0';

                if ((node = xml_new_element(parent, buffer)) == NULL)
                {
                    xml_error("Unable to add declaration node to parent <%s>!",
                              parent ? parent->value.element.name : "null");
                    goto error;
                }

                if (sax_cb)
                {
                    (*sax_cb)(node, XML_SAX_DIRECTIVE, sax_data);

                    if (!xml_release(node))
                    {
                        node = NULL;
                    }
                }

                if (node)
                {
                    if (!first)
                    {
                        first = node;
                    }

                    if (!parent)
                    {
                        parent = node;

                        if (cb)
                        {
                            type = (*cb)(parent);
                        }
                    }
                }
            }
            else if (buffer[0] == '/')
            {
                if (!parent || strcmp(buffer + 1, parent->value.element.name))
                {
                    xml_error("Mismatched close tag <%s> under parent <%s>!",
                              buffer, parent ? parent->value.element.name : "(null)");
                    goto error;
                }

                while (ch != '>' && ch != EOF)
                {
                    ch = (*getc_cb)(p, &encoding);
                }

                node   = parent;
                parent = parent->parent;

                if (sax_cb)
                {
                    (*sax_cb)(node, XML_SAX_ELEMENT_CLOSE, sax_data);

                    xml_release(node);
                }

                if (cb && parent)
                {
                    type = (*cb)(parent);
                }
            }
            else
            {
                if ((node = xml_new_element(parent, buffer)) == NULL)
                {
                    xml_error("Unable to add element node to parent <%s>!",
                              parent ? parent->value.element.name : "null");
                    goto error;
                }

                if (xml_isspace(ch))
                {
                    if ((ch = xml_parse_element(node, p, &encoding, getc_cb)) == EOF)
                    {
                        goto error;
                    }
                }
                else if (ch == '/')
                {
                    if ((ch = (*getc_cb)(p, &encoding)) != '>')
                    {
                        xml_error("Expected > but got '%c' instead for element <%s/>!",
                                  ch, buffer);
                        xml_delete(node);
                        goto error;
                    }

                    ch = '/';
                }

                if (sax_cb)
                {
                    (*sax_cb)(node, XML_SAX_ELEMENT_OPEN, sax_data);
                }

                if (!first)
                {
                    first = node;
                }

                if (ch == EOF)
                {
                    break;
                }

                if (ch != '/')
                {
                    parent = node;

                    if (cb && parent)
                    {
                        type = (*cb)(parent);
                    }
                }
                else if (sax_cb)
                {
                    (*sax_cb)(node, XML_SAX_ELEMENT_CLOSE, sax_data);

                    if (!xml_release(node) && first == node)
                    {
                        first = NULL;
                    }
                }
            }

            bufptr  = buffer;
        }
        else if (ch == '&')
        {
            if ((ch = xml_get_entity(parent, p, &encoding, getc_cb)) == EOF)
            {
                goto error;
            }

            if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
            {
                goto error;
            }
        }
        else if (type == XML_OPAQUE || type == XML_CUSTOM || !xml_isspace(ch))
        {
            if (xml_add_char(ch, &bufptr, &buffer, &bufsize))
            {
                goto error;
            }
        }
    }

    free(buffer);

    if (parent)
    {
        node = parent;

        while (parent->parent != top && parent->parent)
            parent = parent->parent;

        if (node != parent)
        {
            xml_error("Missing close tag </%s> under parent <%s>!",
                      node->value.element.name,
                      node->parent ? node->parent->value.element.name : "(null)");

            xml_delete(first);

            return (NULL);
        }
    }

    if (parent)
    {
        return (parent);
    }
    else
    {
        return (first);
    }

error:

    xml_delete(first);

    free(buffer);

    return (NULL);
}

static int xml_parse_element(XmlNode     *node,  void *p, int *encoding, cb_xml_getc getc_cb)
{
    int ch, quote;
    char *name,  *value, *ptr;
    int namesize, valsize;

    if ((name = malloc(64)) == NULL)
    {
        xml_error("Unable to allocate memory for name!");
        return (EOF);
    }

    namesize = 64;

    if ((value = malloc(64)) == NULL)
    {
        free(name);
        xml_error("Unable to allocate memory for value!");
        return (EOF);
    }

    valsize = 64;

    while ((ch = (*getc_cb)(p, encoding)) != EOF)
    {
        if (xml_isspace(ch))
            continue;

        if (ch == '/' || ch == '?')
        {
            quote = (*getc_cb)(p, encoding);

            if (quote != '>')
            {
                xml_error("Expected '>' after '%c' for element %s, but got '%c'!",
                          ch, node->value.element.name, quote);
                goto error;
            }

            break;
        }
        else if (ch == '<')
        {
            xml_error("Bare < in element %s!", node->value.element.name);
            goto error;
        }
        else if (ch == '>')
        {
            break;
        }

        name[0] = ch;
        ptr     = name + 1;

        if (ch == '\"' || ch == '\'')
        {
            quote = ch;

            while ((ch = (*getc_cb)(p, encoding)) != EOF)
            {
                if (ch == '&')
                {
                    if ((ch = xml_get_entity(node, p, encoding, getc_cb)) == EOF)
                    {
                        goto error;
                    }
                }

                if (xml_add_char(ch, &ptr, &name, &namesize))
                {
                    goto error;
                }

                if (ch == quote)
                {
                    break;
                }
            }
        }
        else
        {
            while ((ch = (*getc_cb)(p, encoding)) != EOF)
            {
                if (xml_isspace(ch) || ch == '=' || ch == '/' || ch == '>' || ch == '?')
                {
                    break;
                }
                else
                {
                    if (ch == '&')
                    {
                        if ((ch = xml_get_entity(node, p, encoding, getc_cb)) == EOF)
                        {
                            goto error;
                        }
                    }

                    if (xml_add_char(ch, &ptr, &name, &namesize))
                    {
                        goto error;
                    }
                }
            }
        }

        *ptr = '\0';

        if (xml_element_get_attr(node, name))
        {
            goto error;
        }

        while (ch != EOF && xml_isspace(ch))
        {
            ch = (*getc_cb)(p, encoding);
        }

        if (ch == '=')
        {
            while ((ch = (*getc_cb)(p, encoding)) != EOF && xml_isspace(ch));

            if (ch == EOF)
            {
                xml_error("Missing value for attribute '%s' in element %s!",
                          name, node->value.element.name);
                goto error;
            }

            if (ch == '\'' || ch == '\"')
            {
                quote = ch;
                ptr   = value;

                while ((ch = (*getc_cb)(p, encoding)) != EOF)
                {
                    if (ch == quote)
                    {
                        break;
                    }
                    else
                    {
                        if (ch == '&')
                        {
                            if ((ch = xml_get_entity(node, p, encoding, getc_cb)) == EOF)
                            {
                                goto error;
                            }
                        }

                        if (xml_add_char(ch, &ptr, &value, &valsize))
                        {
                            goto error;
                        }
                    }
                }

                *ptr = '\0';
            }
            else
            {
                value[0] = ch;
                ptr      = value + 1;

                while ((ch = (*getc_cb)(p, encoding)) != EOF)
                {
                    if (xml_isspace(ch) || ch == '=' || ch == '/' || ch == '>')
                    {
                        break;
                    }
                    else
                    {
                        if (ch == '&')
                        {
                            if ((ch = xml_get_entity(node, p, encoding, getc_cb)) == EOF)
                            {
                                goto error;
                            }
                        }

                        if (xml_add_char(ch, &ptr, &value, &valsize))
                        {
                            goto error;
                        }
                    }
                }

                *ptr = '\0';
            }

            xml_element_set_attr(node, name, value);
        }
        else
        {
            xml_error("Missing value for attribute '%s' in element %s!",
                      name, node->value.element.name);
            goto error;
        }

        if (ch == '/' || ch == '?')
        {
            quote = (*getc_cb)(p, encoding);

            if (quote != '>')
            {
                xml_error("Expected '>' after '%c' for element %s, but got '%c'!",
                          ch, node->value.element.name, quote);
                ch = EOF;
            }

            break;
        }
        else if (ch == '>')
            break;
    }

    free(name);
    free(value);

    return (ch);


error:

    free(name);
    free(value);

    return (EOF);
}

static int  xml_string_getc(void *p,  int  *encoding)
{
    int  ch;
    const char **s;

    s = (const char **)p;

    if ((ch = (*s)[0] & 255) != 0 || *encoding == ENCODE_UTF16LE)
    {
        (*s)++;

        switch (*encoding)
        {
            case ENCODE_UTF8 :
                if (!(ch & 0x80))
                {
                    if (xml_bad_char(ch))
                    {
                        xml_error("Bad control character 0x%02x not allowed by XML standard!", ch);
                        return (EOF);
                    }

                    return (ch);
                }
                else if (ch == 0xfe)
                {
                    if (((*s)[0] & 255) != 0xff)
                    {
                        return (EOF);
                    }

                    *encoding = ENCODE_UTF16BE;
                    (*s)++;

                    return (xml_string_getc(p, encoding));
                }
                else if (ch == 0xff)
                {
                    if (((*s)[0] & 255) != 0xfe)
                    {
                        return (EOF);
                    }

                    *encoding = ENCODE_UTF16LE;
                    (*s)++;

                    return (xml_string_getc(p, encoding));
                }
                else if ((ch & 0xe0) == 0xc0)
                {
                    if (((*s)[0] & 0xc0) != 0x80)
                    {
                        return (EOF);
                    }

                    ch = ((ch & 0x1f) << 6) | ((*s)[0] & 0x3f);

                    (*s)++;

                    if (ch < 0x80)
                    {
                        return (EOF);
                    }

                    return (ch);
                }
                else if ((ch & 0xf0) == 0xe0)
                {

                    if (((*s)[0] & 0xc0) != 0x80 ||
                            ((*s)[1] & 0xc0) != 0x80)
                    {
                        return (EOF);
                    }

                    ch = ((((ch & 0x0f) << 6) | ((*s)[0] & 0x3f)) << 6) | ((*s)[1] & 0x3f);

                    (*s) += 2;

                    if (ch < 0x800)
                    {
                        return (EOF);
                    }

                    return (ch);
                }
                else if ((ch & 0xf8) == 0xf0)
                {
                    if (((*s)[0] & 0xc0) != 0x80 ||
                            ((*s)[1] & 0xc0) != 0x80 ||
                            ((*s)[2] & 0xc0) != 0x80)
                    {
                        return (EOF);
                    }

                    ch = ((((((ch & 0x07) << 6) | ((*s)[0] & 0x3f)) << 6) |
                           ((*s)[1] & 0x3f)) << 6) | ((*s)[2] & 0x3f);

                    (*s) += 3;

                    if (ch < 0x10000)
                    {
                        return (EOF);
                    }

                    return (ch);
                }
                else
                {
                    return (EOF);
                }

            case ENCODE_UTF16BE :
                ch = (ch << 8) | ((*s)[0] & 255);
                (*s) ++;

                if (xml_bad_char(ch))
                {
                    xml_error("Bad control character 0x%02x not allowed by XML standard!",
                              ch);
                    return (EOF);
                }
                else if (ch >= 0xd800 && ch <= 0xdbff)
                {
                    int lch;

                    if (!(*s)[0])
                    {
                        return (EOF);
                    }

                    lch = (((*s)[0] & 255) << 8) | ((*s)[1] & 255);
                    (*s) += 2;

                    if (lch < 0xdc00 || lch >= 0xdfff)
                    {
                        return (EOF);
                    }

                    ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
                }

                return (ch);

            case ENCODE_UTF16LE :
                ch = ch | (((*s)[0] & 255) << 8);

                if (!ch)
                {
                    (*s) --;
                    return (EOF);
                }

                (*s) ++;

                if (xml_bad_char(ch))
                {
                    xml_error("Bad control character 0x%02x not allowed by XML standard!",
                              ch);
                    return (EOF);
                }
                else if (ch >= 0xd800 && ch <= 0xdbff)
                {
                    int lch;

                    if (!(*s)[1])
                    {
                        return (EOF);
                    }

                    lch = (((*s)[1] & 255) << 8) | ((*s)[0] & 255);
                    (*s) += 2;

                    if (lch < 0xdc00 || lch >= 0xdfff)
                    {
                        return (EOF);
                    }

                    ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
                }

                return (ch);
        }
    }

    return (EOF);
}

static int xml_string_putc(int  ch, void *p)
{
    char **pp;

    pp = (char **)p;

    if (ch < 0x80)
    {
        if (pp[0] < pp[1])
            pp[0][0] = ch;

        pp[0] ++;
    }
    else if (ch < 0x800)
    {
        if ((pp[0] + 1) < pp[1])
        {
            pp[0][0] = 0xc0 | (ch >> 6);
            pp[0][1] = 0x80 | (ch & 0x3f);
        }

        pp[0] += 2;
    }
    else if (ch < 0x10000)
    {
        if ((pp[0] + 2) < pp[1])
        {
            pp[0][0] = 0xe0 | (ch >> 12);
            pp[0][1] = 0x80 | ((ch >> 6) & 0x3f);
            pp[0][2] = 0x80 | (ch & 0x3f);
        }

        pp[0] += 3;
    }
    else
    {
        if ((pp[0] + 2) < pp[1])
        {
            pp[0][0] = 0xf0 | (ch >> 18);
            pp[0][1] = 0x80 | ((ch >> 12) & 0x3f);
            pp[0][2] = 0x80 | ((ch >> 6) & 0x3f);
            pp[0][3] = 0x80 | (ch & 0x3f);
        }

        pp[0] += 4;
    }

    return (0);
}

static int xml_write_name(const char *s, void *p,  int (*putc_cb)(int, void *))
{
    char  quote;
    const char *name;

    if (*s == '\"' || *s == '\'')
    {
        if ((*putc_cb)(*s, p) < 0)
        {
            return (-1);
        }

        quote = *s++;

        while (*s && *s != quote)
        {
            if ((name = xml_entity_get_name(*s)) != NULL)
            {
                if ((*putc_cb)('&', p) < 0)
                {
                    return (-1);
                }

                while (*name)
                {
                    if ((*putc_cb)(*name, p) < 0)
                    {
                        return (-1);
                    }

                    name ++;
                }

                if ((*putc_cb)(';', p) < 0)
                {
                    return (-1);
                }
            }
            else if ((*putc_cb)(*s, p) < 0)
            {
                return (-1);
            }

            s ++;
        }

        if ((*putc_cb)(quote, p) < 0)
        {
            return (-1);
        }
    }
    else
    {
        while (*s)
        {
            if ((*putc_cb)(*s, p) < 0)
            {
                return (-1);
            }

            s ++;
        }
    }

    return (0);
}

static int  xml_write_node(XmlNode *node, void  *p, cb_xml_save  cb, int col, cb_xml_putc putc_cb, XmlGlobal  *global)
{
    int  i, width;
    XmlAttribute *attr;
    char  s[255];

    while (node != NULL)
    {
        switch (node->type)
        {
            case XML_ELEMENT :
                col = xml_write_ws(node, p, cb, XML_WS_BEFORE_OPEN, col, putc_cb);

                if ((*putc_cb)('<', p) < 0)
                {
                    return (-1);
                }
                if (node->value.element.name[0] == '?' ||
                        !strncmp(node->value.element.name, "!--", 3) ||
                        !strncmp(node->value.element.name, "![CDATA[", 8))
                {
                    const char *ptr;

                    for (ptr = node->value.element.name; *ptr; ptr ++)
                    {
                        if ((*putc_cb)(*ptr, p) < 0)
                        {
                            return (-1);
                        }
                    }

                    if (!strncmp(node->value.element.name, "?xml", 4))
                    {
                        col = global->wrap;
                    }
                }
                else if (xml_write_name(node->value.element.name, p, putc_cb) < 0)
                {
                    return (-1);
                }

                col += strlen(node->value.element.name) + 1;

                for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
                        i > 0;
                        i --, attr ++)
                {
                    width = strlen(attr->name);

                    if (attr->value)
                    {
                        width += strlen(attr->value) + 3;
                    }

                    if ((col + width) > global->wrap)
                    {
                        if ((*putc_cb)('\n', p) < 0)
                        {
                            return (-1);
                        }

                        col = 0;
                    }
                    else
                    {
                        if ((*putc_cb)(' ', p) < 0)
                        {
                            return (-1);
                        }

                        col ++;
                    }

                    if (xml_write_name(attr->name, p, putc_cb) < 0)
                    {
                        return (-1);
                    }

                    if (attr->value)
                    {
                        if ((*putc_cb)('=', p) < 0)
                        {
                            return (-1);
                        }
                        if ((*putc_cb)('\"', p) < 0)
                        {
                            return (-1);
                        }
                        if (xml_write_string(attr->value, p, putc_cb) < 0)
                        {
                            return (-1);
                        }
                        if ((*putc_cb)('\"', p) < 0)
                        {
                            return (-1);
                        }
                    }

                    col += width;
                }

                if (node->child)
                {
                    if ((*putc_cb)('>', p) < 0)
                    {
                        return (-1);
                    }
                    else
                    {
                        col ++;
                    }

                    col = xml_write_ws(node, p, cb, XML_WS_AFTER_OPEN, col, putc_cb);

                    if ((col = xml_write_node(node->child, p, cb, col, putc_cb, global)) < 0)
                    {
                        return (-1);
                    }

                    if (node->value.element.name[0] != '!' &&
                            node->value.element.name[0] != '?')
                    {
                        col = xml_write_ws(node, p, cb, XML_WS_BEFORE_CLOSE, col, putc_cb);

                        if ((*putc_cb)('<', p) < 0)
                        {
                            return (-1);
                        }
                        if ((*putc_cb)('/', p) < 0)
                        {
                            return (-1);
                        }
                        if (xml_write_string(node->value.element.name, p, putc_cb) < 0)
                        {
                            return (-1);
                        }
                        if ((*putc_cb)('>', p) < 0)
                        {
                            return (-1);
                        }

                        col += strlen(node->value.element.name) + 3;

                        col = xml_write_ws(node, p, cb, XML_WS_AFTER_CLOSE, col, putc_cb);
                    }
                }
                else if (node->value.element.name[0] == '!' ||
                         node->value.element.name[0] == '?')
                {
                    if ((*putc_cb)('>', p) < 0)
                    {
                        return (-1);
                    }
                    else
                    {
                        col ++;
                    }

                    col = xml_write_ws(node, p, cb, XML_WS_BEFORE_OPEN, col, putc_cb);
                }
                else
                {
                    if ((*putc_cb)(' ', p) < 0)
                    {
                        return (-1);
                    }
                    if ((*putc_cb)('/', p) < 0)
                    {
                        return (-1);
                    }
                    if ((*putc_cb)('>', p) < 0)
                    {
                        return (-1);
                    }

                    col += 3;

                    col = xml_write_ws(node, p, cb, XML_WS_BEFORE_OPEN, col, putc_cb);
                }
                break;

            case XML_INTEGER :
                if (node->prev)
                {
                    if (col > global->wrap)
                    {
                        if ((*putc_cb)('\n', p) < 0)
                        {
                            return (-1);
                        }

                        col = 0;
                    }
                    else if ((*putc_cb)(' ', p) < 0)
                    {
                        return (-1);
                    }
                    else
                    {
                        col ++;
                    }
                }

                sprintf(s, "%d", node->value.integer);
                if (xml_write_string(s, p, putc_cb) < 0)
                {
                    return (-1);
                }

                col += strlen(s);
                break;

            case XML_OPAQUE :
                if (xml_write_string(node->value.opaque, p, putc_cb) < 0)
                {
                    return (-1);
                }

                col += strlen(node->value.opaque);
                break;

            case XML_REAL :
                if (node->prev)
                {
                    if (col > global->wrap)
                    {
                        if ((*putc_cb)('\n', p) < 0)
                        {
                            return (-1);
                        }

                        col = 0;
                    }
                    else if ((*putc_cb)(' ', p) < 0)
                    {
                        return (-1);
                    }
                    else
                    {
                        col ++;
                    }
                }

                sprintf(s, "%f", node->value.real);
                if (xml_write_string(s, p, putc_cb) < 0)
                {
                    return (-1);
                }

                col += strlen(s);
                break;

            case XML_TEXT :
                if (node->value.text.whitespace && col > 0)
                {
                    if (col > global->wrap)
                    {
                        if ((*putc_cb)('\n', p) < 0)
                        {
                            return (-1);
                        }

                        col = 0;
                    }
                    else if ((*putc_cb)(' ', p) < 0)
                    {
                        return (-1);
                    }
                    else
                    {
                        col ++;
                    }
                }

                if (xml_write_string(node->value.text.string, p, putc_cb) < 0)
                {
                    return (-1);
                }

                col += strlen(node->value.text.string);
                break;

            case XML_CUSTOM :
                if (global->cbCustomSave)
                {
                    char *data;
                    const char *newline;


                    if ((data = (*global->cbCustomSave)(node)) == NULL)
                    {
                        return (-1);
                    }

                    if (xml_write_string(data, p, putc_cb) < 0)
                    {
                        return (-1);
                    }

                    if ((newline = strrchr(data, '\n')) == NULL)
                    {
                        col += strlen(data);
                    }
                    else
                    {
                        col = strlen(newline);
                    }

                    free(data);
                    break;
                }

            default :
                return (-1);
        }

        node = node->next;
    }

    return (col);
}

static int  xml_write_string(const char *s, void  *p, cb_xml_putc putc_cb)
{
    const char *name;

    while (*s)
    {
        if ((name = xml_entity_get_name(*s)) != NULL)
        {
            if ((*putc_cb)('&', p) < 0)
            {
                return (-1);
            }

            while (*name)
            {
                if ((*putc_cb)(*name, p) < 0)
                {
                    return (-1);
                }
                name ++;
            }

            if ((*putc_cb)(';', p) < 0)
            {
                return (-1);
            }
        }
        else if ((*putc_cb)(*s, p) < 0)
        {
            return (-1);
        }

        s ++;
    }

    return (0);
}

static int  xml_write_ws(XmlNode *node, void *p, cb_xml_save  cb, int  ws, int  col, cb_xml_putc putc_cb)
{
    const char *s;

    if (cb && (s = (*cb)(node, ws)) != NULL)
    {
        while (*s)
        {
            if ((*putc_cb)(*s, p) < 0)
            {
                return (-1);
            }
            else if (*s == '\n')
            {
                col = 0;
            }
            else if (*s == '\t')
            {
                col += XML_TAB;
                col = col - (col % XML_TAB);
            }
            else
            {
                col ++;
            }

            s ++;
        }
    }

    return (col);
}

