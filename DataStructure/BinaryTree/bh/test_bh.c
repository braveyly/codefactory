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
#include "bh.h"


void show_string( void* data, void* prim )
{
	char* str = (char*)data;
	printf( " %s--%s\n", prim, str );
}

int main( )
{
	Bh		* bh;
	char	a[10][10] = { "a", "z", "b", "j", "c" };
	bh = bh_create( 10, strcmp, NULL );

	bh_push( bh, a[0] );
	bh_push( bh, a[1] );
	bh_push( bh, a[2] );
	bh_push( bh, a[3] );
	bh_push( bh, a[4] );
	printf( "bh->heap[0] %s %s %s %s %s\n", (char*)( bh->heap[1] ), (char*)( bh->heap[2] ), (char*)( bh->heap[3] ), (char*)( bh->heap[4] ), (char*)( bh->heap[5] ) );

	//printf( "%s\n",(char*)bh_pop(bh) );
	//printf( "%s\n",(char*)bh_pop(bh) );
	//printf( "%s\n",(char*)bh_pop(bh) );
	//printf( "%s\n",(char*)bh_pop(bh) );
	//printf( "%s\n",(char*)bh_pop(bh) );
	bh_callback( bh, show_string, "liwei" );
	printf( "peek %s\n", (char*)bh_peek( bh ) );
	printf( "verify is %d\n", bh_verify( bh, MAX_HEAP ) );
	bh_destroy( bh );
}

/*
 * End of file
 */
