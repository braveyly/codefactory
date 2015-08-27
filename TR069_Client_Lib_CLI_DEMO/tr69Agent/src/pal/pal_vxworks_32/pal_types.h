#ifndef _PAL_TYPE_H
#define _PAL_TYPE_H

#include <vxWorks.h>
#include <assert.h>
#include "sockLib.h"
#include "sysLib.h"
#include "taskLib.h"
#include "tickLib.h"
#include "msgQLib.h"
#include "semLib.h"
#include "intLib.h"
#include "in.h"

typedef int PAL_BOOL_T;
typedef   size_t PAL_SIZE_T;
typedef   char  PAL_INT8_T;
typedef   short  PAL_INT16_T;
typedef   int  PAL_INT32_T;
typedef   unsigned char  PAL_UINT8_T;
typedef   unsigned short  PAL_UINT16_T;
typedef   unsigned int  PAL_UINT32_T;

typedef unsigned char *POINTER;
typedef unsigned short int UINT2;
typedef unsigned long int UINT4;
#endif
