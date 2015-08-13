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

List* init_list()
{
	List		* l;
	List_Entry	* entry, *entry_return;
	int			*pitmp;
	int			i;

	l = create_list( );

	for( i = 0; i < 50; i++ )
	{
		pitmp  = create_int( i );
		entry  = create_entry( pitmp );
		list_append( l, entry );
	}
	return l;
}

List* reverse_list(List* l)
{
	List_Entry *head, *new_head, *new_next;
	if(!l->head)
		return NULL;

	head = new_head = l->head; // two heads point to head of list

	/*initial shift next operations
	  index 0 points to null, but not index 1
	  index 1 points to index 2
	*/
	head = head ->next; // index 1
	new_next = NULL;    // null
	new_head->next = new_next; // index 0 which points to null

	while(head)
	{
		/*  left -------------> right
		  new_next <- new_head  head -> next
		      1          2        3       4
		  1 -> 2
		  2 -> 3
		  3 -> 4
		  new_head->next = new_next;
		*/
		new_next = new_head;  // new_next shift to new_head
		new_head = head;      // new_head shift to head
		head = head->next;    // head shift to next
		
		new_head->next = new_next; // new_head point to new_next
	}
	return new_head;
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
	List* l;
	l = init_list();
	printf("List show info before sequence order\n");
	show_list(l,show_int);

	l = reverse_list(l);
	printf("List show info before reversed order\n");
	show_list(l,show_int);
}
/*
 * End of file
 */
