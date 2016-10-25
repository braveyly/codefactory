#include <stdio.h>
#include "alloc-testing.h"

int main()
{
	unsigned char* buffer  = (unsigned char*)malloc(1024*sizeof(char));
	int count = alloc_test_get_allocated();
	return 1;
}