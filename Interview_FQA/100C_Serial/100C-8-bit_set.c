#include <stdio.h>

int main()
{
	int a = 10; // 0000 1010
	int set_0_position = 1; // 0000 1000 = 8
	int set_1_position = 2; // 0000 1110 = 14

	int tmp = a|(1<<set_1_position);
	printf("value is %d after set index 2 bit to 1 for %d\n", tmp, a);

	tmp = a & ~(1<<set_0_position);
	printf("value is %d after setting index 1 bit to 0 for %d\n", tmp ,a);
}
