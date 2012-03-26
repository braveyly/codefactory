#ifndef _MY_MEM_POOL_H
#define _MY_MEM_POOL_H
#include <pthread.h>

#define COLLECTION_MODE_NOSYNC            8
struct _cp_mempool;

typedef void (*cp_mempool_callback_fn)(void *prm, 
									   struct _cp_mempool *pool, 
									   void *mem);

typedef struct _cp_mempool
{
	size_t item_size;
	size_t alloc_size;
	size_t items_per_alloc;

	char *reuse_pool;
	char *alloc_pool;
	size_t alloc_pool_pos;
	
	cp_mempool_callback_fn alloc_callback;
	void *callback_prm;

	int mode;
	pthread_mutex_t *lock;
} cp_mempool;

#define cp_mempool_item_size(p) ((p)->item_size)

/* cp_mempool_create_by_option */
cp_mempool *cp_mempool_create_by_option(const int mode, 
                       		            size_t chunksize, 
                                    	size_t multiple);

/* cp_mempool_create_by_option */
cp_mempool *cp_mempool_create(const size_t chunksize);

/* increment refcount */
int cp_mempool_inc_refcount(cp_mempool *pool);

/* cp_mempool_alloc */
void *cp_mempool_alloc(cp_mempool * const pool);

/* cp_mempool_calloc */
void *cp_mempool_calloc(cp_mempool * const pool);

/* cp_mempool_free */
int cp_mempool_free(cp_mempool * const pool, void *data);

/* cp_mempool_destroy */
void cp_mempool_destroy(cp_mempool *pool);

#endif
