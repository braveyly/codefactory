#ifndef _PAL_LOG_H_
#define _PAL_LOG_H_

#include "pal_types.h"
#include "pal_memory.h"
typedef enum _LogModule
{
    LOG_SOAP = 0x0001,
    LOG_HTTP = 0x0002,
    LOG_STUN = 0x0004,
    LOG_TR069_TREE = 0x0008,
    LOG_EVENT = 0x0010,
    LOG_HOST_HANDLER = 0x0020,
    LOG_KICK_HANDLER = 0x0040,
    LOG_MAIN_LOOP = 0x0080,
    LOG_FTP_MODULE = 0x0100,
    LOG_CRQ_MODULE = 0x0200,
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
    T_LOG_INFO = 3,
    T_LOG_WARNING = 4,
    T_LOG_ERROR = 6,
    T_LOG_EMERGENCY = 9,
    /* the end */
    T_LOG_LEVEL_END = 10,
} Pal_LogLevel;

#define IS_VALID_LOG_MODE(m) (((m) >= LOG_DISABLE) && ((m) <= LOG_BOTH))
#define IS_VALID_LOG_LEVEL(l) (((l) >= T_LOG_INFO) && ((l) < T_LOG_LEVEL_END))
PAL_INT32_T pal_init_logger ( void );
PAL_INT32_T pal_set_logger ( LogModule modules, Pal_LogLevel level, LogMode mode, PAL_INT32_T size, PAL_INT8_T *moduleName );
PAL_INT32_T pal_logger ( Pal_LogLevel level, const PAL_INT8_T *file, PAL_INT32_T  line, LogModule module, const PAL_INT8_T *funcName, const PAL_INT8_T *fmt, ... );
void pal_exit_logger ( void );

void pal_get_log_level(Pal_LogLevel *levelPtr);
void pal_get_log_level(Pal_LogLevel *levelPtr);
int strcasecmp (char *s, char *t);
int strncasecmp (char *s, char *t, int c);
char *strdup(const char *s);
#endif /* _PAL_LOG_H_ */
