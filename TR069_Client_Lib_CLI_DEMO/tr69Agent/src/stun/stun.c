/**********************************************************************
Copyright (c) 2005 Wuhan Tecom Co., Ltd.
All Rights Reserved
No portions of this material may be reproduced in any form without the
written permission of:
Wuhan Tecom Co., Ltd.
#18, Huaguang Road
Wuhan, PR China 430074
All information contained in this document is Wuhan Tecom Co., Ltd.
company private, proprietary, and trade secret.
**********************************************************************
File name: stun.c
Author: Hook Guo
Date: June 10th. 2009
Description:
History:
            2009-6-17   add add_event function call to implement stun
                        notification to tr069 (Hook Guo)
************************************************************************/

#include "stun.h"
#ifdef VXWORKS
#else
jmp_buf gJmpBuf;
#endif

extern char gServerIp[IP_LENGTH];
extern int  gServerPort;
extern PAL_INT32_T udp_crq_handle_connection_request ( char *buf, int buf_len );


static ManagementServer gMNGS_back;

/* IP/Port for local usage */
static PAL_INT8_T gStunServerStrIP[INET_ADDRSTRLEN + 1];
static PAL_INT32_T gStunServerPort;

static struct in_addr gStunServerIP;
static PAL_INT8_T gMappedStunServeStrIP[INET_ADDRSTRLEN + 1];
static struct in_addr gMappedStunServeIP;
static PAL_UINT16_T gMappedStunServePort;

/* NAT detect retry intervals */
static PAL_INT32_T gRetryIntervals[STUN_BINDING_REQ_RETRY_TIMES] = { 0, 1000, 2000, 4000, 8000, 16000, 16000, 16000, 16000 };

static PAL_INT32_T gStunMappingLifetime = 0; /* 0 means not checked */
static PAL_INT32_T gStunKeepAlivetime = 0; /* 0 means not checked */
/* life time query binary search upper bound */
PAL_INT32_T gUpperBSearchLifetime = 120;  /*seconds*/


/* child thread handler */
static PAL_THREAD_T gNatDetectTask = 0;
static PAL_THREAD_T gLifetimeQueryTask = 0;
static PAL_THREAD_T gMainPortListenAdapterTask = 0;
static PAL_THREAD_T gMappingMaintainTask = 0;
static PAL_THREAD_T gStunIdleTask = 0;
static PAL_THREAD_T gStunClientTask = 0;

/* UDPConnectionRequestAddress for local usage */
static PAL_INT8_T _UDPConnectionRequestAddress[256];

/* local function declaration */
static void stun_send_bind_change ( void );
static void stun_build_binding_req ( PAL_UINT8_T*, PAL_INT32_T *, PAL_INT32_T, PAL_UINT8_T* );
static Result stun_recv_binding_resp ( PAL_UINT8_T*, PAL_INT32_T*, PAL_INT32_T );
Result stun_snd_binding_req ( PAL_UINT8_T *, PAL_INT32_T, PAL_INT32_T );
static Result stun_handle_udp_conn_req ( PAL_UINT8_T *, PAL_INT32_T );
static Result stun_handle_binding_resp ( PAL_UINT8_T *, PAL_INT32_T );
static Result __handle_binding_resp ( IN PAL_UINT8_T *dataPtr, IN PAL_INT32_T dataLen );
static Result __handle_binding_err_resp ( IN PAL_UINT8_T *dataPtr, IN PAL_INT32_T dataLen );
static void __handle_shared_secret_resp ( PAL_UINT8_T *, PAL_INT32_T );
static void __handle_shared_secret_err_resp ( PAL_UINT8_T *, PAL_INT32_T );
static Result stun_handle_stun_event ( PAL_INT32_T );
static void* stun_binding_mapping_maintain_task();
static void* stun_nat_detect_task ( void * );
static void stun_inform_binding_info ( IN PAL_INT32_T way );
static Result stun_inform_nat_detection_info();
static Result stun_main_loop();
static void* stun_main_port_listening_adapter_task ( void * );
static void* stun_binding_lifetime_query_task ( void * );
static PAL_INT32_T stun_is_trans_id_matched ( PAL_UINT32_T *, PAL_UINT32_T * );
static PAL_INT32_T __msleep ( PAL_INT32_T );
static void __string2hex_common ( PAL_UINT8_T *, PAL_UINT8_T *, PAL_INT32_T );
static void __add_event ( PAL_INT32_T, char*, void *, PAL_INT32_T );
static PAL_INT32_T stun_do_msg_interchange ( PAL_UINT8_T *sndBufPtr
        , PAL_INT32_T sndBufLen
        , PAL_UINT8_T *recvBufPtr
        , PAL_INT32_T *recvBufLenPtr );

static PAL_INT32_T stun_conclude_result();
static void stun_inform_bind_change();
static void stun_set_natfail_udpconnectionaddress();

/*****************************************************************************
 Function Name : stun_build_binding_req
 Description : build binding request packet according to method parameter
 Returns : none
 Parameter : @dataPtr : memory passed in to build packet with
             @dataLenPtr : packet length with header to pass out
             @method : method used to build the packet
                       --ATTRIBUTE_NONE : no attribute
                       --ATTRIBUTE_NAT_QUERY : build change-request and
                       connection-request-binding attributes
                       --ATTRIBUTE_LIFETIME_QUERY : build change-request and
                       response-address attributes
                       --ATTRIBUTE_KEEP_ALIVE : build change-request and
                       connection-request-binding attributes
                       --ATTRIBUTE_INTEGRITY : build message-integrity
                       attribute
******************************************************************************/
static void stun_build_binding_req ( OUT PAL_UINT8_T *dataPtr
                                     , OUT PAL_INT32_T *dataLenPtr
                                     , IN PAL_INT32_T method
                                     , IN PAL_UINT8_T *transid )
{
    PAL_UINT32_T transID[STUN_TRANS_ID_SIZE];
    PAL_INT32_T i;
    PAL_INT32_T currentPos = 0;
    PAL_INT32_T msgLenWithoutHdr;
    PAL_INT32_T usernamelen;


    /* generate transaction id */
    for ( i = 0; i < STUN_TRANS_ID_SIZE; i++ )
    {
        transID[i] = rand();
    }

    if ( transid )
    {
        memcpy ( transID, transid, STUN_TRANS_ID_SIZE );
    }

    /* build message header */
    * ( ( PAL_UINT16_T * ) &dataPtr[STUN_HDR_MSG_TYPE_OFFSET] )
    = htons ( BINDING_REQUEST );


    for ( i = 0; i < STUN_TRANS_ID_SIZE; i++ )
    {
        PAL_UINT32_T *tmpPtr =
            ( PAL_UINT32_T * ) & dataPtr[STUN_HDR_TRANS_ID_OFFSET
                                         + i * sizeof ( PAL_UINT32_T ) ];
        *tmpPtr = transID[i];
    }

    currentPos = STUN_HEADER_LEN;

    /* decide which attribute should be appended */
    if ( method & ATTRIBUTE_INTEGRITY )
    {
        goto integrity;
    }

    if ( method & ATTRIBUTE_LIFETIME_QUERY )
    {
        goto lifetime_query;
    }
    else if ( method & ATTRIBUTE_NAT_QUERY )
    {
        goto nat_query;
    }
    else if ( method & ATTRIBUTE_KEEP_ALIVE )
    {
        goto nat_query;
    }
    else if ( method & ATTRIBUTE_BIND_CHANGE )
    {
        goto bind_change;
    }



nat_query:

    /* build CONNECTION-REQUEST-BINDING attribute */
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( CONNECTION_REQUEST_BINDING );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( SA_CONN_REQ_BINDING_VALUE_LEN );

    memcpy ( &dataPtr[currentPos + SA_VALUE_OFFSET], SA_CONN_REQ_BINDING_VALUE,
             SA_CONN_REQ_BINDING_VALUE_LEN );
    /* update current position */
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + SA_CONN_REQ_BINDING_VALUE_LEN;

    /* build USERNAME attribute */
    usernamelen = ( ( strlen ( gManagementServer.stunUsername ) + 3 ) / 4 ) * 4;
    if ( !usernamelen )
        goto complete;

    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( USERNAME );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( usernamelen );
    memcpy ( &dataPtr[currentPos + SA_VALUE_OFFSET], gManagementServer.stunUsername,
             strlen ( gManagementServer.stunUsername ) );

    /* update current position */
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + usernamelen;

    goto complete;


bind_change:

    /* build CONNECTION-REQUEST-BINDING attribute */
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] )
    = htons ( CONNECTION_REQUEST_BINDING );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] )
    = htons ( SA_CONN_REQ_BINDING_VALUE_LEN );

    memcpy ( &dataPtr[currentPos + SA_VALUE_OFFSET], SA_CONN_REQ_BINDING_VALUE,
             SA_CONN_REQ_BINDING_VALUE_LEN );

    /* update current position */
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + SA_CONN_REQ_BINDING_VALUE_LEN;


    /* build BINDING_CHANGE attribute */
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( BINDING_CHANGE );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( SA_CONN_REQ_BINDING_CHANGE_VALUE_LEN );
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + SA_CONN_REQ_BINDING_CHANGE_VALUE_LEN;


    /* build USERNAME attribute */
    usernamelen = ( ( strlen ( gManagementServer.stunUsername ) + 3 ) / 4 ) * 4;
    if ( !usernamelen )
        goto complete;

    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( USERNAME );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( usernamelen );
    memcpy ( &dataPtr[currentPos + SA_VALUE_OFFSET], gManagementServer.stunUsername,
             strlen ( gManagementServer.stunUsername ) );

    /* update current position */
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + usernamelen;


