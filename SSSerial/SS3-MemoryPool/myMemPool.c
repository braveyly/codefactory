#include <stdlib.h>		       /* for calloc() and malloc() */
#include <string.h>		       /* for memset() */
#include <errno.h>		       /* for errno and EINVAL */
#include "myMemPool.h"

#ifdef CP_HAS_GETPAGESIZE
#include <unistd.h>		       /* for getpagesize() */
#else
int getpagesize() { return 0x2000; }
#endif /* CP_HAS_GETPAGESIZE */

#ifndef WORD_SIZE
#define WORD_SIZE (sizeof(void *))
#endif /* WORD_SIZE */

#define CP_MEMPOOL_TXLOCK(pool, err_ret) { \
    if (!((pool)->mode & COLLECTION_MODE_NOSYNC)) \
	if (pthread_mutex_lock((pool)->lock)) \
	    return err_ret; \
}

#define CP_MEMPOOL_TXUNLOCK(pool, err_ret) { \
    if (!((pool)->mode & COLLECTION_MODE_NOSYNC)) \
	if (pthread_mutex_unlock((pool)->lock)) \
	    return err_ret; \
}

static size_t pagesize = 0;

cp_mempool *cp_mempool_create_by_option(const int mode, 
                                    	size_t item_size, 
                                    	size_t alloc_size)
{
	cp_mempool *pool = (cp_mempool *) calloc(1, sizeof(cp_mempool));
	if (pool == NULL) return NULL;

	pool->mode = mode;

	if (!(mode & COLLECTION_MODE_NOSYNC))
	{
		pool->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
		if (pool->lock == NULL)
		{
			cp_mempool_destroy(pool);
			return NULL;
		}
		pthread_mutex_init(pool->lock, NULL);
	}

	if (pagesize == 0) pagesize = getpagesize();

	/* first, we ensure that item_size is a multiple of WORD_SIZE,
	 * and also that it is at least sizeof(void*). The first
	 * condition may imply the second on *most* platforms, but it
	 * costs us very little to make sure. */
	if (item_size < sizeof(void*)) item_size = sizeof(void*);
	if (item_size % WORD_SIZE)
		item_size += (WORD_SIZE) - (item_size % WORD_SIZE);
	pool->item_size = item_size;
	/* next, we pump up the alloc_size until it is at least big enough
	 * to hold ten chunks plus a void pointer, or ten pages, whichever
	 * is bigger. The reason for doing it this way rather than simply
	 * adding sizeof(void*) to alloc_size is that we want alloc_size to
	 * be a multiple of pagesize (this makes it faster!). */
	if (alloc_size < item_size * 10 + sizeof(void *))
		alloc_size = item_size * 10 + sizeof(void *);
	if (alloc_size < pagesize * 10) alloc_size = pagesize * 10;
	if (alloc_size % pagesize)
		alloc_size += pagesize - (alloc_size % pagesize);
	pool->alloc_size = alloc_size;

	pool->items_per_alloc = (alloc_size - sizeof(void *)) / item_size;

	pool->reuse_pool = NULL;
	pool->alloc_pool = (char *) malloc(alloc_size);
	if (pool->alloc_pool == NULL)
	{
		free(pool);
		return NULL;
	}
	*(void **) pool->alloc_pool = NULL;
			
	return pool;
}


cp_mempool *cp_mempool_create(const size_t item_size)
{
	return cp_mempool_create_by_option(COLLECTION_MODE_NOSYNC, item_size, 0);
}


void *cp_mempool_alloc(cp_mempool * const pool)
{
	void *p;

	CP_MEMPOOL_TXLOCK(pool, NULL);

	if (pool->reuse_pool)
	{
		p = pool->reuse_pool;
		pool->reuse_pool = *(void **)p;
	}
	else
	{
		if (pool->alloc_pool_pos == pool->items_per_alloc)
		{
			p = malloc(pool->alloc_size);
			if (p == NULL) return NULL;
			*(void **) p = pool->alloc_pool;
			pool->alloc_pool = p;
			pool->alloc_pool_pos = 0;
		}
		p = pool->alloc_pool + sizeof(void *) + 
			pool->item_size * pool->alloc_pool_pos++;
	}

	CP_MEMPOOL_TXUNLOCK(pool, NULL);

	return p;
}

void *cp_mempool_calloc(cp_mempool * const pool)
{
	void *p = cp_mempool_alloc(pool);
	if (p)
		memset(p, 0, pool->item_size);
	return p;
}

int cp_mempool_free(cp_mempool * const pool, void *data)
{
	CP_MEMPOOL_TXLOCK(pool, -1);
	*(void **) data = pool->reuse_pool;
	pool->reuse_pool = data;	
	CP_MEMPOOL_TXUNLOCK(pool, -1);
	return 0;
}

void cp_mempool_destroy(cp_mempool *pool)
{
	if (pool)
	{	
    	void *p;
    
    	while ((p = pool->alloc_pool))
    	{
    		pool->alloc_pool = *(void **) pool->alloc_pool;
    		free(p);
    	}
    	
    	if (pool->lock != NULL)
    	{
    		pthread_mutex_destroy(pool->lock);
    		free(pool->lock);
    	}
    	free(pool);	
	}
}