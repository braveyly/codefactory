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
	List		* l;
	List_Entry	* entry, *entry_into_loop;
	int			*pitmp;
	int			i, len_loop = 0, len_handler = 0;

	List_Entry	*fast, *slow;

	l = create_list( );

	for( i = 0; i < 50; i++ )
	{
		pitmp  = create_int( i );
		entry  = create_entry( pitmp );
		list_append( l, entry );
		if( 39 == i )
		{
			entry_into_loop = entry;
		}
	}


	/*
	   create one circle with the entrance point 39,
	   circle length is 39-49 = 11,
	   handle length is 0-38 = 39
	   never call show and pop apis to slist with loop, that causes indefinite circulation
	 */
	list_append( l, entry_into_loop );

	/*judge whether the list has a loop*/
	fast = slow = l->head;
	while( fast && slow )
	{
		printf( "123\n" );
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

	while( fast && slow )
	{
		fast   = fast->next;
		fast   = fast->next;
		slow   = slow->next;
		len_loop++;
		if( fast == slow )
		{
			break;
		}
	}
	printf( "loop length is %d\n", len_loop );

	fast = l->head;
	while( fast && slow )
	{
		fast   = fast->next;
		slow   = slow->next;
		len_handler++;
		if( fast == slow )
		{
			break;
		}
	}
	printf( "handler length is %d\n", len_handler );
}

/*
 * End of file
 */