lifetime_query:
    /* build RESPONSE-ADDRESS attribute */
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( RESPONSE_ADDRESS );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( SA_RESP_ADDR_VALUE_LEN );

    * ( ( PAL_UINT8_T * ) &dataPtr[currentPos + SA_VALUE_OFFSET + 1] ) = 0X01;
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_VALUE_OFFSET + 2] ) = htons ( gMappedStunServePort );
    inet_aton ( gMappedStunServeStrIP, &gMappedStunServeIP );
    * ( ( PAL_UINT32_T * ) &dataPtr[currentPos + SA_VALUE_OFFSET + 4] ) = gMappedStunServeIP.s_addr;

    currentPos += SA_TYPE_LEN + SA_LEN_LEN + SA_RESP_ADDR_VALUE_LEN;
    goto complete;


integrity:
    /* build USERNAME attribute if STUNUsername parameter exists */
    /* build CONNECTION-REQUEST-BINDING attribute */
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( CONNECTION_REQUEST_BINDING );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( SA_CONN_REQ_BINDING_VALUE_LEN );

    memcpy ( &dataPtr[currentPos + SA_VALUE_OFFSET], SA_CONN_REQ_BINDING_VALUE, SA_CONN_REQ_BINDING_VALUE_LEN );
    /* update current position */
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + SA_CONN_REQ_BINDING_VALUE_LEN;


    /* build BINDING_CHANGE attribute */
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( BINDING_CHANGE );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( SA_CONN_REQ_BINDING_CHANGE_VALUE_LEN );
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + SA_CONN_REQ_BINDING_CHANGE_VALUE_LEN;


    /* build USERNAME attribute */
    usernamelen = ( ( strlen ( gManagementServer.stunUsername ) + 3 ) / 4 ) * 4;

    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] ) = htons ( USERNAME );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] ) = htons ( usernamelen );
    memcpy ( &dataPtr[currentPos + SA_VALUE_OFFSET], gManagementServer.stunUsername,
             strlen ( gManagementServer.stunUsername ) );
    /* update current position */
    currentPos += SA_TYPE_LEN + SA_LEN_LEN + usernamelen;


    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_TYPE_OFFSET] )
    = htons ( MESSAGE_INTEGRITY );
    * ( ( PAL_UINT16_T * ) &dataPtr[currentPos + SA_LEN_OFFSET] )
    = htons ( SA_MESSAGE_INTEGRITY_VALUE_LEN );

    /*need to do HMAC to create integrity value*/
    /*first compute msglen*/
    * ( ( PAL_UINT16_T * ) &dataPtr[STUN_HDR_MSG_LEN_OFFSET] ) =
        currentPos + SA_TYPE_LEN + SA_LEN_LEN + SHA1_LEN - STUN_HEADER_LEN;


    hmac_sha1 ( gManagementServer.stunPassword, dataPtr,
                currentPos, ( PAL_UINT8_T * ) &dataPtr[currentPos + SA_TYPE_LEN + SA_LEN_LEN] );

    currentPos += SA_TYPE_LEN + SA_LEN_LEN + SHA1_LEN;


complete:
    *dataLenPtr = currentPos; /* position starts from 0 */

    /* payload length */
    msgLenWithoutHdr = *dataLenPtr - STUN_HEADER_LEN;
    * ( ( PAL_UINT16_T * ) &dataPtr[STUN_HDR_MSG_LEN_OFFSET] ) = htons ( msgLenWithoutHdr );
}

