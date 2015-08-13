/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:bh.c
 * Abstract:binary heap implement
 *
 * Current version:0.1
 * Author:braveyly
 * Date:2012-04-09
 * Modification:first draft

 * Revise version:
 * Author:
 * Date:
 * Modification:
 */
#include "bh.h"

#define PARENT( x )		( ( x ) >> 1 )
#define LEVELSIZE( x )	( 1 << ( x ) )
#define LEFTCHILD( x )	( ( x << 1 ) )


/*
 * Function: void bh_info( Bh *h )
 * Description: display binary heap information
 * Input:  h: the binary heap object
 * Output: none
 * Return: void
 * Others: none
 */
void bh_info( Bh *h )
{
	_( printf( "--show bp info---\n" ) );
	_( printf( "-----hight %d\n", h->heap_height ) );
	_( printf( "-----length %d\n", h->heap_length ) );
	_( printf( "-----end %d\n", h->heap_end ) );
}

/*
 * Function: unsigned int bh_count( Bh *h )
 * Description: get the element count of one bh
 * Input:  h: the bh object
 * Output: none
 * Return: the count
 * Others: none
 */
unsigned int bh_count( Bh *h )
{
	return h->heap_end;
}

/*
 * Function: unsigned int bh_size( Bh *h )
 * Description: get the length of bh array
 * Input:  h: the bh object
 * Output: none
 * Return: the length
 * Others: none
 */
unsigned int bh_size( Bh *h )
{
	return h->heap_length;
}

/*
 * Function: Bh *bh_create( int initial_size,
               				item_compare_fn comp,
               				item_destructor_fn dtr )
 * Description: create one bh object
 * Input:  initial_size: the bh array size that you want
 *         comp: item comparing function
 *		   dtr : item destroying function
 * Output: none
 * Return: Bh*: the bh object newly created
 * Others: none
 */
Bh *bh_create( int initial_size,
               item_compare_fn comp,
               item_destructor_fn dtr )
{
	Bh* h;
	if( initial_size <= 0 )
	{
		return NULL;
	}
	h			   = (Bh*)malloc( sizeof( Bh ) );
	h->heap_length = 0;
	h->heap_height = 0;

	/*heap length is less than initial size*/
	while( h->heap_length + LEVELSIZE( h->heap_height ) <= initial_size )
	{
		h->heap_length += LEVELSIZE( h->heap_height );
		h->heap_height++;
	}
	;
	h->heap_height--; //current heap tree height

	h->heap_end	   = 0;
	h->heap		   = (void*)malloc( sizeof( void* ) * h->heap_length );
	h->comp		   = comp;
	h->dtr		   = dtr;
	bh_info( h );
	return h;
}

/*
 * Function: void bh_destroy( Bh *h )
 * Description: destroy one bh
 * Input:  h: the bh object
 * Output: none
 * Return: void
 * Others: none
 */
void bh_destroy( Bh *h )
{
	_( printf( "bh_destroy\n" ) );
	bh_info( h );

	if( h )
	{
		if( h->heap )
		{
			if( h->dtr )
			{
				int i;
				for( i = 1; i <= h->heap_end; ++i )
				{
					if( h->heap[i] )
					{
						( *h->dtr )( h->heap[i] );
					}
				}
			}
			free( h->heap );
		}
		free( h );
	}
}

/*
 * Function: static int heap_resize( Bh *h )
 * Description: reset the size of the bh array which keeps the original data
 * Input:  h: the bh object
 * Output: none
 * Return: int:
 *			0: succeed
 *		   -1: failed
 * Others: none
 */
static int heap_resize( Bh *h )
{
	if( h->heap_end + 1 >= h->heap_length )
	{
		/* resize heap */
		void			**temp;
		unsigned int	new_length =
		    h->heap_length + LEVELSIZE( h->heap_height + 1 );

		_( printf( "heap [%p] resize %d -> %d", h, h->heap_length, new_length ) );

		temp = realloc( h->heap, new_length * sizeof( void * ) );
		if( temp == NULL )
		{
			printf( "can\'t resize heap" );
			return -1;
		}

		h->heap = temp;
		h->heap_height++;
		h->heap_length = new_length;
	}
	bh_info( h );
	return 0;
}

static void swap( void **a, void **b )
{
	void *c = *a;

	*a = *b;
	*b = c;
}


static void siftup( Bh *h )
{
	unsigned int c = h->heap_end;

	while( c != 1 && h->comp( h->heap[PARENT( c )], h->heap[c] ) <= 0 )
	{
		_( printf( "siftup\n" ) );
		bh_info( h );

		swap( &( h->heap[PARENT( c )] ), &( h->heap[c] ) );
		c = PARENT( c );
	}
}

