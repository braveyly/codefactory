/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************/
#ifndef __OS_DEFS_H__
#define __OS_DEFS_H__

#include <unistd.h>  /* for getopt */
#include <stdio.h>   /* for snprintf */
#include <stdint.h>  /* for the various integer types */
#include <stdlib.h>  /* for NULL */
#include <string.h>  /* for strlen, strncpy */
#include <ctype.h>   /* for isdigit */
#include <syslog.h>  /* for syslog */
#include <stdarg.h>  /* for va_list */

/*!\file os_defs.h
 * \brief Various commonly used, but OS dependent definitions are defined here.
 *
 *  This file is for Linux.
 */

#ifndef NUMBER_TYPES_ALREADY_DEFINED
#define NUMBER_TYPES_ALREADY_DEFINED

/** Unsigned 32 bit integer. */
typedef uint32_t   UINT32;

/** Signed 32 bit integer. */
typedef int32_t    SINT32;

/** Unsigned 16 bit integer. */
typedef uint16_t   UINT16;

/** Signed 16 bit integer. */
typedef int16_t    SINT16;

/** Unsigned 8 bit integer. */
typedef uint8_t    UINT8;

/** Signed 8 bit integer. */
typedef int8_t     SINT8;

#endif /* NUMBER_TYPES_ALREADY_DEFINED */


#ifndef BOOL_TYPE_ALREADY_DEFINED
#define BOOL_TYPE_ALREADY_DEFINED

/**Boolean type; use 1 byte only, possible values are TRUE(1) or FALSE(0) only.
 *
 * TRUE/FALSE defined in cms.h
 */
typedef uint8_t    UBOOL8;

#endif /* BOOL_TYPE_ALREADY_DEFINED */


/** Base64 encoded string representation of binary data.
 *
 * This is to support TR69 data types.
 */
typedef char *     BASE64;

/** String representation of date and time.
 *
 * This is to support TR69 data types.
 */
typedef char *     DATETIME;

/** Invalid file descriptor number */
#define TCM_INVALID_FD  (-1)


/** Invalid process id.
 *
 * Management entities should not need to use this constant.  It is used
 * by OS dependent code in the OAL layer.  But I put this constant here
 * to make it easier to find.
 */
#define TCM_INVALID_PID   0

#endif /* __OS_DEFS_H__ */