/*****************************************************************************
 Function Name : stun_recv_binding_resp
 Description : receive binding response on IP/Port specfied by gStunClientIP
               and gStunClientPort
 Returns : OK on receiving binding response successfully, STUN_ERR_FAILED
           on fatal error, STUN_ERR_TIMEOUT on timeout and
           STUN_ERR_SOCKET_ERROR on socket error
 Parameter : @respDataPtr : memory which to store the binding response packet
             @respDataLenPtr : length of the binding response packet with
             header
             @timeout : timeout value of the pending time
******************************************************************************/
static Result stun_recv_binding_resp ( OUT PAL_UINT8_T *respDataPtr
                                       , OUT PAL_INT32_T *respDataLenPtr
                                       , IN PAL_INT32_T timeout )
{
    PAL_INT32_T udpSockFd;
    fd_set rset;
    PAL_INT32_T retval;
    PAL_INT32_T optval = 1;
    struct sockaddr_in stunCliAddr;
    struct timeval tv;
    char stunCliIP[INET_ADDRSTRLEN + 1];

    if ( timeout < 0 )
    {
        return STUN_ERR_FAILED;
    }

    if ( ( udpSockFd = socket ( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        return STUN_ERR_SOCKET_ERROR;
    }

    if ( setsockopt ( udpSockFd, SOL_SOCKET, SO_REUSEADDR
                      , ( PAL_INT8_T * ) &optval, sizeof ( optval ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        close ( udpSockFd );
        return STUN_ERR_SOCKET_ERROR;
    }

    memset ( &stunCliAddr, 0, sizeof ( struct sockaddr ) );
    stunCliAddr.sin_family = AF_INET;
    stunCliAddr.sin_port = htons ( CPE_CRQ_UDP_PORT );
    /* read WAN IP each time in case of IP change */
    if ( gCB.get_wan_ipaddress ( stunCliIP ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "failed to get wan ip address\n" );
        return STUN_ERR_FAILED;
    }

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "receiving binding response from %s:%d\n", stunCliIP, CPE_CRQ_UDP_PORT );

    inet_aton ( stunCliIP, &stunCliAddr.sin_addr );
    if ( bind ( udpSockFd, ( struct sockaddr * ) &stunCliAddr
                , sizeof ( struct sockaddr ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        close ( udpSockFd );
        return STUN_ERR_SOCKET_ERROR;
    }

    FD_ZERO ( &rset );
    FD_SET ( udpSockFd, &rset );

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout % 1000;

    if ( ( retval = select ( udpSockFd + 1, &rset, NULL, NULL, &tv ) ) == 0 )
    {
        /* timeout */
        close ( udpSockFd );
        return STUN_ERR_TIMEOUT;
    }
    else if ( retval > 0 )
    {
        /* udpSockFd becomes readable */
        PAL_UINT32_T addrLen = sizeof ( struct sockaddr );
        PAL_INT32_T nbytes;

        if ( ( nbytes = recvfrom ( udpSockFd
                                   , respDataPtr, *respDataLenPtr, 0
                                   , ( struct sockaddr * ) & stunCliAddr, &addrLen ) ) == -1 )
        {
            close ( udpSockFd );
            return STUN_ERR_SOCKET_ERROR;
        }
        else
        {
            *respDataLenPtr = nbytes;

            close ( udpSockFd );
            return TCMAGENT_OK;
        }
    }
    else if ( retval == -1 )
    {
        /* error occur */
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        close ( udpSockFd );
        return STUN_ERR_FAILED;
    }

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : stun_snd_binding_req
 Description : send binding request according to the method parameter specfied
 Returns : STUN_ERR_SOCKET_ERROR on socket error, or OK on success
 Parameter : @reqDataPtr : binding request packet to send
             @reqDataLen : binding request packet length with header
             @method : method to send. BINDING_REQ_MAIN_PORT means send the
             binding request upon the main port the tr069 specified,
             BINDING_REQ_SND_PORT means send the binding request upon the port
             different from the main port
******************************************************************************/
Result stun_snd_binding_req ( IN PAL_UINT8_T *reqDataPtr
                              , IN PAL_INT32_T reqDataLen
                              , IN PAL_INT32_T method )
{
    PAL_INT32_T stunSockFd;
    PAL_INT32_T optval;
    struct sockaddr_in stunServerAddr;
    struct sockaddr_in stunClientAddr;
    PAL_UINT16_T portToUse;
    char stunCliIP[INET_ADDRSTRLEN + 1];

    if ( ( stunSockFd = socket ( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        return STUN_ERR_SOCKET_ERROR;
    }

    /* set the ip/port pair reusable */
    optval = 1;
    if ( setsockopt ( stunSockFd, SOL_SOCKET, SO_REUSEADDR, ( PAL_INT8_T * ) &optval
                      , sizeof ( optval ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        close ( stunSockFd );
        return STUN_ERR_SOCKET_ERROR;
    }

    /* set STUN server sockaddr */
    memset ( &stunServerAddr, 0, sizeof ( struct sockaddr ) );
    stunServerAddr.sin_family = AF_INET;
    stunServerAddr.sin_port = htons ( gStunServerPort );
    inet_aton ( gStunServerStrIP, &stunServerAddr.sin_addr );

    /* set STUN client sockaddr */
    portToUse = ( method == BINDING_REQ_MAIN_PORT )
                ? CPE_CRQ_UDP_PORT : STUN_CLIENT_PORT_2;
    memset ( &stunClientAddr, 0, sizeof ( struct sockaddr ) );
    stunClientAddr.sin_family = AF_INET;
    stunClientAddr.sin_port = htons ( portToUse );
    if ( gCB.get_wan_ipaddress ( stunCliIP ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "failed to get wan ip address\n" );
        return STUN_ERR_FAILED;
    }
    inet_aton ( stunCliIP, &stunClientAddr.sin_addr );

    /* bind the stunClientAddr to stunSockFd */
    if ( bind ( stunSockFd, ( struct sockaddr * ) &stunClientAddr
                , sizeof ( struct sockaddr ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        return STUN_ERR_SOCKET_ERROR;
    }

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "sending binding request to %s:%d\n"
                 , gStunServerStrIP, gStunServerPort );

    if ( sendto ( stunSockFd, reqDataPtr, reqDataLen , MSG_NOSIGNAL
                  , ( struct sockaddr * ) &stunServerAddr, sizeof ( struct sockaddr ) ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );

        close ( stunSockFd );
        return STUN_ERR_SOCKET_ERROR;
    }
    else
    {
        close ( stunSockFd );
        return TCMAGENT_OK;
    }
}



/*****************************************************************************
 Function Name : stun_handle_udp_conn_resp
 Description : handle the udp connection request, authenticate the udp
               connection request which is in an http/1.1 packet without
               payload, send a SE_UDP_CONNECTION_REQ_ARRIVED signal to the main
               loop if the authentication passed.
 Returns : OK on authentication passed, STUN_ERR_FAILED on failure
 Parameter : @respDataPtr : memory to store the udp connection request packet
             @respDataLen : length of udp connection request packet
******************************************************************************/
static Result stun_handle_udp_conn_req ( IN PAL_UINT8_T *respDataPtr
        , IN PAL_INT32_T respDataLen )
{
    /* FIXME: this function DOES NOT provide full authentication */

    PAL_INT32_T i;
    PAL_INT8_T *httpReqLinePtr;
    PAL_INT8_T *httpReqURIPtr;
    PAL_INT32_T authStage1 = FALSE;
    PAL_INT32_T authStage2 = FALSE;

    if ( strlen ( gManagementServer.stunUsername ) == 0 )
    {
        return STUN_ERR_UNAUTH;
    }

    for ( i = 0; i < respDataLen; i++ )
    {
        if ( ( * ( ( PAL_INT8_T * ) &respDataPtr[i] ) == '\r' )
                && ( * ( PAL_INT8_T * ) &respDataPtr[i+1] == '\n' ) )
        {
            /* end of HTTP/1.1 Request-Line */
            httpReqLinePtr =
                ( PAL_INT8_T * ) PAL_MALLOC ( ( i + 1 ) * sizeof ( PAL_INT8_T ) );
            if ( httpReqLinePtr == NULL )
            {
                return STUN_ERR_FAILED;
            }

            memcpy ( httpReqLinePtr, respDataPtr, i );
            httpReqLinePtr[i] = '\0';
            /* NOTE: STAGE-1 is to retrieve the HTTP/1.1 Request-Line */
            authStage1 = TRUE;
            break;
        }
    }

    if ( authStage1 )
    {
        /* NOTE: STAGE-2 is to check the Request-URI */
        char *tmpPtr1, *tmpPtr2;
        char *ctx;
        PAL_INT32_T ctxLen;
        PAL_INT32_T httpReqURILen;
        PAL_INT32_T i;
        char ts[STUN_GUESSED_STR_LEN];
        char id[STUN_GUESSED_STR_LEN];
        char un[STUN_GUESSED_STR_LEN];
        char cn[STUN_GUESSED_STR_LEN];
        char sig[STUN_GUESSED_STR_LEN];
        PAL_UINT8_T sha1HashLocal[STUN_SHA1_HASH_SIZE];
        PAL_UINT8_T sha1HashHexLocal[STUN_SHA1_HASH_SIZE * 2];
        PAL_UINT8_T sha1HashHex[STUN_SHA1_HASH_SIZE * 2];
        char *paramNameArray[5] = { "ts="
                                    , "id="
                                    , "un="
                                    , "cn="
                                    , "sig="
                                  };
        char *paramValueArray[5] = {ts, id, un, cn, sig};

        /* get Request-URI portion */
        tmpPtr1 = strchr ( httpReqLinePtr, ' ' );
        tmpPtr2 = strchr ( tmpPtr1 + 1, ' ' );
        httpReqURILen = tmpPtr2 - tmpPtr1;

        httpReqURIPtr = ( PAL_INT8_T * ) PAL_MALLOC ( httpReqURILen * sizeof ( PAL_INT8_T ) );
        if ( httpReqURIPtr == NULL )
        {
            PAL_FREE ( httpReqLinePtr );
            httpReqLinePtr = NULL;

            return STUN_ERR_FAILED;
        }
        memcpy ( httpReqURIPtr, tmpPtr1 + 1, httpReqURILen - 1 );
        httpReqURIPtr[httpReqURILen] = '\0';

        /* parse the Request-URL */
        for ( i = 0; i < 5; i++ )
        {
            tmpPtr1 = strstr ( httpReqURIPtr, paramNameArray[i] );
            if ( tmpPtr1 == NULL )
            {
                return STUN_ERR_FAILED;
            }

            tmpPtr2 = strchr ( tmpPtr1, '&' );
            if ( tmpPtr2 == NULL )
            {
                strcpy ( paramValueArray[i]
                         , tmpPtr1 + sizeof ( paramNameArray[i] ) );
            }
            else
            {
                PAL_INT32_T len = tmpPtr2 - tmpPtr1
                                  - sizeof ( paramNameArray[i] );

                memcpy ( paramValueArray[i]
                         , tmpPtr1 + sizeof ( paramNameArray[i] ), len );
                paramValueArray[i][len] = '\0';
            }
        }

        memcpy ( sha1HashHex, sig, STUN_SHA1_HASH_SIZE * 2 );

        /* compute local HMAC-SHA1 */
        ctxLen = strlen ( ts ) + strlen ( id ) + strlen ( un ) + strlen ( cn );
        ctx = ( PAL_INT8_T * ) PAL_MALLOC ( ( ctxLen + 1 ) * sizeof ( PAL_INT8_T ) );
        if ( ctx == NULL )
        {
            PAL_FREE ( httpReqLinePtr );
            httpReqLinePtr = NULL;
            PAL_FREE ( httpReqURIPtr );
            httpReqURIPtr = NULL;

            return STUN_ERR_FAILED;
        }
        ctx = strcat ( ts, id );
        ctx = strcat ( ctx, un );
        ctx = strcat ( ctx, cn );

        //hmac_sha1(gManagementServer.stunUsername
        //          , strlen(gManagementServer.stunUsername)
        //          , ctx, ctxLen, (char *)sha1HashLocal, STUN_SHA1_HASH_SIZE);

        PAL_FREE ( ctx );
        ctx = NULL;

        __string2hex_common ( sha1HashHexLocal, sha1HashLocal, STUN_SHA1_HASH_SIZE );

        if ( !memcmp ( sha1HashHexLocal, sha1HashHex, STUN_SHA1_HASH_SIZE * 2 ) )
        {
            authStage2 = TRUE;
        }
    } /* end of if (authStage1) */

    PAL_FREE ( httpReqLinePtr );
    PAL_FREE ( httpReqURIPtr );
    httpReqLinePtr = NULL;
    httpReqURIPtr = NULL;

    /* do judgement */
    if ( authStage1 && authStage2 )
    {
        SE_SIGNAL ( SE_UDP_CONNECTION_REQ_ARRIVED );
        return TCMAGENT_OK;
    }
    else
    {
        return STUN_ERR_UNAUTH;
    }
}

/*****************************************************************************
 Function Name : stun_handle_binding_resp
 Description : hanle the binding response packet
 Returns : OK on success, or STUN_ERR_FAILED on fatal error
 Parameter : @respDataPtr : memory to store the binding response packet
             @respDataLen : lenght of binding response packet with header
******************************************************************************/
static Result stun_handle_binding_resp ( IN PAL_UINT8_T *respDataPtr
        , IN PAL_INT32_T respDataLen )
{
    PAL_UINT16_T msgType;
    PAL_UINT16_T msgLength;
    Result   ret = TCMAGENT_OK;

    if ( respDataPtr == NULL && respDataLen < STUN_HEADER_LEN )
    {
        return STUN_ERR_FAILED;
    }

    msgType = * ( ( PAL_UINT16_T * ) & respDataPtr[STUN_HDR_MSG_TYPE_OFFSET] );
    msgLength = * ( ( PAL_UINT16_T * ) & respDataPtr[STUN_HDR_MSG_LEN_OFFSET] );
    msgType = ntohs ( msgType );
    msgLength = ntohs ( msgLength );

    switch ( msgType )
    {
        case BINDING_RESPONSE:

            ret = __handle_binding_resp ( &respDataPtr[STUN_HEADER_LEN]
                                          , msgLength );
            break;

        case BINDING_ERR_RESP:
            ret = __handle_binding_err_resp ( &respDataPtr[STUN_HEADER_LEN]
                                              , msgLength );
            break;
        case SHARED_SECRET_RESP:
            __handle_shared_secret_resp ( NULL, 0 );
            ret = TCMAGENT_OK;
            break;
        case SHARED_SECRET_ERR_RESP:
            __handle_shared_secret_err_resp ( NULL, 0 );
            ret = STUN_ERR_NEED_AUTH;
            break;
        default:
            ret = STUN_ERR_FAILED;

    }

    return ret;
}

static Result __handle_binding_resp ( IN PAL_UINT8_T *dataPtr
                                      , IN PAL_INT32_T dataLen )
{
    PAL_UINT8_T *iterator = dataPtr;
    PAL_INT32_T currentPos = 0;
    PAL_INT32_T attribType;
    PAL_INT32_T attribLen;

    while ( currentPos < dataLen )
    {
        attribType = * ( ( PAL_UINT16_T * ) & iterator[currentPos + SA_TYPE_OFFSET] );
        attribType = ntohs ( attribType );

        attribLen = * ( ( PAL_UINT16_T * ) & iterator[currentPos + SA_LEN_OFFSET] );
        attribLen = ntohs ( attribLen );

        currentPos += SA_TYPE_LEN + SA_LEN_LEN;

        if ( attribType == MAPPED_ADDRESS )
        {
            PAL_UINT16_T port;
            unsigned long ipaddr;
            PAL_INT8_T *ipstraddr;
            struct in_addr tmpaddr;
            //PAL_TIME_T _tick;

            port = * ( ( PAL_UINT16_T * ) & iterator[currentPos + SA_VALUE_PORT_OFFSET] );
            port = ntohs ( port );

            ipaddr = * ( ( PAL_UINT32_T * ) & iterator[currentPos
                         + SA_VALUE_ADDR_OFFSET] );

            tmpaddr.s_addr = ipaddr;
            ipstraddr = inet_ntoa ( tmpaddr );

            strcpy ( gMappedStunServeStrIP, ipstraddr );
            gMappedStunServePort = port;

            return TCMAGENT_OK;

        } /* end of if (attribType == MAPPED_ADDRESS) */

        currentPos += attribLen;
    }

    return STUN_ERR_FAILED;

}

static Result __handle_binding_err_resp ( IN PAL_UINT8_T *dataPtr
        , IN PAL_INT32_T dataLen )
{
    PAL_INT32_T class_value, number_value, error_code;

    if ( dataLen < 4 )
    {
        return STUN_ERR_FAILED;
    }


    //    0   1  2  3  4   5  6  7   8  9  0  1  2  3   4   5  6  7  8  9  0   1  2  3  4   5  6  7  8  9   0  1
    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //  |                   0                                                              |Class   | Number                  |
    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


    class_value =  * ( PAL_INT32_T * ) dataPtr & 0x700 >> 8;
    number_value = * ( PAL_INT32_T * ) dataPtr & 0xFF;

    if ( class_value > 0 && class_value < 7
            && number_value >= 0 && number_value <= 99 )
    {
        error_code = class_value * 100 + number_value;
    }
    else
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "class %d,  number %d\n", class_value, number_value );

        return STUN_ERR_FAILED;
    }

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "error code %d: %s\n", error_code, &dataPtr[4] );
    if ( error_code == 401 )
        return STUN_ERR_NEED_AUTH;
    else
        return STUN_ERR_FAILED;
}

/* not implemented */
static void __handle_shared_secret_resp ( IN PAL_UINT8_T *dataPtr
        , IN PAL_INT32_T dataLen )
{

    /* TODO: ?? */
}

/* not implemented */
static void __handle_shared_secret_err_resp ( IN PAL_UINT8_T *dataPtr
        , IN PAL_INT32_T dataLen )
{
    /* TODO: ?? */
}

/* get stun server address */
static Result stun_address_get()
{
    unsigned long resolvedIP;
    struct in_addr tmpaddr;
    PAL_INT32_T retryTimes = 0;
    struct hostent * hePtr = NULL;


    /* STEP-1: acquire STUN server address */
    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "acquire STUN server address\n" );


    if ( strlen ( gManagementServer.stunServerAddress ) && gManagementServer.stunServerPort > 0 )
    {
        if ( inet_aton ( gManagementServer.stunServerAddress
                         , &gStunServerIP ) == 0 )
        {
resolve:
            /* need to be resolved */
            hePtr = gethostbyname ( gManagementServer.stunServerAddress );
            if ( hePtr != NULL )
            {

                resolvedIP = * ( unsigned long * ) ( hePtr->h_addr_list[0] );
                tmpaddr.s_addr = resolvedIP; /* network order */
                strcpy ( gStunServerStrIP, inet_ntoa ( tmpaddr ) );
                gStunServerPort = gManagementServer.stunServerPort;
            }
            else
            {
#ifdef VXWORKS
#else
                if ( h_errno == HOST_NOT_FOUND )
                {
                    /* TODO: inform tr069 and terminate */
                    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                                 , "host not found\n" );
                    return STUN_ERR_FAILED;
                }
                else if ( h_errno == TRY_AGAIN )
                {
                    __msleep ( 500 );
                    if ( retryTimes++ < 9 )
                    {
                        goto resolve;
                    }
                    else
                    {
                        /* TODO: inform tr069 and terminate */
                        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                                     , "maximum 'TRY AGAIN' time achieved\n" );
                        return STUN_ERR_FAILED;
                    }
                }
                else if ( h_errno == NO_RECOVERY )
                {
                    /* TODO: inform tr069 and terminate */
                    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                                 , "no recovery\n" );
                    return STUN_ERR_FAILED;
                }
                else if ( h_errno == NO_DATA )
                {
                    /* TODO: inform tr069 and terminate */
                    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                                 , "no data\n" );
                    return STUN_ERR_FAILED;
                }

                pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                             , "%s\n", hstrerror ( h_errno ) );
#endif
                return STUN_ERR_FAILED;
            }
        }
        else
        {
            /* ip address already */
            memcpy ( gStunServerStrIP, gManagementServer.stunServerAddress
                     , strlen ( gManagementServer.stunServerAddress ) );

            gStunServerPort = gManagementServer.stunServerPort;
        }
    }
    else
    {
        /* TODO: acquire the stun server address and port from ACS URL */
        if ( !strlen ( gServerIp ) )
            return STUN_ERR_FAILED;


        memcpy ( gStunServerStrIP, gServerIp
                 , strlen ( gServerIp ) );
        gStunServerPort = gServerPort;

    }


    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "gStunServerStrIP = %s, gStunServerPort=%d\n", gStunServerStrIP, gStunServerPort );

    return TCMAGENT_OK;

}


/*****************************************************************************
 Function Name : stun_handle_stun_event
 Description : handle stun event which can either be signaled by tr111 static
               function module or other tr069 component
 Returns : OK on success, or STUN_ERR_FAILED on failure
 Parameter : @eventCode: event code that notify the type of the signal event
******************************************************************************/
static Result stun_handle_stun_event ( IN PAL_INT32_T eventCode )
{
    PAL_INT32_T retval;
    PAL_THREAD_ATTR_T a1, a2, a3, a4, a5;
    a1.priority = 125;
    a1.stackSize = 10 * 1000;
    strcpy ( a1.threadName, "stun_port_listen" );

    a2.priority = 125;
    a2.stackSize = 10 * 1000;
    strcpy ( a2.threadName, "stun_maping" );

    a3.priority = 125;
    a3.stackSize = 10 * 1000;
    strcpy ( a3.threadName, "stun_life_time_quering" );

    a4.priority = 125;
    a4.stackSize = 5 * 1000;
    strcpy ( a4.threadName, "stun_idle" );

    a5.priority = 125;
    a5.stackSize = 5 * 1000;
    strcpy ( a5.threadName, "stun_nat_detect" );


    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "stun_handle_stun_event: %d\n", eventCode );


    if ( eventCode == SE_DO_NAT_DISCOVERY )
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "(stun event): SE_DO_NAT_DISCOVERY\n" );

        stun_client_stop();
        stun_event_list_clean();

