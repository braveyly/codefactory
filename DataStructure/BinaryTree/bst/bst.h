/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:bst.h
 * Abstract:binary search tree api
 *
 * Current version:v0.1
 * Author:braveyly
 * Date:2012-4-3
 * Modification:first version

 * Revise version:
 * Author:
 * Date:
 * Modification:
 */
#ifndef _BST_H
#define _BST_H
#include <stdio.h>

#define DEBUG
#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif


typedef void ( *item_show_fn )( void* );
typedef void ( *item_destroy_fn )( void* );
typedef int  ( *item_compare_fn ) ( void *, void * );

typedef struct _bst_entry_
{
	void				* item;
	struct _bst_entry	* left, *right;
}Bst_Entry;

typedef struct _bst_
{
	int		   cnt;
	item_show_fn show;
	item_destroy_fn dtr;
	item_compare_fn comp;
	Bst_Entry	* root;
}Bst;

/*bt construct and destruct*/
Bst* create_bst(item_show_fn, item_destroy_fn, item_compare_fn);
void destroy_bst( Bst ** );

/*bst insert, delete and search operations*/
int bst_insert(Bst**, void*);
int bst_delete(Bst**, void*);
int bst_search(Bst*,void*);

/*pre, in and post order*/
void pre_order(Bst*);
void in_order(Bst*);
void post_order(Bst*);

#endif


/*
 * End of file
 */

