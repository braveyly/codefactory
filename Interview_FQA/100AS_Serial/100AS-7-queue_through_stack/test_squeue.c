/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:test_squeue.c
 * Abstract:stack queue test case
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
#include "squeue.h"
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
	Ele* tmp;
	if( !data )
	{
		return;
	}

	tmp = (Ele*)data;
	printf( "age is %d\n", tmp->age );
	printf( "name is %s\n", tmp->name );
	printf( "data is %s\n", (char*)tmp->data );
}

/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
int main( )
{
	Squeue	* q;
	Ele		* ele;

	q = create_squeue( show_ele, delete_ele );

	ele = create_ele( 28, "liwei", 10, "hust++life" );
	in_squeue( q, ele );

	ele = create_ele( 24, "lijia", 10, "hust++acon" );
	in_squeue( q, ele );

	ele = create_ele( 26, "lijing", 10, "hust++acon" );
	in_squeue( q, ele );

	printf( "=============\n" );

	ele = out_squeue( q );
	show_ele( ele );

	ele = out_squeue( q );
	show_ele( ele );

	ele = out_squeue( q );
	show_ele( ele );

	ele = out_squeue( q );
	show_ele( ele );
}

/*
 * End of file
 */
