#ifndef _PAL_MSG_H_
#define _PAL_MSG_H_

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "pal_types.h"
#include "../../tr69_agent.h"

typedef PAL_INT32_T PAL_MSG_T;

#define MAX_PAL_RECV_MSG_LEN MAX_SEND_MSG_LEN
#define MAX_PAL_SEND_MSG_LEN MAX_RECV_MSG_LEN
#define MAX_MSG_LEN (MAX_PAL_RECV_MSG_LEN > MAX_PAL_SEND_MSG_LEN ? MAX_PAL_RECV_MSG_LEN : MAX_PAL_SEND_MSG_LEN)

extern PAL_MSG_T pal_create_msg ( PAL_INT32_T key );
extern PAL_INT32_T pal_send_msg ( PAL_MSG_T id, PAL_UINT32_T eventType, char *buf, PAL_SIZE_T buf_len, PAL_INT32_T flags );
extern PAL_INT32_T pal_recv_msg ( PAL_MSG_T id, PAL_UINT32_T *eventType, char *buf, PAL_SIZE_T buf_len, PAL_INT32_T flags );
extern PAL_INT32_T pal_destroy_msg ( PAL_MSG_T msg );

#endif

