#ifndef _EH_MPC_H_
#define _EH_MPC_H_
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_PACKET_LEN  1024
#define MAX_MPC_THREAD 5
pthread_t gmpcTid[MAX_MPC_THREAD];
tcmRet startMpc();
tcmRet stopMpc();
int sendSynTime( void* arg1, void* arg2 );
int keepAliveSync( void* arg1, void* arg2 );
tcmRet sendSecurityModeToIDP ();
uint16_t getOutdoorKeepTime( int outdoorType );
uint16_t getOutdoorKeepTimeBySocket( UBOOL8 needLock, int ktsocket );
void updateKeepAliveStatus(int connfd);
int updateCardInfo( void* arg, void* arg2, int len );
tcmRet sendRegisterNoDeviceACK( int connfd, SSL* ssl );

void releaseTokenByFD(int fd);

#endif
