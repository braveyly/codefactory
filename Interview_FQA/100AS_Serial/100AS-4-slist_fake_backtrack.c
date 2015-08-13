/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100AS-4-fake_backtrack.c
 * Abstract:fake backtrack to delete the prior node 
 *			for single list without header
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

static List		* l;

int* create_int( int i )
{
	int* tmp = (int*)malloc( sizeof( int ) );
	*tmp = i;
	return tmp;
}

void show_int( void* data )
{
	int* tmp = (int*)data;
	printf( "int is %d\n", *tmp );
}

void delete_int( int *i )
{
	free( i );
}

List_Entry* middle_node()
{
	List_Entry	* entry, *entry_return;
	int			*pitmp;
	int			i;

	l = create_list( );

	for( i = 0; i < 50; i++ )
	{
		pitmp  = create_int( i );
		entry  = create_entry( pitmp );
		list_append( l, entry );
		if( 39 == i )
		{
			entry_return = entry;
		}
	}
	return entry_return;
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
	List_Entry	* entry, *next;

	entry = middle_node();

	printf("List show info before deleting the node\n");
	show_list(l,show_int);

	next = entry->next;
	entry->item = next->item;
	next = next->next;
	entry->next = next;

	printf("List show info before deleting the node\n");
	show_list(l,show_int);
}
/*
 * End of file
 */
