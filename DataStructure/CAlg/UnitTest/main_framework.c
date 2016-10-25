#include <stdio.h>
#include "framework.h"

void test1(void)
{
	printf("this is test1\n");
}

void test2(void)
{
	printf("this is test2\n");
}

void test3(void)
{
	printf("this is test3\n");
}


static UnitTestFunction tests[] = {
	test1,
	test2,
	test3,
	NULL
};

int main(int argc, char *argv[])
{
	run_tests(tests);

	return 0;
}
