#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "pal.h"
#include "pal_memory.h"

MemPoolStruct gMemPool;

PAL_INT32_T pal_mpool_init()
{
    PAL_INT32_T ret;
    
    if ((ret = pal_mutex_init(&gMemPool.lock)) != 0)
    {
        return ret;
    }
    
    gMemPool.listHead = NULL;
    gMemPool.memAllocated = 0;
    gMemPool.memFreed = 0;
    
    return 0;
}

PAL_INT32_T pal_mpool_destroy()
{
    PAL_INT32_T ret;
    MemBlockInfoStruct *p = NULL, *q = NULL;
    
    p = gMemPool.listHead;
    
    while(p != NULL)
    {
        q = p;
        p = p->next;
        free(q);
    }
    
    gMemPool.listHead = NULL;
    gMemPool.memAllocated = 0;
    gMemPool.memFreed = 0;
    
    if ((ret = pal_mutex_destroy(&gMemPool.lock)) != 0)
    {
        return ret;
    }
    
    return 0;
}

void *pal_malloc(PAL_UINT32_T size
                , const char *file
                , const char *functionName
                , PAL_INT32_T line)
{
    void *ptr = NULL;
    MemBlockInfoStruct *memInfoNodePtr;
    
    if (size <= 0 || size > INT_MAX)
    {
        return NULL;
    }
    
    if ((ptr = malloc(size)) == NULL)
    {
        return ptr;
    }

    return ptr; //gavin , not need to record the memory status since no memory leak.
	
    memInfoNodePtr = malloc(sizeof(MemBlockInfoStruct));
    if (memInfoNodePtr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        free(ptr);
        return NULL;
    }
    
    strncpy(memInfoNodePtr->fileName, file, 24);
    strncpy(memInfoNodePtr->functionName, functionName, 32);
    memInfoNodePtr->line = line;
    memInfoNodePtr->startAddr = ptr;
    memInfoNodePtr->blockSize = size;
    
    pal_mutex_lock(&gMemPool.lock);
    
    if (gMemPool.listHead == NULL)
    {
        gMemPool.listHead = memInfoNodePtr;
        memInfoNodePtr->next = NULL;
    }
    else
    {
        memInfoNodePtr->next = gMemPool.listHead;
        gMemPool.listHead = memInfoNodePtr;
    }
    
    gMemPool.memAllocated += size;

#if 0
    fprintf(stderr, "pal_malloc[file:%s][function:%s][line:%d][size:%d]"
            "[address:0x%x-0x%x]\n"
           , file, functionName, line, size
           , ptr, ptr + size - 1);
#endif
    pal_mutex_unlock(&gMemPool.lock); 
    
    return ptr;
}

void pal_free(void *ptr
             , const char *file
             , const char *functionName
             , PAL_INT32_T line)
{
    MemBlockInfoStruct *p = NULL, *q = NULL;
    PAL_INT32_T blockSize;
    
    if (ptr != NULL)
    {
        free(ptr);
    }
    else
    {
        return;
    }
	
    return ptr; //gavin , not need to record the memory status since no memory leak.
    
    pal_mutex_lock(&gMemPool.lock);
    
    p = gMemPool.listHead;
    while (p != NULL)    
    {
        if (p->startAddr == ptr)
        {
            blockSize = p->blockSize;
            if (p == gMemPool.listHead)
            {
                gMemPool.listHead = p->next;
                free(p);
            }
            else
            {
                q->next = p->next;
                free(p);
            }
        }
        q = p;
        p = p->next;
    }
    
    gMemPool.memFreed += blockSize;

#if 0
    fprintf(stderr, "pal_free[file:%s][function:%s][line:%d]"
            "[size:%d][address:0x%x-0x%x]\n"
            , file, functionName, line, blockSize, ptr, ptr + blockSize - 1);
#endif
    pal_mutex_unlock(&gMemPool.lock);
}

void pal_mpool_show_stutus()
{
    MemBlockInfoStruct *p = NULL;
    PAL_INT32_T mpoolSize = 0;
    
    pal_mutex_lock(&gMemPool.lock);
    
    p = gMemPool.listHead;
    
    fprintf(stderr, "------->\n");
    while (p != NULL)
    {
        mpoolSize += p->blockSize;
        fprintf(stderr, "status[file:%s][function:%s][line%d][size:%d]"
               "[address:0x%x-0x%x]\n"
               , p->fileName, p->functionName, p->line, p->blockSize
               , p->startAddr, p->startAddr + p->blockSize - 1);
        p = p->next;
    }
    
    fprintf(stderr, "status[current mpool size:%d]\n", mpoolSize);
    fprintf(stderr, "<-------\n");
    pal_mutex_unlock(&gMemPool.lock);
}


PAL_UINT32_T get_endian_flag(void){
	TestEndian  testendian;
	testendian.uint_v = 0x00000001;

	if(testendian.str_v[0] > 0)
		return LITTLE_ENDIAN_MODE;
	else
		return BIG_ENDIAN_MODE;
	
}