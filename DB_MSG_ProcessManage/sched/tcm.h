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

#ifndef __TCM_H__
#define __TCM_H__

/*!\file TCM.h
 * \brief Header file containing common and constant definitions for
 *        the CPE Management System (TCM).  Parameters which may change
 *        depending on vendor preference or board configuration are located
 *        in cms_params.h (which is included by this file at the bottom.)
 */
#include "os_defs.h"

/*!\enum tcmRet
 * \brief Return codes for all external functions, and some internal functions too.
 */
typedef enum
{
   TCMRET_SUCCESS = 0,
   TCMRET_FAILURE,
   TCMRET_RESOURCE_EXCEEDED,
   TCMRET_INVALID_PARAMETER,
   TCMRET_INTERNAL_ERROR,
   TCMRET_DISCONNECTED,
   TCMRET_TIMED_OUT,
   TCMRET_SUCCESS_REBOOT_REQUIRED,
   TCMRET_INVALID_ARGUMENTS,
   TCMRET_SUCCESS_UNRECOGNIZED_DATA_IGNORED,
   TCMRET_NO_MORE_INSTANCES,
   TCMRET_INVALID_IMAGE,
   TCMRET_INVALID_PARAM_VALUE,
   TCMRET_OBJECT_NOT_FOUND,
   TCMRET_CONFLIB_READ_ERROR,
   TCMRET_WRITE_FLASH_ERROR,
   TCMRET_CONFLIB_PARAMETER_INVALID,
   TCMRET_CONFLIB_WRITE_ERROR,
   TCMRET_INVALID_DUPLICATE,
   TCMRET_INVALID_EXIST,
   TCMRET_INVALID_SAME,
   TCMRET_REACH_MAX_NUM,
} tcmRet;

/** Check if the CmsRet code is either SUCCESS or SUCCESS_REBOOT_REQUIRED.
 */
#define IS_TCMRET_A_SUCCESS_VARIANT(r) (((r) == TCMRET_SUCCESS) || \
                                        ((r) == TCMRET_SUCCESS_REBOOT_REQUIRED))

#ifndef TRUE
/** TRUE = 1
 */
#define TRUE  1
#endif

#ifndef FALSE
/** FALSE = 0
 */
#define FALSE 0
#endif

#ifndef true
   #define true  1
#endif

#ifndef false
#define false 0
#endif

/** Maximum value for a UINT32 */
#define MAX_UINT32 4294967295U

/** Maximum value for a SINT32 */
#define MAX_SINT32 2147483647

/** Minimum value for a SINT32 */
#define MIN_SINT32 (-2147483648)

/** Maximum value for a UINT16 */
#define MAX_UINT16  65535

/** Maximum value for a SINT16 */
#define MAX_SINT16  32767

/** Minimum value for a SINT16 */
#define MIN_SINT16  (-32768)


/**
 * This is common used string length types.
 */
#define BUFLEN_4        4     //!< buffer length 4
#define BUFLEN_8        8     //!< buffer length 8
#define BUFLEN_16       16    //!< buffer length 16
#define BUFLEN_18       18    //!< buffer length 18 -- for ppp session id
#define BUFLEN_24       24    //!< buffer length 24 -- mostly for password
#define BUFLEN_32       32    //!< buffer length 32
#define BUFLEN_40       40    //!< buffer length 40
#define BUFLEN_48       48    //!< buffer length 48
#define BUFLEN_64       64    //!< buffer length 64
#define BUFLEN_128      128   //!< buffer length 128
#define BUFLEN_256      256   //!< buffer length 256
#define BUFLEN_264      264   //!< buffer length 264
#define BUFLEN_512      512   //!< buffer length 512
#define BUFLEN_1024     1024  //!< buffer length 1024

#define IIDSTACK_BUF_LEN  40  //!< good length to use for mdm_dumpIidStack
#define MAC_ADDR_LEN    6     //!< Mac address len in an array of 6 bytes
#define MAC_STR_LEN     17    //!< Mac String len with ":". eg: xx:xx:xx:xx:xx:xx
#define VPI_MIN         0     //!< VPI min
#define VPI_MAX         255   //!< VPI max
#define VCI_MIN         32    //!< VCI min
#define VCI_MAX         65535 //!< VCI max

#endif /* __TCM_H__ */
