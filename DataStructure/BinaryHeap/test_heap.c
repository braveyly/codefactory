#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "heap.h"

int cp_hash_compare_int(void *i, void *j)
{
    return *((int *) i) - *((int *) j);
}

/** duplicate an integer */
int *intdup(int *src)
{
	int *res = malloc(sizeof(int));
	if (res) *res = *src;
	return res;
}


int main(int argc, char *argv[])
{
	int *i;
	int j, val;
	int max = 0;
	int log_level = 0;
	cp_heap *heap;

	if (argc > 1) max = atoi(argv[1]);
	if (max == 0) max = 10;
	
	heap = cp_heap_create(cp_hash_compare_int);

	srand(time(NULL));

	for (j = 0; j < max; j++)
	{
		val = rand();
		cp_heap_push(heap, intdup(&val));
	}
		
	while ((i = cp_heap_pop(heap)))
	{
		printf("%d\n", *i);
		max--;
		if (max == 50) cp_heap_contract(heap);
		free(i);
	}

	cp_heap_contract(heap);

	cp_heap_destroy(heap);

	return 0;
}

