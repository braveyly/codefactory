#ifndef _CP_RTREE_H
#define _CP_RTREE_H

/**
 * @addtogroup cp_trie
 */
/** @{ */

#include "common.h"
#include <string.h>
#include <errno.h>
#include "vector.h"
#include "mtab.h"

#define NODE_MATCH(n, i) ((n)->others ? mtab_get((n)->others, *i) : NULL)
#define BRANCH_COUNT(node) mtab_count((node)->others)

typedef int (*cp_trie_match_fn)(void *leaf); 

struct _cp_trie;

/* 
 * cp_trie nodes can have any number of subnodes mapped by an mtab - a hash
 * table designed for character keys
 */
typedef struct _cp_trie_node 
{ 
	mtab *others; 
	void *leaf; 
} cp_trie_node; 

cp_trie_node *cp_trie_node_new(void *leaf); 
void *cp_trie_node_delete(struct _cp_trie *grp, cp_trie_node *node);
void cp_trie_delete_mapping(struct _cp_trie *grp, mtab_node *map_node);

void cp_trie_node_unmap(struct _cp_trie *grp, cp_trie_node **node); 

/**
 * @file trie.h
 * cp_trie is a character trie implementation. Tries allow for prefix matching
 * with O(m) = O(1) time (m being the length of the key). Used to store key - 
 * value mappings, tries have certain advantages over hashtables in that worse 
 * case behavior is still O(1) and no hash function is needed. cp_trie is 
 * technically a compact trie in that collapses unused character paths. 
 */
typedef struct _cp_trie
{ 
	cp_trie_node *root;                /**< root node           */
	int path_count;                    /**< number of entries   */

	int mode;                          /**< collection mode     */

	cp_copy_fn copy_leaf;              /**< leaf copy function  */
	cp_destructor_fn delete_leaf;      /**< leaf destructor     */

	cp_lock *lock;                     /**< collection lock     */
	cp_thread txowner;                 /**< transaction owner   */
	int txtype;                        /**< lock type           */
} cp_trie; 

/** 
 * create a new cp_trie object with the specified collection mode and
 * leaf management functions
 */
cp_trie *cp_trie_create_trie(int mode, 
		                     cp_copy_fn copy_leaf, 
							 cp_destructor_fn delete_leaf);

/** create a new cp_trie object with the specified collection mode */
cp_trie *cp_trie_create(int mode);
/** delete a cp_trie object */
int cp_trie_destroy(cp_trie *grp); 
/** add a mapping to a trie */
int cp_trie_add(cp_trie *grp, char *key, void *leaf); 
/** remove a mapping from a trie */
int cp_trie_remove(cp_trie *grp, char *key, void **leaf); 
/** return the mapping for the longest prefix of the given key */
int cp_trie_prefix_match(cp_trie *grp, char *key, void **leaf);
/** return the mapping for the given key if any */
void *cp_trie_exact_match(cp_trie *grp, char *key);
/** return a vector containing exact match and any prefix matches */
cp_vector *cp_trie_fetch_matches(cp_trie *grp, char *key);
/** return a vector containing all entries in subtree under path given by key */
cp_vector *cp_trie_submatch(cp_trie *grp, char *key);

/** return the number of stored items */
int cp_trie_count(cp_trie *grp);

void cp_trie_set_root(cp_trie *grp, void *leaf); 

int cp_trie_lock(cp_trie *grp, int type);
#define cp_trie_rdlock(grp) (cp_trie_lock(grp, COLLECTION_LOCK_READ))
#define cp_trie_wrlock(grp) (cp_trie_lock(grp, COLLECTION_LOCK_WRITE))
int cp_trie_unlock(cp_trie *grp);

/* get the current collection mode */
int cp_trie_get_mode(cp_trie *grp);
/* sets the bits defined by mode on the trie mode */
int cp_trie_set_mode(cp_trie *grp, int mode);
/* clears the bits defined by mode on the trie mode */
int cp_trie_unset_mode(cp_trie *grp, int mode);

void cp_trie_dump(cp_trie *grp);


/** @} */

#endif

