/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100C-3-swap_without_tmp.c
 * Abstract:swap two variables without the help of the second variable
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


void test1( )
{
	int a  = 1;
	int b  = 2;
	b = a + b - ( a = b );
	printf( "test1:a=%d\n", a );
	printf( "test1:b=%d\n", b );
}

void test2( )
{
	int a  = 1;
	int b  = 2;
	a  = a + b;
	b  = a - b;
	a  = a - b;
	printf( "test2:a=%d\n", a );
	printf( "test2:b=%d\n", b );
}

void test3( )
{
	int a  = 1;
	int b  = 2;
	a ^= b;
	b ^= a;
	a ^= b;
	printf( "test3:a=%d\n", a );
	printf( "test3:b=%d\n", b );
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
	test1( );
	test2( );
	test3( );
}

/*
 * End of file
 */
