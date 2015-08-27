#include "pal_log.h"
#include "pal_debug.h"
#include <loglib.h>
#include <stdarg.h>

static Pal_LogLevel logLevel;
static LogMode logMode;
static LogModule logModule;

#define DEBUG_MAX_STRING_LENGTH 1024
static PAL_INT8_T *moduleName[LOG_MODULE_END];
static PAL_INT8_T *levelLable[T_LOG_LEVEL_END];

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

        if ( LOG_HOST_HANDLER == i )
        {
            moduleName[i] = "HOST_HANDLER";
        }
		
        if ( LOG_KICK_HANDLER == i )
        {
            moduleName[i] = "KICK_HANDLER";
        }
		
        if ( LOG_MAIN_LOOP == i )
        {
            moduleName[i] = "MAIN_LOOP";
        }

        if (LOG_FTP_MODULE == i)
            {
                moduleName[i] = "FTP";
            }
        
        if (LOG_CRQ_MODULE == i)
        {
            moduleName[i] = "CRQ";
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

    
    InitTcDebugTR069();
    return 0;
}

PAL_INT32_T pal_set_logger ( LogModule modules, Pal_LogLevel level
                           , LogMode mode, PAL_INT32_T size
                           , char *moduleName )
{
    logLevel = level;
    logModule |= modules;
    logMode = mode;
	
    return 0;
}

void pal_set_log_level(Pal_LogLevel level)
{
    logLevel = level;
}

void pal_get_log_level(Pal_LogLevel *levelPtr)
{
    *levelPtr = logLevel;
}

void pal_set_log_mode(LogMode mode)
{
    logMode = mode;
}

void pal_get_log_mode(LogMode *modePtr)
{
    *modePtr = logMode;
}

PAL_INT32_T pal_logger ( Pal_LogLevel level, const PAL_INT8_T *file, PAL_INT32_T  line, LogModule module, const PAL_INT8_T *funcName, const PAL_INT8_T *fmt, ... )
{
    PAL_INT8_T *sendbuf;
    PAL_INT8_T  buf[DEBUG_MAX_STRING_LENGTH];
    PAL_INT32_T length;
    va_list ap;
    if ( ( logMode == LOG_DISABLE ) || ( ! ( logModule | module ) ) || ( ( PAL_INT32_T ) level < ( PAL_INT32_T ) logLevel ) )
    {
        return 0;
    }

    //sendbuf = ( char * ) PAL_MALLOC ( DEBUG_MAX_STRING_LENGTH );
    sprintf ( buf, "Module[%s],file[%s][%d](%s)-level[%s]£º", moduleName[ ( PAL_INT32_T ) module], file, line, funcName, levelLable[level] );
    length = strlen ( buf );
    //strcpy ( buf, modulebuf );
    sendbuf = buf;

    va_start ( ap, fmt );
        vsprintf ( sendbuf + length, fmt, ap );
        va_end ( ap );
    /* output in console */
   if ( logMode == LOG_CONSOLE || logMode == LOG_BOTH )
    {
        
        if ( strlen ( sendbuf ) < DEBUG_MAX_STRING_LENGTH - 1 )
        {
            printf ( "%s\n", sendbuf );
        }
        else
        {
           // PAL_FREE ( sendbuf );
            printf("message too long\n");
            return -1;
        }
    }

    if ( logMode == LOG_SYS_LOG || logMode == LOG_BOTH )
    {

        tcdebugPrintfExTR069 ( moduleName[ ( PAL_INT32_T ) module], sendbuf, level );
    }
    //PAL_FREE ( sendbuf );
    return 0;
}

void pal_exit_logger ( void )
{
    logLevel = LOG_DISABLE;
}

int strcasecmp (char *s, char *t)
{
   for (; tolower(*s) == tolower(*t); s++, t++)
   {
      if (*s == '\0' || *t == '\0')
      {
         return 0;
      }
   }
   return *s - *t;
}

int strncasecmp (char *s, char *t, int c)
{
    int i;
    if(s!=NULL){
        if(strlen(s)<c)
            return -1;
    }
    if(t!=NULL){
        if(strlen(t)<c)
            return -1;
    }

   for (i = 0; tolower(*s) == tolower(*t) && i<(c-1); )
   {
      if (*s == '\0' || *t==0)
      {
         return 0;
      }
      s++;
      t++;
      i++;
   }
   return tolower(*s )- tolower(*t);
}

char *strdup(const char *s)		/* I - String to duplicate */
{
  char	*t=NULL;				/* New string pointer */

  if (s == NULL)
    return t;

  if ((t = PAL_MALLOC(strlen(s) + 1)) == NULL)
    return (NULL);

  strcpy(t,s);   

  return t;
}
