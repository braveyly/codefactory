/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:FDBLib.h
 * Abstract:flow data buffer library
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
#ifndef _FDB_LIB_H_
#define _FDB_LIB_H_
#define ONCE_RESIZE_SIZE 10

typedef struct _FDBELEMENT
{
	char				* value;
	int					len;    //real value length
	struct _FDBELEMENT	* prev;
	struct _FDBELEMENT	* next;
}FDBElement;

typedef struct _DOUBLE_LIST
{
	int					count;  // element count
	int					size;   // element buffer length
	struct _FDBELEMENT	*head;
	struct _FDBELEMENT	*rear;
}FDB;

FDB* fdb_init( int count, int size );


int fdb_put( FDB* fdb, char* ptData, int ptLen );


int fdb_get( FDB* fdb, char* gtData, int expectLen, int* realLen );


int fdb_accept( FDB* fdb, int apLen );


void fdb_show( FDB* fdb );


#endif


/*
 * End of file
 */
