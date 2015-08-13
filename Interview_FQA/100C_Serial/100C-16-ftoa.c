/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100C-16-ftoa.c
 * Abstract:implement the convertion from float to string
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

#define NUMBER_OF_DECIMAL	10
#define NUMBER_OF_INTEGER	20


void show_array( char* ca, int size )
{
	int i;
	for( i = 0; i < size; i++ )
	{
		printf( "ca[%d]=%c\n", i, ca[i] );
	}
}

char* my_ftoa( float f )
{
	char	* cp_str;
	int		current_index = -1;
	float	decimal, tmp_decimal;
	int		integer, tmp_integer;
	int		digit = 0, tmp_digit;
	int		i;
	int		size;

	size   = NUMBER_OF_DECIMAL + NUMBER_OF_INTEGER + 2;
	cp_str = (char*)malloc( size );
	memset( cp_str, 0, size );

	if( f < 0 )
	{
		current_index		  += 1;
		cp_str[current_index]  = '-';
	}

	/*get the integer and decimal of the float*/
	integer	   = (int)f;
	decimal	   = f - integer;
	printf( "the integer of float %f is %d\n", f, integer );
	printf( "the decimal of float %f is %f\n", f, decimal );

	/*get the digit of integer*/
	tmp_integer = integer;
	while( tmp_integer )
	{
		tmp_integer = tmp_integer / 10;
		digit++;
	}
	printf( "the integer digit of float %f is %d\n", f, digit );

	/*put the integer into array*/
	tmp_digit	   = digit;
	tmp_integer	   = integer > 0 ? integer : 0 - integer;
	while( tmp_integer )
	{
		printf( "%10 == %d\n", tmp_integer % 10 );
		cp_str[current_index + tmp_digit]  = tmp_integer % 10 + '0';
		tmp_integer						   = tmp_integer / 10;
		tmp_digit--;
	}
	current_index = current_index + digit;
	show_array( cp_str, size );

	/*put the dot into array*/
	current_index		  += 1;
	cp_str[current_index]  = '.';
	show_array( cp_str, size );

	/*put the decimal into array*/
	tmp_decimal = decimal > 0 ? decimal : 0 - decimal; /*if negative, convert it to positive*/
	for( i = 0; i < NUMBER_OF_DECIMAL; i++ )
	{
		tmp_decimal			   = tmp_decimal * 10;
		current_index		  += 1;
		cp_str[current_index]  = ( (int)tmp_decimal ) % 10 + '0';
		tmp_decimal			   = tmp_decimal - (int)tmp_decimal;
	}

	show_array( cp_str, NUMBER_OF_INTEGER + NUMBER_OF_DECIMAL + 2 );
	return cp_str;
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
	float	f = -123.456789012222;
	char	* str;
	str = my_ftoa( f );
	printf( "string:%s\n", str );
	free( str );
}

/*
 * End of file
 */