#ifdef VXWORKS
        retval = pal_thread_create ( &gNatDetectTask
                                     , &stun_nat_detect_task
                                     , &a5
                                     , NULL );
        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }

#else
        retval = pal_thread_create ( &gNatDetectTask
                                     , &stun_nat_detect_task
                                     , NULL
                                     , NULL );
        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }
#endif

    }
    else if ( eventCode == SE_NAT_NOT_IN_USE )
    {
        /* TODO: poll for the status change of local ip/port, then
         * redo nat detection routine */
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "(stun event): SE_NAT_NOT_IN_USE\n" );


        stun_set_natfail_udpconnectionaddress();
        stun_inform_binding_info ( INFORM_BINDING_INFO_WAY_1 );
        stun_inform_nat_detection_info ( FALSE );

        // __add_event(SN_TYPE_NAT_NOT_IN_USE, SN_NAME_NAT_NOT_IN_USE, NULL, 0);
        return TCMAGENT_OK;
    }
    else if ( eventCode == SE_DO_BINDING_LIFETIME_QUERY )
    {
        /* TODO: make tr069 known NAT in use */
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "(stun event): SE_DO_BINDING_LIFETIME_QUERY\n" );

#ifdef VXWORKS
        pal_thread_create ( &gLifetimeQueryTask
                            , &stun_binding_lifetime_query_task
                            , &a3
                            , NULL );
