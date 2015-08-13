/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:
 * Abstract:
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

/*
 * Function: List_Entry* create_entry( void* data )
 * Description: create one default List_Entry object
 * Input:  data: item field
 * Output: none
 * Return: pointer to List_Entry object newly created
 * Others: none
 */
List_Entry* create_entry( void* data )
{
	List_Entry* entry = (List_Entry*)malloc( sizeof( List_Entry ) );
	entry->item	   = data;
	entry->next	   = NULL;
	return entry;
}

/*
 * Function: void destroy_entry( List_Entry* entry )
 * Description: release one List_Entry object
 * Input:  entry: release it
 * Output: none
 * Return: void
 * Others: none
 */
void destroy_entry( List_Entry* entry )
{
	if( entry )
	{
		free( entry );
	}
}

/*
 * Function: List* create_list( )
 * Description: create one default List object
 * Input: none
 * Output: none
 * Return: the List object newly created
 * Others: none
 */
List* create_list( )
{
	List* l = (List*)malloc( sizeof( List ) );
	l->cnt		   = 0;
	l->head		   = NULL;
	l->lock_type   = 0;
	l->lock		   = NULL;
	return l;
}

/*
 *void* destroy_list( List* l, destroy_item del_item )
 * Description: release one List object
 * Input:  l: the List object
 *         del_item: the function that is used to release item field of List_Entry
 * Output: none
 * Return: void
 * Others: none
 */
void* destroy_list( List* l, destroy_item del_item )
{
	List_Entry* next, *cur;
	if( !l )
	{
		printf("none for list\n");
		return;
	}

	if( !l->head )
	{
		printf("none for list entry head\n");
		free( l );
		return;
	}

	cur = l->head;
	while( cur )
	{
		next = cur->next;

		if( del_item )
		{
			( *del_item )( cur->item );
		}

		destroy_entry( cur );
		cur = next;
	}

	free( l );
}


/*
 *int list_append( List* l, List_Entry* entry )
 * Description: append one entry in the tail of the list
 * Input:  l: the List object
 *         entry: the entry object
 * Output: none
 * Return: int:
 		 -1:faild
 		 0:succeed
 * Others: none
 */
int list_append( List* l, List_Entry* entry )
{
	List_Entry* cur;
	if( !l )
	{
		printf("none for list\n");
		return -1;
	}
	cur = l->head;
	if( !cur )
	{
		l->head = entry;
		l->cnt++;
		_(printf("append to the head with cnt[%d]\n", l->cnt));
	}else
	{

		while( cur->next )
		{
			cur = cur->next;
		}
		cur->next = entry;
		l->cnt++;
		_(printf("append to the tail with cnd[%d]\n", l->cnt));
	}
	return 0;
}

/*
 * List_Entry* list_pop( List* l )
 * Description: delete the tail entry of the list and get it
 * Input:  l: the list object
 * Output: none
 * Return: List_Entry*:the entry object deleted from the tail
 * Others: none
 */
List_Entry* list_pop( List* l )
{
	List_Entry* cur, *prior;
	if( !l || !l->head )
	{
		return NULL;
	}
	cur	   = l->head;
	prior  = cur;
	while( cur->next )
	{
		_(printf("search the tail entry with cnt[%d]\n", l->cnt));
		prior  = cur;
		cur	   = cur->next;
	}
	prior->next = NULL;
	l->cnt--;
	_(printf("remove the tail entry with cnt[%d]\n", l->cnt));
	return cur;
}


int list_remove( List* l, List_Entry* entry )
{
}

int list_insert( List* l, List_Entry* offset, List_Entry* entry )
{
}

/*
 * Function: void show_list( List* l, show_item show )
 * Description: show the list information
 * Input:  l:the list object
 *         show: the function that is used to show item filed of List_Entry
 * Output: none
 * Return: viod
 * Others: none
 */
void show_list( List* l, show_item show )
{
	List_Entry* cur;
	if( !l )
	{
		printf( "none for list\n" );
		return;
	}

	if( !l->head )
	{
		printf( "none for list element\n" );
		return;
	}

	if( !show )
	{
		printf( "none for item show function\n" );
		return;
	}

	cur = l->head;
	( *show )( cur->item );
	while( cur->next )
	{
		cur = cur->next;
		( *show )( cur->item );
	}
}

/*
 * Function: void show_entry( List_Entry* entry, show_item show )
 * Description: show one entry object information
 * Input:  entry: the entry object
 *            show: the function that is used to show item field of List_Entry
 * Output: none
 * Return: viod
 * Others: none
 */
void show_entry( List_Entry* entry, show_item show )
{
	if( !entry )
	{
		printf( "none for entry\n" );
		return;
	}

	if( !show )
	{
		printf( "none for item show function\n" );
		return;
	}

	( *show )( entry->item );
}

/*
 * End of file
 */
