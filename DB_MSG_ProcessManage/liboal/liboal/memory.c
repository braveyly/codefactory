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

#include <string.h>
#include "tcm.h"
#include "tcm_mem.h"
#include "tcm_ast.h"

void *tcmMem_alloc(UINT32 size, UINT32 allocFlags)
{
   void *buf = malloc(size);

   if (buf != NULL)
   {
      if (allocFlags & ALLOC_ZEROIZE)
      {
         memset(buf, 0, size);
      }
   }

   return buf;
}

void *tcmMem_realloc(void *origBuf, UINT32 size)
{
   void *buf = NULL;

   buf = realloc(origBuf, size);

   return buf;
}


void tcmMem_free(void *buf)
{
   if (buf != NULL)
   {
      free(buf);
      buf = NULL;
   }
}

UINT32 tcmMem_strnlen(const char *str, UINT32 maxlen, UBOOL8 *isTerminated)
{
   UINT32 len=0;

   while ((len < maxlen) && (str[len] != 0))
   {
      len++;
   }

   if (isTerminated != NULL)
   {
      *isTerminated = (str[len] == 0);
   }

   return len;
}

char *tcmMem_strdupFlags(const char *str, UINT32 flags)
{
   UINT32 len;
   void *buf;

   if (str == NULL)
   {
      return NULL;
   }

   len = strlen(str);
   buf = tcmMem_alloc(len+1, flags);
   if (buf == NULL)
   {
      return NULL;
   }

   strncpy((char *) buf, str, len+1);
   return ((char *) buf);
}

char *tcmMem_strdup(const char *str)
{
   return tcmMem_strdupFlags(str, 0);
}

char *tcmMem_strndupFlags(const char *str, UINT32 maxlen, UINT32 flags)
{
   UINT32 len;
   char *buf;

   if (str == NULL)
   {
      return NULL;
   }

   len = tcmMem_strnlen(str, maxlen, NULL);

   buf = (char *) tcmMem_alloc(len+1, flags);
   if (buf == NULL)
   {
      return NULL;
   }

   strncpy(buf, str, len);
   buf[len] = 0;

   return buf;
}

char *tcmMem_strndup(const char *str, UINT32 maxlen)
{
   return tcmMem_strndupFlags(str, maxlen, 0);
}




