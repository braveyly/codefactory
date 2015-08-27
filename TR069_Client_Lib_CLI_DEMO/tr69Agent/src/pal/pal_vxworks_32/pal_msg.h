#ifndef _PAL_MSG_H_
#define _PAL_MSG_H_

#include "pal_types.h"

typedef   MSG_Q_ID PAL_MSG_T;
#define MAX_MSG_NUM 32
#define MAX_MSG_LEN 8912

PAL_MSG_T pal_create_msg (PAL_INT32_T key );
PAL_INT32_T pal_send_msg ( PAL_MSG_T id, PAL_UINT32_T eventType, PAL_INT8_T *buf, PAL_INT32_T buf_len, PAL_INT32_T flags );
PAL_INT32_T pal_recv_msg ( PAL_MSG_T id, PAL_UINT32_T *eventType, PAL_INT8_T *buf, PAL_INT32_T buf_len, PAL_INT32_T flags );
PAL_INT32_T pal_destroy_msg ( PAL_MSG_T msg );

#endif
