#ifndef _CP_AVL_H
#define _CP_AVL_H

/** @{ */
/**
 * @file
 *
 * avl tree definitions
 *
 * avl is a height balanced binary tree. Named after G. M. Adelson-Velskii and
 * E. M. Landis who introduced the data structure in an 1962 article  "An 
 * algorithm for the organization of information".
 *
 * The delete and insert method implementations are recursive.
 */

#include "common.h"
#include "vector.h"


struct _cp_avltree;

typedef struct _cp_avlnode
{
	void *key;
	void *value;
	
	/* balance factor */
	int balance;
	
	int height; /* this is used for debugging only, although it could replace
				   the balance field */
	
	struct _cp_avlnode *left;
	struct _cp_avlnode *right;
} cp_avlnode;

/* (internal) allocate a new node */
cp_avlnode *cp_avlnode_create(void *key, void *value);
/* (internal) deallocate a node */
void cp_avltree_destroy_node(struct _cp_avltree *tree, cp_avlnode *node);
/* (internal) deallocate a node and its subnodes */
void cp_avltree_destroy_node_deep(struct _cp_avltree *owner, cp_avlnode *node);


/* tree wrapper object */
typedef struct _cp_avltree
{
	cp_avlnode *root;            	/* root node */
	
	int items;                   	/* item count */

	int mode;					 	/* mode flags */
	cp_compare_fn cmp;           	/* key comparison function */
	cp_copy_fn key_copy;         	/* key copy function */
	cp_destructor_fn key_dtr;  	  	/* key destructor */
	cp_copy_fn value_copy;       	/* value copy function */
	cp_destructor_fn value_dtr;  	/* value destructor */
	cp_lock *lock;
	cp_thread transaction_owner;	/* set if a transaction is in progress */
	int txtype;                  	/* lock type */
} cp_avltree;

/* 
 * default create function - equivalent to create_by_option with mode 
 * COLLECTION_MODE_NOSYNC
 */
cp_avltree *cp_avltree_create(cp_compare_fn cmp);

/*
 * complete parameter create function. Note that setting COLLECTION_MODE_COPY
 * without specifying a copy function for either keys or values will result in
 * keys or values respectively being inserted by value, with no copying 
 * performed. Similarly, setting COLLECTION_MODE_DEEP without specifying a 
 * destructor function for keys or values will result in no destructor call
 * for keys or values respectively. This allows using the copy/deep mechanisms
 * for keys only, values only or both.
 */
cp_avltree *
	cp_avltree_create_by_option(int mode, cp_compare_fn cmp, 
								cp_copy_fn key_copy, cp_destructor_fn key_dtr,
								cp_copy_fn val_copy, cp_destructor_fn val_dtr);

/* 
 * recursively destroy the tree structure 
 */
void cp_avltree_destroy(cp_avltree *tree);

/*
 * recursively destroy the tree structure with the given destructor functions
 */
void cp_avltree_destroy_custom(cp_avltree *tree, 
							   cp_destructor_fn key_dtr,
							   cp_destructor_fn val_dtr);

/* insertion function */
void *cp_avltree_insert(cp_avltree *tree, void *key, void *value);

/* retrieve the value mapped to the given key */
void *cp_avltree_get(cp_avltree *tree, void *key);

/* find the value of the closest key by operator */
void *cp_avltree_find(cp_avltree *tree, void *key, cp_op op);

/* return non-zero if a mapping for 'key' could be found */
int cp_avltree_contains(cp_avltree *tree, void *key);

/* delete a mapping */
void *cp_avltree_delete(cp_avltree *tree, void *key);

/* 
 * perform a pre-order iteration over the tree, calling 'callback' on each 
 * node
 */
int cp_avltree_callback_preorder(cp_avltree *tree, 
								 cp_callback_fn callback, 
								 void *prm);
/* 
 * perform an in-order iteration over the tree, calling 'callback' on each 
 * node
 */
int cp_avltree_callback(cp_avltree *tree, cp_callback_fn callback, void *prm);
/* 
 * perform a post-order iteration over the tree, calling 'callback' on each 
 * node
 */
int cp_avltree_callback_postorder(cp_avltree *tree, 
								  cp_callback_fn callback, 
								  void *prm);

/* return the number of mappings in the tree */
int cp_avltree_count(cp_avltree *tree);

/* 
 * lock tree for reading or writing as specified by type parameter. 
 */
int cp_avltree_lock(cp_avltree *tree, int type);
/* read lock */
#define cp_avltree_rdlock(tree) (cp_avltree_lock((tree), COLLECTION_LOCK_READ))
/* write lock */
#define cp_avltree_wrlock(tree) (cp_avltree_lock((tree), COLLECTION_LOCK_WRITE))
/* unlock */
int cp_avltree_unlock(cp_avltree *tree);


/* return the table mode indicator */
int cp_avltree_get_mode(cp_avltree *tree);
/* set mode bits on the tree mode indicator */
int cp_avltree_set_mode(cp_avltree *tree, int mode);
/* unset mode bits on the tree mode indicator. if unsetting 
 * COLLECTION_MODE_NOSYNC and the tree was not previously synchronized, the 
 * internal synchronization structure is initalized.
 */
int cp_avltree_unset_mode(cp_avltree *tree, int mode);

/* print tree to stdout */
void cp_avltree_dump(cp_avltree *tree);


/** @} */

#endif

