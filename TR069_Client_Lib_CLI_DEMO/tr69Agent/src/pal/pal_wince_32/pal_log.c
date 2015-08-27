#include "pal_log.h"

static Pal_LogLevel logLevel;
static LogMode logMode;
static LogModule logModule;

static char *moduleName[LOG_MODULE_END];
static char *levelLable[T_LOG_LEVEL_END];

PAL_INT32_T pal_init_logger ( void )
{
    PAL_INT32_T i;
    for ( i = 0; i < LOG_MODULE_END; i++ )
    {
        if ( LOG_SOAP == i )
        {
            moduleName[i] = "SOAP";
        }

        if ( LOG_HTTP == i )
        {
            moduleName[i] = "HTTP";
        }

        if ( LOG_STUN == i ) 
        {
            moduleName[i] = "STUN";
        }

        if ( LOG_TR069_TREE == i )
        {
            moduleName[i] = "TR069_TREE";
        }
		
        if ( LOG_EVENT == i )
        {
            moduleName[i] = "EVENT";
        }
		
        if ( LOG_HOST_HANDLE == i )
        {
            moduleName[i] = "HOST_HANDLER";
        }
		
        if ( LOG_KICK_HANDLE == i )
        {
            moduleName[i] = "KICK_HANDLER";
        }
		
        if ( LOG_MAIN_LOOP == i )
        {
            moduleName[i] = "MAIN_LOOP";
        }
    }

    for ( i = 0; i < T_LOG_LEVEL_END; i++ )
    {
        if ( T_LOG_WARNING == i ) 
        {
            levelLable[i] = "WARNING";
        }

        if ( T_LOG_ERROR == i )
        {
            levelLable[i] = "ERROR";
        }

        if ( T_LOG_EMERGENCY == i )
        {
            levelLable[i] = "EMERGENCY";
        }
    }

    return 0;
}

PAL_INT32_T pal_set_logger(LogModule modules, Pal_LogLevel level
                          , LogMode mode, PAL_INT32_T size, char *moduleName)
{
    logLevel = level;
    logModule = modules;
    logMode = mode;
	
    return 0;
}

PAL_INT32_T pal_logger(Pal_LogLevel level, const char *file
                      , PAL_INT32_T  line, LogModule module
                      , const char *funcName, const char *fmt, ...)
{
    FILE *logger = NULL;
    va_list ap;

    if ( ( logMode == LOG_DISABLE ) || ( ! ( logModule & module ) ) 
       || ( ( PAL_INT32_T ) level < ( PAL_INT32_T ) logLevel ) )
    {
        return -1;
    }

    /* output in console */
    if ( logMode == LOG_CONSOLE || logMode == LOG_BOTH )
    {
        logger = stderr;

        fprintf(logger, "Module[%s],file[%s][%d](%s)-level[%s]:"
               , moduleName[(int)module], file, line, funcName
               , levelLable[(PAL_INT32_T)level]);

        va_start(ap, fmt);
        vfprintf(logger, fmt, ap);
        va_end(ap);
    }

#if 0
    if ( logMode == LOG_SYS_LOG || logMode == LOG_BOTH )
    {
        if ( T_LOG_WARNING <= level && level <= T_LOG_EMERGENCY )
        {
            PAL_INT32_T sysLevel;

	        if (T_LOG_WARNING == level)
	        {
	            sysLevel = LOG_WARNING;
	        } 
            else if (T_LOG_ERROR == level)
            {
	            sysLevel = LOG_ERR;
	        }
            else if (T_LOG_EMERGENCY == level)
            {
	            sysLevel = LOG_EMERG;
	        }
		 
            syslog(sysLevel, "Module[%s],file[%s][%d](%s):"
                   ,  moduleName[(PAL_INT32_T)module], file, line, funcName);
            syslog(sysLevel, fmt);
        }
    }
#endif

    fflush ( logger );

    return 0;
}

void pal_exit_logger ( void )
{
    logMode = LOG_DISABLE;
}
