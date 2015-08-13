/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:
 * Abstract:
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
#include <stdio.h>

typedef struct _Example_1
{
	short	a : 4;
	short	b : 1;
	short	c : 12;
	int		d : 4;
	int		e : 20;
	char	f : 1;
	int		g : 1;
}Example_1;

typedef struct _Example_2
{
	short	a : 4;
	short	b : 1;
	short	c : 12;
	int		d : 4;
	int		e : 20;
	char	f : 1;
	short	g : 1;
}Example_2;

typedef struct _Example_3
{
	short	a : 4;
	char	b : 1;
	short	c : 12;
	int		d : 4;
	int		e : 20;
	char	f : 1;
	int		g : 1;
}Example_3;

typedef struct _Example_4
{
	short	a : 4;
	char	b : 1;
	char	c : 8;
	char	c0 : 8;
	char	c00 : 8;
	int		c1 : 1;
	short	c2 : 1;
	short	c3 : 1;
	int		d : 4;
	int		e : 20;
	char	f : 1;
	int		g : 1;
}Example_4;


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
	printf( "example_1 size is %d\n", sizeof( Example_1 ) );    //16
	printf( "example_2 size is %d\n", sizeof( Example_2 ) );    //12
	printf( "example_3 size is %d\n", sizeof( Example_3 ) );    //20
	printf( "example_4 size is %d\n", sizeof( Example_4 ) );    //28
}

/*
 * End of file
 */
