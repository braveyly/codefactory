#ifndef _APP_MONITOR_H_
#define _APP_MONITOR_H_

#define CHECK_PERIOD_TIME 10
#define MAX_PARAMETER_NUMBER 10
#define MAX_PARAMETER_VALUE_LEN 20
#define MAX_MONITOR_CLIENT_NUMBER 20
#define APP_FILE_PRE_PATH "/root"
#define AUTO_LAUNCH_APP_BY_MONITOR 1

typedef struct MONITORCLIENT
{
    int fd;
    char lockFileName[MAX_PARAMETER_VALUE_LEN];
    int argc;
    char argv[MAX_PARAMETER_NUMBER][MAX_PARAMETER_VALUE_LEN];
}MonitorClient;

#endif