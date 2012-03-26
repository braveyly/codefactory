#ifndef _COMMON_H_
#define _COMMON_H_
/** @{ */
/**
 * @file
 *
 * The c collection classes provide plain c implementations of famous data
 * structures and algorithms such as linked lists, hash tables, hash lists and
 * an extensible tree implementation (see graph.h). Behavior in terms of
 * synchronization and member uniqueness may be set on initialization using the
 * appropriate constructor function with the required flags. The default 
 * implementations are synchronized, and mostly allow mutliple values with the
 * notable exception of the cp_hashtable and cp_hashlist collections which do 
 * not by default. Other subtle differences deriving from data structure 
 * characteristics or implementation inconsistencies would suggest reading the 
 * inline documentation in the header files for the specific collection you 
 * intend to use. 
 *
 * This header file defines macros and function types used commonly throughout 
 * the package.
 */

/** use collection defaults */
#define COLLECTION_MODE_PLAIN             0
/** collection copies and deletes elements (keys, values) */
#define COLLECTION_MODE_DEEP              1
/** collection allows non-unique keys */
#define COLLECTION_MODE_MULTIPLE_VALUES   2
/** collection stores copies of elements (keys, values) */
#define COLLECTION_MODE_COPY              4
/** no synchronization - suitable for the single threaded situation or if you 
  * want to do the synchronization yourself. */
#define COLLECTION_MODE_NOSYNC            8
/** 
 * The collection does not resize underlying hashtables. It might make sense 
 * to set this temporarily in code sections that shouldn't be unexpectedly 
 * slowed down by a resize operation, but resize should be allowed if the 
 * table fill factor is expected to go over ~70%, which is the point at which
 * hashtable performace is rumored to start degrading. 
 */
#define COLLECTION_MODE_NORESIZE         16
/**
 * hashlist multiple values are returned in list order (O(N)) rather than 
 * insertion order (O(1))
 */
#define COLLECTION_MODE_LIST_ORDER		 32
/**
 * indicates a transaction is in progress
 */
#define COLLECTION_MODE_IN_TRANSACTION   64

/** no lock */
#define COLLECTION_LOCK_NONE   0
/** lock for reading */
#define COLLECTION_LOCK_READ   1
/** lock for writing */
#define COLLECTION_LOCK_WRITE  2

typedef enum 
{
  CP_OP_LT = 1, 
  CP_OP_LE = 2, 
  CP_OP_EQ = 3, 
  CP_OP_NE = 4, 
  CP_OP_GE = 5, 
  CP_OP_GT = 6
} cp_op;

/**
 * copy function.
 *
 * In cases where the collection holds copies rather than references to the
 * original objects. To do this you need to provide a copy function for
 * the items.
 */
typedef void *(*cp_copy_fn)(void *);

/**
 * destructor function.
 */
typedef void (*cp_destructor_fn)(void *);

/**
 * comparator functions implement strcmp semantics - 0 for identical keys, 
 * non-zero otherwise.
 */
typedef int (*cp_compare_fn)(void *, void *);

/**
 * callback function for iterator callback etc
 */
typedef int (*cp_callback_fn)(void *entry, void *client_prm);


typedef pthread_t cp_thread;
typedef pthread_rwlock_t cp_lock;
typedef pthread_mutex_t cp_mutex;
typedef pthread_cond_t cp_cond;
#define cp_thread_create(thread, attr, fn, prm) pthread_create(&(thread), attr, fn, prm)
#define cp_thread_join pthread_join
#define cp_thread_detach pthread_detach
#define cp_thread_self pthread_self
#define cp_mutex_init pthread_mutex_init
#define cp_mutex_lock pthread_mutex_lock
#define cp_mutex_unlock pthread_mutex_unlock
#define cp_mutex_destroy pthread_mutex_destroy
#define cp_cond_init pthread_cond_init
#define cp_cond_wait pthread_cond_wait
#define cp_cond_signal pthread_cond_signal
#define cp_cond_broadcast pthread_cond_broadcast
#define cp_cond_destroy pthread_cond_destroy
#define cp_lock_init pthread_rwlock_init
#define cp_lock_rdlock pthread_rwlock_rdlock
#define cp_lock_wrlock pthread_rwlock_wrlock
#define cp_lock_unlock pthread_rwlock_unlock
#define cp_lock_destroy pthread_rwlock_destroy
#define cp_thread_equal pthread_equal

#endif