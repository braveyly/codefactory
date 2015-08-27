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

#include <sys/types.h>
#include <signal.h>

#include "tcm.h"
#include "tcm_ast.h"

tcmRet tcmAst_assertFunc(const char *filename, UINT32 lineNumber, const char *exprString, SINT32 expr)
{
   if (expr == 0)
   {
      SINT32 rc;
      SINT32 pid = getpid();

      if( pid <= 0 )
      {
         return TCMRET_INVALID_ARGUMENTS;
      }

      if( (rc = kill(pid, SIGABRT)) < 0)
      {
         return TCMRET_INVALID_ARGUMENTS;
      }
   }

   return TCMRET_SUCCESS;
}

