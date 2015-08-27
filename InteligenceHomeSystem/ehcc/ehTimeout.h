#ifndef _EH_TIMEOUT_H_
#define _EH_TIMEOUT_H_
#include <sys/time.h>

#define UTC_TIME_SYSTEM
long gcardTimeout;
long gidpPairFailedTimes;

tcmRet initGlobalTimeout();
int isTimeout ( time_t startTime, long timeoutPeriod );
char* convertTimevalToChar ( struct timeval tmVal );
#endif
