#include <ehSslSocket.h>
#include "ehDeviceDef.h"
#include "ehTimeout.h"
#include "ehDevice.h"
#include "ehMq.h"
#include "ehIndexDevice.h"
#include "ehTransMsgMap.h"
#include "ehHex.h"
#include "ehMutex.h"
#include "writedb.h"

#define LOCAL_ETH_NAME     "br0"
#define WAN_ETH_NAME       "eth2.2"

pthread_mutex_t remoteMsgMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t remoteMsgCond = PTHREAD_COND_INITIALIZER;
int remoteMsgReadyFlag = 0;

tcmRet initSsl ( char* keyPath, char* certPath )
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    gctx = SSL_CTX_new ( SSLv23_server_method() );
    if ( gctx == NULL )
    {
        ERR_print_errors_fp ( stdout );
        return TCMRET_FAILURE;
    }

    if ( SSL_CTX_use_certificate_file ( gctx, certPath,
                                        SSL_FILETYPE_PEM ) <= 0 )
    {
        ERR_print_errors_fp ( stdout );
        return TCMRET_FAILURE;
    }

    if ( SSL_CTX_use_PrivateKey_file ( gctx, keyPath, SSL_FILETYPE_PEM ) <= 0 )
    {
        ERR_print_errors_fp ( stdout );
        return TCMRET_FAILURE;
    }

    if ( !SSL_CTX_check_private_key ( gctx ) )
    {
        ERR_print_errors_fp ( stdout );
        return TCMRET_FAILURE;
    }

    return TCMRET_SUCCESS;
}

