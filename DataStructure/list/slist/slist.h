/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:slist.h
 * Abstract:single list api
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
#ifndef _SLIST_H
#define _SLIST_H
#include <stdio.h>


//#define _printf(arg...) printf(##arg)  //for C99, vc6.0 is disable
/*
#ifdef DEBUG
#define _printf printf
#else
#define _printf
#endif
*/
//#define DEBUG
#ifdef DEBUG
#define _(CODE) (printf("DEBUG-%d:",__LINE__), CODE)
#else
#define _(CODE)
#endif


typedef void ( *show_item )( void* );
typedef void ( *destroy_item )( void* );

typedef struct _list_entry_
{
	void				* item;
	struct _list_entry	* next;
}List_Entry;

typedef struct _list_
{
	int			cnt;
	List_Entry	* head;
	int			lock_type;
	void		* lock;
}List;

/*entry construct and destruct*/
List_Entry* create_entry( void* );


void destroy_entry( List_Entry* );


/*list construct and destruct*/
List* create_list( );


void* destroy_list( List *, destroy_item );

/*append and pop in the tail*/
int list_append( List* l, List_Entry* entry );


List_Entry* list_pop( List* );


/*insert and remove in any position, reserve*/
int list_insert( List* l, List_Entry* offset, List_Entry* entry );


int list_remove( List* l, List_Entry* entry );


/*show list and entry*/
void	show_list( List *, show_item );
void	show_entry( List_Entry *, show_item );
#endif


/*
 * End of file
 */
