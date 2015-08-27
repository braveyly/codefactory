#ifndef _PAL_DEBUG_H_
#define _PAL_DEBUG_H_
#include "pal_types.h"



//void tcdebugPrintfExTR069(char * pModule, int iType, const char * format, ...);
void tcdebugPrintfExTR069(PAL_INT8_T * pModule, PAL_INT8_T * sendbuf, PAL_INT32_T iType);

void InitTcDebugTR069(void);





#endif /* _PAL_DEBUG_H_ */
