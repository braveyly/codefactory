#ifndef _CIRCLE_ARRAY_H_
#define _CIRCLE_ARRAY_H_
#include <stdio.h>

typedef void* ArrayValue;
typedef struct _CIRCLEARRAY
{
	ArrayValue* data;
	int length;
	int _alloced;
	int _index;
}CircleArray;

CircleArray* circle_array_new(int len);
int circle_array_insert(CircleArray* arr, ArrayValue value);
ArrayValue circle_array_get(CircleArray* arr, int index);
void circle_array_free(CircleArray* arr);
#endif 