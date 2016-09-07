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

#ifndef __TCM_MEM_H__
#define __TCM_MEM_H__


/*!\file cms_mem.h
 * \brief Header file for the CMS Memory Allocation API.
 * This is in the cms_util library.
 */

/** zeroize the buffer before returning it to the caller. */
#define ALLOC_ZEROIZE          0x01

/** allocate memory in the shared memory region.  This should only bed
 * used by internal cms_core and mdm libraries when creating nodes in
 * the MDM tree.
 */
#define ALLOC_SHARED_MEM       0x02


/** allocate memory
 * @param size The number of bytes to allocate.
 * @param allocFlags See ALLOC_xxx for possible flags.
 * @return On succcess, a buffer of requested size is returned.
 * On error, NULL is returned.
 */
void *tcmMem_alloc(UINT32 size, UINT32 allocFlags);


/** Increase buffer size of previously allocated memory.
 *
 * The same allocFlags that was used to allocate the original buffer
 * will be used to allocate the new buffer.
 * Note that unlike the LINUX realloc, cmsMem_realloc cannot
 * take a NULL buffer.  If you have a null pointer and you want
 * to allocate a buffer, use cmsMem_alloc instead.
 * Like the LINUX realloc, if size is 0, this function is equivalent
 * to cmsMem_free.
 * In the future, if size is less than the original buffer size,
 * the buffer will be shrunk.  Currently, cmsMem_realloc does not
 * shrink buffer size, it only increase buffer size.
 *
 * @param buf (IN) previously allocated buffer.
 * @param size (IN) The new buffer size.
 *
 * @return On succcess, a buffer of requested size is returned with
 *         the contents of the original buffer, which is freed.
 *         On error, NULL is returned but the original buffer is left unmodified.
 */
void *tcmMem_realloc(void *buf, UINT32 size);


/** Free previously allocated memory
 * @param buf Previously allocated buffer.
 */
void tcmMem_free(void *buf);


/** Free a buffer and set the pointer to null.
 */
#define TCMMEM_FREE_BUF_AND_NULL_PTR(p) \
   do { \
      if ((p) != NULL) {tcmMem_free((p)); (p) = NULL;}   \
   } while (0)


/** Free the existing char pointer and set it to a copy of the specified string.
 */
#define TCMMEM_REPLACE_STRING(p, s) \
   do {\
      if ((p) != NULL) {tcmMem_free((p));} \
      (p) = tcmMem_strdup((s)); \
   } while (0)


/** Free the existing char pointer and set it to a copy of the specified string
 *  and specify the way the string is allocated.
 */
#define TCMMEM_REPLACE_STRING_FLAGS(p, s, f) \
   do {\
      if ((p) != NULL) {tcmMem_free((p));} \
      (p) = tcmMem_strdupFlags((s), (f)); \
   } while (0)


/** Replace the existing string p if it is different than the new string s
 */
#define REPLACE_STRING_IF_NOT_EQUAL(p, s)    \
   if ((p) != NULL) { \
      if (strcmp((p), (s))) { tcmMem_free((p)); (p) = tcmMem_strdup((s)); } \
   } else { \
      (p) = tcmMem_strdup((s));                 \
   }


/** Replace the existing string p if it is different than the new string s
 * and specify the way the string is allocated.
 */
#define REPLACE_STRING_IF_NOT_EQUAL_FLAGS(p, s, f)     \
   if ((p) != NULL) { \
      if (strcmp((p), (s))) {tcmMem_free((p)); (p) = tcmMem_strdupFlags((s), (f)); } \
   } else { \
      (p) = tcmMem_strdupFlags((s), (f));       \
   }



/** Duplicate a string.
 *
 * This function should be used only if the caller knows the string
 * is properly NULL terminated.  It should not be called on a buffer
 * whose contents cannot be trusted.  Consider using cmsMem_strndup
 * instead.
 *
 * @param str (IN) string to duplicate.
 * @return On success, a duplicate of the given string will be returned.
 * Otherwise, NULL will be returned.  Caller must free the string by
 * calling cmsMem_free().
 */
char *tcmMem_strdup(const char *str);


