#include <stdio.h>

void deal_array_1(int** a)
{
	printf( "%d\n", a[0][0] );
}

void deal_array_2(int* a[10])
{
	printf( "%d\n", a[0][0] );
}

void deal_array_3(int (*a)[10])
{
	printf( "%d\n", a[0][0] );
}

void deal_array_4(int a[10][4])
{
	printf( "%d\n", a[0][1] );
}

int main()
{
	int a[10][4] = {{4,5,6,7},{40,50,60,70},{400,500,600,700}};
	//deal_array_1(a);	//seg error
	//deal_array_2(a);	//seg error
	deal_array_3(a);	//ok
	deal_array_4(a);	//ok
}