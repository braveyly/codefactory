/******************************************************************************
//  Description:
//      It provides modules to prepare os.conf before launcher is started.
//
*****************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int ComptoolDebugFlag = 0;
#define DBUG_COMPTOOL(...)         do{if(ComptoolDebugFlag) {printf("[DBG_INFO:%s %d] ", __FILE__, __LINE__);printf(__VA_ARGS__);}} while(0)

#define PATH_NEW_CFG   "/var/newcfg"
#define PATH_COMP_SETTING "/etc/comptoolsetting.conf"

#define LEN_LINE 128
#define LEN_PATH 128

#define CHECK_FILE_OK                  0
#define CHECK_ERROR_OPEN_FILE          1
#define CHECK_ERROR_FORMAT             2

#define SET_FILE_OK                    0
#define SET_ERROR_OPEN_FILE            1
#define SET_ERROR_FORMAT               2
#define SET_LACK_PARA                  3


#define COMPAT_OK                      0
#define COMPAT_OLDFILE_ERROR           1
#define COMPAT_FILE_ERROR              2
#define COMPAT_MALLOC_ERROR            3
#define COMPAT_FAIL                    4
#define COMPAT_GET_ERROR               5
#define COMPAT_SAME_VERSION            6

#define CFGSEARCH_OK                   0
#define CFGSEARCH_START_FAIL           1
#define CFGSEARCH_END_FAIL             2


#define VWMTA_VERSION_KEY              "sys_version"

#define VERSION_SAME                    0
#define VERSION_CHANGE                  1

#define KEY_SEARCH_SUCCESS              0
#define KEY_SEARCH_FAIL                 1
#define KEY_SEARCH_FILE_ERROR           2

char g_path_old[LEN_PATH]; // path old os.conf
char g_path_curr[LEN_PATH]; // path os.conf in image
char g_status_checkosconf[16];
char g_status_compatibility[16];
char g_vwmta_version[16];

char* comptool_fgets ( char* line, int len, char** buff )
{
   char* first_place;
   int i;
   first_place = strstr ( *buff, "\n" );

   if ( first_place == NULL )
      return NULL;

   *first_place = '\0';

   i = strlen ( *buff );

   *first_place = '\n';

   if ( i >= len )
      return NULL;

   memset ( line, 0, len );

   memcpy ( line, *buff, i + 1 );

   *buff = *buff + i + 1;

   return line;
}

int parse_setting()
{
   FILE *fileset;
   char setting_line[LEN_LINE];
   char *tmp, *name_set;
   int flag_checkos = 0, flag_checkcomp = 0, flag_old = 0, flag_curr = 0, flag_debug = 0;
   char debug_flag[13];

   fileset = fopen ( PATH_COMP_SETTING, "r" );

   if ( fileset == NULL )
   {
      printf ( "[InitCFG]Open setting file error, path = %s!\n", PATH_COMP_SETTING );
      return SET_ERROR_OPEN_FILE;
   }

   memset ( setting_line, 0, sizeof ( setting_line ) );

   while ( fgets ( setting_line, sizeof ( setting_line ), fileset ) != NULL )
   {
      tmp = strstr ( setting_line, "=" );

      if ( tmp == NULL )
      {
         fclose ( fileset );
         return SET_ERROR_FORMAT;
      }

      name_set = setting_line;

      *tmp = '\0';

      if ( strcmp ( name_set, "checkosconf" ) == 0 )
      {
         flag_checkos = 1;
         sprintf ( g_status_checkosconf, tmp + 1 );
         g_status_checkosconf[strlen ( tmp + 1 ) - 1] = '\0';
         continue;
      }

      if ( strcmp ( name_set, "checkcompatibility" ) == 0 )
      {
         flag_checkcomp = 1;
         sprintf ( g_status_compatibility, tmp + 1 );
         g_status_compatibility[strlen ( tmp + 1 ) - 1] = '\0';
         continue;
      }

      if ( strcmp ( name_set, "oldcfg" ) == 0 )
      {
         flag_old = 1;
         sprintf ( g_path_old, tmp + 1 );
         g_path_old[strlen ( tmp + 1 ) - 1] = '\0';
         continue;
      }

      if ( strcmp ( name_set, "currcfg" ) == 0 )
      {
         flag_curr = 1;
         sprintf ( g_path_curr, tmp + 1 );
         g_path_curr[strlen ( tmp + 1 ) - 1] = '\0';
         continue;
      }

      if ( strcmp ( name_set, "debugflag" ) == 0 )
      {
         flag_debug = 1;
         sprintf ( debug_flag, tmp + 1 );
         debug_flag[strlen ( tmp + 1 ) - 1] = '\0';

         if ( strcmp ( debug_flag, "enable" ) == 0 )
            ComptoolDebugFlag = 1;
         else
            ComptoolDebugFlag = 0;

         continue;
      }
   }

   fclose ( fileset );

   if ( flag_checkos == 1 && flag_checkcomp == 1 && flag_old == 1 && flag_curr == 1 && flag_debug == 1 )
      return SET_FILE_OK;
   else
      return SET_LACK_PARA;
}

void pare_default_setting()
{
   sprintf ( g_status_checkosconf, "enable" );
   g_status_checkosconf[strlen ( "enable" ) ] = '\0';

   sprintf ( g_status_compatibility, "enable" );
   g_status_compatibility[strlen ( "enable" ) ] = '\0';

   sprintf ( g_path_old, "/etc/os.conf" );
   g_path_old[strlen ( "/etc/os.conf" ) ] = '\0';

   sprintf ( g_path_curr, "/etc/default.conf" );
   g_path_curr[strlen ( "/etc/default.conf" ) ] = '\0';

   ComptoolDebugFlag = 0;
}

int check_os_conf ( char* file_path )
{
   FILE *oldcfg;

   struct stat fbuf;
   char *buffer, *head_buffer;
   char line[LEN_LINE];
   size_t i;
   char *tmpequal, *tmp1, *tmp2, *tmp3, *tmp4;
   int len_error, flag_error = CHECK_FILE_OK;

   //old file open
   oldcfg = fopen ( file_path, "r" );

   if ( oldcfg == NULL )
   {
      printf ( "[InitCFG]Open file error, path = %s!\n", file_path );
      return CHECK_ERROR_OPEN_FILE;
   }

   //get curr cfg into memery
   if ( stat ( file_path, &fbuf ) < 0 )
   {
      printf ( "[InitCFG]Get file status error!\n" );
      fclose ( oldcfg );
      return CHECK_ERROR_OPEN_FILE;
   }

   if ( fbuf.st_size == 0 )
   {
      fclose ( oldcfg );
      return CHECK_ERROR_FORMAT;
   }

   buffer = malloc ( fbuf.st_size );

   head_buffer = buffer;

   if ( buffer == NULL )
   {
      printf ( "[InitCFG]malloc memery error!\n" );
      fclose ( oldcfg );
      return CHECK_ERROR_OPEN_FILE;
   }

   i = fread ( buffer, 1, fbuf.st_size, oldcfg );

   if ( i != fbuf.st_size )
   {
      printf ( "[InitCFG]Read original configuration file error!\n" );
      flag_error = CHECK_ERROR_OPEN_FILE;
   }

   fclose ( oldcfg );

   if ( flag_error == CHECK_FILE_OK )
   {
      while ( comptool_fgets ( line, sizeof ( line ), &buffer ) != NULL )
      {
         tmpequal = strstr ( line, "=" );

         if ( tmpequal != NULL )
         {
            //have "=",   ok
            continue;
         }
         else
         {
            tmp1 = strstr ( line, "[" );
            tmp2 = strstr ( line, "]" );

            if ( tmp1 != NULL && tmp2 != NULL )
            {
               //app name, ok
               continue;
            }

            tmp3 = strstr ( line, "<" );

            tmp4 = strstr ( line, ">" );

            if ( tmp3 != NULL && tmp4 != NULL )
            {
               //table name, ok
               continue;
            }

            //not "=", not app, not table, error
            DBUG_COMPTOOL ( "Error format, this line is:\n%s\n", line );

            flag_error = CHECK_ERROR_FORMAT;

            break;
         }
      }
   }

   free ( head_buffer );

   return flag_error;
}

int cfg_search_part ( char* startStr, char* endStr, char **ppstart, char **ppend, char* pStream )
{
   *ppstart = strstr ( pStream, startStr );

   if ( *ppstart == NULL )
      return CFGSEARCH_START_FAIL;

   *ppend = strstr ( *ppstart + 1, endStr );

   if ( *ppend == NULL )
      return CFGSEARCH_END_FAIL;

   return CFGSEARCH_OK;
}

int get_file_key_value ( char* file_path, char* para_name, char* para_value )
{
   FILE *fcheckfile;
   char line[LEN_LINE];
   char *pParaname, *pParaEqual;
   int flag_result = KEY_SEARCH_FAIL;

   fcheckfile = fopen ( file_path, "r" );

   if ( fcheckfile == NULL )
   {
      printf ( "[InitCFG]Open check file error!\n" );
      return KEY_SEARCH_FILE_ERROR;
   }

   while ( fgets ( line, sizeof ( line ), fcheckfile ) != NULL )
   {
      pParaname = strstr ( line, VWMTA_VERSION_KEY );

      if ( pParaname != NULL )
      {
         pParaEqual = strstr ( pParaname, "=" );

         if ( pParaEqual != NULL )
         {
            strcpy ( para_value, pParaEqual + 1 );
            para_value[strlen ( pParaEqual + 1 ) - 1] = '\0';
            flag_result = KEY_SEARCH_SUCCESS;
            break;
         }
      }
   }

   fclose ( fcheckfile );

   DBUG_COMPTOOL ( "path = %s, paraname = %s, paravalue = %s\n", file_path, para_name, para_value );
   return flag_result;
}

int check_vwmta_version ( char* path_old, char* path_curr )
{
   int flag_result = VERSION_CHANGE;
   //always fix the os.conf when reboot
   //return flag_result;

   int result_oldversion, result_currversion;
   char str_oldversion[64], str_currversion[64];

   memset ( str_oldversion, 0, 64 );
   memset ( str_currversion, 0, 64 );
   result_oldversion = get_file_key_value ( path_old, VWMTA_VERSION_KEY, str_oldversion );
   result_currversion = get_file_key_value ( path_curr, VWMTA_VERSION_KEY, str_currversion );

   if ( result_currversion == KEY_SEARCH_SUCCESS && result_oldversion == KEY_SEARCH_SUCCESS )
   {
      DBUG_COMPTOOL ( "old version = %s, curr version = %s\n", str_oldversion, str_currversion );

      if ( strcmp ( str_oldversion, str_currversion ) == 0 )
         flag_result = VERSION_SAME;
   }

   return flag_result;
}

void fix_os_conf ( char* path_src, char* path_des )
{
   char cmd[256];
   sprintf ( cmd, "cp %s %s", path_src, path_des );
   system ( cmd );
   DBUG_COMPTOOL ( cmd );
   DBUG_COMPTOOL ( "\n\n" );
}

#define COMPTOOL_SUCCESS_MALLOC 0
#define COMPTOOL_FAIL_MALLOC        1

int Compatibility_os_conf ( char* path_old, char* path_curr, char* path_tmp )
{
   FILE *foldcfg, *fcurrcfg, *fnewcfg;

   struct stat foldbuf, fcurrbuf;
   char *oldbuffer, *currbuffer, *head_currbuffer;
   char line[LEN_LINE];
   size_t i;
   char *pCurrAppNameStart, *pCurrAppNameEnd, *pOldAppStart, *pOldAppEnd;
   char *pCurrEqual, *pOldApptable, *pOldParaName, *pOldEqual;
   char cfg_appname[LEN_LINE] = {0};
   char cfg_paraname[LEN_LINE] = {0};
   int flag_result = COMPAT_OK;
   int search_app_old;
   int flag_free_old = COMPTOOL_FAIL_MALLOC, flag_free_cur = COMPTOOL_FAIL_MALLOC;

   //old file open
   foldcfg = fopen ( path_old, "r" );

   if ( foldcfg == NULL )
   {
      printf ( "[Compatibility_os_conf]Open old file error! file_path = %s\n", path_old );
      return COMPAT_FILE_ERROR;
   }

   //current file open
   fcurrcfg = fopen ( path_curr, "r" );

   if ( fcurrcfg == NULL )
   {
      printf ( "[Compatibility_os_conf]Open current file error!file_path = %s\n", path_curr );
      return COMPAT_FILE_ERROR;
   }

   //new file open
   fnewcfg = fopen ( path_tmp, "w+" );

   if ( fnewcfg == NULL )
   {
      printf ( "[Compatibility_os_conf]Open new file error!file_path = %s\n", path_tmp );
      return COMPAT_FILE_ERROR;
   }

   //get old cfg into memery
   if ( stat ( path_old, &foldbuf ) < 0 )
   {
      printf ( "[Compatibility_os_conf]Get file status error!\n" );
      return COMPAT_FILE_ERROR;
   }

   oldbuffer = malloc ( foldbuf.st_size );

   if ( oldbuffer == NULL )
   {
      printf ( "[Compatibility_os_conf]malloc memery error!\n" );
      flag_result = COMPAT_MALLOC_ERROR;
      goto ken;
   }

   flag_free_old = COMPTOOL_SUCCESS_MALLOC;

   i = fread ( oldbuffer, 1, foldbuf.st_size, foldcfg );

   if ( i != foldbuf.st_size )
   {
      printf ( "[Compatibility_os_conf]Read original configuration file error!\n" );
      flag_result = COMPAT_FILE_ERROR;
      goto ken;
   }

   //get curr cfg into memery
   if ( stat ( path_curr, &fcurrbuf ) < 0 )
   {
      printf ( "[Compatibility_os_conf]Get file status error!\n" );
      return COMPAT_FILE_ERROR;
   }

   currbuffer = malloc ( fcurrbuf.st_size );

   head_currbuffer = currbuffer;

   if ( currbuffer == NULL )
   {
      printf ( "[Compatibility_os_conf]malloc memery error!\n" );
      flag_result = COMPAT_MALLOC_ERROR;
      goto ken;
   }

   flag_free_cur = COMPTOOL_SUCCESS_MALLOC;

   i = fread ( currbuffer, 1, fcurrbuf.st_size, fcurrcfg );

   if ( i != fcurrbuf.st_size )
   {
      printf ( "[Compatibility_os_conf]Read original configuration file error!\n" );
      flag_result = COMPAT_FILE_ERROR;
      goto ken;
   }

   if ( comptool_fgets ( line, sizeof ( line ), &currbuffer ) == NULL ) // the first line must be ID name, for example [xxxxxxxx]
   {
      flag_result = COMPAT_GET_ERROR;
      goto ken;
   }

   //while get ID
   while ( 1 )
   {
      if ( !cfg_search_part ( "[", "]\n", &pCurrAppNameStart, &pCurrAppNameEnd, line ) && line[0] == '[' )
      {
         fputs ( line, fnewcfg );
         strncpy ( cfg_appname, pCurrAppNameStart, pCurrAppNameEnd - pCurrAppNameStart + 1 );
         cfg_appname[pCurrAppNameEnd - pCurrAppNameStart + 1] = '\0';
         DBUG_COMPTOOL ( "cfg_appname = %s, strlen = %d\n", cfg_appname,  pCurrAppNameEnd - pCurrAppNameStart + 1 );

         search_app_old = cfg_search_part ( cfg_appname, "\n[", &pOldAppStart, &pOldAppEnd, oldbuffer );

         //get the same app part in oldbuff

         if ( search_app_old == CFGSEARCH_START_FAIL ) //fail to search the app name in the old os.conf, so copy all current parameter to the new file
         {
            DBUG_COMPTOOL ( "Fail to search the app part: %s in the old os.conf\n", cfg_appname );

            while ( comptool_fgets ( line, sizeof ( line ), &currbuffer ) != NULL )
            {
               if ( line[0] != '[' )
                  fputs ( line, fnewcfg );
               else
                  break;
            }

            continue;
         }
         else// success to search the app name in the old os.conf, so get the old configuration, and copy the value behind the name
         {
            if ( search_app_old == CFGSEARCH_OK ) //not the last app
            {
               //search the old app part, make it as a string for search, after finishing searching, return to before
               * ( pOldAppEnd + 1 ) = '\0';
            }

            //check the current buffer, until find another app, has '=' is parameter, does not have '=' maybe table or maybe another app start
            while ( comptool_fgets ( line, sizeof ( line ), &currbuffer ) != NULL )
            {
               pCurrEqual = strchr ( line, '=' );

               if ( pCurrEqual != NULL ) //signal parameter
               {
                  //find the parameter name in curr
                  *pCurrEqual = '\0';
                  strcpy ( cfg_paraname, line );
                  cfg_paraname[strlen ( line ) ] = '\0';
                  DBUG_COMPTOOL ( "cfg_paraname = %s\n", cfg_paraname );

                  if ( strcmp ( VWMTA_VERSION_KEY, cfg_paraname ) == 0 ) // if it is version, do not get the old setting
                  {
                     pOldParaName = NULL;
                  }
                  else
                  {
                     int len_oldparaname = 2;
                     pOldParaName = pOldAppStart;
                     //search the parameter name in the old configuration

                     while ( ( pOldParaName = strstr ( pOldParaName + len_oldparaname, cfg_paraname ) ) != NULL )
                     {
                        pOldEqual = strchr ( pOldParaName, '=' );
                        *pOldEqual = '\0';

                        while ( * ( pOldParaName ) != '\n' ) // find the line head
                           pOldParaName--;

                        pOldParaName++;

                        len_oldparaname = strlen ( pOldParaName );

                        if ( strcmp ( pOldParaName, cfg_paraname ) == 0 ) // if the name are the same, find the correct name
                        {
                           *pOldEqual = '=';
                           break;
                        }
                        else // if the name just the same with this name's part, continue search
                        {
                           *pOldEqual = '=';
                           continue;
                        }
                     }
                  }

                  // find the parameter name in the old os.conf, copy the setting to the new os.conf
                  if ( pOldParaName != NULL )
                  {
                     pOldEqual = strchr ( pOldParaName, '=' );

                     if ( pOldEqual == NULL )
                     {
                        DBUG_COMPTOOL ( "Error old format!\n" );
                        flag_result = COMPAT_OLDFILE_ERROR;
                        goto ken;
                     }
                     else
                     {
                        while ( *pOldEqual  != '\n' )
                        {
                           *pCurrEqual = *pOldEqual;
                           pCurrEqual++;
                           pOldEqual++;
                        }

                        *pCurrEqual = '\n';

                        * ( pCurrEqual + 1 ) = '\0';
                        DBUG_COMPTOOL ( "Success to find the %s, new line: %s\n", cfg_paraname, line );
                     }
                  }
                  else // do not find the parameter name in the old os.conf, return =, and put the current line to the new file
                  {
                     *pCurrEqual = '=';
                     DBUG_COMPTOOL ( "Fail to search the app parameter name: %s \n", cfg_paraname );
                  }

                  fputs ( line, fnewcfg );
               }
               else // signal parameter finish, check table parameter
               {
                  DBUG_COMPTOOL ( "\nCan not find the =, in the %s\n", line );
                  pOldApptable = strstr ( pOldAppStart, "\n<" );

                  if ( pOldApptable != NULL )
                  {
                     DBUG_COMPTOOL ( "find the table \n%s\n\n", pOldApptable + 1 );
                     fputs ( pOldApptable + 1, fnewcfg );
                     //fseek(fnewcfg, -1, SEEK_CUR);//remove the '\0'

                     // Ken add at 2010-07-27, for the default configuration has tables
                     while( 1 )
                     {
                        // util find the next app in the current buffer
                        if ( comptool_fgets ( line, sizeof ( line ), &currbuffer ) == NULL )
                        {
                           break;
                        }
                        else
                        {
                           if ( line[0] == '[' )
                             break;
                        }
                     }
                  }
                  else
                  {
                     // Here we check whether the default configuration has the tables
                     if ( strchr( line, '<') != NULL )
                     {
                        fputs ( line, fnewcfg );
                        while( 1 )
                        {
                           // util find the next app in the current buffer
                           if ( comptool_fgets ( line, sizeof ( line ), &currbuffer ) == NULL )
                           {
                              break;
                           }
                           else
                           {
                              if ( line[0] == '[' )
                                break;
                              else
                                fputs ( line, fnewcfg ); 
                           }
                        }
                     }
                  }

                  break;
               }
            }

            if ( search_app_old == CFGSEARCH_OK ) // not the last app
               * ( pOldAppEnd + 1 ) = '[';
         }
      }
      else
      {
         DBUG_COMPTOOL ( "End line = %s\n\n", line );
         flag_result = COMPAT_OK;
         goto ken;
      }
   }

ken:

   fclose ( foldcfg );
   fclose ( fcurrcfg );
   fclose ( fnewcfg );

   if ( flag_free_old == COMPTOOL_SUCCESS_MALLOC )
   {
      DBUG_COMPTOOL ( "Free the old buffer\n" );
      free ( oldbuffer );
   }

   if ( flag_free_cur == COMPTOOL_SUCCESS_MALLOC )
   {
      DBUG_COMPTOOL ( "Free the current buffer\n" );
      free ( head_currbuffer );
   }

   return flag_result;
}

int main ( int argc, char **argv )
{
   int result_compat = COMPAT_FAIL;
   int result_version;

   if ( parse_setting() )
   {
      DBUG_COMPTOOL ( "%s file is Error, get the default setting\n\n", PATH_COMP_SETTING );
      pare_default_setting();
   }

   DBUG_COMPTOOL ( "g_path_old = %s\n", g_path_old );

   DBUG_COMPTOOL ( "g_path_curr = %s\n", g_path_curr );
   DBUG_COMPTOOL ( "g_status_checkosconf = %s\n", g_status_checkosconf );
   DBUG_COMPTOOL ( "g_status_compatibility = %s\n", g_status_compatibility );
   DBUG_COMPTOOL ( "ComptoolDebugFlag = %d\n", ComptoolDebugFlag );

   if ( strcmp ( g_status_checkosconf, "enable" ) == 0 )
   {
      int file_status = check_os_conf ( g_path_old );

      if ( file_status == CHECK_FILE_OK )
      {
         DBUG_COMPTOOL ( "Path = %s, os.conf is ok, continue to check compatibility\n", g_path_old );
      }
      else
      {
         fix_os_conf ( g_path_curr, g_path_old );
         return COMPAT_OLDFILE_ERROR;
      }
   }
   else
   {
      DBUG_COMPTOOL ( "Disable the function to check the old os.conf\n" );
   }

   if ( strcmp ( g_status_compatibility, "enable" ) == 0 )
   {
      result_version = check_vwmta_version ( g_path_old, g_path_curr );

      if ( result_version == VERSION_CHANGE )
      {
         result_compat = Compatibility_os_conf ( g_path_old, g_path_curr, PATH_NEW_CFG );

         if ( result_compat == COMPAT_OK )
            fix_os_conf ( PATH_NEW_CFG, g_path_old );
         else
            fix_os_conf ( g_path_curr, g_path_old );

         char rmcmd[30];

         sprintf ( rmcmd, "rm -rf %s", PATH_NEW_CFG );

         system ( rmcmd );
      }
      else
      {
         DBUG_COMPTOOL ( "The same version, no need to check the compatibility\n\n" );
         result_compat = COMPAT_SAME_VERSION;
      }

      DBUG_COMPTOOL ( "compatibility result = %d\n", result_compat );

      return result_compat;
   }
   else
   {
      DBUG_COMPTOOL ( "Disable the function to compatibility\n\n" );
   }
}


