#include "CircleArray.h"


typedef struct _Point3DValue
{
	int x;
	int y;
}Point3DValue;


int main()
{
	Point3DValue a;
	Point3DValue b;
	Point3DValue c;
	Point3DValue d;
	Point3DValue e;
	Point3DValue* tmp;
	CircleArray* arr = NULL;
	a.x = 1;
	a.y = 1;
	b.x = 2;
	b.y = 2;
	c.x = 3;
	c.y = 3;
	d.x = 4;
	d.y = 4;
	e.x = 5;
	e.y = 5;
	arr = circle_array_new(3);

	circle_array_insert(arr, &a);
	circle_array_insert(arr, &b);
	circle_array_insert(arr, &c);
	tmp = circle_array_get(arr, 0);
	tmp = circle_array_get(arr, 1);
	tmp = circle_array_get(arr, 2);

	circle_array_insert(arr, &d);

	tmp = circle_array_get(arr, 0);
	tmp = circle_array_get(arr, 1);
	tmp = circle_array_get(arr, 2);
	circle_array_insert(arr, &e);
	tmp = circle_array_get(arr, 0);
	tmp = circle_array_get(arr, 1);
	tmp = circle_array_get(arr, 2);
	circle_array_free(arr);
	return 0;
}