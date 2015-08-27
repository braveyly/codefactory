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

#include "tcm.h"
#include "tcm_mem.h"
#include "tcm_strconv.h"

tcmRet tcmUtl_binaryBufToHexString(const UINT8 *binaryBuf, UINT32 binaryBufLen, char **hexStr)
{
   UINT32 i, j;

   if (hexStr == NULL)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   *hexStr = tcmMem_alloc((binaryBufLen*2)+1, ALLOC_ZEROIZE);
   if (*hexStr == NULL)
   {
      return TCMRET_RESOURCE_EXCEEDED;
   }

   for (i=0, j=0; i < binaryBufLen; i++, j+=2)
   {
      sprintf(&((*hexStr)[j]), "%02x", binaryBuf[i]);
   }

   return TCMRET_SUCCESS;
}


tcmRet tcmUtl_hexStringToBinaryBuf(const char *hexStr, UINT8 **binaryBuf, UINT32 *binaryBufLen)
{
   UINT32 len;
   UINT32 val;
   UINT32 i, j;
   char tmpbuf[3];
   tcmRet ret = TCMRET_SUCCESS;

   len = strlen(hexStr);
   if (len % 2 != 0)
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   *binaryBuf = tcmMem_alloc(len/2, 0);
   if (*binaryBuf == NULL)
   {
      return TCMRET_RESOURCE_EXCEEDED;
   }

   for (i=0, j=0; j < len; i++, j+=2)
   {
      tmpbuf[0] = hexStr[j];
      tmpbuf[1] = hexStr[j+1];
      tmpbuf[2] = 0;

      ret = tcmUtl_strtoul(tmpbuf, NULL, 16, &val);
      if (ret != TCMRET_SUCCESS)
      {
         tcmMem_free(*binaryBuf);
         *binaryBuf = NULL;
         return ret;
      }
      else
      {
         (*binaryBuf)[i] = (UINT8) val;
      }
   }

   /* if we get here, we were successful, set length */
   *binaryBufLen = len / 2;

   return ret;
}

