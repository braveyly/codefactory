#ifndef _EH_SSL_SOCKET_H_
#define _EH_SSL_SOCKET_H_
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include <tcm_log.h>

#define MAX_BUF 1000
SSL_CTX *gctx;
int gehMaxFd;
int gehServerFd;
fd_set readFdsMaster;
pthread_t gsslTid;
extern pthread_mutex_t remoteMsgMutex;
extern pthread_cond_t remoteMsgCond;
extern int remoteMsgReadyFlag;

#define UPDATE_MAXFD(f)  (gehMaxFd = (f > gehMaxFd) ? f : gehMaxFd)
#define DEFAULT_EHCC_SOCKET_PORT 6666
#define DEFAULT_EHCC_LISTEN_SUM 9
#define MAX_BUF_LEN 1000
#define SA struct sockaddr
tcmRet generateCertKey ( int sslBitNum, int sslValidity );
tcmRet initSsl ( char* keyPath, char* certPath );
int initServerSocket();
tcmRet waitforRemoteMsgbySelect();
tcmRet startWriteMsgToRemoteDevice();
tcmRet closeSsl();
tcmRet setKeepAlive( int kvFd ); 
tcmRet initKeepAlive();

#endif
