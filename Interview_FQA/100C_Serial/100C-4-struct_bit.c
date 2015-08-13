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

typedef struct _Bit_Field
{
	unsigned char	a : 4;
	unsigned char	b : 5;
	unsigned char : 5;
	unsigned char c : 4;
}Bit_Field;

typedef struct _Bit_Field_2 //size is 4 bytes, unit bit number is 4*8 = 32 = 4+5+3+4+4+5+4+3
{
	int	a : 4;
	int	b : 5;
	int i: 3;
	int c : 4;
	int	d : 4;
	int	e : 5;
	int f : 4;
	int g : 3;
}Bit_Field_2;

typedef struct _Bit_Field_3 //size is 8 bytes, for bits account is 33
{
	int	a : 4;
	int	b : 5;
	int i: 3;
	int c : 4;
	int	d : 4;
	int	e : 5;
	int f : 4;
	int g : 3;
	int   : 1;
}Bit_Field_3;
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
	Bit_Field* pbit, bit;
	printf( "Bit_Field size is %d sizeof(unsigned) is %d\n", sizeof( Bit_Field ) / sizeof( char ), sizeof( char ) );
	printf( "Bit_Field_2 size is %d sizeof(unsigned) is %d\n", sizeof( Bit_Field_2 ) / sizeof( char ), sizeof( char ) );

	bit.a  = 1;
	bit.b  = 7;
	bit.c  = 15; 
	printf( "%d,%d,%d\n", bit.a, bit.b, bit.c ); 
	pbit = &bit; pbit->a = 0; 
	pbit->b &= 3; 
	pbit->c |= 1; 
	printf( "%d,%d,%d\n", pbit->a, pbit->b, pbit->c );
}

/*
 * End of file
 */
