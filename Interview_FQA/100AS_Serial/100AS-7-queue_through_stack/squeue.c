/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:squeue.c
 * Abstract:queue implemented by stack
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
#include "squeue.h"


/*
 * Function: Queue* create_queue( item_show_fn show, item_destroy_fn dtr )
 * Description: create one queue
 * Input:  show: item show function
 *         dtr: item destroy function
 * Output: none
 * Return: the queue object newly created
 * Others: none
 */
Squeue* create_squeue(item_show_fn show, item_destroy_fn dtr)
{
	Squeue* q = (Squeue*)malloc( sizeof( Squeue ) );
	memset( q, 0, sizeof( Squeue ) );
	q->s1 = create_stack(show, dtr);
	q->s2 = create_stack(show, dtr);
	return q;
}

/*
 * Function: void destroy_queue( Queue * q )
 * Description: destroy queue object
 * Input:  q: the queue object
 * Output: none
 * Return: void
 * Others: none
 */
void destroy_squeue( Squeue * q )
{
	if(q->s1)
		destroy_stack(q->s1);
	if(q->s2)
		destroy_stack(q->s2);
	free(q);
}

/*
 * Function: int in_queue( Queue* q, void* item )
 * Description: push one item into queue
 * Input:  q: the queue object
 *         item: the item pointer
 * Output: none
 * Return: int:the queue element count
 * Others: none
 */
int in_squeue( Squeue* q, void* item )
{
	if( NULL == q || NULL == item ||
		NULL == q->s1 || NULL == q->s2 )
	{
		printf( "queue/item is NULL\n" );
		return -1;
	}

	return stack_push(q->s1, item);
}

/*
 * Function: void* out_queue( Queue* q )
 * Description: pop one element out of queue
 * Input:  q: the queue object
 * Output: none
 * Return: void:the item returned
 * Others: none
 */
void* out_squeue( Squeue* q )
{
	int i;
	if( NULL == q ||
		NULL == q->s1 || NULL == q->s2 )
	{
		printf( "queue/front is NULL\n" );
		return NULL;
	}
	
	i = get_count(q->s1);
	printf( "stack s1 cnt is %d\n", get_count(q->s1) );
	printf( "stack s2 cnt is %d\n", get_count(q->s2) );

	while( i )
	{
		stack_push(q->s2,stack_pop(q->s1));
		i--;
	}

	return stack_pop(q->s2);
}

/*
 * Function: int get_count( Queue* q )
 * Description: get the count of one queue
 * Input:  q: the queue
 * Output: none
 * Return: int:the queue element count
 * Others: none
 */
int get_squeue_count( Squeue* q )
{
	if( NULL == q )
	{
		printf( "queue is NULL\n" );
		return -1;
	}
	return q->s1->cnt+q->s2->cnt;
}

/*
 * End of file
 */
