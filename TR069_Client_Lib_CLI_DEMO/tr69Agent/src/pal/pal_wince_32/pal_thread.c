#include "pal_thread.h"

PAL_INT32_T pal_thread_create(PAL_THREAD_T *t
                             , PAL_THREAD_START_ROUTINE *func
                             , PAL_THREAD_ATTR_T *attr
                             , void *arg)
{   
    HANDLE h = CreateThread(NULL /* thread attributes, must be NULL */
                           , 0 /* stack size, ignored */
                           , (LPTHREAD_START_ROUTINE)func /* start routine */
                           , arg /* value passed to thread */
                           , 0 /* creationg flags, 0 means run immediately
                                  after createion */
                           , NULL /* thread id */);
    
    if (h == NULL)
    {
        return -1;
    }

    /* set thread attributes, here only priority can be set. stack size
     * can be set using link options */
    if (attr != NULL)
    {
        if (CeSetThreadPriority(h, attr->priority) == FALSE)
        {
            TerminateThread(h, 0);
            return -1;
        }
    }

    return 0;
}

void pal_thread_exit()
{
    HANDLE h = GetCurrentThread();

    TerminateThread(h, 0);
}
