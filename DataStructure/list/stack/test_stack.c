/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:test_stack.c
 * Abstract:stack test case
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
#include "stack.h"
typedef struct _ele_
{
	int age;
	char* name;
	int len;
	void* data;
}Ele;


/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
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

/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
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

/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
void show_ele( void* data )
{
	Ele* tmp = (Ele*)data;
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
	Stack	* s;
	Ele		* ele;

	s = create_stack( show_ele, delete_ele );

	ele = create_ele( 28, "liwei", 10, "hust++life" );
	stack_push( s, ele );

	ele = create_ele( 24, "lijia", 10, "hust++acon" );
	stack_push( s, ele );

	ele = create_ele( 26, "lijing", 10, "hust++acon" );
	stack_push( s, ele );

	stack_show( s );

	printf( "=============\n" );

	ele = stack_pop( s );
	show_ele( ele );

	ele = stack_pop( s );
	show_ele( ele );
}

/*
 * End of file
 */
