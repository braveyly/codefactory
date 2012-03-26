#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "appMonitor.h"
#include "appBeMonitored.h"

int keepFileLock ( char* appName )
{
    int fd;
    int lock_result;
    char cmdBuf[100] = {0};
    char filePath[100] = {0};

    printf ( "Name is %s\n", appName );
    sprintf ( filePath, "%s/%s", APP_FILE_PRE_PATH, appName );
    sprintf ( cmdBuf, "touch %s", filePath );
    system ( cmdBuf );
    fd = open ( filePath, O_RDWR );
    if ( fd < 0 )
    {
        printf ( "Failed to open file %s\n", filePath );
        return 1;
    }
    else
    {
        printf ( "Succeed to open file %s\n", filePath );
    }

    lock_result = lockf ( fd, F_LOCK, 0 );
    if ( 0 == lock_result )
    {
        printf ( "Succeed to lock file %s\n", filePath );
    }
    else
    {
        printf ( "Failed to lock file %s\n", filePath );
    }
    return 0;
}