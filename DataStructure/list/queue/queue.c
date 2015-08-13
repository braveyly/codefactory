/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:queue.c
 * Abstract:queue implement
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
#include "queue.h"


/*
 * Function: Queue* create_queue( item_show_fn show, item_destroy_fn dtr )
 * Description: create one queue
 * Input:  show: item show function
 *         dtr: item destroy function
 * Output: none
 * Return: the queue object newly created
 * Others: none
 */
Queue* create_queue( item_show_fn show, item_destroy_fn dtr )
{
	Queue* q = (Queue*)malloc( sizeof( Queue ) );
	memset( q, 0, sizeof( Queue ) );
	q->cnt	   = 0;
	q->show	   = show;
	q->dtr	   = dtr;
	q->front	   = NULL;
	q->rear    = NULL;
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
void destroy_queue( Queue * q )
{
	Queue_Entry* head, *prior;
	if( q )
	{
		if( q->front )
		{
			prior  = q->front;
			head   = prior->next;

			do
			{
				if( q->dtr )
				{
					( *( q->dtr ) )( prior->item ); //destroy item
				}
				free( prior );                      //destroy item
				prior  = head;
				head   = head->next;
			}
			while( prior );
		}
		free( q );                                  //destroy stack
	}
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
int in_queue( Queue* q, void* item )
{
	Queue_Entry *entry;
	if( NULL == q || NULL == item )
	{
		printf( "queue/item is NULL\n" );
		return -1;
	}
	entry		   = (Queue_Entry*)malloc( sizeof( Queue_Entry ) );
	entry->item	   = item;
	entry->next	   = NULL;

	if( NULL == q->rear )
	{
		q->rear = entry;
		q->front  = entry;
		q->cnt++;
	}else
	{
		q->rear->next = entry;
		q->rear = entry;
		q->cnt++;
	}
	return q->cnt;
}

/*
 * Function: void* out_queue( Queue* q )
 * Description: pop one element out of queue
 * Input:  q: the queue object
 * Output: none
 * Return: void:the item returned
 * Others: none
 */
void* out_queue( Queue* q )
{
	Queue_Entry *entry;
	if( NULL == q || NULL == q->front )
	{
		printf( "queue/front is NULL\n" );
		return NULL;
	}
	entry	   = q->front;
	q->front   = q->front->next;
	q->cnt--;
	return entry->item;
}

/*
 * Function: void queue_show( Queue* q )
 * Description: show one queue
 * Input:  q: the queue object
 * Output: none
 * Return: void
 * Others: none
 */
void queue_show( Queue* q )
{
	Queue_Entry *entry;
	if( NULL == q )
	{
		printf( "queue is NULL\n" );
		return NULL;
	}
	entry = q->front;
	printf( "queue cnt is %d\n", q->cnt );
	while( entry )
	{
		if( q->show )
		{
			( *( q->show ) )( entry->item );
		}
		entry = entry->next;
	}
}

/*
 * Function: int get_count( Queue* q )
 * Description: get the count of one queue
 * Input:  q: the queue
 * Output: none
 * Return: int:the queue element count
 * Others: none
 */
int get_count( Queue* q )
{
	if( NULL == q )
	{
		printf( "queue is NULL\n" );
		return -1;
	}
	return q->cnt;
}

/*
 * End of file
 */
