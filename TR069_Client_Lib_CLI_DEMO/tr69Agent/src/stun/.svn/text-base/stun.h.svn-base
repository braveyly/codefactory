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
File name: stun.h
Author: Hook Guo
Date: June 10th. 2009
Description:
History:
************************************************************************/

#ifndef _STUN_H_
#define _STUN_H_
#include "hmac.h"
#include "pal.h"
#include "stun_public.h"
#include "mainloop_public.h"
#include "tr69_agent_public.h"
#include "tr69_agent.h"
#include "event_public.h"
#include "host_handler_public.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define STUN_CLIENT_PORT_2 13479

#ifndef STUN_SRV_ADDR_LEN
#define STUN_SRV_ADDR_LEN 128
#endif

#define DEFAULT_STUN_SRV_PORT 3478

typedef enum
{
    MAPPED_ADDRESS = 0x0001,
    RESPONSE_ADDRESS = 0x0002,
    CHANGE_REQUEST = 0x0003,
    SOURCE_ADDRESS = 0x0004,
    CHANGED_ADDRESS = 0x0005,
    USERNAME = 0x0006,
    PASSWORD = 0x0007,
    MESSAGE_INTEGRITY = 0x0008,
    ERROR_CODE = 0x0009,
    UNKNOWN_ATTRIBUTE = 0x000a,
    REFLECTED_FROM = 0x000b,
    CONNECTION_REQUEST_BINDING = 0XC001,
    BINDING_CHANGE = 0XC002
}StunMsgAttribType;

typedef enum
{
    BINDING_REQUEST = 0X0001,
    BINDING_RESPONSE = 0X0101,
    BINDING_ERR_RESP = 0X0111,
    SHARED_SECRET_REQ = 0X0002,
    SHARED_SECRET_RESP = 0X0102,
    SHARED_SECRET_ERR_RESP = 0X0112
}StunMsgHdrType;

#define DPRINT_LEVEL_FATAL -1
#define DPRINT_LEVEL_ALTER -2
#define DPRINT_LEVEL_INFO 0

#define STUN_TRANS_ID_SIZE 4 /* size in 4 bytes */

#define CHANGE_IP 0X04
#define CHANGE_PORT 0X02
#define CHANGE_BOTH CHANGE_IP | CHANGE_PORT
#define CHANGE_NONE 0

/* stun header */
#define STUN_HEADER_LEN 20
#define STUN_MAPPED_ADDRESS_ATTRIB_LEN 24
#define STUN_CHANAGE_ADDRESS_ATTRIB_LEN 8
#define STUN_HDR_MSG_TYPE_OFFSET 0
#define STUN_HDR_MSG_TYPE_LEN 2
#define STUN_HDR_MSG_LEN_OFFSET 2
#define STUN_HDR_MSG_LEN_LEN 2
#define STUN_HDR_TRANS_ID_OFFSET 4
#define STUN_HDR_TRANS_ID_LEN 16

/* stun attribute */
#define SA_TYPE_OFFSET 0
#define SA_LEN_OFFSET 2
#define SA_VALUE_OFFSET 4
#define SA_VALUE_PORT_OFFSET 2
#define SA_VALUE_ADDR_OFFSET 4

#define SA_TYPE_LEN 2
#define SA_LEN_LEN 2
#define SA_VALUE_PORT_LEN 2
#define SA_VALUE_ADDR_LEN 4 /* IPV4 */

#define SA_MAPPED_ADDR_VALUE_LEN 8
#define SA_CHANGE_REQ_VALUE_LEN 4
#define SA_RESP_ADDR_VALUE_LEN 8
#define SA_CONN_REQ_BINDING_VALUE_LEN 0X0014
#define SA_CONN_REQ_BINDING_VALUE "dslforum.org/TR-111 "
#define SA_CONN_REQ_BINDING_CHANGE_VALUE_LEN 0X0

#define SA_MESSAGE_INTEGRITY_VALUE_LEN 0X40


#define STUN_BINDING_REQ_RETRY_TIMES 9

/* which attribute should be append in the binding request */
#define ATTRIBUTE_NONE 0X00
#define ATTRIBUTE_NAT_QUERY 0x01
#define ATTRIBUTE_LIFETIME_QUERY 0x02
#define ATTRIBUTE_KEEP_ALIVE 0x04
#define ATTRIBUTE_INTEGRITY 0X08
#define ATTRIBUTE_BIND_CHANGE 0X10


#define BINDING_REQ_MAIN_PORT 1
#define BINDING_REQ_SND_PORT 2

typedef struct _StunEvent
{
    int eventCode;
    struct _StunEvent *nextEventPtr;
}StunEvent, *PStunEvent;

typedef struct _StunEventHeader
{
    StunEvent *eventPtr;
    int eventCount;
    PAL_MUTEX_T lock;
}StunEventHeader, *PStunEventHeader;

