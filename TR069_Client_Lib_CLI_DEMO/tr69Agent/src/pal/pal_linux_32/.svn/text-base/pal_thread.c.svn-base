#include "pal_thread.h"

PAL_INT32_T pal_thread_create ( PAL_THREAD_T *t, void * ( *func ) ( void *arg ), PAL_THREAD_ATTR_T *a, void *arg )
{
    pthread_attr_t thread_attr;
	
    pthread_attr_init ( &thread_attr );

    if ( 0 != pthread_attr_setdetachstate ( &thread_attr, PTHREAD_CREATE_DETACHED ) )
    {
        return -1;
    }

    if ( NULL != a )
    {
        struct sched_param param; 

	    pthread_attr_getschedparam (&thread_attr, &param);
	    param.sched_priority = a->priority;
	    pthread_attr_setschedparam (&thread_attr, &param);
        pthread_attr_setstacksize (&thread_attr, a->stackSize);
    }

    return pthread_create ( t, &thread_attr, func, arg );
}

void pal_thread_exit ( void )
{
    pthread_exit (0);
}

/* add by Hook Guo, June 10th. 2009 */
PAL_INT32_T pal_thread_cancel(PAL_THREAD_T *t)
{
    PAL_INT32_T ret = -1;
    if(t && *t)
    {
         ret = pthread_cancel(t);
         *t = 0;		 
    }

     return ret;			
}
