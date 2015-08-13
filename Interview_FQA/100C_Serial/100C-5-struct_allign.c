#include <stdio.h>

#pragma pack(4)

typedef struct _Example_1
{
	short a;
	long b;
}Example_1;

typedef struct _Example_2
{
	char c;
	Example_1 exam1;
	short d;
}Example_2;

typedef struct _Example_3
{
	char c;
	short a;
	long b;
	short d;
}Example_3;

int main()
{                                                        //pack(4)   pack(2)
	printf( "example_1 size is %d\n",sizeof(Example_1) );//8         6
	printf( "example_2 size is %d\n", sizeof(Example_2) );//16       10
	printf( "example_3 size is %d\n", sizeof(Example_3) );//12       10
}