int initServerSocket ( int socketPort, int socketListenSum, char* socketBindIP )
{

    if ( socketPort == 0 )
    {
        socketPort = DEFAULT_EHCC_SOCKET_PORT;
    }
    if ( socketListenSum == 0 )
    {
        socketListenSum = DEFAULT_EHCC_LISTEN_SUM;
    }
    struct sockaddr_in server_addr;
    int sockfd;
    int flag=1,len=sizeof(int);
    
    if ( ( sockfd = socket ( PF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
        tcmLog_error ( "Create socket failed, the reason is %s",
                       strerror ( errno ) );
        return -1;
    }
    else
    {
        tcmLog_notice ( "Socket created" );
    }
    
    if( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1 )
    {
       tcmLog_error( "setsockopt SO_REUSEADDR failed" );
    }
    
    bzero ( &server_addr, sizeof ( server_addr ) );
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons ( socketPort );
    if ( socketBindIP != NULL )
        server_addr.sin_addr.s_addr = inet_addr ( socketBindIP );
    else
        server_addr.sin_addr.s_addr = INADDR_ANY;

    if ( bind ( sockfd, ( struct sockaddr * ) &server_addr,
                sizeof ( struct sockaddr ) ) == -1 )
    {
        tcmLog_error ( "Socket bind failed, the reason is %s",
                       strerror ( errno ) );
        return -1;
    }
    else
    {
        tcmLog_notice ( "Socket binded" );
    }

    if ( listen ( sockfd, socketListenSum ) == -1 )
    {
        tcmLog_error ( "Socket listen failed" );
        return -1;
    }
    else
    {
        tcmLog_notice ( "Socket is listening" );
    }

    return sockfd;
}

tcmRet initKeepAlive()
{
    system ( "echo 5 > /proc/sys/net/ipv4/tcp_keepalive_time" );
    system ( "echo 2 > /proc/sys/net/ipv4/tcp_keepalive_intvl" );
    system ( "echo 3 > /proc/sys/net/ipv4/tcp_keepalive_probes" );
    return TCMRET_SUCCESS;
}

tcmRet setKeepAlive ( int kvFd )
{
    int keepAliveFlag = 1;
    int ret;
    ret = setsockopt ( kvFd, SOL_SOCKET, SO_KEEPALIVE,
                       ( void * ) & keepAliveFlag,
                       sizeof ( keepAliveFlag ) );

    return ( ( ret == 0 ) ? TCMRET_SUCCESS : TCMRET_FAILURE );
}

tcmRet ehSslRead ( SSL* rdSsl, homeMessage** rdBuf,
                   unsigned int rdTimeout )
{
    tcmLog_debug ( "Start SSL read message" );
    homeMessage* msgTmp;
    int rc;

    if ( rdBuf == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }
    else
    {
        *rdBuf = NULL;
    }

    msgTmp = ( homeMessage * ) malloc ( sizeof ( homeMessage ) );

    if ( msgTmp == NULL )
    {
        return TCMRET_INTERNAL_ERROR;
    }

    rc = SSL_read ( rdSsl, msgTmp, sizeof ( homeMessage ) );

    /* new 2.6.21 kernel seems to give us this before rc==0 */
    if ( ( rc == 0 ) ||
            ( ( rc == -1 ) && ( errno == 131 ) ) )
    {
        /* broken connection */
        free ( msgTmp );
        return TCMRET_DISCONNECTED;
    }
    else if ( rc < -1 || rc != sizeof ( homeMessage ) )
    {
        free ( msgTmp );
        return TCMRET_INTERNAL_ERROR;
    }

    uint16_t valueLenTmp = msgTmp->valueLen;
    msgTmp->valueLen = ntohs ( valueLenTmp );
    if ( msgTmp->valueLen > 0 )
    {
        int totalReadSoFar = 0;
        int totalRemaining = msgTmp->valueLen;
        char *inBuf;

        /* there is additional data in the message */
        msgTmp = ( homeMessage * ) realloc ( msgTmp,
                 sizeof ( homeMessage ) + msgTmp->valueLen );
        if ( msgTmp == NULL )
        {
            free ( msgTmp );
            return TCMRET_INTERNAL_ERROR;
        }

        inBuf = ( char * ) ( msgTmp + 1 );
        while ( totalReadSoFar < msgTmp->valueLen )
        {
            rc = SSL_read ( rdSsl, inBuf, totalRemaining );
            if ( ( rc == 0 ) ||
                    ( ( rc == -1 ) && ( errno == 131 ) ) )
            {
                free ( msgTmp );
                return TCMRET_DISCONNECTED;
            }
            else if ( rc < -1 )
            {
                free ( msgTmp );
                return TCMRET_INTERNAL_ERROR;
            }
            else
            {
                inBuf += rc;
                totalReadSoFar += rc;
                totalRemaining -= rc;
            }
        }
    }

    /*Start Hex printf*/
    tcmLog_debug ( "Read HEX buf from SSL" );
    if ( ( sizeof ( homeMessage ) + msgTmp->valueLen ) < 200 )
        dump ( (void*)msgTmp, sizeof ( homeMessage ) + msgTmp->valueLen );
    /*End Hex printf*/
    *rdBuf = msgTmp;
    return TCMRET_SUCCESS;
}

tcmRet ehSslWrite ( SSL* wtSsl, homeMessage* wtBuf )
{
    unsigned int totalLen;
    int rc;
    tcmRet ret = TCMRET_SUCCESS;
    uint16_t valueLenTmp = ntohs ( wtBuf->valueLen );
    totalLen = sizeof ( homeMessage ) + valueLenTmp;
    /*Start Hex printf*/
    tcmLog_debug ( "Write HEX buf to SSL" );
    if ( totalLen < 200 )
        dump ( (void*)wtBuf, totalLen );
    /*End Hex printf*/
    rc = SSL_write ( wtSsl, ( char * ) wtBuf, totalLen );
    if ( rc < 0 )
    {
        if ( errno == EPIPE )
        {
            /*
             * This could happen when smd tries to write to an app that
             * has exited.  Don't print out a scary error message.
             * Just return an error code and let upper layer app handle it.
             */
            return TCMRET_DISCONNECTED;
        }
        else
        {
            ret = TCMRET_INTERNAL_ERROR;
        }
    }
    else if ( rc != ( int ) totalLen )
    {
        ret = TCMRET_INTERNAL_ERROR;
    }
    return ret;

}


tcmRet ehRead ( int rdFd, homeMessage** rdBuf, unsigned int rdTimeout )
{
    homeMessage* msgTmp;
    int rc;

    if ( rdBuf == NULL )
    {
        return TCMRET_INVALID_PARAMETER;
    }
    else
    {
        *rdBuf = NULL;
    }

    msgTmp = ( homeMessage * ) malloc ( sizeof ( homeMessage ) );

    if ( msgTmp == NULL )
    {
        return TCMRET_INTERNAL_ERROR;
    }
    rc = read ( rdFd, msgTmp, sizeof ( homeMessage ) );

    /* new 2.6.21 kernel seems to give us this before rc==0 */
    if ( ( rc == -1 ) && ( errno == 131 ) )
    {
        tcmLog_error ( "First read rc == -1 \n" );
        /* broken connection */
        free ( msgTmp );
        return TCMRET_DISCONNECTED;
    }
    else if ( rc < -1 || rc != sizeof ( homeMessage ) )
    {
        tcmLog_error ( "First read rc < -1\n" );
        free ( msgTmp );
        return TCMRET_DISCONNECTED;
    }

    uint16_t valueLenTmp = msgTmp->valueLen;
    msgTmp->valueLen = ntohs ( valueLenTmp );
    if ( msgTmp->valueLen > 0 )
    {
        int totalReadSoFar = 0;
        int totalRemaining = msgTmp->valueLen;
        char *inBuf;

        /* there is additional data in the message */
        msgTmp = ( homeMessage * ) realloc ( msgTmp,
                 sizeof ( homeMessage ) + msgTmp->valueLen );
        if ( msgTmp == NULL )
        {
            free ( msgTmp );
            return TCMRET_INTERNAL_ERROR;
        }

        inBuf = ( char * ) ( msgTmp + 1 );
        while ( totalReadSoFar < msgTmp->valueLen )
        {
            rc = read ( rdFd, inBuf, totalRemaining );
            if ( ( rc == -1 ) && ( errno == 131 ) )
            {
                tcmLog_error ( "Second read rc == -1\n" );
                free ( msgTmp );
                return TCMRET_DISCONNECTED;
            }
            else if ( rc < -1 )
            {
                tcmLog_error ( "Second read rc < -1 \n" );
                free ( msgTmp );
                return TCMRET_DISCONNECTED;
            }
            else
            {
                inBuf += rc;
                totalReadSoFar += rc;
                totalRemaining -= rc;
            }
        }
    }
    /*Start Hex printf*/
    tcmLog_debug ( "Read HEX buf from socket fd %d", rdFd );
    if ( ( sizeof ( homeMessage ) + msgTmp->valueLen ) < 200 )
        dump ( (void*)msgTmp, sizeof ( homeMessage ) + msgTmp->valueLen );
    /*End Hex printf*/
    *rdBuf = msgTmp;
    return TCMRET_SUCCESS;
}

tcmRet ehWrite ( int wtFd, homeMessage* wtBuf )
{
    unsigned int totalLen;
    int rc;
    tcmRet ret = TCMRET_SUCCESS;
    uint16_t valueLenTmp = ntohs ( wtBuf->valueLen );
    totalLen = sizeof ( homeMessage ) + valueLenTmp;
    /*Start Hex printf*/
    tcmLog_debug ( "Write HEX buf to socket fd %d", wtFd );
    if ( totalLen < 200 )
        dump ( (void*)wtBuf, totalLen );
    /*End Hex printf*/
    rc = write ( wtFd, ( char * ) wtBuf, totalLen );
    if ( rc < 0 )
    {
        if ( errno == EPIPE )
        {
            /*
             * This could happen when smd tries to write to an app that
             * has exited.  Don't print out a scary error message.
             * Just return an error code and let upper layer app handle it.
             */
            return TCMRET_DISCONNECTED;
        }
        else
        {
            ret = TCMRET_INTERNAL_ERROR;
        }
    }
    else if ( rc != ( int ) totalLen )
    {
        ret = TCMRET_INTERNAL_ERROR;
    }
    return ret;

}

tcmRet cvtNtoHEndian ( homeMessage** cvtHomeMsg )
{
    uint16_t protocolVersionTmp = ( *cvtHomeMsg )->protocolVersion;
    ( *cvtHomeMsg )->protocolVersion =  ntohs ( protocolVersionTmp );

    uint16_t payloadLenTmp = ( *cvtHomeMsg )->payloadLen;
    ( *cvtHomeMsg )->payloadLen = ntohs ( payloadLenTmp );

    uint16_t valueTypeTmp = ( *cvtHomeMsg )->eventType;
    ( *cvtHomeMsg )->eventType = ntohs ( valueTypeTmp );

    return TCMRET_SUCCESS;
}

tcmRet cvtHtoNEndian ( homeMessage** cvtHomeMsg )
{
    uint16_t protocolVersionTmp = ( *cvtHomeMsg )->protocolVersion;
    ( *cvtHomeMsg )->protocolVersion =  htons ( protocolVersionTmp );

    uint16_t payloadLenTmp = ( *cvtHomeMsg )->payloadLen;
    ( *cvtHomeMsg )->payloadLen = htons ( payloadLenTmp );

    uint16_t valueTypeTmp = ( *cvtHomeMsg )->eventType;
    ( *cvtHomeMsg )->eventType = htons ( valueTypeTmp );

    uint16_t valueLenTmp = ( *cvtHomeMsg )->valueLen;
    ( *cvtHomeMsg )->valueLen = htons ( valueLenTmp );

    return TCMRET_SUCCESS;
}

int getPeerMacbySocketFd ( int sockfd, char *buf, char* localethname,
                           char* localwanethname )
{
    int ret = 0;
    struct arpreq arpreq;
    struct arpreq arpreqwan;
    struct sockaddr_in dstadd_in;
    socklen_t  len = sizeof ( struct sockaddr_in );
    memset ( &arpreq, 0, sizeof ( struct arpreq ) );
    memset ( &arpreqwan, 0, sizeof ( struct arpreq ) );
    memset ( &dstadd_in, 0, sizeof ( struct sockaddr_in ) );
    if ( getpeername ( sockfd, ( struct sockaddr* ) &dstadd_in, &len ) < 0 )
    {
        tcmLog_error ( "get peer name error, %s\n", strerror ( errno ) );
        return -1;
    }
    else
    {
        memcpy ( ( struct sockaddr_in * ) &arpreq.arp_pa,
                 ( struct sockaddr_in * ) &dstadd_in,
                 sizeof ( struct sockaddr_in ) );
        strcpy ( arpreq.arp_dev, localethname );
        arpreq.arp_pa.sa_family = AF_INET;
        arpreq.arp_ha.sa_family = AF_UNSPEC;

        memcpy ( ( struct sockaddr_in * ) &arpreqwan.arp_pa,
                 ( struct sockaddr_in * ) &dstadd_in,
                 sizeof ( struct sockaddr_in ) );
        strcpy ( arpreqwan.arp_dev, localwanethname );
        arpreqwan.arp_pa.sa_family = AF_INET;
        arpreqwan.arp_ha.sa_family = AF_UNSPEC;

        if ( ioctl ( sockfd, SIOCGARP, &arpreq ) >= 0 )
        {
            unsigned char* ptr = ( unsigned char * ) arpreq.arp_ha.sa_data;
            sprintf ( buf, "%02x:%02x:%02x:%02x:%02x:%02x",
                      *ptr, * ( ptr + 1 ),
                      * ( ptr + 2 ), * ( ptr + 3 ),
                      * ( ptr + 4 ), * ( ptr + 5 ) );
        }
        else if ( ioctl ( sockfd, SIOCGARP, &arpreqwan ) >= 0 )
        {
            unsigned char* ptr = ( unsigned char * ) arpreqwan.arp_ha.sa_data;
            sprintf ( buf, "%02x:%02x:%02x:%02x:%02x:%02x",
                      *ptr, * ( ptr + 1 ),
                      * ( ptr + 2 ), * ( ptr + 3 ),
                      * ( ptr + 4 ), * ( ptr + 5 ) );
        }
        else
        {
            tcmLog_debug ( "IO Ctrl error,cannot use arp protocal" );
            return 0;
        }
    }
    return ret;
}
#define MAX_SOCKET_FD_NUM 300
#define SOCKET_ERROR_CHECK_SWITCH 1
tcmRet waitforRemoteMsgbySelect()
{
    struct timeval tm;
    fd_set readFdsMasterTmp;
    int maxFdTmp, actualFd;
    int rv;
    socklen_t sockAddressLen;
    struct sockaddr_in cliAddress;
    int connFd;
    tcmRet ret;
    int error, errlen;
    SSL* sslArray[MAX_SOCKET_FD_NUM];
    int i;

    for ( i = 0 ; i < MAX_SOCKET_FD_NUM; i++ )
    {
        sslArray[i] = NULL;
    }

#ifdef SSL_CONNECT
    if ( TCMRET_SUCCESS != initSsl ( "/etc_ro/sslcert/Ehome4SSLPrivateKey.pem",
                                     "/etc_ro/sslcert/Ehome4SSLCACert.pem" ) )
    {
        tcmLog_error ( "Init ssl failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_debug ( "Init ssl successfully" );
    }
#endif

    if ( -1 == ( gehServerFd = initServerSocket ( 0, 0, NULL ) ) )
    {
        tcmLog_error ( "Init server socket failed" );
        return TCMRET_FAILURE;
    }
    else
    {
        tcmLog_debug ( "Init server socket successfully" );
#ifdef AUTO_KEEP_ALIVE
        setKeepAlive ( gehServerFd );
#endif
    }

    FD_ZERO ( &readFdsMaster );
    gehMaxFd = 0;
    FD_SET ( gehServerFd, &readFdsMaster );
    UPDATE_MAXFD ( gehServerFd );

    while ( 1 )
    {
        tm.tv_sec = 4;
        tm.tv_usec = 0;
        FD_ZERO ( &readFdsMasterTmp );
        readFdsMasterTmp = readFdsMaster;
        maxFdTmp = gehMaxFd;
        tcmLog_notice("Max fd: %d, server fd: %d", gehMaxFd, gehServerFd);
        rv = select ( maxFdTmp + 1, &readFdsMasterTmp, NULL, NULL, NULL );
        if ( rv < 0 )
        {
            tcmLog_error ( " REMOTE select failed, the reason is %s",
                           strerror ( errno ) );
#ifdef SOCKET_ERROR_CHECK_SWITCH
            /*Error check for sockets*/
            int i;
            for ( i = 0; i < maxFdTmp + 2; i ++ )
            {
                getsockopt ( i, SOL_SOCKET, SO_ERROR, &error,
                             ( socklen_t * ) &errlen );
                if ( error == -1 )
                {
                    tcmLog_error ( "socket %d is error no is %d errlen is %d,",
                                   i, error, errlen );
                    continue;
                }
            }
            /*End Error check*/
#endif
            continue;
        }

        if ( rv == 0 )
        {
            tcmLog_notice ( " REMOTE select timeout" );
            continue;
        }

        for ( actualFd = 0 ; actualFd < maxFdTmp + 1; actualFd++ )
        {
            /*fd-set match with select*/
            if ( !FD_ISSET ( actualFd, &readFdsMasterTmp ) )
            {
                continue;
            }

            tcmLog_notice ( "Select return actualfd: %d server: fd %d maxfd: %d",
                           actualFd, gehServerFd, maxFdTmp );
            if ( actualFd == gehServerFd )
            {
                tcmLog_notice ( "Server socket %d have new socket fd",
                               actualFd );

#ifdef SOCKET_ERROR_CHECK_SWITCH
                /*Start Error check for socket*/
                getsockopt ( actualFd, SOL_SOCKET, SO_ERROR, &error,
                             ( socklen_t * ) &errlen );
                if ( error == -1 )
                {
                    tcmLog_debug ( "Before accept actualFd == gehServerFd "
                                   "This socket %d is error no is %d  "
                                   "errlen is %d", actualFd, error,
                                   errlen );
                    continue;
                }
                /*End Error check*/
#endif
                sockAddressLen = sizeof ( struct sockaddr_in );
                while( ( connFd = accept ( gehServerFd, ( SA * ) & cliAddress, 
                                           &sockAddressLen ) ) == -1 )
                {
                   if( errno != EINTR )
                   {
                      tcmLog_error( "Accept failed for NO_EINTR reason" );
                      break;
                   }
                   tcmLog_error( "Accept failed for EINTR reason,another try" );
                }
                
                if ( 3 > connFd )
                {

#ifdef SOCKET_ERROR_CHECK_SWITCH
                    /*Start Error check for socket*/
                    /*first deal with server socket*/
                    getsockopt ( actualFd, SOL_SOCKET, SO_ERROR, &error,
                                 ( socklen_t * ) &errlen );
                    if ( error == -1 )
                    {
                        tcmLog_debug ( "After acccept actualFd == gehServerFd "
                                       "This socket %d is error no is %d "
                                       "errlen is %d",
                                       actualFd, error, errlen );
                        continue;
                    }
                    /*second deal with connection socket*/
                    getsockopt ( connFd, SOL_SOCKET, SO_ERROR, &error,
                                 ( socklen_t * ) &errlen );
                    if ( error == -1 )
                    {
                        tcmLog_debug ( "gehServerFd accept connFd "
                                       "socket %d is error no is %d "
                                       "errlen is %d",
                                       connFd, error, errlen );
                        continue;
                    }
                    /*End Error check*/
#endif
                    tcmLog_error ( "Accept socket error, reason is %s",
                                   strerror ( errno ) );
                    continue;
                }
                else
                {
                    tcmLog_notice ( "Accepted! Client IP %s ,FD is %d",
                                    inet_ntoa ( cliAddress.sin_addr ), connFd );
#ifdef AUTO_KEEP_ALIVE
                    setKeepAlive ( connFd );
#endif

                }

                moduleMutexLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
                /*have new connection, judge peer device by its mac, not by its ip,
                  which may be the same for two devices*/
                char peerDeviceMac[200];
                memset ( peerDeviceMac, 0, sizeof(peerDeviceMac) );
                if ( 0 != getPeerMacbySocketFd ( connFd, peerDeviceMac,
                                                 LOCAL_ETH_NAME,
                                                 WAN_ETH_NAME ) )
                {
                    tcmLog_error ( "getPeerMacbySocketFd error" );
                    close ( connFd );
                    moduleMutexUnLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
                    continue;
                }
                tcmLog_notice ( "Peer device MAC is %s", peerDeviceMac );
                int devIndex = getDeviceIndexByMac(peerDeviceMac);
                DeviceInfo* ehDeviceTmp = getDeviceInfoByDeviceIndex(devIndex);

                /*match device unit with socket fd if find the device[IDP/ODP]
                  by mac*/
                if ( ehDeviceTmp != NULL )
                {
                    tcmLog_notice ( "%s exist in device list. Manage this"
                                    "socket fd",
                                    inet_ntoa ( cliAddress.sin_addr ) );
#ifdef SSL_CONNECT
                    FD_SET ( connFd, &readFdsMaster );
                    /*Change maxfd value.This result in puzzle for FD_ISSET*/
                    UPDATE_MAXFD ( connFd );
                    g_devArray[devIndex - 1].connfd = connFd;
                    g_devArray[devIndex - 1].ssl = SSL_new ( gctx );
                    SSL_set_fd ( g_devArray[devIndex - 1].ssl,
                                 g_devArray[devIndex - 1].connfd );
                    if ( SSL_accept ( g_devArray[devIndex - 1].ssl ) == -1 )
                    {
                        tcmLog_error ( "SSL accept failed, the reason is %s",
                                       strerror ( errno ) );
                        SSL_shutdown ( g_devArray[devIndex - 1].ssl );
                        SSL_free ( g_devArray[devIndex - 1].ssl );
                        FD_CLR ( connFd, &readFdsMaster );
                        close ( connFd );
                        tcmMem_free ( ehDeviceTmp );
                        moduleMutexUnLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
                        continue;
                    }
#else
                    FD_SET ( connFd, &readFdsMaster );
                    /*Change maxfd value.This result in puzzle for FD_ISSET*/
                    UPDATE_MAXFD ( connFd );
                    g_devArray[devIndex - 1].connfd = connFd;
#endif
                }
                else
                    /*Just keep socket and ssl in one array if cannot find the
                      device[SMP] by mac*/
                {
                    tcmLog_notice ( "%s not exist in device list. "
                                    "Manage this socket fd %d",
                                    inet_ntoa ( cliAddress.sin_addr ),
                                    connFd );

#ifdef SSL_CONNECT
                    FD_SET ( connFd, &readFdsMaster );
                    UPDATE_MAXFD ( connFd );
                    /*just keep socket fd and SSL pointer in array, not match
                      device yet*/
                    sslArray[connFd] = SSL_new ( gctx );
                    /*arrayindex == socket fd*/
                    SSL_set_fd ( sslArray[connFd], connFd );
                    if ( SSL_accept ( sslArray[connFd] ) == -1 )
                    {
                        tcmLog_error ( "SSL accept failed, the reason is %s",
                                       strerror ( errno ) );
                        SSL_shutdown ( sslArray[connFd] );
                        SSL_free ( sslArray[connFd] );
                        FD_CLR ( connFd, &readFdsMaster );
                        close ( connFd );
                        moduleMutexUnLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
                        continue;
                    }
#else
                    FD_SET ( connFd, &readFdsMaster );
                    UPDATE_MAXFD ( connFd );
#endif

                }
                
                tcmMem_free(ehDeviceTmp);
                moduleMutexUnLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
            }
            else
            {
                /***1 step: read protocol message***/
                tcmLog_notice ( "Client socket %d has data reading", actualFd );
#ifdef SOCKET_ERROR_CHECK_SWITCH
                /*Start Error check for socket*/
                getsockopt ( actualFd, SOL_SOCKET, SO_ERROR, &error,
                             ( socklen_t * ) &errlen );
                if ( error == -1 )
                {
                    tcmLog_debug ( "actualFd != gehServerFd "
                                   "This socket %d is error type is %d "
                                   "errlen is %d, so we donot receive its data",
                                   actualFd, error, errlen );
#ifndef SSL_CONNECT
                    FD_CLR ( actualFd, &readFdsMaster );
                    close ( actualFd );
                    if ( actualFd == gehMaxFd )
                    {
                        gehMaxFd = gehMaxFd - 1;
                    }
#endif
                    continue;
                }
                /*End Error Check*/
#endif
                moduleMutexLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);

                int devIndex = getDeviceIndexByFd(actualFd);
                DeviceInfo* ehDeviceTmp = getDeviceInfoByDeviceIndex(devIndex);
                homeMessage* recvHomeMsg;

                /*deal with data without device match case*/
                if ( ehDeviceTmp != NULL ) /*this fd has match device*/
                {
                    tcmLog_debug ( "client socket %d device exist in list",
                                   actualFd );
#ifdef SSL_CONNECT
                    ret = ehSslRead ( ehDeviceTmp->ssl, &recvHomeMsg, 0 );
#else
                    ret = ehRead ( ehDeviceTmp->connfd, &recvHomeMsg, 0 );
#endif

                    if ( TCMRET_SUCCESS !=  ret )
                    {
                        if ( ret == TCMRET_DISCONNECTED )
                        {
                            tcmLog_notice ( "Client num[%s]fd[%d] from ip[%s]"
                                           "mac[%s] Disconnected",
                                           ehDeviceTmp->userName,
                                           ehDeviceTmp->connfd,
                                           ehDeviceTmp->ip, ehDeviceTmp->mac );
#ifdef SSL_CONNECT
                            SSL_free ( g_devArray[devIndex - 1].ssl );
                            FD_CLR ( actualFd, &readFdsMaster );
                            close ( actualFd );

                            g_devArray[devIndex - 1].connfd = ZERO_FD;
#else
                            FD_CLR ( actualFd, &readFdsMaster );
                            close ( actualFd );
                            if ( actualFd == gehMaxFd )
                            {
                                gehMaxFd = gehMaxFd - 1;
                            }
                            g_devArray[devIndex - 1].connfd = ZERO_FD;
                            showIndexDeviceArray();
#endif
                        }
                        else
                        {
                            /*read failed,keep connection still?*/
                            tcmLog_error ( "Socket %d from %s Read data failed",
                                           ehDeviceTmp->connfd,
                                           ehDeviceTmp->ip );
                        }

                        if ( ehDeviceTmp ->status == DEVICE_STATUS_CONNECTED )
                        {
                            updateStatusByDeviceIndex(DEVICE_STATUS_DISCONNECTED, devIndex);
                            gen_db_file();
                            UpdatePagingConfig();                            
                        }
                    }
                }
                pthread_mutex_unlock ( &g_devQueueMutex );

                if ( ehDeviceTmp != NULL )
                    if ( ret == TCMRET_DISCONNECTED )
                    {
                        pthread_mutex_lock ( &g_homeIdpTokenMutex );
                        releaseTokenByFD ( ehDeviceTmp->connfd );
                        pthread_mutex_unlock ( &g_homeIdpTokenMutex );
                        free ( ehDeviceTmp ); //FREE DEVICE HERE
                        continue; //HERE RETURN
                    }

                /*This socket fd hasnot match device yet,so we need its packet
                  header number info to match device*/
                if ( ehDeviceTmp == NULL )
                {
                    tcmLog_notice ( "This client socket %d device donot exist"
                                   " in queue", actualFd );
#ifdef SSL_CONNECT
                    ret = ehSslRead ( sslArray[actualFd], &recvHomeMsg, 0 );
#else
                    ret = ehRead ( actualFd, &recvHomeMsg, 0 );
#endif
                    if ( TCMRET_SUCCESS !=  ret )
                    {
                        if ( ret == TCMRET_DISCONNECTED )
                        {
                            tcmLog_notice ( "Client socket %d Disconnected",
                                           actualFd );
#ifdef SSL_CONNECT
                            //SSL_shutdown ( g_devArray[arrayIndexTmp].ssl );
                            /*Have not match ssl with device, just free SSL array*/
                            SSL_free ( sslArray[actualFd] );
                            FD_CLR ( actualFd, &readFdsMaster );
                            close ( actualFd );
                            if ( actualFd == gehMaxFd )
                            {
                                gehMaxFd = gehMaxFd - 1;
                            }
#else
                            FD_CLR ( actualFd, &readFdsMaster );
                            close ( actualFd );
                            if ( actualFd == gehMaxFd )
                            {
                                gehMaxFd = gehMaxFd - 1;
                            }
#endif
                            continue;
                        }
                        else
                        {
                            /*read failed,keep connection still?*/
                            tcmLog_error ( "Socket %d Read data failed",
                                           actualFd );
                        }
                        continue;
                    }
                    /*Only for match device with socket fd, Only call once*/
                    if ( TCMRET_SUCCESS == ret )
                    {
                        moduleMutexLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
                        uint8_t number;
                        getMsgNumber ( ( uint8_t * ) recvHomeMsg, &number );
                        char numberStr[20];
                        sprintf ( numberStr, "%d", number );
                        int deviceIndex = getDeviceIndexByNumber( numberStr );
                        if ( deviceIndex <= 0 )
                        {
                            tcmLog_notice ( "no number %d match in DB, "
                                            "this socket is invalid, drop it",
                                            number );
                            sendRegisterNoDeviceACK( actualFd, 
                                                     sslArray[actualFd] );
#ifdef SSL_CONNECT
                            //SSL_shutdown ( g_devArray[arrayIndexTmp].ssl );
                            SSL_free ( sslArray[actualFd] );
                            FD_CLR ( actualFd, &readFdsMaster );
                            close ( actualFd );
                            if ( actualFd == gehMaxFd )
                            {
                                gehMaxFd = gehMaxFd - 1;
                            }
#else
                            FD_CLR ( actualFd, &readFdsMaster );
                            close ( actualFd );
                            if ( actualFd == gehMaxFd )
                            {
                                gehMaxFd = gehMaxFd - 1;
                            }
#endif

                            moduleMutexUnLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
                            continue;
                        }
                        else
                        {
                            g_devArray[deviceIndex - 1].connfd = actualFd;
                            g_devArray[deviceIndex - 1].ssl = sslArray[actualFd];
                            ehDeviceTmp = getDeviceInfoByNumber ( numberStr );
                        }
                        moduleMutexUnLock(SELECT_MODULE,GDEVQUEUEMUTEX,&g_devQueueMutex);
                    }
                }

                cvtNtoHEndian ( &recvHomeMsg );

                /*2 step:filter protocol message*/
                uint16_t eventTypeTmp;
                getMsgEventType ( recvHomeMsg, &eventTypeTmp );
                if( CMD_IDP_TO_MFCB_KEEP_ALIVE != eventTypeTmp && 
                    CMD_ODP_TO_MFCB_KEEP_ALIVE != eventTypeTmp )
                   tcmLog_notice ( "Receive MSG[%d][%s] when device status[%d][%s]",
                                   eventTypeTmp,getMsgStr(eventTypeTmp),
                                   ehDeviceTmp->status,
                                   gdeviceStatusStr[ehDeviceTmp->status] );
                if ( ehDeviceTmp->status == DEVICE_STATUS_PAIRING &&
                        eventTypeTmp == CMD_IDP_TO_MFCB_REGISTER &&
                        ehDeviceTmp->pairFailedTimes < gidpPairFailedTimes )
                {
                    ;
                }
                else if ( ehDeviceTmp->status == DEVICE_STATUS_DISCONNECTED &&
                          eventTypeTmp == CMD_IDP_TO_MFCB_REGISTER &&
                          ehDeviceTmp->pairFailedTimes <
                          gidpPairFailedTimes )
                {
                    ;
                }
                else if ( ehDeviceTmp->status == DEVICE_STATUS_PAIRING &&
                          eventTypeTmp == CMD_ODP_TO_MFCB_REGISTER &&
                          ehDeviceTmp->pairFailedTimes <
                          gidpPairFailedTimes )
                {
                    ;
                }
                else if ( ehDeviceTmp->status == DEVICE_STATUS_DISCONNECTED &&
                          eventTypeTmp == CMD_ODP_TO_MFCB_REGISTER &&
                          ehDeviceTmp->pairFailedTimes <
                          gidpPairFailedTimes )
                {
                    ;
                }
                else if ( ehDeviceTmp->status == DEVICE_STATUS_PAIRING &&
                          eventTypeTmp == CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO &&
                          ehDeviceTmp->pairFailedTimes <
                          gidpPairFailedTimes )
                {
                    ;
                }
                else if ( ehDeviceTmp->status == DEVICE_STATUS_DISCONNECTED &&
                          eventTypeTmp == CMD_ODP_TO_MFCB_QUERY_ACCOUNT_INFO &&
                          ehDeviceTmp->pairFailedTimes <
                          gidpPairFailedTimes )
                {
                    ;
                }
                else if (   ( ehDeviceTmp->status == DEVICE_STATUS_CONNECTED ) && 
                            ( eventTypeTmp != CMD_IDP_TO_MFCB_REGISTER || 
                              eventTypeTmp != CMD_ODP_TO_MFCB_REGISTER ) )
                {
                    ;
                }
                else
                {
                    tcmLog_notice ( "Ignore this message" );
                    free ( ehDeviceTmp );
                    continue;
                }

                /*3 step:check first, or no need convert to CtrlBlock*/
                if ( TCMRET_SUCCESS != checkHomeMsg ( recvHomeMsg ) )
                {
                    tcmLog_notice ( "Msg is invalid, so drop it" );
                    free ( recvHomeMsg );
                    free ( ehDeviceTmp );
                    continue;
                }

                /*4 step:convert protocol message to message control block*/
                homeMsgCtrlBlock* OneMsg;
                if ( TCMRET_SUCCESS != ( ret =
                                             convertHomeMsgToCtrlBlock (
                                                 ( void * ) recvHomeMsg,
                                                 ehDeviceTmp->connfd,
                                                 ehDeviceTmp->ssl, &OneMsg ) ) )
                {
                    tcmLog_error ( "Convert protocol to message failed, "
                                   "the reason is %d", ret );
                    tcmMem_free ( recvHomeMsg );
                    tcmMem_free ( OneMsg );
                    tcmMem_free( ehDeviceTmp );
                    continue;
                }
                tcmMem_free ( recvHomeMsg );
                /*will not use it*/
                tcmMem_free ( ehDeviceTmp );

                pthread_mutex_lock ( &g_homeMsgQueueMutex );
                if ( TCMRET_SUCCESS != ( ret =
                                             putEhMsgToQueue ( &gehomeMsgQueue,
                                                               OneMsg ) ) )
                {
                    tcmLog_error ( "Put msg to queue failed,the reason is %d",
                                   ret );
                    free ( OneMsg );
                    pthread_mutex_unlock ( &g_homeMsgQueueMutex );
                    continue;
                }
                pthread_mutex_unlock ( &g_homeMsgQueueMutex );

                pthread_mutex_lock ( &remoteMsgMutex );
                pthread_cond_signal ( &remoteMsgCond );
                remoteMsgReadyFlag += 1;
                pthread_mutex_unlock ( &remoteMsgMutex );

            }
        }

    }//while (1)
}

int writeMsgThreadFun ( void* arg )
{

}

tcmRet startWriteMsgToRemoteDevice()
{
    tcmLog_debug ( "Start creating SSL write message thread\n" );
    pthread_attr_t  attr;

    if ( ( pthread_attr_init ( &attr ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_init \n" );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_attr_setschedpolicy ( &attr, SCHED_RR ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_attr_setschedpolicy \n" );
        return TCMRET_FAILURE;
    }

    if ( ( pthread_create ( &gsslTid, &attr, writeMsgThreadFun, NULL ) != 0 ) )
    {
        tcmLog_error ( "Error, pthread_create \n" );
        return TCMRET_FAILURE;
    }

    tcmLog_debug ( "Successfully create SSL write thread\n" );
    return TCMRET_SUCCESS;
}


tcmRet closeSsl()
{
    if ( gehServerFd != 0 )
    {
        close ( gehServerFd );
    }

    if ( gctx != NULL )
    {
        SSL_CTX_free ( gctx );
    }

    if ( gsslTid != 0 )
    {
        pthread_cancel ( gsslTid );
        pthread_join ( gsslTid, NULL );
    }

    return TCMRET_SUCCESS;
}
