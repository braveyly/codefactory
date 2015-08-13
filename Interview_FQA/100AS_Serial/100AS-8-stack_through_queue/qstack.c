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
#include "qstack.h"


/*
 * Function: Stack* create_stack( item_show_fn show, item_destroy_fn dtr )
 * Description: create one stack
 * Input:  show: item show function
 *         dtr: item destroy function
 * Output: none
 * Return: the stack object newly created
 * Others: none
 */
Qstack* create_qstack( item_show_fn show, item_destroy_fn dtr )
{
	Qstack* s = (Qstack*)malloc( sizeof( Qstack ) );
	s->q1  = create_queue( show, dtr );
	s->q2  = create_queue( show, dtr );
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
void destroy_qstack( Qstack * s )
{
	if( s )
	{
		if( s->q1 )
		{
			destroy_queue( s->q1 );
		}
		if( s->q2 )
		{
			destroy_queue( s->q2 );
		}

		free( s );
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
int qstack_push( Qstack* s, void* item )
{
	int ret;
	if( s->q1 )
	{
		ret = in_queue( s->q1, item );
	}else if( s->q2 )
	{
		ret = in_queue( s->q2, item );
	}else
	{
		ret = -1;
	}
	return ret;
}

/*
 * Function: void* stack_pop( Stack* s )
 * Description: pop one element out of stack
 * Input:  s: the stack object
 * Output: none
 * Return: void:the item returned
 * Others: none
 */
void* qstack_pop( Qstack* s )
{
	int i, cnt1, cnt2;
	cnt1   = get_count( s->q1 );
	cnt2   = get_count( s->q2 );
	if( cnt1 > 0 )
	{
		for( i = 0; i < cnt1 - 1; i++ )
		{
			in_queue( s->q2, out_queue( s->q1 ) );
		}
		return out_queue( s->q1 );
	}

	if( cnt2 > 0 )
	{
		for( i = 0; i < cnt2 - 1; i++ )
		{
			in_queue( s->q1, out_queue( s->q2 ) );
		}
		return out_queue( s->q2 );
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
int get_qstack_count( Qstack* s )
{
	if( NULL == s )
	{
		printf( "stack is NULL\n" );
		return -1;
	}
	return get_count( s->q1 ) + get_count( s->q2 );
}

/*
 * End of file
 */
