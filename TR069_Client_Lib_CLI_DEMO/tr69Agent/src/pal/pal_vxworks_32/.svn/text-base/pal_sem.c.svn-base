#include "pal_sem.h"

PAL_INT32_T pal_sem_init ( PAL_SEM_T *sem )
{
    *sem = semBCreate ( SEM_Q_FIFO, SEM_EMPTY );
    if(*sem != ERROR)
        return 0;
    else
        return -1;
}
PAL_INT32_T pal_sem_post ( PAL_SEM_T *sem )
{
    return semGive ( *sem );
}
PAL_INT32_T pal_sem_wait ( PAL_SEM_T *sem )
{
    return semTake ( *sem, WAIT_FOREVER );
}

PAL_INT32_T pal_sem_timedwait(PAL_SEM_T *sem, PAL_INT32_T sec)
{
    return semTake ( *sem, sec*100 );
}

PAL_INT32_T pal_sem_destroy ( PAL_SEM_T *sem )
{
    return semDelete ( *sem );
}
