#include "pal_mutex.h"

PAL_INT32_T pal_mutex_init(PAL_MUTEX_T *mutex)
{
    *mutex = CreateMutex(NULL, TRUE, NULL);

    return (*mutex == NULL) ? -1 : 0;
}

PAL_INT32_T pal_mutex_lock(PAL_MUTEX_T *mutex)
{
    DWORD retval;
    
    if ((retval = WaitForSingleObject(*mutex, INFINITE)) == WAIT_OBJECT_0)
    {
        return 0;
    }
    else if (retval == WAIT_FAILED)
    {
        return -1;
    }
}

PAL_INT32_T pal_mutex_unlock(PAL_MUTEX_T *mutex)
{
    return (ReleaseMutex(*mutex) == TRUE) ? 0 : -1;
}

PAL_INT32_T pal_mutex_destroy(PAL_MUTEX_T *mutex)
{
    return (CloseHandle(*mutex) == TRUE) ? 0 : -1;
}
