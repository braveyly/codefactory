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

#ifndef __TCM_AST_H__
#define __TCM_AST_H__

/*!\file tcm_ast.h
 * \brief Header file for assert functions.
 *
 *  For production builds, define NDEBUG to disable asserts.
 */

/** Check that an assumed condition is TRUE.
 *
 * This should be used during development only.  For production code,
 * compile with NDEBUG defined.
 */
#define tcmAst_assert(expr) tcmAst_assertFunc(__FILE__, __LINE__, __STRING(expr), (expr))

/** The actual assert function, don't call this directly, use the macro.
 *
 * @param filename (IN) file where the assert occured.
 * @param lineNumber (IN) Linenumber of the assert statement.
 * @param exprString (IN) The actual expression that is being asserted.
 * @param expr       (IN) The result of the evaluation of the expression.  0 is fail,
 *                        non-zero is pass.
 */
tcmRet tcmAst_assertFunc(const char *filename, UINT32 lineNumber, const char *exprString, SINT32 expr);

#endif /* __TCM_AST_H__ */