extern PAL_SEM_T gTr111MainLoopSem;
extern StunEventHeader gStunEventList;

/* set UDPConnectionRequestAddress directly */
#define INFORM_BINDING_INFO_WAY_1 1
/* send a binding request with optional attribute */
#define INFORM_BINDING_INFO_WAY_2 2

#define STUN_GUESSED_STR_LEN 128
#define STUN_GUESSED_REQ_LEN 128
#define STUN_GUESSED_RESP_LEN 1460

#define STUN_SHA1_HASH_SIZE 20

#define SE_SIGNAL(eventCode) \
    stun_event_list_lock(); \
    stun_event_list_push(eventCode); \
    stun_event_list_signal(); \
    stun_event_list_unlock()

#define  STUN_RECV_TIME_OUT   3000 /* mini seconds */
#define STUN_IDLE_POLLING_INTERVAL 3000 /* mini seconds */
#define STUN_EVENT_LIST_POLLING_TIMEOUT 30 /* seconds */

/* keep alive binding request should be sent at the period of
 * (gServerMappingLifetime * STUN_KEEP_ALIVE_PERIOD_FACTOR) */
#define STUN_KEEP_ALIVE_PERIOD_FACTOR 0.2


#define  STUN_CONCLUDE_ERROR                  0
#define  STUN_CONCLUDE_NAT_DETECTED   1
#define  STUN_CONCLUDE_NAT_NOT_USE     2
#define  STUN_CONCLUDE_BIND_CHANGE     3

extern Result stun_snd_binding_req(IN PAL_UINT8_T *reqDataPtr
                                   , IN PAL_INT32_T reqDataLen
                                   , IN PAL_INT32_T method);


/**************************************************************************
 Function Name: stun_client_stop
 Description: terminate the tr111 module
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
void stun_client_stop();

/**************************************************************************
 Function Name: stun_event_list_init
 Description: initiate the STUN-EVENT list
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
void stun_event_list_init();

/**************************************************************************
 Function Name: stun_event_list_push
 Description: push a new event at the top of the STUN-EVENT list
              NOTE: the stun event list follows a FIFO order
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter:
        @eventCode: stun event code that specifies an paticular stun event
**************************************************************************/
Result stun_event_list_push(PAL_INT32_T eventCode);

/**************************************************************************
 Function Name: stun_event_list_pull
 Description:
 Returns: the event code of the event at the top of the list on success, or
          STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
PAL_INT32_T stun_event_list_pull();

/**************************************************************************
 Function Name: stun_event_list_destory
 Description: destory the stun event list
              NOTE: the function should be called only in tr111 module
                    termination procedure
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
void stun_event_list_destroy();

/**************************************************************************
 Function Name: stun_event_list_wait
 Description: wait for stun event, if no comming stun event occur, the
              current thread will fall into a sleep at an semaphore
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
Result stun_event_list_wait();

/**************************************************************************
 Function Name: stun_event_list_signal
 Description: signal to the tr111 mail loop to notify that an comming event
              NOTE: sending a signal to the tr111 mail loop should follow:
               STEP-1) stun_event_list_lock();
               STEP-2) stun_event_list_push(SE_xxx);
               STEP-3) stun_event_list_signal();
               STEP-4) stun_event_list_unlock();
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
Result stun_event_list_signal();

/**************************************************************************
 Function Name: stun_event_list_lock
 Description: lock the stun event list in case of racing condition
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
Result stun_event_list_lock();

/**************************************************************************
 Function Name: stun_event_list_unlock
 Description: unlock the stun event list
 Returns: STUN_OK on success, STUN_ERR_FAILED on failure
 Parameter: none
**************************************************************************/
Result stun_event_list_unlock();

/**************************************************************************
 Function Name: stun_event_list_count
 Description: get the events count in the event list
 Returns: events count
 Parameter: none
**************************************************************************/
PAL_INT32_T stun_event_list_count();

/* tr111 stun event code */
#define SE_NONE 1 /* no event */
#define SE_DO_NAT_DISCOVERY 2 /* do NAT discovery */
#define SE_NAT_NOT_IN_USE 3 /* NAT not in use */
#define SE_DO_BINDING_LIFETIME_QUERY 4 /* do binding lifetime query */
#define SE_NAT_MAPPING_LIFETIME_DISCOVERED 5 /* NAT mapping lifetime discovered */
#define SE_NAT_MAPPING_CHANGED 6 /* NAT mapping changed */
#define SE_NAT_MAPPING_LOST 7 /* NAT mapping lost */
#define SE_UDP_CONNECTION_REQ_ARRIVED 8 /* udp connection request arrived */
#define SE_DO_STUN_CONF_CHANGE 9 /* do terminate tr111 main loop */
#define SE_DO_ACSURL_CHANGE 10 /* do terminate tr111 main loop */
#endif /* _STUN_H_ */
