/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:test_slist.c
 * Abstract:single list test case
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
#include "slist.h"
typedef struct _ele_
{
	int age;
	char* name;
	int len;
	void* data;
}Ele;


Ele* create_ele( int age, char* name, int len, void* data )
{
	Ele* tmp;
	tmp = (Ele*)malloc( sizeof( Ele ) / sizeof( char ) );
	if( !tmp )
	{
		return NULL;
	}

	memset( tmp, 0, sizeof( Ele ) );
	tmp->age = age;
	if( name )
	{
		tmp->name = (char*)malloc( strlen( name ) + 1 );
		strcpy( tmp->name, name );
	}
	tmp->len = len;
	if( len )
	{
		tmp->data = (char*)malloc( len );
	}

	if( data )
	{
		memcpy( tmp->data, data, len );
	}

	return tmp;
}

void delete_ele( void* data )
{
	Ele* tmp = (Ele*)data;
	if( tmp->name )
	{
		free( tmp->name );
	}
	if( tmp->data )
	{
		free( tmp->data );
	}
}

void show_ele( void* data )
{
	Ele* tmp = (Ele*)data;
	printf( "age is %d\n", tmp->age );
	printf( "name is %s\n", tmp->name  );
	printf( "data is %s\n", (char*)tmp->data );
}

int main( )
{
	List		* l;
	List_Entry	* entry;
	Ele			* ele;

	l = create_list( );

	ele	   = create_ele( 28, "liwei", 10, "hust++life" );
	entry  = create_entry( ele );
	list_append( l, entry );

	ele	   = create_ele( 24, "lijia", 10, "hust++acon" );
	entry  = create_entry( ele );
	list_append( l, entry );

	ele	   = create_ele( 26, "lijing", 10, "hust++acon" );
	entry  = create_entry( ele );
	list_append( l, entry );

	entry = list_pop( l );
	show_entry( entry, show_ele );
	printf( "\n\n\n\n" );
	show_list( l, show_ele );

	destroy_list( l, delete_ele );
}

/*
 * End of file
 */