/** Duplicate a string with ability to specify how memory is allocated.
 *
 * See cmsMem_strdup.
 *
 * @param str (IN) string to duplicate.
 * @param flags (IN) flags to pass to the cmsMem_alloc when allocating
 *                   storage for the duplicated string.
 * @return On success, a duplicate of the given string will be returned.
 * Otherwise, NULL will be returned.  Caller must free the string by
 * calling cmsMem_free().
 */
char *tcmMem_strdupFlags(const char *str, UINT32 flags);


/** Duplicate at most maxlen characters in the string.
 *
 * Use this function instead of strdup when you are not sure if the
 * string is NULL terminated.  If you use strdup and the string is not NULL
 * terminated, your program will crash.
 *
 * maxlen does not include the NULL terminating character.
 * So if str is "abcde" and maxlen is 5, then this function will
 * allocate a buffer of length 6 and return a properly terminated
 * string buffer containing "abcde" and the NULL terminating character.
 *
 * @param str (IN) string to duplicate.
 * @param maxlen (IN) maximum number of characters to copy.  The function
 *                    will allocate one extra byte for the NULL terminating
 *                    character if necessary.
 * @return On success, a duplicate of the given string will be returned.
 * Otherwise, NULL will be returned.  Caller must free the string by
 * calling cmsMem_free().
 */
char *tcmMem_strndup(const char *str, UINT32 maxlen);


/** Duplicate at most maxlen characters in the string with the ability to
 * specify how memory is allocated.
 *
 * See cmsMem_strndup.
 *
 * @param str (IN) string to duplicate.
 * @param maxlen (IN) maximum number of characters to copy.  The function
 *                    will allocate one extra byte for the NULL terminating
 *                    character if necessary.
 * @param flags (IN) flags to pass to the cmsMem_alloc when allocating storage
 *                   for the string.
 * @return On success, a duplicate of the given string will be returned.
 * Otherwise, NULL will be returned.  Caller must free the string by
 * calling cmsMem_free().
 */
char *tcmMem_strndupFlags(const char *str, UINT32 maxlen, UINT32 flags);


/** Return the length of the string, but constrained by maxlen
 *
 * This function works like strlen, except it will not look further
 * than maxlen bytes in the given string.
 *
 * @param str (IN) the string whose length we want to determine.
 * @param maxlen (IN) the maximum expected length of the string.
 * @param isTerminated (OUT) this is an optional pointer to a bool.
 *                           If this pointer is not null, then it will be
 *                           set to true if the string is maxlen or less.
 *
 * @return Number of bytes in the string, constrained by maxlen.
 */
UINT32 tcmMem_strnlen(const char *str, UINT32 maxlen, UBOOL8 *isTerminated);


/** How much debug information to put at the beginning of a malloc'd block,
 *  to keep track of original length and allocation flags (and also to
 *  detect buffer under-write).
 *
 * We allocate a header regardless of CMS_MEM_DEBUG.
 *
 * The memory code assumes this value is 12, so you'll need to modify
 * memory.c if you change this constant.
 */
#define TCM_MEM_HEADER_LENGTH   12


/** How much debug information to put at the end of a malloc'd block,
 *  (to detect buffer over-write) used only if CMS_MEM_DEBUG is defined.
 *
 * The memory code assumes this value is 8, so you'll need to modify
 * memory.c if you change this constant.
 */
#define TCM_MEM_FOOTER_LENGTH   8


/** Pattern put at the memory block footer (to detect buffer over-write).
 *
 * Used only if CMS_MEM_DEBUG is defined.
 * If allocation is not on 4 byte boundary, the bytes between the user
 * requested bytes and the 4 byte boundary are filled with the
 * last byte of the CMS_MEM_FOOTER_PATTERN.
 */
#define TCM_MEM_FOOTER_PATTERN   0xfdfdfdfd


/** Pattern put in freshly allocated memory blocks, which do not have
 *  ALLOC_ZEROIZE flag set (to detect use before initialization.)
 *
 * Used only if CMS_MEM_DEBUG is defined.
 */
#define TCM_MEM_ALLOC_PATTERN    ((UINT8) 0x95)


/** Pattern put in freed memory blocks (to detect use after free.)
 *
 * Used only if CMS_MEM_DEBUG is defined.
 */
#define TCM_MEM_FREE_PATTERN     ((UINT8) 0x97)


#endif /* __TCM_MEM_H__ */
