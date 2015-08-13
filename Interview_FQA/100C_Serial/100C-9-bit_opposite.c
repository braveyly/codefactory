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
	int a						   = 10;    // 0000 1010
	int set_opposite_position_1	   = 1;     // 0000 1000 = 8
	int set_opposite_position_2	   = 2;     // 0000 1110 = 14

	int tmp = a ^ ( 1 << set_opposite_position_1 );
	printf( "value after setting index 1 bit to opposite for %d is %d\n", a, tmp );

	tmp = a ^ ( 1 << set_opposite_position_2 );
	printf( "value after setting index 2 bit to opposite for %d is %d\n", a, tmp );
}

/*
 * End of file
 */
