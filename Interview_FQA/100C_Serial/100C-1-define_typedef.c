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

typedef int * pint;
#define PINT int*


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
	int	e  = 100;
	const pint	 a  = &e, b = &e;
	const PINT	c  = &e, d = e;

	printf( "before *a = 1\n" );
	*a = 1;

	printf( "before *b = 1\n" );
	*b = 1;

	printf( "before *c = 1\n" );
	//*c = 2;//compile error:l-value specifier const object
	//*d = 2;//compile error

	printf( "before a = &e\n" );
	//a = &e;//compile error:l-value secifies const object

	printf( "before b = &e\n" );
	//b = &e;//compile error:l-value specifies const object

	printf( "before c = &e\n" );
	c = &e;
	//d = &e;//compile warning from int* to int conversation
}

/*
 * End of file
 */
