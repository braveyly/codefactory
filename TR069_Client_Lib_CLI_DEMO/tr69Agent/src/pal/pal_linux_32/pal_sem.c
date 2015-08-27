#include "pal_sem.h"
#include "pal_time.h"

PAL_INT32_T pal_sem_init ( PAL_SEM_T *sem )
{
    return sem_init ( sem, 0, 0 );
}

PAL_INT32_T pal_sem_post ( PAL_SEM_T *sem )
{
    return sem_post ( sem );
}

PAL_INT32_T pal_sem_wait ( PAL_SEM_T *sem )
{
    return sem_wait ( sem );
}

/* add by Hook Guo, June 10th. 2009 */
PAL_INT32_T pal_sem_timedwait(PAL_SEM_T *sem, PAL_INT32_T sec)
{
    PAL_TIME_T _t;
    struct timespec _ts;

    _t = pal_time_current(NULL);
    _ts.tv_sec = _t + sec;
    _ts.tv_nsec = 0;

    return sem_timedwait(sem, &_ts);
}


PAL_INT32_T pal_sem_destroy ( PAL_SEM_T *sem )
{
    return sem_destroy ( sem );
}

