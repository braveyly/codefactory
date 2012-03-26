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

typedef struct _cp_mapping
{
	void *key;
	void *value;
} cp_mapping;

#define cp_mapping_key(m) 	((m)->key)
#define cp_mapping_value(m)	((m)->value)

cp_mapping *cp_mapping_create(void *key, void *value);

typedef int (*cp_mapping_cmp_fn)(cp_mapping *a, cp_mapping *b);


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

#define CP_MEMORY_ALLOCATION_FAILURE                       10000
#define CP_INVALID_FUNCTION_POINTER                        10010
#define CP_THREAD_CREATION_FAILURE                         10020
#define CP_INITIALIZATION_FAILURE                          10030
#define CP_LOCK_FAILED                                     10040
#define CP_UNLOCK_FAILED                                   10050

#define CP_LOADLIB_FAILED                                  11010
#define CP_LOADFN_FAILED                                   11020
#define CP_MODULE_NOT_LOADED                               11030

#define CP_IO_ERROR                                        12000
#define CP_OPEN_PORT_FAILED                                12010
#define CP_HTTP_FETCH_FAILED                               12020
#define CP_INVALID_RESPONSE                                12030
#define CP_HTTP_EMPTY_REQUEST                              12100
#define CP_HTTP_INVALID_REQUEST_LINE                       12110
#define CP_HTTP_INVALID_STATUS_LINE                        12111
#define CP_HTTP_UNKNOWN_REQUEST_TYPE                       12120
#define CP_HTTP_INVALID_URI                                12130
#define CP_HTTP_INVALID_URL                                12131
#define CP_HTTP_VERSION_NOT_SPECIFIED                      12140
#define CP_HTTP_1_1_HOST_NOT_SPECIFIED                     12150
#define CP_HTTP_INCORRECT_REQUEST_BODY_LENGTH              12160
#define CP_SSL_CTX_INITIALIZATION_ERROR                    12200
#define CP_SSL_HANDSHAKE_FAILED                            12210
#define CP_SSL_VERIFICATION_ERROR                          12220

#define CP_LOG_FILE_OPEN_FAILURE                           13000
#define CP_LOG_NOT_OPEN                                    13010

#define CP_INVALID_VALUE                                   14000
#define CP_MISSING_PARAMETER                               14010
#define CP_BAD_PARAMETER_SET                               14020
#define CP_ITEM_EXISTS                                     14030
#define CP_ITEM_DOES_NOT_EXIST                             14031
#define CP_UNHANDLED_SIGNAL                                14040
#define CP_FILE_NOT_FOUND                                  14050
#define CP_METHOD_NOT_IMPLEMENTED                          14060
#define CP_INVALID_FILE_OFFSET                             14070
#define CP_CORRUPT_FILE                                    14080
#define CP_CORRUPT_INDEX                                   14081
#define CP_UNIQUE_INDEX_VIOLATION                          14090

#define CP_REGEX_COMPILATION_FAILURE                       15000
#define CP_COMPILATION_FAILURE                             15010

#define CP_DBMS_NO_DRIVER                                  16000
#define CP_DBMS_CONNECTION_FAILURE                         16010
#define CP_DBMS_QUERY_FAILED                               16020
#define CP_DBMS_CLIENT_ERROR                               16030
#define CP_DBMS_STATEMENT_ERROR                            16040

#endif
