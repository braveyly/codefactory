#ifndef _PAL_H_
#define _PAL_H_

#include "pal_types.h"
#include "pal_thread.h"
#include "pal_time.h"
#include "pal_msg.h"
#include "pal_mutex.h"
#include "pal_sem.h"
#include "pal_log.h"
#include "pal_file.h"

/* socket related header */
#include <windows.h>
#include <winbase.h>
#include <winsock2.h>

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* STDIN_FILEON, STDOUT_FILENO, STDERR_FILENO */
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

/* standard C */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <setjmp.h>
#include <ctype.h>
#include <time.h>

#endif /* _PAL_H_ */

