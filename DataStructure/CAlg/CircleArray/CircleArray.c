#include "CircleArray.h"


CircleArray* circle_array_new(int len)
{
	CircleArray *new_circlelist;

	/* If the length is not specified, use a sensible default */

	if (len <= 0) {
		len = 16;
	}

	/* Allocate the new ArrayList and fill in the fields.  There are
	 * initially no entries. */

	new_circlelist = (CircleArray *) malloc(sizeof(CircleArray));

	if (new_circlelist == NULL) {
		return NULL;
	}

	new_circlelist->_alloced = len;
	new_circlelist->length = 0;
	new_circlelist->_index = 0;
	/* Allocate the data array */

	new_circlelist->data = malloc(len * sizeof(ArrayValue));

	if (new_circlelist->data == NULL) {
		free(new_circlelist);
		return NULL;
	}

	return new_circlelist;
}

int circle_array_insert(CircleArray* arr, ArrayValue value)
{
	if(NULL == arr)
	{
		return -1;
	}
	if(NULL == arr->data)
	{
		return -1;
	}

	if(arr->length == arr->_alloced)
	{
		if(arr->_index == (arr->_alloced - 1))
		{
			arr->_index = 0;
		}
		else
		{
			arr->_index++;
		}
		arr->data[arr->_index] = value;
		return 0;
	}

	arr->data[arr->length] = value;
	arr->length++;
	arr->_index = arr->length - 1;

	return 0;
}

ArrayValue circle_array_get(CircleArray* arr, int index)
{
	ArrayValue tmp = 0;
	if(NULL == arr)
	{
		return 0;
	}

    if(NULL == arr->data)
	{
		return 0;
	}
	if(index>=arr->length)
	{
		return 0;
	}
	
	if(arr->length < arr->_alloced)
	{
		tmp = arr->data[index];
	}

	if(index<=arr->_index)
	{
		tmp = arr->data[arr->_index-index];
	}
	else
	{
		tmp = arr->data[arr->length+arr->_index-index];
	}
	

	return tmp;

}

void circle_array_free(CircleArray* arr)
{
	if(NULL != arr)
	{
		free(arr->data);
		free(arr);
	}
}