/*
 * Function: int bh_push( Bh *h, void *in )
 * Description: push one item into bh
 * Input:  h: the bh object
 *         in: the item
 * Output: none
 * Return: int:
 *			0: succeed
 *		   -1: failed
 * Others: none
 */
int bh_push( Bh *h, void *in )
{
	int				rc = 0;
	unsigned int	he;

	if( ( rc = heap_resize( h ) ) )
	{
		goto DONE;
	}


	/*
	   root is 1, not 0, to do the same shift operation to get parent
	   for left and right children
	 */
	h->heap_end++;
	he			   = h->heap_end;
	h->heap[he]	   = in;

	siftup( h );

	bh_info( h );

DONE:
	return rc;
}

static void siftdown( Bh *h, unsigned int p )
{
	unsigned int c;

	_( printf( "siftdown start\n" ) );

	for( c = LEFTCHILD( p ); c < h->heap_end; p = c, c = LEFTCHILD( p ) )
	{
		_( printf( " c = %i, h->heap_end = %i\n", c, h->heap_end ) );

		if( h->comp( h->heap[c], h->heap[c + 1] ) <= 0 )
		{
			c++;
		}

		/* c points to the largest among the children of p */
		if( h->comp( h->heap[p], h->heap[c] ) <= 0 )
		{
			swap( &( h->heap[p] ), &( h->heap[c] ) );
		} else
		{
			return;
		}
	}
	if( c == h->heap_end && h->comp( h->heap[p], h->heap[c] ) <= 0 )
	{
		swap( &( h->heap[p] ), &( h->heap[c] ) );
	}
}

/*
 * Function: void *bh_pop( Bh *h )
 * Description: delete the root element and return it
 * Input:  h: the bh object
 * Output: none
 * Return: void*: the root element
 * Others: none
 */
void *bh_pop( Bh *h )
{
	int		rc;
	void	*retval = NULL;

	if( h == NULL )
	{
		return NULL;
	}

	bh_info( h );

	if( h->heap != NULL && h->heap_end >= 1 )
	{
		retval = h->heap[1];

		if( h->dtr )
		{
			( *h->dtr )( retval );
		}

		h->heap[1] = h->heap[h->heap_end]; // root points to the last node
		h->heap_end--;
		siftdown( h, 1 );
	}

	return retval;
}

/*
 * Function: void *bh_peek( Bh *h )
 * Description: get the root element, but not delete it
 * Input:  h: the bh object
 * Output: none
 * Return: void*: the root element
 * Others: none
 */
void *bh_peek( Bh *h )
{
	void *item = NULL;
	if( h == NULL )
	{
		return NULL;
	}

	if( h->heap && h->heap_end >= 1 )
	{
		item = (void *)( h->heap[1] );
	}

	return item;
}

/*
 * Function: void bh_callback( Bh *h, item_show_fn cb, void *prm )
 * Description: traversal the bh array, and show item with cb function with prm parameter
 * Input:  h: the bh object
 *         cb: item show function
 *		  prm: the first parameter to show item
 * Output: none
 * Return: void
 * Others: none
 */
void bh_callback( Bh *h, item_show_fn cb, void *prm )
{
	int				rc;
	unsigned int	i;

	if( h == NULL )
	{
		return;
	}

	if( !h->heap || h->heap_end < 1 )
	{
		return;
	}

	for( i = 1; i <= h->heap_end; i++ )
	{
		( *cb )( h->heap[i], prm );
	}
}

/*
 * Function: int bh_verify( Bh *h, heap_type type )
 * Description: verify the bh whether it confirms with MIN or MAX heap
 * Input:  h: the bh object
 *         type: MIN_HEAP or MAX_HEAP
 * Output: none
 * Return: int:
 				1:confirm
 			    0: not confirm
 * Others: none
 */
int bh_verify( Bh *h, heap_type type )
{
	unsigned int i;

	if( !h || !h->heap || h->heap_end < 1 )
	{
		return 1;
	}

	for( i = h->heap_end; i > PARENT( h->heap_end ); --i )
	{
		unsigned int c = i;
		if( MIN_HEAP == type )
		{
			while( c != 1 && ( h->comp( h->heap[PARENT( c )], h->heap[c] ) < 0 ) )
			{
				c = PARENT( c );
			}
		}else
		{
			while( c != 1 && ( h->comp( h->heap[PARENT( c )], h->heap[c] ) > 0 ) )
			{
				c = PARENT( c );
			}
		}

		if( c != 1 )
		{
			return 0;
		}
	}
	return 1;
}

/*
 * End of file
 */
