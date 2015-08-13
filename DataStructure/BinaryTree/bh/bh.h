/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:bh.h
 * Abstract:Min/Max binary heap
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
#ifndef _BH_H
#define _BH_H
#include <stdio.h>

#define DEBUG
#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif

typedef enum _heap_type
{
	MIN_HEAP = 0,
	MAX_HEAP,
}heap_type;
typedef int ( *item_compare_fn ) ( void *, void * );
typedef void (*item_destructor_fn)(void *);
typedef void (*item_show_fn)(void*, void*);

typedef struct _bh
{
    void **heap;                //pointer array to store binary tree
    unsigned int heap_height;   //tree height, root level is 0
    unsigned int heap_end;      //array index of last element
    unsigned int heap_length;	//array length which equals to the node total of the full tree

    item_compare_fn comp;
    item_destructor_fn dtr;
} Bh;

void bh_info(Bh *h);

Bh *bh_create( int initial_size,
               item_compare_fn comp, 
               item_destructor_fn dtr);

/* push an item */
int bh_push(Bh *h, void *in);

/* get the item at the top of the heap */
void *bh_peek(Bh *h);

/* remove the item at the top of the heap */
void *bh_pop(Bh *h);

/* contract the heap to the minimal size to accomodate current item count */
int bh_contract(Bh *h);

/* destroy the heap */
void bh_destroy(Bh *h);

/* return the number of items stored on the heap */
unsigned int bh_count(Bh *h);

/* return the maximal number of items that can be stored before another resize
 * is triggered 
 */
unsigned int bh_size(Bh *h);

int bh_verify(Bh *h);

/* run a callback on all stored elements */
void bh_callback(Bh *h, item_show_fn cb, void *prm);

#endif


/*
 * End of file
 */

