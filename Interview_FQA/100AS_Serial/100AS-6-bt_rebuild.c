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
#include "bt.h"


void show_int( void* data )
{
	int* str = (int*)data;
	printf( " %d\n", *str );
}

int cmp_int( int a, int b )
{
	if( a > b )
	{
		return 1;
	} else if( a == b )
	{
		return 0;
	} else
	{
		return -1;
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
void rebuild( void* preorder, void* inorder, Bt_Entry** root, int length, compare_item cmp )
{
	Bt_Entry* entry;
	int		left_len, right_len, i;
	if( NULL == preorder || NULL == inorder || length == 0 )
	{
		return;
	}

	entry			   = create_entry( preorder );
	*root			   = entry;
	( *root )->right   = NULL;
	( *root )->left	   = NULL;

	for( i = 0; i < length; i++ )
	{
		if( 0 == cmp( inorder[i], preorder[0] ) )
		{
			left_len   = i;
			right_len  = length - i - 1;
			printf( "left_len is %d right_len is %d\n", left_len, right_len );
			break;
		}
	}
	if( left_len )
	{
		rebuild( preorder + 1, inorder, &( ( *root )->left ), left_len, cmp );
	}
	if( right_len )
	{
		rebuild( preorder + 1 + left_len, inorder + left_len + 1, &( ( *root )->right ), right_len, cmp );
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
int main( )
{
	int depth				   = 0;
	Bt	* bt				   = create_bt( );
	int pre_order_array[10]	   = { 8, 7, 4, 5, 9, 2, 3, 6, 0, 1 };
	int in_order_array[10]	   = { 4, 5, 7, 9, 8, 6, 3, 2, 1, 0 };
	int i;
	rebuild( pre_order_array, in_order_array, &( bt->root ), 10, cmp_int );
	post_order( bt->root, show_int );
}

/*
 * End of file
 */
