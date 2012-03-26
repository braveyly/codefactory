#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "ehTimer.h"
#include "appMonitor.h"

static MonitorClient gmonitorClient[MAX_MONITOR_CLIENT_NUMBER];

void initSignal()
{
    signal ( SIGTERM, SIG_IGN );
    signal ( SIGKILL, SIG_IGN );
    signal ( SIGCHLD, SIG_IGN );
}

void initMC ( MonitorClient* pMClient, int MCNum )
{
    int i, j;
    for ( i = 0; i < MCNum; i++ )
    {
        pMClient[i].fd = 0;
        pMClient[i].argc = 0;
        memset ( pMClient[i].lockFileName, 0,
                 sizeof ( pMClient[i].lockFileName ) );
        for ( j = 0; j < MAX_PARAMETER_NUMBER; j++ )
        {
            memset ( pMClient[i].argv[j], 0,
                     sizeof ( pMClient[i].argv[j] ) );
        }
    }
}

int registerOneMC ( MonitorClient* pMClient, int MCNum, int argc, char** argv )
{
    int i = 0, j;
    for ( i = 0; i < MCNum; i++ )
        if ( !pMClient[i].argc )
        {
            for ( j = 0; j < argc; j++ )
            {
                strcpy ( pMClient[i].argv[j],  argv[j] );
            }
            pMClient[i].argc = argc;
            if ( '.' == * ( pMClient[i].argv[0] + 0 ) )
                strcpy ( pMClient[i].lockFileName, pMClient[i].argv[0] + 2 );
            i++;
            break;
        }
}

void dumpMCInfo ( MonitorClient* pMClient, int MCNum )
{
    int i;
    while ( MCNum + 1 )
    {
        if ( pMClient[MCNum].argc )
        {
            printf ( "\n\nNow show info of APP[%d]\n", MCNum );
            printf ( "  argc=[%d]\n", pMClient[MCNum].argc );
            for ( i = 0; i < pMClient[MCNum].argc; i++ )
            {
                printf ( "  argv[%d]=[%s]\n", i, pMClient[MCNum].argv[i] );
            }
            printf ( "  lockFileName=[%s]\n", pMClient[MCNum].lockFileName );
        }
        MCNum--;
    }
}

void checkMC ( void* arg1, void* arg2 )
{
    printf ( "\n==Go on to check MC==\n" );
    int i;
    char filePath[MAX_PARAMETER_VALUE_LEN] = {0};
    char cmdBuf[MAX_PARAMETER_VALUE_LEN] = {0};
    int lock_result;
    int waitResult;
    for ( i = 0; i < MAX_MONITOR_CLIENT_NUMBER; i++ )
    {
        /*1st step:Open lock file of APP*/
        if ( ( 0 == gmonitorClient[i].fd ) && ( gmonitorClient[i].argc ) )
        {
            memset ( filePath, 0, sizeof ( filePath ) );
            sprintf ( filePath, "%s/%s", APP_FILE_PRE_PATH, gmonitorClient[i].lockFileName );
            gmonitorClient[i].fd = open ( filePath, O_RDWR );
            if ( gmonitorClient[i].fd < 0 )
            {
                printf ( "Failed to open file %s\n", filePath );
#ifdef AUTO_LAUNCH_APP_BY_MONITOR
                sprintf ( cmdBuf, "touch %s", filePath );
                system ( cmdBuf );
                gmonitorClient[i].fd = open ( filePath, O_RDWR );
                if ( gmonitorClient[i].fd > 0 )
                {
                    printf ( "Succeed to create then open file %s again by monitor\n", filePath );
                }
                else
                {
                    printf ( "Failed to create file %s by monitor\n", filePath );
                }
#endif
            }
            else
            {
                printf ( "Succeed to open file %s/%s by monitor\n", APP_FILE_PRE_PATH, gmonitorClient[i].lockFileName );
            }
        }

        /*2nd step: Test fds that have been opened*/
        if ( gmonitorClient[i].fd > 0 )
        {
            lock_result = lockf ( gmonitorClient[i].fd, F_TEST, 0 );
            /*File has not lock, launch app*/
            if ( 0 == lock_result )
            {
                printf ( "File %s/%s HAS NOT been locked, so launch its app\n", APP_FILE_PRE_PATH, gmonitorClient[i].lockFileName );

                if ( 0 == fork() )
                {
                    if ( 0 == fork() )
                    {
                        execlp ( gmonitorClient[i].argv[0],
                                 gmonitorClient[i].argv[0],
                                 gmonitorClient[i].argv[1], NULL );
                    }
                    else
                    {
                        exit ( 1 );
                    }
                }
                else
                {
                    wait ( &waitResult );
                }
                /*
                execlp ( gmonitorClient[i].argv[0],
                         gmonitorClient[i].argv[0],
                         gmonitorClient[i].argv[1], NULL );
                */

            }
            /*File has lock, do nothing*/
            else
            {
                printf ( "File %s/%s HAS been locked, so do nothing\n", APP_FILE_PRE_PATH, gmonitorClient[i].lockFileName );
            }
        }
    }
}

int startTimertoCheckMC()
{
    struct timeval tmv;
    tmv.tv_sec = CHECK_PERIOD_TIME;
    tmv.tv_usec = 0;
    int timerID = installTimer ( checkMC , tmv, PERIOD );
    if ( TCMRET_SUCCESS != startTimer ( timerID ) )
    {
        tcmLog_notice ( "Start timer id %d error", timerID );
    }
}

int gointoFairyProcess()
{
    pid_t pid;
    if ( ( pid = fork() ) < 0 )
        return -1;
    else if ( pid != 0 )
        exit ( 0 );
    setsid();
    //chdir ( "/" );
    //umask ( 0 );
    return 0;
}

int main ( int argc, char* argv[] )
{
    gointoFairyProcess();
    char parArray[MAX_PARAMETER_NUMBER][MAX_PARAMETER_VALUE_LEN];
    char* ppArray[MAX_PARAMETER_NUMBER];
    int i = 0;

    initSignal();
    initMC ( gmonitorClient, MAX_MONITOR_CLIENT_NUMBER );

    for ( i = 0; i < MAX_PARAMETER_NUMBER; i++ )
    {
        memset ( parArray[i], 0 , sizeof ( parArray[i] ) );
        ppArray[i] = parArray[i];
    }

    strcpy ( parArray[0], "./b" );
    sprintf ( parArray[1], "%s/b", APP_FILE_PRE_PATH );
    registerOneMC ( gmonitorClient,
                    MAX_MONITOR_CLIENT_NUMBER,
                    2, ppArray );

    strcpy ( parArray[0], "./c" );
    sprintf ( parArray[1], "%s/c", APP_FILE_PRE_PATH );
    registerOneMC ( gmonitorClient,
                    MAX_MONITOR_CLIENT_NUMBER,
                    2, ppArray );

    dumpMCInfo ( gmonitorClient, MAX_MONITOR_CLIENT_NUMBER );

    startTimertoCheckMC();

    while ( 1 )
    {
        sleep ( 1000 );
    }
}
