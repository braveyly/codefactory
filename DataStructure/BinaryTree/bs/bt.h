/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:bt.h
 * Abstract:binary tree api
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
#ifndef _BS_H
#define _BS_H
#include <stdio.h>

#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif


typedef void ( *show_item )( void* );
typedef void ( *destroy_item )( void* );
typedef int ( *compare_item) ( void *, void * );

typedef struct _bt_entry_
{
	void				* item;
	struct _bt_entry	* left, *right;
}Bt_Entry;

typedef struct _bt_
{
	int		   cnt;
	Bt_Entry	* root;
	int		   lock_type;
	void		* lock;
}Bt;

/*entry construct and destruct*/
Bt_Entry* create_entry( void* );
void destroy_entry( Bt_Entry* );
void	show_entry( Bt_Entry *, show_item );


/*bt construct and destruct*/
Bt* create_bt();
Bt* init_bt(Bt*, void**,int, compare_item );
void* destroy_bt( Bt *, destroy_item );

int is_empty(Bt*);
int calc_tree_depth(Bt_Entry*);

Bt_Entry* get_entry(Bt_Entry*, void* , compare_item);
int set_entry(Bt_Entry*, void*, void*, compare_item);

Bt_Entry* get_root(Bt*);
Bt_Entry* get_parent(Bt_Entry*,void*,compare_item);
Bt_Entry* get_left(Bt_Entry*, void*, compare_item);
Bt_Entry* get_right(Bt_Entry*,void*, compare_item);

void pre_order(Bt_Entry*, show_item);
void in_order(Bt_Entry*, show_item);
void post_order(Bt_Entry*, show_item);
void level_order(Bt_Entry*, show_item);

#endif


/*
 * End of file
 */