#else
        pal_thread_create ( &gLifetimeQueryTask
                            , &stun_binding_lifetime_query_task
                            , NULL
                            , NULL );
#endif

        return TCMAGENT_OK;
    }
    else if ( eventCode == SE_NAT_MAPPING_LIFETIME_DISCOVERED )
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "(stun event): SE_NAT_MAPPING_LIFETIME_DISCOVERED\n" );

        stun_inform_nat_detection_info ( TRUE );
        stun_inform_binding_info ( INFORM_BINDING_INFO_WAY_1 );

        //__add_event(SN_TYPE_UDP_CONN_REQ_ADDR_CHANGED, SN_NAME_UDP_CONN_REQ_ADDR_CHANGED, NULL, 0);

        stun_inform_binding_info ( INFORM_BINDING_INFO_WAY_2 );

        /* launch a child thread to listen on gStunClientIP/gStunClientPort */
#ifdef VXWORKS
        retval = pal_thread_create ( &gMainPortListenAdapterTask
                                     , &stun_main_port_listening_adapter_task
                                     , &a1
                                     , NULL );

        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }

        /* launch a child thread to maintain the binding mapping */
        retval = pal_thread_create ( &gMappingMaintainTask
                                     , &stun_binding_mapping_maintain_task
                                     , &a2
                                     , NULL );

        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }
#else
        /* launch a child thread to listen on gStunClientIP/gStunClientPort */
        retval = pal_thread_create ( &gMainPortListenAdapterTask
                                     , &stun_main_port_listening_adapter_task
                                     , NULL
                                     , NULL );

        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }

        /* launch a child thread to maintain the binding mapping */
        retval = pal_thread_create ( &gMappingMaintainTask
                                     , &stun_binding_mapping_maintain_task
                                     , NULL
                                     , NULL );

        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }
#endif
        return TCMAGENT_OK;
    }
    else if ( eventCode == SE_NAT_MAPPING_CHANGED )
    {
        /* FIXME: this stun event seems unusable because when mapping
         * changed, a SE_NAT_MAPPING_LOST event will be generated, which
         * will repeat the nat_detect, lifetime_query, and mapping_maintain
         * process */

        stun_inform_binding_info ( INFORM_BINDING_INFO_WAY_1 );
        //__add_event(SN_TYPE_UDP_CONN_REQ_ADDR_CHANGED, SN_NAME_UDP_CONN_REQ_ADDR_CHANGED, NULL, 0);

    }
    else if ( eventCode == SE_NAT_MAPPING_LOST )
    {
        /* SE_NAT_MAPPING_LOST stun event can either be genarated
         * because of receiving binding response timeout or
         * gStunClientIP/gStunClientPort changed */
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "(stun event): SE_NAT_MAPPING_LOST\n" );

        stun_client_stop();
        stun_event_list_clean();

        stun_set_natfail_udpconnectionaddress();

        stun_inform_nat_detection_info ( FALSE );
        stun_inform_binding_info ( INFORM_BINDING_INFO_WAY_1 );

        //__add_event(SN_TYPE_NAT_NOT_IN_USE, SN_NAME_NAT_NOT_IN_USE, NULL, 0);

#ifdef VXWORKS
        retval = pal_thread_create ( &gNatDetectTask
                                     , &stun_nat_detect_task
                                     , &a5
                                     , NULL );
        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }

#else
        retval = pal_thread_create ( &gNatDetectTask
                                     , &stun_nat_detect_task
                                     , NULL
                                     , NULL );
        if ( retval == -1 )
        {
            return STUN_ERR_FAILED;
        }
#endif
    }
    else if ( eventCode == SE_UDP_CONNECTION_REQ_ARRIVED )
    {
        /* TODO: make tr069 known the udp connection request arrived */
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "(stun event): SE_UDP_CONNECTION_REQ_ARRIVED\n" );

        __add_event ( SN_TYPE_UDP_CONN_REQ_ARRIVED, SN_NAME_UDP_CONN_REQ_ARRIVED, NULL, 0 );

        return TCMAGENT_OK;
    }

    return STUN_ERR_FAILED;
}

/*****************************************************************************
 Function Name : stun_nat_detect
 Description : probe whether there is NAT in use between CPE and ACS, this
               function is first launched at the time when the address of
               stun server is retrieved, it can be relaunched when mapping lost
               in order to build a new mapping
 Returns : OK on success, or STUN_ERR_FAILED on failure
 Parameter : none
******************************************************************************/
static void* stun_nat_detect_task ( IN void *argPtr )
{
    PAL_INT32_T reqDataLen; /* guessed big enough */
    PAL_UINT8_T reqDataPtr[STUN_GUESSED_REQ_LEN];
    PAL_INT32_T respDataLen;
    PAL_UINT8_T respDataPtr[STUN_GUESSED_RESP_LEN];
    PAL_INT32_T sleep_time[9] = {6, 60, 120, 240, 480, 960, 1920, 3840, 7200};
    PAL_INT32_T retriedTimes = 0;
    PAL_INT32_T _timeout;
    PAL_INT32_T retval, i;
    Result  ret;


    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "stun_nat_detect_task launched\n" );

    i = 0;

retry_address:
    if ( stun_address_get() != TCMAGENT_OK )
    {
        sleep ( sleep_time[i] );
        if ( ++i == 9 )
        {
            i = 0;
        }
        goto retry_address;
    }

    while ( 1 )
    {

        if ( retriedTimes >= STUN_BINDING_REQ_RETRY_TIMES )
            retriedTimes = STUN_BINDING_REQ_RETRY_TIMES - 1;

        _timeout = gRetryIntervals[retriedTimes];

        memset ( reqDataPtr, 0, STUN_GUESSED_REQ_LEN );
        memset ( respDataPtr, 0, STUN_GUESSED_RESP_LEN );
        reqDataLen = STUN_GUESSED_REQ_LEN;
        respDataLen = STUN_GUESSED_RESP_LEN;

        __msleep ( _timeout );
        stun_build_binding_req ( reqDataPtr
                                 , &reqDataLen, ATTRIBUTE_NAT_QUERY, NULL );

        ret = stun_snd_binding_req ( reqDataPtr, reqDataLen
                                     , BINDING_REQ_MAIN_PORT );
        if ( ret != TCMAGENT_OK )
        {
            continue;
        }

        ret = stun_recv_binding_resp ( respDataPtr
                                       , &respDataLen, STUN_RECV_TIME_OUT );

        if ( ret == TCMAGENT_OK )
        {

            /* check the equality of transaction id */
            if ( ( respDataLen > STUN_HEADER_LEN ) && ( stun_is_trans_id_matched (
                        ( PAL_UINT32_T * ) &reqDataPtr[STUN_HDR_TRANS_ID_OFFSET]
                        , ( PAL_UINT32_T * ) &respDataPtr[STUN_HDR_TRANS_ID_OFFSET] ) == TRUE ) )
            {

                ret = stun_handle_binding_resp ( respDataPtr, respDataLen );
                if ( ret == TCMAGENT_OK )
                {
                    retval = stun_conclude_result();
                    if ( retval == STUN_CONCLUDE_NAT_DETECTED
                            || retval == STUN_CONCLUDE_BIND_CHANGE )
                    {
                        SE_SIGNAL ( SE_DO_BINDING_LIFETIME_QUERY );

                    }
                    else if ( retval == STUN_CONCLUDE_NAT_NOT_USE )
                    {
                        SE_SIGNAL ( SE_NAT_NOT_IN_USE );

                    }
                    else
                    {
                        retriedTimes++;
                        continue;
                    }
                    break;
                }
                else
                {
                    retriedTimes++;
                    continue;
                }
            }
            else
            {
                retriedTimes++;
                continue;
            }
        }
        else
        {
            retriedTimes++;
            continue;
        }
    }

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "stun_nat_detect_task exit\n" );

    return NULL;
}




