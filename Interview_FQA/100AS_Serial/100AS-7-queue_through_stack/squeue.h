/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:squeue.h
 * Abstract:queue implemented by stack api
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
#ifndef _SQUEUE_H
#define _SQUEUE_H
#include <stdio.h>
#include "stack.h"

#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif

typedef struct _squeue_
{
	Stack* s1;
	Stack* s2;
}Squeue;

/*queue construct and destruct*/
Squeue* create_squeue(item_show_fn show, item_destroy_fn dtr);
void destroy_squeue( Squeue * );

/*in in the head and out in the tail*/
int in_squeue( Squeue*, void*);
void* out_squeue(Squeue*);

void squeue_show(Squeue*);
int get_squeue_count( Squeue* q );

#endif


/*
 * End of file
 */
