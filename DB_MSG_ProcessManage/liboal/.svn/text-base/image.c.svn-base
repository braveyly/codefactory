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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/signal.h>

#include "tcm.h"
#include "tcm_util.h"

static tcmRet flashImage(char *imagePtr, UINT32 imageLen, tcmImageFormat format);

// depending on the image type, do the brcm image or whole flash image
tcmRet flashImage(char *imagePtr, UINT32 imageLen, tcmImageFormat format)
{
    return TCMRET_SUCCESS;
}


UINT32 tcmImg_getImageFlashSize(void)
{
   UINT32 flashSize=0;
   //tcmRet ret;

#if 0
   ret = devCtl_boardIoctl(BOARD_IOCTL_FLASH_READ,
                           FLASH_SIZE,
                           0, 0, 0, &flashSize);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("Could not get flash size, return 0");
      flashSize = 0;
   }
#endif

   return flashSize;
}


UINT32 tcmImg_getBroadcomImageTagSize(void)
{
   return 0;
}


UINT32 tcmImg_getConfigFlashSize(void)
{
   UINT32 realSize;

   realSize = tcmImg_getRealConfigFlashSize();

   return realSize;
}


UINT32 tcmImg_getRealConfigFlashSize(void)
{
   //tcmRet ret;
   UINT32 size=0;

#if 0
   ret = devCtl_boardIoctl(BOARD_IOCTL_GET_PSI_SIZE, 0, NULL, 0, 0, (void *)&size);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error("boardIoctl to get config flash size failed.");
      size = 0;
   }
#endif

   return size;
}


UBOOL8 tcmImg_willFitInFlash(UINT32 imageSize)
{
   UINT32 flashSize;

   flashSize = tcmImg_getImageFlashSize();

   return (flashSize > (imageSize + TCM_IMAGE_OVERHEAD));
}


UBOOL8 tcmImg_isConfigFileLikely(const char *buf)
{
   const char *header = "<?xml version";
   const char *dslCpeConfig = "<DslCpeConfig";
   UINT32 len, i;
   UBOOL8 likely=FALSE;

   if (strncmp(buf, "<?xml version", strlen(header)) == 0)
   {
      len = strlen(dslCpeConfig);
      for (i=20; i<50 && !likely; i++)
      {
         if (strncmp(&(buf[i]), dslCpeConfig, len) == 0)
         {
            likely = TRUE;
         }
      }
   }

   return likely;
}


/** General entry point for writing the image.
 *  The image can be a flash image or a config file.
 *  This function will first determine the image type, which has the side
 *  effect of validating it.
 */
tcmRet tcmImg_writeImage(char *imagePtr, UINT32 imageLen, void *msgHandle)
{
   tcmImageFormat format;
   tcmRet ret;

   if ((format = tcmImg_validateImage(imagePtr, imageLen, msgHandle)) == TCM_IMAGE_FORMAT_INVALID)
   {
      ret = TCMRET_INVALID_IMAGE;
   }
   else
   {
      ret = tcmImg_writeValidatedImage(imagePtr, imageLen, format, msgHandle);
   }

   return ret;
}

tcmRet tcmImg_writeValidatedImage(char *imagePtr, UINT32 imageLen, tcmImageFormat format, void *msgHandle)
{
   tcmRet ret=TCMRET_SUCCESS;
   char* imgBcmBuf = imagePtr;
   UINT32 imgBcmLen = imageLen;

   switch(format)
   {
   case TCM_IMAGE_FORMAT_BROADCOM:
   case TCM_IMAGE_FORMAT_FLASH:
      ret = flashImage(imgBcmBuf, imgBcmLen, format);
      break;

   case TCM_IMAGE_FORMAT_XML_CFG:
      break;

   default:
       ret = TCMRET_INVALID_IMAGE;
       break;
    }

   return ret;
}


tcmImageFormat tcmImg_validateImage(const char *imageBuf, UINT32 imageLen, void *msgHandle)
{
   tcmImageFormat result = TCM_IMAGE_FORMAT_INVALID;
   //tcmRet ret = TCMRET_SUCCESS;
   //const char* imgBcmBuf = imageBuf;
   //UINT32 imgBcmLen = imageLen;

   if (imageBuf == NULL)
   {
      return TCM_IMAGE_FORMAT_INVALID;
   }

   if (imageLen > TCM_CONFIG_FILE_DETECTION_LENGTH &&
       tcmImg_isConfigFileLikely(imageBuf))
   {
   }

   return result;
}