static PAL_INT32_T stun_conclude_result()
{
    char stunCliIP[INET_ADDRSTRLEN + 1];
    char newudpconnectionaddress[256];
    PAL_INT32_T  ret;


    memset ( stunCliIP, 0, sizeof ( stunCliIP ) );
    memset ( newudpconnectionaddress, 0, sizeof ( newudpconnectionaddress ) );


    if ( gCB.get_wan_ipaddress ( stunCliIP ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "failed to get wan ip address\n" );
        return STUN_CONCLUDE_ERROR;
    }


    if ( ( strcmp ( stunCliIP, gMappedStunServeStrIP ) != 0 ) )
    {
        snprintf ( newudpconnectionaddress
                   , 256
                   , "%s:%d"
                   , gMappedStunServeStrIP
                   , gMappedStunServePort );

        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "UDPConnectionRequestAddress: %s\n"
                     , newudpconnectionaddress );

        if ( strcmp ( newudpconnectionaddress, _UDPConnectionRequestAddress ) )
        {
            strcpy ( _UDPConnectionRequestAddress, newudpconnectionaddress );
            ret = STUN_CONCLUDE_BIND_CHANGE;
        }
        else
            ret = STUN_CONCLUDE_NAT_DETECTED;


    }
    else
    {
        /* no address translation is in use */
        snprintf ( _UDPConnectionRequestAddress
                   , 256
                   , "%s:%d"
                   , gMappedStunServeStrIP
                   , gMappedStunServePort );


        ret = STUN_CONCLUDE_NAT_NOT_USE;
    }

    return ret;

}


/*****************************************************************************
 Function Name : stun_inform_nat_detection_info
 Description : inform the NAT detection to the ACS
 Returns : OK on success, or STUN_ERR_FAILED on failure
******************************************************************************/
static Result stun_inform_nat_detection_info ( PAL_BOOL_T nat_detect_flag )
{
    PAL_UINT8_T sndBufPtr[MAX_TCM_SEND_MSG_LEN];
    PAL_INT32_T sndBufLen;
    PAL_UINT8_T recvBufPtr[MAX_TCM_SEND_MSG_LEN];
    PAL_INT32_T recvBufLen = MAX_TCM_SEND_MSG_LEN;
    MsgType msgType = SET_VALUE_EX;
    MsgValueType msgValueType = MSG_BOOL;
    char *msgName = "InternetGatewayDevice.ManagementServer.NATDetected";
    PAL_INT32_T msgNameLen;
    PAL_BOOL_T msgValue = nat_detect_flag;
    PAL_INT32_T msgValueLen;
    PAL_INT32_T ret;

    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                 , "tell the truth of NAT detected\n" );

    /* set variables in message */
    msgNameLen = strlen ( msgName ) + 1;
    msgValueLen = sizeof ( PAL_BOOL_T );

    /* build message */
    assemble_msg ( ( char * ) sndBufPtr /* buffer */
                   , &sndBufLen /* buffer length */
                   , msgType /* message type */
                   , msgName /* message name */
                   , msgValueType /* message value type */
                   , ( void * ) &msgValue /* message value */
                   , msgValueLen /* message value len */ );

    ret = stun_do_msg_interchange ( sndBufPtr, sndBufLen, recvBufPtr, &recvBufLen );
    if ( ret != TCMAGENT_OK )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "function stun_do_msg_interchange failed\n" );
        return STUN_ERR_FAILED;
    }

    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : stun_inform_binding_info
 Description : inform the UDPConnectionRequestAddress to the ACS
 Returns : OK on success, or STUN_ERR_FAILED on failure
 Parameter : @way: method used to notify, INFORM_BINDING_INFO_WAY_1 means set
             UDPConncetionRequestAddress directly, INFORM_BINDING_INFO_WAY_2
             means use a binding request with optional attributes to inform the
             ACS, see TR-111 specification for more detail
******************************************************************************/
static void stun_inform_binding_info ( IN PAL_INT32_T way )
{
    if ( way == INFORM_BINDING_INFO_WAY_1 )
    {
        stun_inform_bind_change();
    }
    else if ( way == INFORM_BINDING_INFO_WAY_2 )
    {
        /* TODO: sending binding request with optional attribute */
        stun_send_bind_change();
    }

}

static void stun_inform_bind_change()
{
    /* UDPConnectionRequestAddress has been changed, send a message to
    * host_handler to make the mainloop know the change, so as to
    * send an inform packet to acs server */
    PAL_UINT8_T sndBufPtr[MAX_TCM_SEND_MSG_LEN];
    PAL_INT32_T sndBufLen;
    PAL_UINT8_T recvBufPtr[MAX_TCM_SEND_MSG_LEN];
    PAL_INT32_T recvBufLen = MAX_TCM_SEND_MSG_LEN;
    MsgType msgType = SET_VALUE_EX;
    MsgValueType msgValueType = MSG_STRING;
    char *msgName = "InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress";
    PAL_INT32_T msgNameLen;
    void *msgValue = NULL;
    PAL_INT32_T msgValueLen;
    PAL_INT32_T ret;

    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                 , "tell the truth of UDPConnectionRequestAddress changed\n" );

    /* set variables in message */
    msgNameLen = strlen ( msgName ) + 1;
    msgValue = ( void * ) _UDPConnectionRequestAddress;
    msgValueLen = strlen ( _UDPConnectionRequestAddress ) + 1;

    /* build message */
    assemble_msg ( ( char * ) sndBufPtr /* buffer */
                   , &sndBufLen /* buffer length */
                   , msgType /* message type */
                   , msgName /* message name */
                   , msgValueType /* message value type */
                   , msgValue /* message value */
                   , msgValueLen /* message value len */ );

    ret = stun_do_msg_interchange ( sndBufPtr, sndBufLen, recvBufPtr, &recvBufLen );
    if ( ret != TCMAGENT_OK )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "function stun_do_msg_interchange failed\n" );
        return ;
    }

    return ;
}


static void stun_set_natfail_udpconnectionaddress()
{
    char stunCliIP[INET_ADDRSTRLEN + 1];

    memset ( stunCliIP, 0, sizeof ( stunCliIP ) );

    if ( gCB.get_wan_ipaddress ( stunCliIP ) == -1 )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "failed to get wan ip address\n" );
        return ;
    }

    snprintf ( _UDPConnectionRequestAddress, 256, "%s:%d", stunCliIP, CPE_CRQ_UDP_PORT );
    return ;
}


/*****************************************************************************
 Function Name : stun_main_loop
 Description : tr111 module main loop
 Returns : OK if loop ends peacefully, or STUN_ERR_FAILED on failure
 Parameter : none
******************************************************************************/
static Result stun_main_loop()
{
    PAL_INT32_T eventCode;

    while ( TRUE )
    {
        /* wait for event to come */
        stun_event_list_wait();
        stun_event_list_lock();
        eventCode = stun_event_list_pull();
        stun_event_list_unlock();

        if ( eventCode == SE_NONE )
        {
            /* TODO: ?? */
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                         , "(stun event): SE_NONE\n" );
        }
        else if ( eventCode == SE_DO_STUN_CONF_CHANGE )
        {
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                         , "(stun event): SE_DO_TERM_MAIN_LOOG\n" );

            stun_client_stop();
            stun_event_list_clean();

            if ( gManagementServer.stunEnable == TRUE )
            {
                stun_handle_stun_event ( SE_DO_NAT_DISCOVERY );
            }
        }
        else if ( eventCode == SE_DO_ACSURL_CHANGE )
        {
            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                         , "(stun event): SE_DO_TERM_MAIN_LOOG\n" );

            if ( gManagementServer.stunEnable == TRUE &&
                    ( !strlen ( gManagementServer.stunServerAddress ) || !gManagementServer.stunServerPort ) )
            {
                stun_handle_stun_event ( SE_DO_NAT_DISCOVERY );
            }
        }
        else
        {
            stun_handle_stun_event ( eventCode );
        }

        /* check stunEnable variable */
    }

    stun_client_stop();
    stun_event_list_clean();
    return TCMAGENT_OK;
}

void *stun_client_task ( void *param )
{

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "stun client mainloop thread launched, stun enable %d\n", gManagementServer.stunEnable );

    /* initiate STUN EVENT LIST */
    stun_event_list_init();

    gMNGS_back = gManagementServer;

    SE_SIGNAL ( SE_DO_STUN_CONF_CHANGE );

    /* STEP-3: Launch tr111 main loop, NOTE: the tr111 main loop wouldn't
     * be invoked untill a binding response is received, when binding response
     * is received, the handle_binding_resp routine will find out whether
     * NAT is in use, anyway, a signal will send to the tr111 main loop the
     * inform the event  */
    stun_main_loop();

    return NULL;
}

/*****************************************************************************
 Function Name : stun_client_start
 Description : start tr111 stun module
 Returns : return OK on succeed, or STUN_ERR_FAILED on failure
 Parameter : none
******************************************************************************/
Result stun_client_start()
{
#ifdef VXWORKS
    PAL_THREAD_ATTR_T a;
    a.priority = 125;
    a.stackSize = 1000 * 5;
    strcpy ( a.threadName, "stun client" );
    if ( pal_thread_create ( &gStunClientTask, &stun_client_task, &a, NULL ) != 0 )
    {
        return STUN_ERR_FAILED;
    }

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "stun client started\n" );
#else
    //pal_set_logger(LOG_STUN, T_LOG_INFO, LOG_CONSOLE, 0, NULL);

    if ( pal_thread_create ( &gStunClientTask, &stun_client_task, NULL, NULL ) != 0 )
    {
        return STUN_ERR_FAILED;
    }
#endif
    return TCMAGENT_OK;
}

