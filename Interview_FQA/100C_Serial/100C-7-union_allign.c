/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100C-7-union_allign.c
 * Abstract:union variable memory allignance 
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

union
{
	int		i;
	char	x[2];
} a;


/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
void main( )
{
/*
	address lower ----> address higher
		0x0A 0x01 0x00 0x00
		
    -----small ending system-----
	i = 0x01 0x0A
	2^8+10 = 266;
	-----big ending system-----
	i = 0x0A 0x01 0x00 0x00
*/
	a.x[0] = 10;
	a.x[1] = 1;
	printf( "%d\n", a.i );
}

/*
 * End of file
 */
