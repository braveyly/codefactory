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
#include "bt.h"


void show_string( void* data )
{
	char* str = (char*)data;
	printf( " %s\n", str );
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
	int		depth	   = 0;
	Bt		* bt	   = create_bt( );
	char	a[10][10]  = { "2", "8", "1", "3", "9", "4", "7", "5", "6", "0" };
	char	*b[10];
	int		i;
	Bt_Entry* entry;

	for( i = 0; i < 10; i++ )
	{
		b[i] = a[i];
	}
	init_bt( bt, b, 10, strcmp );
	printf("pre order for this bt\n");
	pre_order(bt->root,show_string);
	printf("in order for this bt\n");
	in_order( bt->root, show_string );
	printf("post order for this bt\n");
	post_order(bt->root,show_string);
	printf("level order for this bt\n");
	level_order(bt->root,show_string);
	special_level_order(bt->root,2,show_string);

	depth = calc_tree_depth( bt->root );
	printf( "depth is %d\n", depth );

	entry = get_entry( bt->root, "3", strcmp );
	if( entry )
	{
		printf( "entry item is %s\n", entry->item );
	} else
	{
		printf( "entry is NULL\n" );
	}

	set_entry( bt->root, "3", "33", strcmp );
	in_order( bt->root, show_string );

	entry = get_parent( bt->root, "33", strcmp );
	printf( "parent item is %s\n", entry->item );

	entry = get_right( bt->root, "5", strcmp );

	if( entry )
	{
		printf( "right item is %s\n", entry->item );
	}

	entry = get_left( bt->root, "8", strcmp );

	if( entry )
	{
		printf( "left item is %s\n", entry->item );
	}

	destroy_bt( bt, NULL );
}

/*
 * End of file
 */
