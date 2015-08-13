/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:stack.h
 * Abstract:stack api
 *
 * Current version:v0.1
 * Author:braveyly
 * Date:2012-4-9
 * Modification:first version

 * Revise version:
 * Author:
 * Date:
 * Modification:
 */
#ifndef _STACK_H
#define _STACK_H
#include <stdio.h>


#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif


typedef void ( *item_show_fn )( void* );
typedef void ( *item_destroy_fn )( void* );

typedef struct _stack_entry_
{
	void				* item;
	struct _list_entry	* next;
}Stack_Entry;

typedef struct _stack_
{
	int			cnt;
	Stack_Entry	* head;
	item_show_fn show;
	item_destroy_fn dtr;
}Stack;

/*stack construct and destruct*/
Stack* create_stack(show_item, destroy_item);
void destroy_stack( Stack * );

/*append and pop in the tail*/
int stack_push( Stack*, void*);
void* stack_pop(Stack*);

void stack_show(Stack*);
int get_count(Stack*);

#endif


/*
 * End of file
 */
