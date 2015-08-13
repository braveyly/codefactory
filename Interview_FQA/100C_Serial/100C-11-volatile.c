/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:100C-11-volatile.c
 * Abstract:test volatile key word under debug and release mode.
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
void main( )
{
	volatile int	i  = 10;
	int				a  = i;
	int				b;

	printf( "i= %d\n", a );
	//下面汇编语句的作用就是改变内存中i的值，但是又不让编译器知道
	__asm {
		mov dword ptr [ebp - 4], 20h
	}

	b = i;
	printf( "i= %d\n", b );
}

/*
 * End of file
 */
