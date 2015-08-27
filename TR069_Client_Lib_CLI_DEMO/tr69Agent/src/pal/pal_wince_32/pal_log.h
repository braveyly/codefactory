#ifndef _PAL_LOG_H_
#define _PAL_LOG_H_

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include "pal_types.h"

typedef enum _LogModule
{
    LOG_SOAP = 0x0001,
    LOG_HTTP = 0x0002,
    LOG_STUN = 0x0004,
    LOG_TR069_TREE = 0x0008,
    LOG_EVENT = 0x0010,
    LOG_HOST_HANDLER = 0x0020,
    LOG_KICK_HANDLE = 0x0040,
    LOG_MAIN_LOOP = 0x0080,
    LOG_FTP_MODULE = 0x0100,
    LOG_CRQ_MODULE = 0x0200,
    /* the end */
    LOG_MODULE_END,
} LogModule;

typedef enum _LogMode
{
    LOG_DISABLE,
    LOG_CONSOLE,
    LOG_SYS_LOG,
    LOG_BOTH,
} LogMode;

typedef enum _Pal_LogLevel
{
    T_LOG_INFO,
    T_LOG_WARNING,
    T_LOG_ERROR,
    T_LOG_EMERGENCY,
    /* the end */
    T_LOG_LEVEL_END,
} Pal_LogLevel;


extern PAL_INT32_T pal_init_logger ( void );
extern PAL_INT32_T pal_set_logger ( LogModule modules, Pal_LogLevel level
                                  , LogMode mode, PAL_INT32_T size
                                  , char *moduleName );
extern PAL_INT32_T pal_logger ( Pal_LogLevel level, const char *file
                              , PAL_INT32_T  line, LogModule module
                              , const char *funcName, const char *fmt, ... );
extern void pal_exit_logger ( void );

#endif /* _PAL_LOG_H_ */
