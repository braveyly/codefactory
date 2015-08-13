/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100AS-1-max_k.c
 * Abstract:Get the max k elements from array
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
#include <stdio.h>
#include <string.h>

/*utility functions*/
typedef enum _sort_mode_
{
	ASCEND_MODE = 0,
	DESCEND_MODE,
}sort_mode;
typedef int ( *cp_compare_fn )( void *, void * );
typedef int ( *cp_callback_fn )( void *entry, void *client_prm );


int callback_print_array( void** array, int size, cp_callback_fn print_fn )
{
	int i = 0;
	for( i = 0; i < size; i++ )
	{
		( *print_fn )( array[i], &i );
	}
	return 1;
}

int print_my_array( char* ele, int* index )
{
	printf( "[%d]=[%s]\n", *index, ele );
}

void swap( void** ele_a, void** ele_b )
{
	void* tmp;
	printf( "%s<->%s\n", *ele_a, *ele_b );
	tmp	   = *ele_a;
	*ele_a = *ele_b;
	*ele_b = tmp;
}

/*real function to get max k elements through bubble ordering method*/
void** get_max_k_by_sort_bubble( void** array, int size, int k, cp_compare_fn cmp_fn, sort_mode mode )
{
	int		i, begin_index = 0, end_index = size, repeat_flag;
	void	* temp;

	if( ASCEND_MODE != mode && DESCEND_MODE != mode )
	{
		mode = ASCEND_MODE;
	}

	do
	{
		repeat_flag = 0;
		for( i = begin_index; i < end_index - 1; i++ )
		{
			/*If ASCEND_MODE, Move MAX to end; Else Move MIN to end*/
			if( ( *cmp_fn )( array[i], array[i + 1] ) > 0 &&
			    ASCEND_MODE == mode ||
			    ( *cmp_fn )( array[i], array[i + 1] ) < 0 &&
			    DESCEND_MODE == mode )
			{
				swap( &array[i], &array[i + 1] );
				repeat_flag = 1;
			}
		}
		end_index--;
	}
	while( repeat_flag && ( k + end_index > size ) );

	return array;
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
	char* a[5] = { "z", "g", "r", "y", "f" };
	char** b   = ( char** )get_max_k_by_sort_bubble( &a, 5, 2, ( cp_compare_fn )strcmp, ASCEND_MODE );
	callback_print_array( b, 5, print_my_array );
}

/*
 * End of file
 */
