/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100c-12-type_range.c
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


void test_unsigned_char_1( )
{
#define MAX 255
	unsigned char array[MAX], i;
	for( i = 0; i <= MAX; i++ )
	{
		printf( "i=%d\n", i );
		array[i] = i;
	}
}

void test_unsigned_char_2( )
{
#define MAX 255
	unsigned char array[MAX], i;
	for( i = 1; i <= MAX; i++ )
	{
		printf( "i=%d\n", i );
		array[i - 1] = i;
	}
}

void test_unsigned( void )
{
	unsigned int	a  = 6;
	int				b  = -20;
	( a + b > 6 ) ? printf( "> 6\n" ) : printf( "<= 6\n" );
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
	test_unsigned( );
}

/*
 * End of file
 */
