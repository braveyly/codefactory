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


void test_array( char array[] )
{
	printf( " sizeof( array ) is %d\n", sizeof( array ) );
	array++;
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
	char	a[10];
	char	* p = a;
	printf( "sizeof(a) is %d\n", sizeof( a ) );
	printf( "sizeof(p) is %d\n", sizeof( p ) );
	test_array( a );
	++p;
}

/*
 * End of file
 */
