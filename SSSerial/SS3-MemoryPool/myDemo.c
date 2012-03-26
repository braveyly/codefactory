#include "myMemPool.h"

typedef struct _node
{
	char *item;
	struct _node *next;
} node;

void printMemPoolInfo( cp_mempool* pool )
{
	printf( "Start to put out memory pool info\n" );
	printf( "pool->alloc_pool [%d]\n", pool->alloc_pool );
	printf( "pool->reuse_pool [%d]\n", pool->reuse_pool );
	printf( "pool->alloc_pool_pos [%d]\n", pool->alloc_pool_pos );
	printf( "pool->items_per_alloc [%d]\n", pool->items_per_alloc );
	printf( "pool->alloc_size [%d]\n", pool->alloc_size );
	printf( "pool->item_size [%d]\n", pool->item_size );	
}

int main()
{
	cp_mempool *node_pool;
    node_pool = cp_mempool_create_by_option(COLLECTION_MODE_NOSYNC, 
                                            sizeof(node), 20);

	node* n1, *n2, *n3, *n4, *n5;

    n1 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );

    n2 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );

    n3 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );

    n4 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );

    n5 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );
	
	cp_mempool_free(node_pool, n1);
	printMemPoolInfo( node_pool );

	cp_mempool_free(node_pool, n3);
	printMemPoolInfo( node_pool );

	cp_mempool_free(node_pool, n5);
	printMemPoolInfo( node_pool );
	
    n2 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );

    n2 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );

    n2 = (node *) cp_mempool_alloc(node_pool);
	printMemPoolInfo( node_pool );
}
