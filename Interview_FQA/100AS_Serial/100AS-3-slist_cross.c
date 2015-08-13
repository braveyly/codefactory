/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:test_slist_circle.c
 * Abstract:test slist with circle
 *
 * Current version:
 * Author:
 * Date:
 * Modification:

 * Revise version:
 * Author:
 * Date:
 * Modification:
 */
#include "slist.h"


/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
int* create_int( int i )
{
	int* tmp = (int*)malloc( sizeof( int ) );
	*tmp = i;
	return tmp;
}

/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
void show_int( void* data )
{
	int* tmp = (int*)data;
	printf( "int is %d\n", *tmp );
}

/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
void delete_int( int *i )
{
	free( i );
}

/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
int main( )
{
	List		* l1, *l2;
	List_Entry	* entry, *tail_l2;
	int			*pitmp;
	int			i;

	List_Entry	*fast, *slow;

	l1 = create_list( );
	l2 = create_list( );
	printf( "begin\n" );
	for( i = 0; i < 50; i += 2 )
	{
		pitmp  = create_int( i );
		entry  = create_entry( pitmp );
		list_append( l1, entry );
	}

	for( i = 1; i < 50; i += 2 )
	{
		pitmp  = create_int( i );
		entry  = create_entry( pitmp );
		list_append( l2, entry );
	}

	for( i = 50; i < 100; i += 5 )
	{
		pitmp  = create_int( i );
		entry  = create_entry( pitmp );


		/*cross at the entrance 50
		   never append one entry to l1 and l2 at the same time except the entrance entry
		   or make one close circle with one entry, that is , it points itself with next
		 */
		if( 50 == i )
		{
			list_append( l1, entry );
		}
		list_append( l2, entry );
	}

	printf( "show l1 and l2 before attach the tail of l2 to the head of l1\n" );
	show_list( l1, show_int );
	show_list( l2, show_int );

	/*make the tail of l2 point to the head of l1*/
	tail_l2 = l2->head;
	while( tail_l2->next )
	{
		tail_l2 = tail_l2->next;
	}
	tail_l2->next = l1->head;

	/*judge whether the list has a loop*/
	fast = slow = l1->head;
	while( fast && slow )
	{
		fast   = fast->next;
		fast   = fast->next;
		slow   = slow->next;
		if( fast == slow )
		{
			break;
		}
	}
	if( fast && slow )
	{
		printf( "loop exists in list\n" );
	} else
	{
		printf( "no loop exists in list\n" );
	}

	tail_l2->next = NULL;
	printf( "show l1 and l2 after deattaching\n" );
	show_list( l1, show_int );
	show_list( l2, show_int );
}

/*
 * End of file
 */
