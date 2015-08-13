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
#include <stdarg.h>


void va_fun_1( int start, ... )
{
	va_list arg_ptr;
	int		nArgValue  = start;
	int		nArgCount  = 1;
	va_start( arg_ptr, start );
	while( nArgValue != -1 )
	{
		printf( "arg %d is:%d\n", nArgCount, nArgValue );
		nArgValue = va_arg( arg_ptr, int );
		++nArgCount;
	}
	return;
}


void va_fun_2( int number, ... )
{
	va_list vl;
	int		count = number;
	va_start( vl, number );
	printf( "the para number is %d\n", count );
	printf( "the first va para is %d\n", va_arg( vl, int ) );
	printf( "the second va para is %s\n", va_arg( vl, char* ) );
	printf( "the third va para is %f\n", va_arg( vl, double ) );
}

int main( )
{
	va_fun_1( 5, 1, 7, -1 );
	printf( "................\n" );
	va_fun_1( 2, 4, -1 );
	printf( "................\n" );
	va_fun_1( -1 );
	printf( "................\n" );

	va_fun_2( 3, 8, "liwei", 2.543 );
}


/*
 * End of file
 */
