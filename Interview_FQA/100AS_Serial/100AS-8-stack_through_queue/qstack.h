/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:qstack.h
 * Abstract:stack through queue api
 *
 * Current version:v0.1
 * Author:braveyly
 * Date:2012-4-10
 * Modification:first version

 * Revise version:
 * Author:
 * Date:
 * Modification:
 */
#ifndef _STACK_H
#define _STACK_H
#include <stdio.h>
#include "queue.h"


#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif


typedef void ( *item_show_fn )( void* );
typedef void ( *item_destroy_fn )( void* );


typedef struct _qstack_
{
	Queue *q1;
	Queue *q2;
}Qstack;

/*stack construct and destruct*/
Qstack* create_qstack(show_item, destroy_item);
void destroy_qstack( Qstack * );

/*append and pop in the tail*/
int qstack_push( Qstack*, void*);
void* qstack_pop(Qstack*);
int get_qstack_count( Qstack* s );

#endif


/*
 * End of file
 */
