/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:FDBLib.c
 * Abstract:flow data base library implementation file
 *
 * Current version:v0.2
 * Author:braveyly
 * Date:2012-04-28
 * Modification:1. encapsulate element with struct FDB
 				2. optimize interface APIs
 				3. add compat and resize functions

 * Revise version:v0.1
 * Author:braveyly
 * Date:2012-02-01
 * Modification:first draft
 */
#include <stdio.h>
#include "FDBLib.h"


/*
 * Function: void fdb_show( FDB* fdb )
 * Description: show fdb object
 * Input:  the FDB object point
 * Output: the FDB object information
 * Return: void
 * Others: none
 */
void fdb_show( FDB* fdb )
{
	int			i		   = 0;
	FDBElement	* elemTmp  = NULL;
	if( NULL == fdb )
	{
		printf( "fdb is NULL\n" );
		return;
	}
	printf( "fdb count is %d\n", fdb->count );
	printf( "fdb size is %d\n", fdb->size );

	elemTmp = fdb->head;
	while( elemTmp != fdb->rear )
	{
		printf( "element length is %d\n", elemTmp->len );
		printf( "element value is %s\n", elemTmp->value );
		elemTmp = elemTmp->next;
	}
}

/*
 * Function: FDB* fdb_init( int count, int size )
 * Description: initialize one FDB object
 * Input: the fdb list element number and size
 * Output:
 * Return: FDB pointer object
 * Others: none
 */
FDB* fdb_init( int count, int size )
{
	FDB			* fdb;
	FDBElement	* elemTmp  = NULL;
	int			i		   = 0;
	int			ret		   = 0;

	if( count <= 0 || size <= 0 )
	{
		return NULL;
	}

	fdb = (FDB*)malloc( sizeof( FDB ) / sizeof( char ) );
	if( NULL == fdb )
	{
		return NULL;
	}
	memset( fdb, 0, sizeof( FDB ) / sizeof( char ) );

	fdb->count = count;
	fdb->size  = size;

	for( i = 0; i < fdb->count; i++ )
	{
		elemTmp = ( FDBElement* )malloc( sizeof( FDBElement ) / sizeof( char ) );
		if( NULL == elemTmp )
		{
			ret = -1; break;
		}
		memset( elemTmp, 0, sizeof( FDBElement ) / sizeof( char ) );

		elemTmp->value = ( char* )malloc( fdb->size );
		if( NULL == elemTmp->value )
		{
			ret = -1; break;
		}
		memset( elemTmp->value, 0, fdb->size );

		if( 0 == i )
		{
			fdb->head  = elemTmp;
			fdb->rear  = elemTmp;
			continue;
		}

		fdb->rear->next	   = elemTmp;
		elemTmp->prev	   = fdb->rear;
		fdb->rear		   = elemTmp;
		fdb->rear->next	   = fdb->head;
		fdb->head->prev	   = fdb->rear;
	} //finish construct the double list

	fdb->rear = fdb->head; // it indicates no data that rear points head,
	//  and rear points the first null element
	return ret == -1 ? NULL : fdb;
}

static int _fdb_recycle( FDB* fdb, FDBElement* free_ele )
{
	if( fdb->head == free_ele )
	{
		return -1;
	}

	free_ele->prev->next   = free_ele->next;
	free_ele->next->prev   = free_ele->prev;

	fdb->rear->next->prev  = free_ele;
	free_ele->next		   = fdb->rear->next;
	fdb->rear->next		   = free_ele;
	free_ele->prev		   = fdb->rear;

	return 0;
}

static int _fdb_compat( FDB* fdb )
{
	int			i		   = 0;
	FDBElement	* elemTmp  = NULL, *elemNext = NULL;
	int			ret		   = 0;

	if( fdb->head == fdb->rear )
	{
		printf( "list is NULL\n" );
		return -1;
	}

	elemTmp	   = fdb->head;
	elemNext   = elemTmp->next;

	while( elemNext != fdb->rear )
	{
		if( elemTmp->len + elemNext->len <= fdb->size )
		{
			memcpy( elemTmp->value + elemTmp->len, elemNext->value, elemNext->len );
			elemTmp->len = elemTmp->len + elemNext->len;
			_fdb_recycle( fdb, elemNext );
			elemNext = elemTmp->next;
			ret++;
		}else
		{
			elemTmp	   = elemTmp->next;
			elemNext   = elemTmp->next;
		}
	}
	return ret;
}

