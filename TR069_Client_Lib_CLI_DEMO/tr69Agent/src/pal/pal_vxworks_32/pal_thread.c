#include "pal_thread.h"

PAL_INT32_T pal_thread_create ( PAL_THREAD_T *t, void * ( *func ) ( void *arg ), PAL_THREAD_ATTR_T *a, void *arg )
{
    *t = taskSpawn ( a->threadName, a->priority, 0, a->stackSize, ( FUNCPTR ) func, ( int ) arg, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    if(*t == ERROR)
    {
        printf ("\nkiffen task failure\n");
        return -1;
    }
    return 0;
}

void pal_thread_exit (void)
{
    exit ( 0 );
}

PAL_INT32_T pal_thread_cancel(PAL_THREAD_T *t)
{
    return taskDelete(*t);
}

