/*
 * Copyright (c), 2012, Braveyly
 * All rights reserved
 *
 * File name:preprocess.c
 * Abstract:preprocess commands
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
#pragma warning(disable:4716)
#define MAX 1
#define MIN 1


/*
 * Function: int TEST_1( )
 * Description: test #if #elif #else #endif
 * Output: none
 * Return: 
 * Others: none
 */
int TEST_1( )
{
#if defined( MAX ) && defined( MIN )
	printf( "define MAX && MIN\n" );
#elif !defined( MAX ) && defined( MIN )
	printf( "donot define MAX, define MIN\n" );
#elif defined( MAX ) && !defined( MIN )
	printf( "define MAX, donot define MIN\n" );
#else
	printf( "donot define MAX and MIN\n" );
#endif
}

/*
 * Function: int TEST_2( )
 * Description: execute #if #else #elif #endif in compiling procedure when a is undefined.
 *				a is negative in VC6.0,so goto a<3 branch
 * Output: none
 * Return:
 * Others: none
 */
int TEST_2( )
{
	const int a = 8;
#if a > 3
	printf( "a is >3\n" );
#elif a < 3
	printf( "a is <3\n" );
#else
	printf( "a is =3\n" );
#endif
}

/*
 * Function: int TEST_3( )
 * Description: MAX, which is 1,  is defined in compiling procedure,so goto MAX>3
 * Output: none
 * Return: 
 * Others: none
 */
int TEST_3( )
{
#if MAX > 3
	printf( "MAX is >3\n" );
#elif MAX < 3
	printf( "MAX is <3\n" );
#else
	printf( "MAX is =3\n" );
#endif
}

/*
 * Function: int TEST_4( )
 * Description: test #line command which changes current file name and line number
 * Output: none
 * Return:
 * Others: none
 */
int TEST_4( )
{
//#line 100 "li"
	printf( "current line is %d\n", __LINE__ );
	printf( "current file is %s\n", __FILE__ );

	printf( "current line is %d\n", __LINE__ );
	printf( "current line is %d\n", __LINE__ );
}

/*
 * Function: int TEST_5( )
 * Description: test #pragma message, which print out message in compiling procedure
 * Output: none
 * Return:
 * Others: none
 */
int TEST_5( )
{
#ifdef MAX
#pragma message ("MAX defined")
#endif
}

/*
 * Function: int TEST_6( )
 * Description: test __LINE__,__FILE__,__DATE__,__TIME__
 * Output: none
 * Return:
 * Others: none
 */
int TEST_6( )
{
	printf( "line is %d\n", __LINE__ );
	printf( "file name is %s\n", __FILE__ );
	printf( "date is %s\n", __DATE__ );
	printf( "time is %s\n", __TIME__ );
	//printf("whether standard programm %d\n",__STDC__);
}

/*test # command, which change flag to string*/
/*show complicated expression*/
#define SQR( x ) printf( "The square of " # x " is %d\n", ( x ) * ( x ) )
int TEST_7( )
{
	SQR( 8 + 9 );
}

/*test # command,which change flag to string*/
/*convert enum to string*/
typedef struct _Message_
{
	int type;
	char* str;
}Message;


#define MSG( NAME ) { NAME, # NAME }
typedef enum _CMD
{
	quit,
	enter,
	reboot,
	open,
}CMD;




/*test ## command,which connect two flags*/
/*enum to string and function name*/
typedef struct _Menu
{
	char* name;
	int ( *function )( );
}Menu;
#define MN( NAME ) { # NAME, NAME ## _command }

#define STR( NAME ) # NAME

int quit_command( )
{
	printf( "111111111\n" );
}

int enter_command( )
{
	printf( "222222222\n" );
}


int reboot_command( )
{
	printf( "333333\n" );
}

int open_command( )
{
	printf( "4444444\n" );
}

int main( )
{
	int		i;
	Message msg_array[4] = {
		MSG( quit ),
		MSG( enter ),
		MSG( reboot ),
		MSG( open )
	};

	Menu	menu_array[4] = {
		MN( quit ),
		MN( enter ),
		MN( reboot ),
		MN( open ),
	};
//#error "asdljkaskjdaklj"
	TEST_1( );
	TEST_2( );
	TEST_3( );
	TEST_4( );
	TEST_5( );
	TEST_6( );
	TEST_7( );
	for( i = 0; i < 4; i++ )
	{
		printf( "type is %d string is %s\n", msg_array[i].type, msg_array[i].str );
	}

	for( i = 0; i < 4; i++ )
	{
		printf( "name is %s\n", menu_array[i].name );
		( *( menu_array[i].function ) )( );
	}

	printf( "convert i to string %s\n", STR( i ) );
}

/*
 * End of file
 */
