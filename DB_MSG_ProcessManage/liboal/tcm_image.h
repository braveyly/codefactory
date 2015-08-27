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
#ifndef __TCM_IMAGE_H__
#define __TCM_IMAGE_H__

#include "tcm.h"

#define  TCM_IMAGE_MIN_LEN    2048
/*
 Modified by Tracy Liu, at 2009-05-14 13:54:01
 reason: Add for checking the setting file size
*/
#define  TCM_SETTING_MIN_LEN  2048

typedef enum
{
    TCM_IMAGE_FORMAT_INVALID=0,   /**< invalid or unrecognized format */
    TCM_IMAGE_FORMAT_BROADCOM=1,  /**< broadcom image (with our header) */
    TCM_IMAGE_FORMAT_FLASH=2,     /**< raw flash image */
    TCM_IMAGE_FORMAT_XML_CFG=3    /**< CMS XML config file */
} tcmImageFormat;

tcmImageFormat tcmImg_validateImage(const char *imagePtr, UINT32 imageLen, void *msgHandle);


tcmRet tcmImg_writeImage(char *imagePtr, UINT32 imageLen, void *msgHandle);


tcmRet tcmImg_writeValidatedImage(char *imagePtr, UINT32 imageLen, tcmImageFormat format, void *msgHandle);


UINT32 tcmImg_getImageFlashSize(void);


UINT32 tcmImg_getConfigFlashSize(void);


UINT32 tcmImg_getRealConfigFlashSize(void);


UINT32 tcmImg_getBroadcomImageTagSize(void);


#define  TCM_IMAGE_OVERHEAD          256

UBOOL8 tcmImg_willFitInFlash(UINT32 imageLen);

#define TCM_CONFIG_FILE_DETECTION_LENGTH 64

UBOOL8 tcmImg_isConfigFileLikely(const char *buf);

#endif /*__TCM_IMAGE_H__*/

