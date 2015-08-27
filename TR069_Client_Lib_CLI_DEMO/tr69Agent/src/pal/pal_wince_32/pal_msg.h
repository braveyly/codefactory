#ifndef _PAL_MSG_H_
#define _PAL_MSG_H_

#include <windows.h>
#include <winbase.h>
#include <msgqueue.h>
#include "pal_types.h"

typedef HANDLE PAL_MSG_T;

#define MAX_PAL_RECV_MSG_LEN 8192
#define MAX_PAL_SEND_MSG_LEN 8192

#ifndef MSGQUEUE_MSGALERT
#define MSGQUEUE_MSGALERT 0x00000001
#endif

extern PAL_MSG_T pal_create_msg ( void );
extern PAL_INT32_T pal_send_msg ( PAL_MSG_T id, char *buf, PAL_INT32_T buf_len, PAL_INT32_T flags );
extern PAL_INT32_T pal_recv_msg ( PAL_MSG_T id, char *buf, PAL_INT32_T buf_len, PAL_INT32_T flags );
extern PAL_INT32_T pal_destroy_msg ( PAL_MSG_T msg );

#endif
