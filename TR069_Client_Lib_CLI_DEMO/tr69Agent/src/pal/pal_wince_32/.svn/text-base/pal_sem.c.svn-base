#include "pal_sem.h"

PAL_INT32_T pal_sem_init ( PAL_SEM_T *sem )
{
    *sem = CreateSemaphore(NULL, 0, MAX_SEM_COUNT_VALUE, NULL);
    
    return (*sem == NULL) ? -1 : 0;
}

PAL_INT32_T pal_sem_post ( PAL_SEM_T *sem )
{
    if (ReleaseSemaphore(*sem, 1, NULL) != 0)
    {
        /* succeed */
        return 0;
    }
    else
    {
        /* failure */
        return -1;
    }
}

PAL_INT32_T pal_sem_wait ( PAL_SEM_T *sem )
{
    DWORD retval;
    
    if ((retval = WaitForSingleObject(*sem, INFINITE)) == WAIT_OBJECT_0)
    {
        return 0;
    }
    else if (retval == WAIT_FAILED)
    {
        return -1;
    }
}

PAL_INT32_T pal_sem_destroy ( PAL_SEM_T *sem )
{
    return (CloseHandle(*sem) == TRUE) ? 0 : -1;
}