static int _fdb_resize( FDB* fdb )
{
	int			i, ret = 0;
	FDBElement	* elemTmp;

	for( i = 0; i < ONCE_RESIZE_SIZE; i++ )
	{
		elemTmp = ( FDBElement* )malloc( sizeof( FDBElement ) / sizeof( char ) );
		if( NULL == elemTmp )
		{
			ret = -1; break;
		}
		memset( elemTmp, 0, sizeof( FDBElement ) / sizeof( char ) );

		elemTmp->value = ( char* )malloc( fdb->size );
		if( NULL == elemTmp->value )
		{
			ret = -1; break;
		}
		memset( elemTmp->value, 0, fdb->size );

		fdb->rear->next->prev  = elemTmp;
		elemTmp->next		   = fdb->rear->next;

		fdb->rear->next	   = elemTmp;
		elemTmp->prev	   = fdb->rear;
	}
	fdb->count += ONCE_RESIZE_SIZE;

	return ret;
}

/*
 * Function: int fdb_put( FDB* fdb, char* data, int len )
 * Description: put one element into fdb
 * Input:  the element data and length
 * Output: 
 * Return: -1: failure
 			0: succeed
 * Others: none
 */
int fdb_put( FDB* fdb, char* data, int len )
{
	int ret = 0;

	if( len > fdb->size )
	{
		printf( "Packet data length is bigger than element default length\n" );
		ret = -1;
		goto end;
	}

	if( fdb->rear->next == fdb->head ) //the last element donot hold data
	{
		printf( "Double circle list is full\n" );
		if( _fdb_compat( fdb ) > 0 )
		{
			printf( "compat list succeed\n" );
		}else if( _fdb_resize( fdb ) == 0 )
		{
			printf( "resize list succeed\n" );
		}else
		{
			printf( "no methods to think about\n" );
			ret = -1;
			goto end;
		}
	}

	if( fdb->rear->next != fdb->head )
	{
		fdb->rear->len = len;
		memcpy( fdb->rear->value, data, len );
		fdb->rear = fdb->rear->next;
	}

end:
	return ret;
}

/*
 * Function: int fdb_get( FDB* fdb, char* gtData, int expectLen, int* realLen )
 * Description: get the buffer of the specific length 
 * Input:  the FDB object, the expected data length
 * Output: the data, and the real data length
 * Return: -1: failure
 			0: succeed
 * Others: none
 */
/*do the best to fufill the need of expectlen length*/
int fdb_get( FDB* fdb, char* gtData, int expectLen, int* realLen )
{
	FDBElement* elemTmp = NULL;

	if( fdb->rear == fdb->head )
	{
		printf( "double list is NULL\n" );
		return -1;
	}

	elemTmp = fdb->head;

	*realLen = 0;
	do
	{
		memcpy( gtData + *realLen, elemTmp->value, elemTmp->len );
		*realLen  += elemTmp->len;
		elemTmp	   = elemTmp->next;
	}
	while( fdb->head != elemTmp->next && ( *realLen ) < expectLen );

	return 0;
}

static int _shift_fdb_element( FDBElement* ele, int size, int offset )
{
	char* tmp = (char*)malloc( size );
	int left_len;
	if( NULL == tmp )
	{
		return -1;
	}
	memset( tmp, 0, size );

	left_len = ele->len - offset;
	printf( "element length is %d\n", ele->len );
	printf( "element size is %d\n", size );
	printf( "element offset is %d\n", offset );

	memcpy( tmp, ele->value + offset, left_len );
	memset( ele->value, 0, size );
	memcpy( ele->value, tmp, left_len );
	ele->len = left_len;

	return 0;
}

/*
 * Function: int fdb_accept( FDB* fdb, int apLen )
 * Description: accept the data of the specific length, actually delete data 
 * Input:  the FDB object, the accepted length
 * Output:
 * Return: -1: failure
 			0: succeed
 * Others: none
 */
int fdb_accept( FDB* fdb, int apLen )
{
	FDBElement	* elemTmp  = NULL;
	int			len		   = 0;

	if( fdb->rear == fdb->head )
	{
		printf( "double list is NULL\n" );
		return -1;
	}

	elemTmp = fdb->head;

	do
	{
		len		  += elemTmp->len;
		elemTmp	   = elemTmp->next;
	}
	while( fdb->head != elemTmp->next && len < apLen );

	if( fdb->head == elemTmp->next )
	{
		printf( "accept the total list\n" );
		fdb->rear = fdb->head;
		return 0;
	}

	if( len == apLen )
	{
		fdb->head = elemTmp;
	}else if( len > apLen )
	{
		fdb->head = elemTmp->prev;
		_shift_fdb_element( fdb->head, fdb->size, fdb->head->len - ( len - apLen ) );
	}

	return 0;
}

/*
 * End of file
 */
