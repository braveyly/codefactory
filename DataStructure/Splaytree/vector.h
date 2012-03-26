#ifndef _CP_VECTOR_H
#define _CP_VECTOR_H

#include "common.h"


/**
 * A simple 'safe array' implementation.
 */
typedef struct _cp_vector
{
    void **mem;            		 /**< Pointer to memory               */
    int size;              		 /**< size of vector (element count)  */
    int head;              		 /**< index of 1st element            */
    int tail;              		 /**< index of Nth element            */

	int mode;              		 /**< collection mode                 */
	cp_copy_fn copy_item;        /**< item copy function              */
	cp_destructor_fn free_item;  /**< item destructor function        */
	cp_lock *lock;         		 /**< rwlock                          */
} cp_vector;

cp_vector *cp_vector_create_by_option(int size, 
									  int mode, 
									  cp_copy_fn copy_item,
									  cp_destructor_fn free_item);
cp_vector *cp_vector_create(int size);
cp_vector *cp_vector_wrap(void **data, int len, int mode);
void cp_vector_destroy(cp_vector *v);
void cp_vector_destroy_custom(cp_vector *v, cp_destructor_fn dtr);
void *cp_vector_add_element(cp_vector *v, void *element);
void *cp_vector_element_at(cp_vector *v, int index);
void *cp_vector_set_element(cp_vector *v, int index, void *element);
void *cp_vector_remove_element_at(cp_vector *v, int index);
int cp_vector_size(cp_vector *v);

/* ----------------------------------------------------------------- */


/** @} */

#endif /* _COLLECTION_VECTOR_H */

