#include "pal_mutex.h"


PAL_INT32_T pal_mutex_init ( PAL_MUTEX_T *mutex )
{
    *mutex = semMCreate ( SEM_Q_PRIORITY );
    if(*mutex != ERROR)
        return 0;
    else
        return -1;
}

PAL_INT32_T pal_mutex_lock ( PAL_MUTEX_T *mutex )
{
    return semTake ( *mutex, WAIT_FOREVER );
}

PAL_INT32_T pal_mutex_unlock ( PAL_MUTEX_T *mutex )
{
    return semGive ( *mutex );
}

PAL_INT32_T pal_mutex_destroy ( PAL_MUTEX_T *mutex )
{
    return semDelete ( *mutex );
}