/*****************************************************************************
 Function Name : stun_wakeup
 Description : wakeup tr111 stun module
 Returns :none
 Parameter : none
******************************************************************************/
void stun_wakeup()
{
    if ( gMNGS_back.stunEnable != gManagementServer.stunEnable
            || strcmp ( gMNGS_back.stunServerAddress, gManagementServer.stunServerAddress )
            || gMNGS_back.stunServerPort != gManagementServer.stunServerPort )
    {
        SE_SIGNAL ( SE_DO_STUN_CONF_CHANGE );
    }
    else if ( strcmp ( gMNGS_back.acsURL, gManagementServer.acsURL ) )
    {
        SE_SIGNAL ( SE_DO_ACSURL_CHANGE );
    }

    gMNGS_back = gManagementServer;
}


/*****************************************************************************
 Function Name : stun_client_stop
 Description : stop tr111 stun module
 Returns : none
 Parameter : none
******************************************************************************/
void stun_client_stop()
{
    pal_thread_cancel ( &gNatDetectTask );
    pal_thread_cancel ( &gLifetimeQueryTask );
    pal_thread_cancel ( &gMainPortListenAdapterTask );
    pal_thread_cancel ( &gMappingMaintainTask );
    pal_thread_cancel ( &gStunIdleTask );
    pal_thread_cancel ( &gStunClientTask );

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "stun client stopped\n" );
}

/*****************************************************************************
 Function Name : stun_main_port_listening_adapter_task
 Description : thread used to listen on the gStunClientIP and gStunClientPort,
               it stands like an adapter between udp connection request and
               binding response which would use the same destination ip and port
 Returns : NULL
 Parameter : NULL
******************************************************************************/
static void* stun_main_port_listening_adapter_task ( IN void *argPtr )
{
    PAL_INT32_T timeout = gStunKeepAlivetime * 1100;/*1.1*1000*/
    PAL_INT32_T retval;
    PAL_UINT8_T respDataPtr[STUN_GUESSED_RESP_LEN];
    PAL_INT32_T respDataLen;
    PAL_UINT8_T firstByte;


    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "port listening adapter task start, timeout value: %d\n", timeout );


    while ( TRUE )
    {
        respDataLen = STUN_GUESSED_RESP_LEN;
        memset ( respDataPtr, 0, sizeof ( respDataPtr ) );
        retval = stun_recv_binding_resp ( respDataPtr
                                          , &respDataLen, timeout );

        if ( ( retval == STUN_ERR_SOCKET_ERROR ) || ( retval == STUN_ERR_FAILED ) )
        {
            SE_SIGNAL ( SE_NAT_MAPPING_LOST );
            goto end;
        }
        else if ( retval == STUN_ERR_TIMEOUT )
        {
            /* timeout may means the local address has been changed
             * so tr111 should rediscovery the mapping */
            SE_SIGNAL ( SE_NAT_MAPPING_LOST );
            goto end;
        }
        else if ( retval == TCMAGENT_OK )
        {
            firstByte = respDataPtr[0];

            if ( firstByte == 1 || firstByte == 0 )
            {
                pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                             , "binding response arrived\n" );

                retval = stun_handle_binding_resp ( respDataPtr, respDataLen );
                if ( retval == TCMAGENT_OK )
                {
                    retval = stun_conclude_result();
                    if ( retval == STUN_CONCLUDE_BIND_CHANGE )
                    {
                        SE_SIGNAL ( SE_NAT_MAPPING_CHANGED );
                        stun_inform_binding_info ( INFORM_BINDING_INFO_WAY_2 );
                    }
                    else if ( retval == STUN_CONCLUDE_NAT_NOT_USE )
                    {
                        SE_SIGNAL ( SE_NAT_NOT_IN_USE );

                    }
                    else
                    {
                        continue;
                    }
                }
            }
            else
            {
                /* FIXME: i deem it as an ascii char */
                pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                             , "udp connection request arrived\n" );

                retval = udp_crq_handle_connection_request ( respDataPtr, respDataLen );
                if ( retval == TCMAGENT_OK )
                {
                    SE_SIGNAL ( SE_UDP_CONNECTION_REQ_ARRIVED );
                }

            }

        }
    }

end:

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "port listening adapter task terminated\n" );

    return NULL;
}

/*****************************************************************************
 Function Name : stun_binding_lifetime_query_task
 Description : thread used to find the binding lifetime
 Returns : NULL
 Parameter : NULL
******************************************************************************/
static void* stun_binding_lifetime_query_task ( IN void *argPtr )
{
    PAL_INT32_T retval;
    PAL_INT32_T lowerBSearchLifetime = 1;
    PAL_INT32_T upperBSearchLifetime = gUpperBSearchLifetime;
    PAL_INT32_T binarySearchLifetime =
        ( lowerBSearchLifetime + upperBSearchLifetime ) / 2;
    PAL_INT32_T lifetime = binarySearchLifetime;
    PAL_UINT8_T reqDataPtr[STUN_GUESSED_REQ_LEN];
    PAL_INT32_T reqDataLen;
    PAL_UINT8_T respDataPtr[STUN_GUESSED_RESP_LEN];
    PAL_INT32_T respDataLen;

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "binding life time query task started\n" );
    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "Try lifetime: %d\n", lifetime );


    while ( TRUE )
    {
        __msleep ( lifetime * 1000 );

        memset ( reqDataPtr, 0, sizeof ( reqDataPtr ) );
        memset ( respDataPtr, 0, sizeof ( respDataPtr ) );
        reqDataLen  = sizeof ( reqDataPtr );
        respDataLen = sizeof ( respDataPtr );

        stun_build_binding_req ( reqDataPtr
                                 , &reqDataLen, ATTRIBUTE_LIFETIME_QUERY, NULL );

        retval = stun_snd_binding_req ( reqDataPtr
                                        , reqDataLen
                                        , BINDING_REQ_SND_PORT );
        if ( retval != TCMAGENT_OK )
        {
            SE_SIGNAL ( SE_NAT_MAPPING_LOST );
            goto end;
        }

        retval = stun_recv_binding_resp ( respDataPtr
                                          , &respDataLen, STUN_RECV_TIME_OUT );

        if ( retval == TCMAGENT_OK )
        {
            /* binding response received */
            lowerBSearchLifetime = lifetime + 1;
            lifetime = ( lowerBSearchLifetime + upperBSearchLifetime ) / 2;

            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                         , "Try lifetime: %d\n", lifetime );
        }
        else if ( retval == STUN_ERR_TIMEOUT )
        {
            /* timeout */
            PAL_INT32_T retval2;

            if ( ( ( lifetime - lowerBSearchLifetime ) <= 10 )
                    || ( lifetime <= ( lowerBSearchLifetime + 1 ) ) )
            {
                upperBSearchLifetime = lifetime - 1;
                lifetime = lowerBSearchLifetime;
            }
            else
            {
                upperBSearchLifetime = lifetime - 1;
                lifetime = ( upperBSearchLifetime + lowerBSearchLifetime ) / 2;
            }

            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                         , "binding response timeout. Try lifetime: %d\n"
                         , lifetime );

            /* TODO: a new NAT mapping should be set */
            memset ( reqDataPtr, 0, sizeof ( reqDataPtr ) );
            memset ( respDataPtr, 0, sizeof ( respDataPtr ) );
            reqDataLen  = sizeof ( reqDataPtr );
            respDataLen = sizeof ( respDataPtr );

            stun_build_binding_req ( reqDataPtr
                                     , &reqDataLen
                                     , ATTRIBUTE_NAT_QUERY, NULL );
            retval2 = stun_snd_binding_req ( reqDataPtr
                                             , reqDataLen
                                             , BINDING_REQ_MAIN_PORT );
            if ( retval2 != TCMAGENT_OK )
            {
                SE_SIGNAL ( SE_NAT_MAPPING_LOST );
                goto end;
            }

            retval2 = stun_recv_binding_resp ( respDataPtr
                                               , &respDataLen
                                               , STUN_RECV_TIME_OUT );
            if ( retval2 != TCMAGENT_OK )
            {
                /* SOCKET failture or TIMEOUT failure */
                SE_SIGNAL ( SE_NAT_MAPPING_LOST );
                goto end;
            }

            retval2 = stun_handle_binding_resp ( respDataPtr, respDataLen );
            if ( retval2 == TCMAGENT_OK )
            {
                continue;
            }
            else
            {
                SE_SIGNAL ( SE_NAT_MAPPING_LOST );
                goto end;
            }
            /* new NAT mapping is setup, go on the lifetime query task */
        }
        else
        {
            SE_SIGNAL ( SE_NAT_MAPPING_LOST );
            /* failure */
            goto end;
        }

        if ( ( ( upperBSearchLifetime - lifetime ) <= 10 )
                || ( upperBSearchLifetime <= ( lifetime + 1 ) ) )
        {
            /* lifetime query complete. inform the tr111
             * main loop that mapping lifetime retrieved.
             * NOTE: if upperBSearchLifetime - lowerBSearchLifetime
             * is less than 1 seconds, it is better to deem that
             * the lifetime is found, although the value of lifetime
             * may not be as acurate as possible, but more efficiency
             * can be retrieved */
            gStunMappingLifetime = lifetime;

            if ( gManagementServer.stunMinKeepAlivePeriod &&
                    gManagementServer.stunMaxKeepAlivePeriod )
            {
                gStunKeepAlivetime = gStunMappingLifetime > gManagementServer.stunMinKeepAlivePeriod
                                     ? gStunMappingLifetime : gManagementServer.stunMinKeepAlivePeriod;
                gStunKeepAlivetime = gStunMappingLifetime < gManagementServer.stunMaxKeepAlivePeriod
                                     ? gStunMappingLifetime : gManagementServer.stunMaxKeepAlivePeriod;
            }
            else
            {
                gStunKeepAlivetime = gStunMappingLifetime;
            }

            pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                         , "lifetime retrieved, lifetime: %d\n", lifetime );

            SE_SIGNAL ( SE_NAT_MAPPING_LIFETIME_DISCOVERED );
            goto end;
        }
    } /* end of while (TRUE) */

