/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:
 * Abttract:
 *
 * Current version:
 * Author:
 * Date:
 * Modification:

 * Revise version:
 * Author:
 * Date:
 * Modification:
 */
#include <string.h>
#include "bst.h"


void show_string( void* data )
{
	char* str = (char*)data;
	printf( " %s\n", str );
}


int main( )
{
	int		depth	   = 0;
	Bst		* bst	   = create_bst( show_string, NULL, strcmp );
	char	a[10][10]  = { "2", "8", "1", "3", "9", "4", "7", "5", "6", "0" };
	char	*b[10];
	int		i;
	int		ret;

	for( i = 0; i < 10; i++ )
	{
		b[i] = a[i];
	}

	bst_insert( &bst, b[0] );
	bst_insert( &bst, b[1] );
	bst_insert( &bst, b[2] );
	bst_insert( &bst, b[3] );
	bst_insert( &bst, b[4] );
	in_order( bst );
	ret = bst_search( bst, b[2] );
	printf( "search result %d\n", ret );

	ret = bst_delete( &bst, b[0] );
	printf( "0 in order after delete %d\n", ret );
	in_order( bst );

	ret = bst_delete( &bst, b[1] );
	printf( "1 in order after delete %d\n", ret );
	in_order( bst );

	ret = bst_delete( &bst, b[2] );
	printf( "2 in order after delete %d\n", ret );
	in_order( bst );
	printf("bst %p\n", bst);

	destroy_bst(&bst);
	
	printf("bst %p\n", bst);

	ret = bst_delete( &bst, b[3] );
	printf( "3 in order after delete %d\n", ret );
	in_order( bst );

	ret = bst_delete( &bst, b[4] );
	printf( "4 in order after delete %d\n", ret );

	in_order( bst );

}

/*
 * End of file
 */
