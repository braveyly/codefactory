/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:stack.c
 * Abstract:stack implement
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


/*
 * Function: Stack* create_stack( item_show_fn show, item_destroy_fn dtr )
 * Description: create one stack
 * Input:  show: item show function
 *         dtr: item destroy function
 * Output: none
 * Return: the stack object newly created
 * Others: none
 */
Stack* create_stack( item_show_fn show, item_destroy_fn dtr )
{
	Stack* s = (Stack*)malloc( sizeof( Stack ) );
	memset( s, 0, sizeof( Stack ) );
	s->cnt	   = 0;
	s->show	   = show;
	s->dtr	   = dtr;
	s->head	   = NULL;
	return s;
}

/*
 * Function: void destroy_stack( Stack * s )
 * Description: destroy stack object
 * Input:  s: the stack object
 * Output: none
 * Return: void
 * Others: none
 */
void destroy_stack( Stack * s )
{
	Stack_Entry* head, *prior;
	if( s )
	{
		if( s->head )
		{
			prior  = s->head;
			head   = prior->next;

			do
			{
				if( s->dtr )
				{
					( *( s->dtr ) )( prior->item ); //destroy item
				}
				free( prior );                      //destroy item
				prior  = head;
				head   = head->next;
			}
			while( prior );
		}
		free( s );                                  //destroy stack
	}
}

/*
 * Function: int stack_push( Stack* s, void* item )
 * Description: push one item into stack
 * Input:  s: the stack object
 *         item: the item pointer
 * Output: none
 * Return: int:the stack element count
 * Others: none
 */
int stack_push( Stack* s, void* item )
{
	Stack_Entry *entry;
	if( NULL == s || NULL == item )
	{
		printf( "stack/item is NULL\n" );
		return -1;
	}
	entry		   = (Stack_Entry*)malloc( sizeof( Stack_Entry ) );
	entry->item	   = item;
	entry->next	   = NULL;

	if( NULL == s->head )
	{
		s->head = entry;
		s->cnt++;
	}else
	{
		entry->next	   = s->head;
		s->head		   = entry;
		s->cnt++;
	}
	return s->cnt;
}

/*
 * Function: void* stack_pop( Stack* s )
 * Description: pop one element out of stack
 * Input:  s: the stack object
 * Output: none
 * Return: void:the item returned
 * Others: none
 */
void* stack_pop( Stack* s )
{
	Stack_Entry *entry;
	if( NULL == s || NULL == s->head )
	{
		printf( "stack is NULL\n" );
		return NULL;
	}
	entry	   = s->head;
	s->head	   = entry->next;
	s->cnt--;
	return entry->item;
}

/*
 * Function: void stack_show( Stack* s )
 * Description: show one stack
 * Input:  s: the stack object
 * Output: none
 * Return: void
 * Others: none
 */
void stack_show( Stack* s )
{
	Stack_Entry *entry;
	if( NULL == s )
	{
		printf( "stack is NULL\n" );
		return NULL;
	}
	entry = s->head;
	printf( "stack cnt is %d\n", s->cnt );
	while( entry )
	{
		if( s->show )
		{
			( *( s->show ) )( entry->item );
		}
		entry = entry->next;
	}
}

/*
 * Function: int get_count( Stack* s )
 * Description: get the count of one stack
 * Input:  s: the stack
 * Output: none
 * Return: int:the stack element count
 * Others: none
 */
int get_count( Stack* s )
{
	if( NULL == s )
	{
		printf( "stack is NULL\n" );
		return -1;
	}
	return s->cnt;
}

/*
 * End of file
 */
