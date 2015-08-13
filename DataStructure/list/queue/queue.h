/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:queue.h
 * Abstract:queue api
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
#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdio.h>


#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif


typedef void ( *item_show_fn )( void* );
typedef void ( *item_destroy_fn )( void* );

typedef struct _queue_entry_
{
	void				* item;
	struct _queue_entry	* next;
}Queue_Entry;

typedef struct _queue_
{
	int			cnt;
	Queue_Entry	* front;
	Queue_Entry * rear;
	item_show_fn show;
	item_destroy_fn dtr;
}Queue;

/*queue construct and destruct*/
Queue* create_queue(show_item, destroy_item);
void destroy_queue( Queue * );

/*in in the head and out in the tail*/
int in_queue( Queue*, void*);
void* out_queue(Queue*);

void queue_show(Queue*);
int get_count(Queue*);

#endif


/*
 * End of file
 */
