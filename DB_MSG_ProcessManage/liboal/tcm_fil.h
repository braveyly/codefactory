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

#ifndef __TCM_FIL_H__
#define __TCM_FIL_H__

#include "tcm.h"

/*!\file tcm_fil.h
 * \brief Header file for file and directory utilities functions.
 */

UBOOL8 tcmFil_isFilePresent(const char *filename);

SINT32 tcmFil_getSize(const char *filename);

tcmRet cmsFil_copyToBuffer(const char *filename, UINT8 *buf, UINT32 *bufSize);

#endif /* __TCM_FIL_H__ */
