#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "pal.h"
#include "pal_memory.h"

MemPoolStruct gMemPool;
int ifdebug = 0;

void *pal_malloc ( PAL_UINT32_T size
                   , const char *file
                   , const char *functionName
                   , PAL_INT32_T line )
{
    void *ptr = NULL;
   // MemBlockInfoStruct *memInfoNodePtr;

    if ( size <= 0 || size > INT_MAX )
    {
        return NULL;
    }

    if ( ( ptr = malloc ( size ) ) == NULL )
    {
        return ptr;
    }
  
#if 0
    /*TCDEBUG("pal_malloc[file:%s][function:%s][line:%d][size:%d], [address:0x%x-0x%x]\n"
              , file, functionName, line, size
              , ptr, ptr + size - 1 );*/
              if(ifdebug)
                {
    fprintf ( stderr, "pal_malloc[file:%s][function:%s][line:%d][size:%d]"
              "[address:0x%x]\n"
              , file, functionName, line, size
              , ptr);
                }
#endif


    return ptr;
}

void pal_free ( void *ptr
                , const char *file
                , const char *functionName
                , PAL_INT32_T line )
{
   // MemBlockInfoStruct *p = NULL, *q = NULL;
    PAL_INT32_T blockSize;
    
    if ( ptr != NULL )
    {
        free ( ptr );
        //ptr = NULL;
    }
    else
    {
        return;
    }

#if 0
    /*TCDEBUG("pal_free[file:%s][function:%s][line:%d],[address:0x%x]\n"
              , file, functionName, line, ptr);*/
              if(ifdebug)
    fprintf ( stderr, "pal_free[file:%s][function:%s][line:%d]"
              "[address:0x%x]\n"
              , file, functionName, line, ptr);
#endif
}

PAL_UINT32_T get_endian_flag(void){
	TestEndian  testendian;
	testendian.uint_v = 0x00000001;

	if(testendian.str_v[0] > 0)
		return LITTLE_ENDIAN_MODE;
	else
		return BIG_ENDIAN_MODE;
	
}