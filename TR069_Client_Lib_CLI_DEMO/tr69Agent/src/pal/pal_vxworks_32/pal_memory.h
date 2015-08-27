#ifndef _PAL_MEMORY_H_
#define _PAL_MEMORY_H_
#include "pal_mutex.h"
void *pal_malloc ( PAL_UINT32_T size, const char *file, const char *functionName, PAL_INT32_T line );

void pal_free ( void *ptr, const char *file, const char *functionName, PAL_INT32_T line );

#define PAL_MALLOC(size) \
    pal_malloc(size, __FILE__, __func__, __LINE__)

#define PAL_FREE(ptr) \
    pal_free(ptr, __FILE__, __func__, __LINE__)
//void PAL_MALLOC(size) ;
//void PAL_FREE(ptr);
typedef struct _MemBlockInfoStruct
{
    void *startAddr;
    char fileName[24];
    char functionName[32];
    PAL_INT32_T line;
    PAL_INT32_T blockSize;
    struct _MemBlockInfoStruct *next;

} MemBlockInfoStruct, *PMemBlockInfoStruct;

typedef struct _MemPoolStruct
{
    PAL_INT32_T memAllocated;
    PAL_INT32_T memFreed;
    MemBlockInfoStruct *listHead;
    PAL_MUTEX_T lock;
} MemPoolStruct, *PMemPoolStruct;

PAL_UINT32_T get_endian_flag(void);

typedef enum{
	LITTLE_ENDIAN_MODE = 0,
        BIG_ENDIAN_MODE =1
};

typedef union _TestEndian {
    PAL_UINT32_T uint_v;
    char   str_v[4];
}TestEndian;

#define PAL_HOST_FREE(x)  hostentFree(x)
#endif /* _PAL_MEMORY_H */
