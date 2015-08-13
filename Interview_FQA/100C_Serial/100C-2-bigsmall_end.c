/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100C-2-bigsmall_end.c
 * Abstract:judge whether the system ending mode is big or small
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
	typedef union _End_Mode {
		int		i;
		char	ch;
	}End_Mode;

	End_Mode t;


/*
   small ending for t.i = 1; then t.ch is equal to 1
   address lower ----> address higher
   0x01      0x00      0x00      0x00
 */
	t.i = 1;
	if( t.ch == 1 )
	{
		printf( "small\n" );
	} else
	{
		printf( "big " );
	}
}

/*
 * End of file
 */
