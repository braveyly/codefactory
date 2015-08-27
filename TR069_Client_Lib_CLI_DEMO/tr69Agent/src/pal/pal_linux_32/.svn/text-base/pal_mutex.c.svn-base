#include "pal_mutex.h"


PAL_INT32_T pal_mutex_init ( PAL_MUTEX_T *mutexPtr )
{
    return pthread_mutex_init ( mutexPtr, NULL );
}

PAL_INT32_T pal_mutex_lock ( PAL_MUTEX_T *mutexPtr )
{
    return pthread_mutex_lock ( mutexPtr );
}

PAL_INT32_T pal_mutex_unlock ( PAL_MUTEX_T *mutexPtr )
{
    return pthread_mutex_unlock ( mutexPtr );
}

PAL_INT32_T pal_mutex_destroy ( PAL_MUTEX_T *mutexPtr )
{
    return pthread_mutex_destroy ( mutexPtr );
}