end:

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "lifetime query task terminated\n" );

    return NULL;
}

/*****************************************************************************
 Function Name : stun_binding_mapping_maintain_task
 Description : thread function used to maintain the mapping, it will send a
               binding request to the stun server periodicaly with the time
               limit no more than binding lifetime
 Returns : NULL
 Parameter : NULL
******************************************************************************/
static void* stun_binding_mapping_maintain_task ( IN void *argPtr )
{
    PAL_INT32_T retval;
    PAL_INT32_T keepAlivePeriod;
    PAL_UINT8_T reqDataPtr[STUN_GUESSED_REQ_LEN];
    PAL_INT32_T reqDataLen;

    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "binding mapping maintain task start\n" );


    keepAlivePeriod = gStunKeepAlivetime;

    while ( TRUE )
    {
        __msleep ( keepAlivePeriod * 1000 );
        memset ( reqDataPtr, 0, sizeof ( reqDataPtr ) );
        reqDataLen = STUN_GUESSED_REQ_LEN;
        stun_build_binding_req ( reqDataPtr
                                 , &reqDataLen
                                 , ATTRIBUTE_KEEP_ALIVE, NULL );

        retval = stun_snd_binding_req ( reqDataPtr
                                        , reqDataLen, BINDING_REQ_MAIN_PORT );
        if ( retval != TCMAGENT_OK )
        {
            goto end;
        }

        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "keep alive message sent\n" );
    }

end:

    /*mapping maintain end, restart to do nat detect*/
    SE_SIGNAL ( SE_NAT_MAPPING_LOST );
    pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                 , "mapping maintain task terminated\n" );
    return NULL;
}



static void stun_send_bind_change ( void )
{
    Result            retval;
    PAL_UINT8_T reqDataPtr[STUN_GUESSED_REQ_LEN];
    PAL_INT32_T reqDataLen;
    PAL_UINT8_T respDataPtr[STUN_GUESSED_RESP_LEN];
    PAL_INT32_T respDataLen;
    PAL_INT32_T  need_auth_flag = 0;
    PAL_UINT8_T  store_transcton_id[STUN_HDR_TRANS_ID_LEN+1];

    pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                 , "enter\n" );
    memset ( store_transcton_id, 0, sizeof ( store_transcton_id ) );

need_auth:

    memset ( reqDataPtr, 0, sizeof ( reqDataPtr ) );
    memset ( respDataPtr, 0, sizeof ( respDataPtr ) );
    reqDataLen = STUN_GUESSED_REQ_LEN;
    respDataLen = STUN_GUESSED_RESP_LEN;


    if ( need_auth_flag )
    {
        stun_build_binding_req ( reqDataPtr
                                 , &reqDataLen,  ATTRIBUTE_INTEGRITY, store_transcton_id );
    }
    else
        stun_build_binding_req ( reqDataPtr
                                 , &reqDataLen, ATTRIBUTE_BIND_CHANGE, NULL );


    retval = stun_snd_binding_req ( reqDataPtr
                                    , reqDataLen
                                    , BINDING_REQ_MAIN_PORT );
    if ( retval != TCMAGENT_OK )
    {
        goto end;
    }

    retval = stun_recv_binding_resp ( respDataPtr
                                      , &respDataLen, STUN_RECV_TIME_OUT );

    if ( retval == TCMAGENT_OK )
    {
        if ( ( respDataLen > STUN_HEADER_LEN ) && ( stun_is_trans_id_matched (
                    ( PAL_UINT32_T * ) &reqDataPtr[STUN_HDR_TRANS_ID_OFFSET]
                    , ( PAL_UINT32_T * ) &respDataPtr[STUN_HDR_TRANS_ID_OFFSET] ) == TRUE ) )
        {

            retval = stun_handle_binding_resp ( respDataPtr, respDataLen );

            if ( retval == STUN_ERR_NEED_AUTH )
            {
                memcpy ( store_transcton_id, &respDataPtr[STUN_HDR_TRANS_ID_OFFSET], STUN_HDR_TRANS_ID_LEN );

                need_auth_flag = 1;
                goto  need_auth;
            }
            else if ( retval == TCMAGENT_OK )
            {
                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                             , "inform bind change to stun server success\n" );
            }
            else
            {

                pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                             , "inform bind change to stun server failed\n" );

            }
        }
    }

end:

    return ;
}


PAL_INT32_T stun_is_trans_id_matched ( PAL_UINT32_T *reqTransIDPtr
                                       , PAL_UINT32_T *respTransIDPtr )
{
    PAL_INT32_T i;

    for ( i = 0; i < STUN_TRANS_ID_SIZE; i++ )
    {
        if ( *reqTransIDPtr != *respTransIDPtr )
        {
            /* transaction id dismatched */
            break;
        }

        reqTransIDPtr++;
        respTransIDPtr++;
    }

    if ( i == STUN_TRANS_ID_SIZE )
    {
        /* transaction id matched */
        return TRUE;
    }
    else
    {
        /* transaction id dismatched */
        return FALSE;
    }
}


static PAL_INT32_T stun_do_msg_interchange ( PAL_UINT8_T *sndBufPtr
        , PAL_INT32_T sndBufLen
        , PAL_UINT8_T *recvBufPtr
        , PAL_INT32_T *recvBufLenPtr )
{
    Result ret;
    Module srcModule;
    PAL_INT32_T nbytes;

    if ( ( ret = msg_send ( HOST_HANDLER, TCMAGENT_STUN, ( char * ) sndBufPtr, sndBufLen ) ) < 0 )
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );
        return STUN_ERR_FAILED;
    }

    nbytes = msg_recv ( TCMAGENT_STUN, &srcModule, ( char * ) recvBufPtr, *recvBufLenPtr );
    *recvBufLenPtr = nbytes;

    if ( nbytes < 0 || srcModule != HOST_HANDLER )
    {
        pal_logger ( T_LOG_INFO, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );
        return STUN_ERR_FAILED;
    }

    return TCMAGENT_OK;
}

static PAL_INT32_T __msleep ( IN PAL_INT32_T msec )
{
    PAL_INT32_T retval;
    struct timeval tv;

    tv.tv_sec = msec / 1000;
    tv.tv_usec = msec % 1000;

    retval = select ( STDOUT_FILENO + 1, NULL, NULL, NULL, &tv );
    if ( retval == 0 )
    {
        /* timeout */
        return TCMAGENT_OK;
    }
    else if ( retval == -1 )
    {
        return STUN_ERR_FAILED;
    }

    return TCMAGENT_OK;
}

static void __string2hex_common ( IN PAL_UINT8_T *bin
                                  , OUT PAL_UINT8_T *hex
                                  , IN PAL_INT32_T hashLen )
{
    PAL_UINT16_T i;
    PAL_UINT8_T j;

    for ( i = 0; i < hashLen; i++ )
    {
        j = ( bin[i] >> 4 ) & 0xf;

        if ( j <= 9 )
        {
            hex[i*2] = ( j + '0' );
        }
        else
        {
            hex[i*2] = ( j + 'a' - 10 );
        }

        j = bin[i] & 0xf;

        if ( j <= 9 )
        {
            hex[i*2+1] = ( j + '0' );
        }
        else
        {
            hex[i*2+1] = ( j + 'a' - 10 );
        }
    }

    hex[hashLen * 2] = '\0';
}

static void __add_event ( PAL_INT32_T type, char *name, void *data, PAL_INT32_T dataLen )
{
    StunNotifyArg *eventArgPtr = NULL;

    eventArgPtr = ( StunNotifyArg * ) PAL_MALLOC ( sizeof ( StunNotifyArg ) );
    if ( eventArgPtr == NULL )
    {
        pal_logger ( T_LOG_ERROR, __FILE__, __LINE__, LOG_STUN, __func__
                     , "%s\n", strerror ( errno ) );
        return;
    }
    eventArgPtr->notificationType = type;
    eventArgPtr->data = data;
    eventArgPtr->dataLen = dataLen;
    add_event ( NOW, STUN_NOTIFY, name, eventArgPtr );
}
