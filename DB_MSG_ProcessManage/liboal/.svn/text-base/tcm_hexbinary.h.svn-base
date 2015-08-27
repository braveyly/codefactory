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

#ifndef __TCM_HEXBINARY_H__
#define __TCM_HEXBINARY_H__

/*!\file tcm_hexbinary.h
 * \brief Header file for HexBinary to Binary conversion functions.
 *
 * These functions allow callers to convert HexBinary strings to
 * binary buffers and back.
 *
 */
tcmRet tcmUtl_binaryBufToHexString(const UINT8 *binaryBuf, UINT32 binaryBufLen, char **hexStr);


/** Convert a null terminated hex string into a binary buffer.
 *
 * @param hexStr (IN) Input hex string. There must be an even number of
 *                    characters in the string.  This means if the first
 *                    value is less than 128, it must have a preceding 0.
 * @param binaryBuf (OUT) This function will allocate a buffer and put
 *                   the decoded binary data into the buffer.  The caller
 *                   is responsible for freeing the buffer.
 * @param binaryBufLen (OUT) The length of the binary buffer.
 *
 * @return CmsRet enum.
 */
tcmRet tcmUtl_hexStringToBinaryBuf(const char *hexStr, UINT8 **binaryBuf, UINT32 *binaryBufLen);

#endif  /* __TCM_HEXBINARY_H__ */
