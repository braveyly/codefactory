/**********************************************************************
Copyright (c) 2009 Wuhan Tecom Co., Ltd.
All Rights Reserved
No portions of this material may be reproduced in any form without the
written permission of:
Wuhan Tecom Co., Ltd.
#18, Huaguang Road
Wuhan, PR China 430074
All information contained in this document is Wuhan Tecom Co., Ltd.
company private, proprietary, and trade secret.
************************************************************************/

#include "tcmAgent.h"
#include "conflib.h"
#include "configapi.h"
#include "configact.h"
#include "define.c"

extern ApplyStatus get_device_summary ( char *name, char *retValue );
extern ApplyStatus get_device_info ( char *name, char *retValue );
extern ApplyStatus get_time ( char *name, char *retValue );
extern ApplyStatus get_ipPingDiagnostics ( char *name, char *retValue );
extern ApplyStatus get_forwarding ( char *name, char *retValue, int index );
int gTr69SessionRunning = 0; //for check tr069 session status


#define MAX_NUM_OF_MATCH_PARAMETERS sizeof( gMatchParameters ) / sizeof( ConflibTR069MatchParameter )

/*************for tcmagent log******************/
FILE *logHandle;
#define TCMAGENT_LOG_PATH "/bin/conf/tcmAgent.log"
#define TCMAGENT_LOG_TMP_PATH "/bin/conf/tcmAgent.log.tmp"
#define TCMAGENT_LOG_SWITCH_FILE "/tmp/tcmAgentlogswitch"
#define MAX_LOG_SIZE 1024*50
#define BASIC_LOG_LINES 100
int init_tcmAgent_log()
{
    if ( access ( TCMAGENT_LOG_PATH, R_OK ) == 0 )
    {
        struct stat buf;
        stat ( TCMAGENT_LOG_PATH, &buf );
        if ( buf.st_size > MAX_LOG_SIZE )
        {
            char cmd[100];
			memset( cmd, 0, 100 );
			sprintf( cmd, "cp %s %s", TCMAGENT_LOG_PATH, TCMAGENT_LOG_TMP_PATH );
            system( cmd );

			memset( cmd, 0, 100 );
			sprintf( cmd, "cat /dev/null>%s", TCMAGENT_LOG_PATH );			
			system( cmd );
			
	        memset( cmd, 0, 100 );
			sprintf( cmd, "cat &(tail -%d %s)>> %s", BASIC_LOG_LINES, TCMAGENT_LOG_TMP_PATH, TCMAGENT_LOG_PATH );
			system( cmd );

	        memset( cmd, 0, 100 );
			sprintf( cmd, "rm -f %s", TCMAGENT_LOG_TMP_PATH );
			system( cmd );

			sleep(3);
        }		 
    }
    if( ( logHandle = fopen( TCMAGENT_LOG_PATH, "ab" ) ) == NULL )
    {
       TR069_ERROR ( "Init tcmAgent log failed\n" );
	   return -1;
    }
	return 0;
}

void uninit_tcmAgent_log()
{
    if( logHandle != NULL )
      fclose( logHandle );
}


static char buff[4096];
void tcmAgent_printf(const char *func, UINT32 lineNum, char *fmt, ...)
{

  va_list ap;
  
  memset(buff,0,4096);
  sprintf( buff, "[%s] %d: ", func, lineNum );
  va_start(ap, fmt);
  vsprintf(buff+strlen(buff),fmt,ap);
  va_end(ap);

  if(!logHandle)
	return ;
  if( '\n' == *(buff + strlen(buff) - 1) )
  {
      fprintf(logHandle,"%s",buff);
  }
  else
  {
      fprintf(logHandle,"%s\n",buff);
  }
  fflush( logHandle );
}
/************************************************/

/***********************************************************************
Function: string_to_num
Description:  set the value of node of entry.

Return: 0 - success, other - failure.
Input:   str - the string of num.
Output: retValue - value
Others:
************************************************************************/
static int string_to_num ( char *str, int *retValue )
{
   int ret = 1;
   int tmpValue;

   if ( NULL != str && strlen ( str ) > 0 )
   {
      int i;

      for ( i = 0; i < strlen ( str ); i++ )
      {
         if ( !isdigit ( str[i] ) && ( '-' != str[i] || i != 0 || strlen ( str ) <= 1 ) )
         {
            ret = 0;
            break;
         }
      }
   }
   else
   {
      *retValue = 0;
   }

   if ( ret )
   {
      if ( '-' == str[0] )
         tmpValue = 0 - atoi ( str + 1 );
      else
         tmpValue = atoi ( str );
   }

   *retValue = tmpValue;

   return ret;
}

/***********************************************************************
Function: get_msg_type
Description:  get the msg type from the buffer.

Return: void.
Input:   totalLen - the length of valid message in the buffer except the totalLen option.
            buf
Output: totalLen
            buf
            type
Others:
************************************************************************/
static void get_msg_type ( unsigned int *totalLen, char **buf, MsgType *type )
{
   *type = ( MsgType ) * ( *buf );
   *buf += sizeof ( MsgType );
   *totalLen -= sizeof ( MsgType );
}

/***********************************************************************
Function: get_msg_value
Description:  get the msg value from the buffer.

Return: 0 - success else - failure.
Input:   totalLen - the length of valid message in the buffer except the totalLen option.
            buf
Output: totalLen
            buf
            value
Others: now, it only support get value of unsigned int type, it's enough
************************************************************************/
static int get_msg_value ( char **buf, unsigned int *bufLen, unsigned int *value )
{
   MsgValueType type;

   if ( *bufLen < MAX_LEN_FOR_MSG_VALUE_TYPE )
   {
      return -1;
   }

   type = * ( *buf );

   if ( !IS_MSG_VALUE_TYPE_VALID ( type ) )
   {
      return -1;
   }

   *buf += sizeof ( type );

   *bufLen -= sizeof ( type );

   if ( MSG_UINT != type )
   {
      return -1;
   }

   memcpy ( value, *buf, sizeof ( int ) );

   *buf += sizeof ( unsigned int );
   *bufLen -= sizeof ( unsigned int );

   return 0;
}

/***********************************************************************
Function: get_msg_name
Description:  get the msg name from the buffer.

Return: 0 - success else - failure.
Input:   totalLen - the length of valid message in the buffer except the totalLen option.
            buf
Output: totalLen
            buf
            name
Others:
************************************************************************/
static int get_msg_name ( char **buf,  unsigned int *totalLen, char *name )
{
   unsigned int nameLen;
   int ret;

   if ( *totalLen < MAX_LEN_FOR_MSG_NAME_LENGTH )
   {
      return -1;
   }
   else
   {
      nameLen = * ( *buf );

      if ( nameLen > MAX_TR069_NAME_LEN + 1 )
      {
         return -1;
      }

      *buf += sizeof ( nameLen );

      *totalLen -= ( sizeof ( nameLen ) + nameLen );
      memcpy ( name, *buf, nameLen );
      *buf += nameLen;
   }

   return 0;
}

/***********************************************************************
Function: put_msg_type
Description:  put msg type to the buffer.

Return: void
Input:   totalLen - the length of valid message in the buffer except the totalLen option.
            buf
            type
Output: totalLen
            buf
Others:
************************************************************************/
static void put_msg_type ( unsigned int *totalLen, char **buf, MsgType type )
{
   * ( *buf ) = ( int ) type;
   *buf += MAX_MSG_TYPE_LEN;
   *totalLen += MAX_MSG_TYPE_LEN;
}

/***********************************************************************
Function: put_msg_name
Description:  put msg name to the buffer.

Return: void
Input:   totalLen - the length of valid message in the buffer except the totalLen option.
            buf
            name
            nameLen
Output: totalLen
            buf
Others:
************************************************************************/
static void put_msg_name ( unsigned int *totalLen, char **buf, char *name, unsigned int nameLen )
{
   * ( *buf ) = nameLen;
   *buf += MAX_LEN_FOR_MSG_NAME_LENGTH;
   memcpy ( *buf, name, nameLen );
   *buf += nameLen;
   *totalLen += ( MAX_LEN_FOR_MSG_NAME_LENGTH + nameLen );
}

/***********************************************************************
Function: put_msg_value
Description:  put msg value to the buffer.

Return: void
Input:   totalLen - the length of valid message in the buffer except the totalLen option.
            buf
            type
            value
Output: totalLen
            buf
Others:
************************************************************************/
static void put_msg_value ( unsigned int *totalLen, char **buf, MsgValueType type, void *value )
{
   * ( *buf ) = type;
   *buf += MAX_LEN_FOR_MSG_VALUE_TYPE;
   *totalLen += MAX_LEN_FOR_MSG_VALUE_TYPE;

   switch ( type )
   {

      case TCMAGENT_INT:

      case TCMAGENT_BOOL:

      case TCMAGENT_UNSIGNED:
         memcpy ( *buf, value, sizeof ( int ) );
         *buf += sizeof ( int );
         *totalLen += sizeof ( int );
         break;

      case TCMAGENT_STRING:
         strcpy ( *buf, ( char * ) value );
         *buf += ( strlen ( ( char * ) value ) + 1 );
         *totalLen += ( strlen ( ( char * ) value ) + 1 );
         break;
   }
}


/***********************************************************************
Function: get_ipPingDiagnostics
Description:  the callback function of getting the node of ipPingDiagnostics.

Return: ApplyStatus
Input:   name - the configuration node name.
Output: retValue
Others:
************************************************************************/
ApplyStatus get_ipPingDiagnostics ( char *name, char *retValue )
{

   ApplyStatus ret = APPLY_ERROR;
   return ret;
}


/***********************************************************************
Function: get_device_summary
Description:  the callback function of getting the node of device summary.

Return: ApplyStatus
Input:   name - the configuration node name.
Output: retValue
Others:
************************************************************************/
ApplyStatus get_device_summary ( char *name, char *retValue )
{
   ApplyStatus ret = APPLY_ERROR;

   if ( NULL != name && NULL != retValue )
   {
      ret = APPLIED;

      if ( 0 == strcmp ( name, "InternetGatewayDevice.DeviceSummary" ) )
      {
         strcpy ( retValue, "tecomtech" );
      }
      else
      {
         ret = APPLY_ERROR;
      }
   }

   return ret;
}

/***********************************************************************
Function: get_uptime
Description:  get the up time of system.

Return: 0 - success other - failure.
Input:
Output: sec
Others:
************************************************************************/

static int get_uptime ( char *sec )
{
#define UPTIME_LEN 128
#define SEC_PER_MIN 60
#define SEC_PER_HOUR SEC_PER_MIN * 60
#define SEC_PER_DAY SEC_PER_HOUR * 24

   char *p, buf[UPTIME_LEN], uptimeStr[UPTIME_LEN];
   FILE *fp;
   unsigned int uptimeInteger, len;

   if ( NULL == ( fp = fopen ( "/proc/uptime", "r" ) ) )
   {
      strcpy ( sec, "0" );
      return -1;
   }
   else
   {
      if ( 0 > fgets ( buf, UPTIME_LEN, fp ) )
      {
         strcpy ( sec, "0" );
         return -1;
      }

      fclose ( fp );
   }


   p = strchr ( buf, ' ' );

   if ( NULL != p )
   {
      *p = '\0';
   }

   p = strchr ( buf, '.' );

   if ( NULL != p )
   {
      *p = '\0';
   }

   strcpy ( sec, buf );

   return 0;
}


/***********************************************************************
Function: get_device_info
Description:  the callback function of getting the node of device info.

Return: ApplyStatus
Input:   name - the configuration node name.
Output: retValue
Others:
************************************************************************/
ApplyStatus get_device_info ( char *name, char *retValue )
{
    ApplyStatus ret = APPLY_ERROR;

    if ( NULL != name && NULL != retValue )
    {
        ret = APPLIED;

	 if ( 0 == strcmp ( name, "InternetGatewayDevice.DeviceInfo.UpTime" ) )
        {
	     get_uptime (retValue);
        }
        else if ( 0 == strcmp ( name, "InternetGatewayDevice.DeviceInfo.DeviceLog" ) )
        {
            FILE *fp;
	     unsigned int readBytes;

	     retValue[0] = '\0';
            if ( NULL == ( fp = fopen (logName , "rb" ) ) )
            {
                printf ( "\nopen file %s failure!\n", logName );
                retValue[0] = '\0';
            }
            else
            {
                if ( 0 > (readBytes = fread ( retValue, 1, MAX_FILE_SIZE, fp ) ))
                {
                    printf ( "\nread file %s failure\n", logName );
                    retValue[0] = '\0';
                } else {
                    retValue[readBytes] = '\0';
                }

		  fclose (fp);
            }
        }
        else
        {
            ret = APPLY_ERROR;
        }
    }

    return ret;
}

void get_current_local_time ( char *time )
{
#define UPTIME_LEN 128
#define SEC_PER_MIN 60
#define SEC_PER_HOUR SEC_PER_MIN * 60
#define SEC_PER_DAY SEC_PER_HOUR * 24

   char *p, buf[UPTIME_LEN], uptimeStr[UPTIME_LEN];
   FILE *fp;
   int uptimeInteger, len;
   int year, month, day, hour, min, sec;

   if ( ( fp = fopen ( "/proc/uptime", "r" ) ) == NULL )
   {
      fprintf ( stderr, "%s\n", strerror ( errno ) );
      return;
   }

   if ( fgets ( buf, UPTIME_LEN, fp ) == NULL )
   {
      fprintf ( stderr, "%s\n", strerror ( errno ) );
      return;
   }

   if ( fclose ( fp ) == -1 )
   {
      fprintf ( stderr, "%s\n", strerror ( errno ) );
      return;
   }

   p = strchr ( buf, ' ' );

   if ( p == NULL )
   {
      fprintf ( stderr, "pal_uptime failed\n" );
      return;
   }

   len = p - buf;

   memcpy ( uptimeStr, buf, len );
   uptimeStr[len] = '\0';
   uptimeInteger = atoi ( uptimeStr );

   /* FIXME: year, month should be calculated */
   year = 0;
   month = 0;

   day = uptimeInteger / SEC_PER_DAY;
   hour = ( uptimeInteger % SEC_PER_DAY ) / SEC_PER_HOUR;
   min = ( uptimeInteger % SEC_PER_HOUR ) / SEC_PER_MIN;
   sec = uptimeInteger % SEC_PER_MIN;

   sprintf ( time, "%04d-%02d-%02dT%02d:%02d:%02d"
             , year, month, day, hour, min, sec );
}

/***********************************************************************
Function: get_time
Description:  the callback function of getting the node of time.

Return: ApplyStatus
Input:   name - the configuration node name.
Output: retValue
Others:
************************************************************************/
ApplyStatus get_time ( char *name, char *retValue )
{
   ApplyStatus ret = APPLY_ERROR;

   if ( NULL != name && NULL != retValue )
   {
      ret = APPLIED;

      if ( 0 == strcmp ( name, "InternetGatewayDevice.Time.CurrentLocalTime" ) )
      {
         get_current_local_time ( retValue );
      }
      else
      {
         ret = APPLY_ERROR;
      }
   }

   return ret;
}

/***********************************************************************
Function: get_forwarding
Description:  the callback function of getting the node of forwarding.

Return: ApplyStatus
Input:   name - the configuration node name.
            instanceID - the instance ID of instance of multi-node.
Output: retValue
Others:
************************************************************************/
ApplyStatus get_forwarding ( char *allName, char *retValue, int instanceID )
{
   ApplyStatus ret = APPLY_ERROR;
   char *name;

   if ( NULL != allName && NULL != retValue )
   {

      if ( NULL == ( name = strrchr ( allName, '.' ) ) )
      {
         ret = APPLY_ERROR;
      }
      else
      {
         name++;
         ret = APPLIED;

         if ( 0 == strcmp ( name, "Status" ) )
         {
            strcpy ( retValue, "active" );
         }
         else
         {
            ret = APPLY_ERROR;
         }
      }
   }

   return ret;
}

/***********************************************************************
Function: tcmCB_session_begin
Description:  the callback function which is used by tcmAgent.
                   the function will be called when the tr069 session begins.

Return: 0 - success other - failure
Input:
Output:
Others:
************************************************************************/
int tcmCB_session_begin ( void )
{
   gTr69SessionRunning  = 1;
   TR069_NOTICE ( "tr069 session begin, config file is lock for tcm agent!" );
   return 0;
}

/***********************************************************************
Function: tcmCB_session_end
Description:  the callback function which is used by tcmAgent.
                   the function will be called when the tr069 session end.

Return: 0 - success other -failure
Input:
Output:
Others:
************************************************************************/
int tcmCB_session_end ( SessionInfo *pInfo )
{
   gTr69SessionRunning  = 0;
   TR069_NOTICE ( "tr069 session end, config file is unlock============\n" );
   config_api_write_to_flash ( );
   return 0;
}

/*********************************************************
Function: template_match_compare
Description:
templateStr: InternetGatewayDevice.Layer3Forwarding.Forwarding.0.DestIPAddress
name       : InternetGatewayDevice.Layer3Forwarding.Forwarding.12.DestIPAddress

Return: if match, return 1; else return 0
Input:
Output:
Others:
**********************************************************/
/*This function is used by tcmCB_set_value, tcmCB_get-value, get_noninstanceid_match_row_id*/
int template_match_compare ( char* templateStr, char* name )
{
   char* ptemplate;
   char* pname;

   if ( ( strlen ( name ) - strlen ( templateStr ) ) == 1 || ( strlen ( name ) - strlen ( templateStr ) == 0 ) )
   {
      if ( ( ptemplate = strstr ( templateStr, TEMPLATE_STR ) ) == NULL )
      {
         return 0;
      }

      /*compare sub string before ".0."*/
      if ( memcmp ( templateStr, name, ptemplate - templateStr ) == 0 )
      {
         if ( ( pname = strstr ( name + ( ptemplate - templateStr ) + 1, "." )  ) == NULL )
         {
            return 0;
         }

         /*Compare sub string after ".0."*/
         if ( strcmp ( ptemplate + TEMPLATE_LEN, pname + 1 ) == 0 )
         {
            return 1;
         }
      }

   }

   return 0;
}

/************************************************
Function: get_noninstanceid_match_row_id
Description:
templateStr:InternetGatewayDevice.Layer3Forwarding.Forwarding.0.DestIPAddress
name         :InternetGatewayDevice.Layer3Forwarding.Forwarding.8.DestIPAddress
Return: if match, instance ID index; else return -1
Input:
Output:
Others:
*************************************************/

int get_noninstanceid_match_row_id ( char* templateStr, char* name )
{
   char* pTemplate;
   char* pFirstDot;
   char* pSecondDot;
   char rowIDStr[MAX_INSTANCE_ID_LEN + 1];/*1 is for '\0'*/
   int rowID;

   if ( template_match_compare ( templateStr, name ) )
   {
      if ( ( pTemplate = strstr ( templateStr, TEMPLATE_STR ) ) == NULL )
      {
         return -1;
      }

      pFirstDot = name + ( pTemplate - templateStr );

      if ( ( pSecondDot = strstr ( pFirstDot + 1, "." ) ) == NULL )
      {
         return -1;
      }

      if ( pSecondDot - pFirstDot == MAX_INSTANCE_ID_LEN + 1 || pSecondDot - pFirstDot == MAX_INSTANCE_ID_LEN )
      {
         memcpy ( rowIDStr, pFirstDot + 1, pSecondDot - pFirstDot - 1 );
         rowIDStr[pSecondDot-pFirstDot-1] = '\0';

         if ( ! string_to_num ( rowIDStr, &rowID ) )
         {
            return -1;
         }

         name = pSecondDot + 1;

         return rowID;
      }
   }

   return -1;
}


/****************************************
Function: get_instanceid_match_row_id
Description:
templateStr:InternetGatewayDevice.Layer3Forwarding.Forwarding.
name         :InternetGatewayDevice.Layer3Forwarding.Forwarding.8.DestIPAddress
Return: if match, instance ID index; else return -1
Input:
Output:
Others:
*****************************************/
int get_instanceid_match_row_id ( char* tr069Name, char* name )
{
   char rowIDStr[MAX_INSTANCE_ID_LEN + 1];/*1 is for '\0'*/
   int rowIDStart = strlen ( tr069Name );
   int rowIDEnd;
   int rowID;

   if ( NULL != strchr ( name + rowIDStart, '.' ) )
   {
      rowIDEnd = strchr ( name + rowIDStart, '.' ) - name ;
   }
   else
   {
      return -1;
   }

   if ( rowIDEnd > rowIDStart && rowIDEnd - rowIDStart < MAX_INSTANCE_ID_LEN + 1 )
   {
      strncpy ( rowIDStr, name + rowIDStart, rowIDEnd - rowIDStart );
      * ( rowIDStr + rowIDEnd - rowIDStart ) = '\0';
   }
   else
   {
      return -1;
   }

   if ( string_to_num ( rowIDStr, &rowID ) )
   {
      return rowID;
   }
   else
   {
      return -1;
   }
}



/***********************************************************************
Function: tcmCB_set_value
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to change the configuration in demo.

Return:        ApplyStatus
Input:          name
                   value
Output:
Others:
************************************************************************/
ApplyStatus tcmCB_set_value ( char *name, char *value )
{
   TR069_DEBUG ( "Start set value\t name: %s  value: %s", name, value );
   int i;
   ApplyStatus ret = APPLY_ERROR;
   int rowID = 0;
   int parID = 0;

   for ( i = 0; i < MAX_NUM_OF_MATCH_PARAMETERS; i++ )
   {

/*****************Write os.conf with values of parameters which donot belong to object*************/
      if ( gMatchParameters[i].conflibRowID == FLAG_NO_ROW && gMatchParameters[i].conflibParID != FLAG_STATUS_STYLE && strcmp ( gMatchParameters[i].tr069Name, name ) == 0 )
      {
         TR069_DEBUG ( "Find this parameter in non-object, appID %d parID %d", gMatchParameters[i].conflibAppID, gMatchParameters[i].conflibParID );

         char result[MAX_LEN_OF_VALUE];

         if ( ( gMatchParameters[i].conflibAppID == ID_DHCPD && gMatchParameters[i].conflibParID == 1 ) || \
               gMatchParameters[i].conflibAppID == ID_SERVICE || \
               gMatchParameters[i].conflibAppID == ID_ACCESSMODE || \
               gMatchParameters[i].conflibAppID == ID_UPNPD || \
               ( gMatchParameters[i].conflibAppID == ID_TR069 && gMatchParameters[i].conflibParID == PARA_TR069_INFORMSTATUS ) || \
               ( gMatchParameters[i].conflibAppID == ID_SDKD && gMatchParameters[i].conflibParID == PARA_SDKD_PKM ) || \
               ( gMatchParameters[i].conflibAppID == ID_SDKD && gMatchParameters[i].conflibParID == PARA_SDKD_ALPHA_DELIM ) || \
               ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_WM_AUTO_CON ) || \
               ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_DHCPCD_AUTO_START ) || \
               ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_GW_PING_CHECK ) || \
               ( gMatchParameters[i].conflibAppID == ID_IGMPPROXY && gMatchParameters[i].conflibParID == PARA_IGMP_STATUS ) || \
               ( gMatchParameters[i].conflibAppID == ID_LOG && gMatchParameters[i].conflibParID == PARA_LOG_STATUS ) || \
               ( gMatchParameters[i].conflibAppID == ID_WAN && gMatchParameters[i].conflibParID == PARA_MNG_STATUS ) || \
               ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_MNG_DHCPCD_AUTO_START ) )
         {
               if ( strcmp ( value, "1" ) == 0 )
               {
                   strcpy ( result, "enable" );
               }
               else if ( strcmp ( value, "0" ) == 0 )
               {
                   strcpy ( result, "disable" );
               }
         }
         else
         {
            strcpy ( result, value );
         }

        /****Prepare for tr069 app action  ,read value before calling config_api_write_parameter****/
         char valueOriginal[MAX_LEN_OF_VALUE];
         if ( gMatchParameters[i].conflibAppID == ID_TR069 )
         {
            if ( 0 != config_api_read_parameter ( gMatchParameters[i].conflibAppID, gMatchParameters[i].conflibParID, valueOriginal ) )
            {
               TR069_ERROR ( "config_api_write_parameter Error for AppID: %d ParID:%d", gMatchParameters[i].conflibAppID,  gMatchParameters[i].conflibParID );
               return ret;
            }
			else
			{
			    if( strcmp( valueOriginal, "enable" ) == 0 )
			    {
			        memset( valueOriginal, 0, MAX_LEN_OF_VALUE );
					strcpy( valueOriginal, "1" );
			    }
				else if( strcmp( valueOriginal, "disable" ) == 0 )
				{
			        memset( valueOriginal, 0, MAX_LEN_OF_VALUE );
					strcpy( valueOriginal, "0" );				
				}
			}
         }
       
         if ( 0 != config_api_write_parameter ( gMatchParameters[i].conflibAppID, gMatchParameters[i].conflibParID, result ) )
         {
            TR069_ERROR ( "config_api_write_parameter Error for AppID: %d ParID:%d", gMatchParameters[i].conflibAppID,  gMatchParameters[i].conflibParID );
            return ret;
         }

         if (  gMatchParameters[i].conflibAppID == ID_DHCPD || \
               gMatchParameters[i].conflibAppID == ID_SNTP || \
               gMatchParameters[i].conflibAppID == ID_DMZ || \
               gMatchParameters[i].conflibAppID == ID_SERVICE || \
               gMatchParameters[i].conflibAppID == ID_WAN || \
               gMatchParameters[i].conflibAppID == ID_DNSMASQ || \
               gMatchParameters[i].conflibAppID == ID_ACCESSMODE || \
               gMatchParameters[i].conflibAppID == ID_UPNPD || \
               gMatchParameters[i].conflibAppID == ID_AUTOCM || \
               gMatchParameters[i].conflibAppID == ID_DDNS || \
               gMatchParameters[i].conflibAppID == ID_LOG || \
               gMatchParameters[i].conflibAppID == ID_IGMPPROXY )
         {
            TR069_DEBUG  ( "===Start refresh apps===" );
            config_act_refresh_apps ( gMatchParameters[i].conflibAppID, APP_RESTART );
            TR069_DEBUG  ( "===End   refresh apps===" );
         }

		 if( gMatchParameters[i].conflibAppID == ID_LAN )
		 {
            TR069_DEBUG  ( "===Start refresh LAN apps===" );
            #if 0
			config_act_refresh_apps(ID_TR069, APP_RESTART);
			config_act_refresh_apps(ID_LAN, APP_RESTART);
			config_act_refresh_apps(ID_THTTPD, APP_RESTART);
			config_act_refresh_apps(ID_DNSMASQ, APP_RESTART);
			#endif
			TR069_DEBUG  ( "===End refresh LAN apps===" );
		 }

		 if( gMatchParameters[i].conflibAppID == ID_SDKD || \ 
		 	( gMatchParameters[i].conflibAppID == ID_CAPL && gMatchParameters[i].conflibParID == PARA_APP_RAPL_SEL_POLICY ) || \
		 	( gMatchParameters[i].conflibAppID == ID_RAPL && gMatchParameters[i].conflibParID == PARA_APP_CAPL_SEL_POLICY ) )
		 {
		     TR069_DEBUG  ( "===Start refresh wimax apps===" );
			 config_act_refresh_apps ( gMatchParameters[i].conflibAppID, APP_RESTART );
			 config_act_refresh_apps(ID_AUTOCM, APP_RESTART);
			 TR069_DEBUG  ( "===End refresh wimax apps===" );
		 }
		 
         if ( gMatchParameters[i].conflibAppID == ID_TR069 )
         {
            /**Execute tr069 action if value is not equal to the original value**/
            if ( 0 != strcmp ( valueOriginal, value ) )
            {
               TR069_DEBUG  ( "===Start refresh tr069 apps===" );
               config_act_refresh_apps ( gMatchParameters[i].conflibAppID, APP_RESTART );
               TR069_DEBUG  ( "===End   refresh tr069 apps===" );
            }
            else
            {
               TR069_NOTICE ( "No need action for tr069!" );
            }
         }
         //config_api_write_to_flash ( );
         ret = APPLIED;
         break;
      }

/*********************Just to get row ID to prepare for writing os.conf with values of parameters which belong to object**********/
      /*So FLAG_INSTANCE_ID must be the fisrt for one app*/
      if ( gMatchParameters[i].conflibParID == FLAG_INSTANCE_ID && strstr ( name, gMatchParameters[i].tr069Name )  )
      {
         TR069_DEBUG ( "Find this parameter in object" );
         rowID = get_instanceid_match_row_id ( gMatchParameters[i].tr069Name, name );
         TR069_DEBUG ( "name: %s rowID: %d", name, rowID );

         if ( rowID == -1 )
         {
            TR069_ERROR ( "Error: get_instanceid_match_row_id " );
            return ret;
         }

         continue;
      }


/*********************Get parameter ID, and write os.conf with application ID, row ID and parameter ID******************/
      if ( gMatchParameters[i].conflibRowID == FLAG_TEMPLATE_ROW && gMatchParameters[i].conflibParID != FLAG_INSTANCE_ID && template_match_compare ( gMatchParameters[i].tr069Name, name )  )
      {
         parID = gMatchParameters[i].conflibParID;

         if ( parID && rowID )
         {
            /*For action start, there are 4 steps*/
            char* beforeValue[MAX_NUM_OF_ROW_PARA];
            char* afterValue[MAX_NUM_OF_ROW_PARA];
            char tmpBeforeResult[MAX_NUM_OF_ROW_PARA][MAX_LEN_OF_VALUE];
            char tmpAfterResult[MAX_NUM_OF_ROW_PARA][MAX_LEN_OF_VALUE];
            int a;
            char result[MAX_LEN_OF_VALUE];

			/* The 1st step: read all values of this row, saved in tmpBeforeResult */
            for ( a = 0; a < MAX_NUM_OF_ROW_PARA; a++ )
            {
               if ( -1 != config_api_read_table_parameter ( gMatchParameters[i].conflibAppID, rowID, a + 1, tmpBeforeResult[a] ) )
               {
                  beforeValue[a] = tmpBeforeResult[a];
               }
            }


            if ( gMatchParameters[i].conflibAppID == ID_ROUTE && gMatchParameters[i].conflibParID == 3 )
            {
               if ( strcmp ( value, "1" ) == 0 )
               {
                  strcpy ( result, "true" );
               }
               else if ( strcmp ( value, "0" ) == 0 )
               {
                  strcpy ( result, "true" );
               }
            }
			else if( gMatchParameters[i].conflibAppID == ID_ACCOUNT && gMatchParameters[i].conflibParID == PARA_ACCOUNT_CPASSWORD )
            {
                int cvrtPSW[PSW_Len+1];
                int k;
                char tBuf[3*PSW_Len+1];
                char tmpBuf[3*PSW_Len+1];
				char passwd[PSW_Len];
				strcpy( passwd, value );
				memset ( ( void* ) & ( cvrtPSW[0] ), 0, sizeof ( PSW_Len ) );

                for ( k = 0; k < PSW_Len; k++ )
                    convertPSW ( passwd[k], & ( cvrtPSW[k] ), k );

                strcpy ( tBuf, "" );

                for ( k = 0; k < PSW_Len; k++ )
                {
                    strcpy ( tmpBuf, tBuf );
                    sprintf ( tBuf, "%s%X:", tmpBuf, cvrtPSW[k] );
                }

                //printf ( "Convert PSW === (%s)\n", tBuf );
				strcpy ( result, tBuf );
            }
			else
            {
                strcpy ( result, value );
            }
            /*The 2ed step: call config_api_write_table_parameter */
            if ( 0 != config_api_write_table_parameter ( gMatchParameters[i].conflibAppID, rowID, parID, result ) )
            {
               TR069_ERROR ( "[config_api_write_table_parameter] Error for AppID: %d\t RowID: %d\t ParID: %d\n", gMatchParameters[i].conflibAppID, rowID, gMatchParameters[i].conflibParID );
               return ret;
            }
            /* The 3rd step: read all values of this row , saved in tmpAfterResult*/
            for ( a = 0; a < MAX_NUM_OF_ROW_PARA; a++ )
            {
               if ( -1 != config_api_read_table_parameter ( gMatchParameters[i].conflibAppID, rowID, a + 1, tmpAfterResult[a] ) )
               {
                  afterValue[a] = tmpAfterResult[a];
               }
            }

            //config_api_write_to_flash ( );
            /* The 4th step: use tmpBeforeResult and tmpAfterResult to execute action */
            if ( gMatchParameters[i].conflibAppID == ID_PORTFORWARD || \
                  gMatchParameters[i].conflibAppID == ID_FIREWALL || \
                  gMatchParameters[i].conflibAppID == ID_ROUTE || \
                  gMatchParameters[i].conflibAppID == ID_IPCONTROL )
            {
               TR069_DEBUG  ( "===Start refresh table===" );
               config_act_refresh_tables ( gMatchParameters[i].conflibAppID, RULE_DELETE, rowID, NULL );
               TR069_DEBUG  ( "===middle refresh table===" );
               config_act_refresh_tables ( gMatchParameters[i].conflibAppID, RULE_ADD, rowID, afterValue );
               TR069_DEBUG  ( "===End  refresh table===" );
            }
			/*wimax table action donot need tmpBeforeResult and tmpAfterResult*/
            if (  gMatchParameters[i].conflibAppID == ID_CHANNELPLAN || \
                  gMatchParameters[i].conflibAppID == ID_HNSP || \
                  gMatchParameters[i].conflibAppID == ID_CAPL || \
                  gMatchParameters[i].conflibAppID == ID_RAPL )
            {
               TR069_DEBUG  ( "===Start refresh wimax table action=== " );
               config_act_refresh_apps ( gMatchParameters[i].conflibAppID, APP_RESTART );
			   config_act_refresh_apps(ID_AUTOCM, APP_RESTART);
               TR069_DEBUG  ( "===end refresh wimax table action=== " );
            }
            ret = APPLIED;
            break;
         }
      }
   }
   if( ret != 0 )
   {
       TR069_NOTICE( "Set value failed, ret == %d\n", ret );
   }
   else
   {
       TR069_DEBUG( "Set value OK\n" );   
   }
   return ret;
}

/***********************************************************************
Function: tcmCB_get_value
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to get the configuration in demo.

Return: ApplyStatus
Input:   name
Output: value
Others:
************************************************************************/
ApplyStatus tcmCB_get_value ( char *name, char *value )
{

   TR069_DEBUG ( "name = %s", name );
   int i;
   ApplyStatus ret = APPLY_ERROR;

   if ( NULL == name )
   {
      return ret;
   }

   /*ConfigFile is an exception, whose style is not TCMAGENT_STATUS*/
   /*Actually, in template xml file, it is status*/
   if ( 0 == strcmp ( name, "InternetGatewayDevice.DeviceConfig.ConfigFile" ) )
   {
      TR069_NOTICE( "Get value failed, as not implemented this parameter" );
   }

   if( 0 == strcmp( name, "InternetGatewayDevice.DeviceInfo.UpTime" ) )
   {
       ret = get_device_info( name, value );
   }
   
   for ( i = 0; i < MAX_NUM_OF_MATCH_PARAMETERS; i++ )
   {
      if ( TCMAGENT_STATUS != gMatchParameters[i].status )
      {
         continue;
      }

      /*For parameters which donot belong to object*/
      if ( FLAG_NO_ROW == gMatchParameters[i].conflibRowID && strcmp ( gMatchParameters[i].tr069Name, name ) == 0 )
      {
         ret = ( * ( get_status_func ) gMatchParameters[i].baseAddress ) ( name, value );
         break;
      }

      /*For parameters which belong to object*/
      if ( FLAG_TEMPLATE_ROW == gMatchParameters[i].conflibRowID )
      {
         int rowID;

         if ( template_match_compare ( gMatchParameters[i].tr069Name, name ) )
         {
            rowID = get_noninstanceid_match_row_id (  gMatchParameters[i].tr069Name, name );
            TR069_DEBUG ( "rowID = %d name = %s", rowID, name );

            if ( rowID == -1 )
            {
               TR069_ERROR ( "Error: get_noninstanceid_match_row_id" );
               break;
            }

            ret = ( * ( get_multi_object_status_func ) gMatchParameters[i].baseAddress ) ( name, value, rowID );

            break;
         }
      }
   }

   if( 0 != ret )
   {
       TR069_ERROR( "Get value failed, ret == %d\n", ret );
   }
   else
   {
       TR069_DEBUG( "Get value OK, value %s\n", value );
   }
   return ret;
}
/***********************************************************************
Function: determingObjectIndex
Description:  get free and min row id of this table

Return: object index
Input:  
Output:
Others:
************************************************************************/
extern tcmEntityInfo entityInfoArray[];

int determingObjectIndex( int appIndex )
{
/****************test entityInfoArray**********/
#if 0
int a = sizeof(entityInfoArray)/sizeof(tcmEntityInfo);
int b;
for(b=0;b<a;b++)
{
printf( "maxrownum %d\n", entityInfoArray[b].row_maxnum );
}
#endif
/************************************************/
    char tmpResult[MAX_LEN_OF_VALUE];
	int i = -1;
	TR069_DEBUG( "This app's row_maxnum is %d", entityInfoArray[appIndex-1].row_maxnum );
	for( i =1 ; i < entityInfoArray[appIndex-1].row_maxnum; i++ )
	{
	    if( 0 > config_api_read_table_parameter( appIndex, i, 1, tmpResult ) )
	    {
            TR069_NOTICE ( "Get the free and min row index %d", i );		
	        return i;
	    }
	}

	if( i == entityInfoArray[appIndex-1].row_maxnum )
	{
        TR069_NOTICE ( "Table has the max number rows" );		
	    return 0;
	}
	return -1;
}
/***********************************************************************
Function: tcmCB_add_object
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to add the new instance of multi-node to the configuration in demo.

Return: ApplyStatus
Input:   name
            index
Output:
Others:
************************************************************************/
extern tcmEntityInfo entityInfoArray[];
ApplyStatus tcmCB_add_object ( char *name, int *pIndex )
{
   TR069_DEBUG ( "Start add object\tname: %s  index: %d", name, index );
   int i;
   ApplyStatus ret = APPLY_ERROR;

   if ( NULL == name )
   {
      return ret;
   }

   for ( i = 0; i < MAX_NUM_OF_MATCH_PARAMETERS; i++ )
   {
      if ( gMatchParameters[i].conflibParID != FLAG_INSTANCE_ID )
      {
         continue;
      }

      if ( strcmp ( gMatchParameters[i].tr069Name, name ) == 0 && gMatchParameters[i].conflibAppID != ID_ACCOUNT )
      {
          *pIndex = determingObjectIndex(gMatchParameters[i].conflibAppID);
	      if( *pIndex == 0 )
	      {
	          TR069_NOTICE( "There is no free row index" );
			  return -1;
	      }
		  else if( *pIndex == -1 )
		  {
		      TR069_ERROR( "Dertermine row free index error" );
			  return -1;
		  }
         char* value[MAX_NUM_OF_ROW_PARA];
         int j;

         for ( j = 0; j < MAX_NUM_OF_MATCH_PARAMETERS; j++ )
         {
            if ( gMatchParameters[j].conflibRowID == FLAG_TEMPLATE_ROW && \
                  gMatchParameters[j].conflibParID != FLAG_INSTANCE_ID  && \
                  gMatchParameters[j].conflibParID != FLAG_STATUS_STYLE && \
                  gMatchParameters[j].conflibAppID == gMatchParameters[i].conflibAppID )
            {
               if ( gMatchParameters[j].conflibParID >= 1 )
               {
                  if ( gMatchParameters[j].conflibAppID == ID_ROUTE && gMatchParameters[j].conflibParID == 3 )
                  {
                     if (  strcmp ( gMatchParameters[j].defaultValue, "1" ) == 0 )
                     {
                        value[ gMatchParameters[j].conflibParID - 1 ] = "true";
                     }
                     else
                     {
                        value[ gMatchParameters[j].conflibParID - 1 ] = "false";
                     }
                  }
                  else
                  {
                     value[ gMatchParameters[j].conflibParID - 1 ] = gMatchParameters[j].defaultValue;
                  }
				  TR069_DEBUG( "Find app's parameter, index %d default value %s", gMatchParameters[j].conflibParID, value[ gMatchParameters[j].conflibParID - 1 ] );
               }
            }
         }

         if ( 0 != config_api_add_table_row ( gMatchParameters[i].conflibAppID, *pIndex, value ) )
         {
            TR069_ERROR ( "config_api_add_table_row Error for AppID: %d\t RowID: %d", gMatchParameters[i].conflibAppID, *pIndex );
            return ret;
         }
         else
         {
            if (  gMatchParameters[i].conflibAppID == ID_PORTFORWARD || \
                  gMatchParameters[i].conflibAppID == ID_FIREWALL || \
                  gMatchParameters[i].conflibAppID == ID_ROUTE || \
                  gMatchParameters[i].conflibAppID == ID_IPCONTROL )
            {
               TR069_DEBUG  ( "===start refresh table RULE_ADD===" );
               config_act_refresh_tables ( gMatchParameters[i].conflibAppID, RULE_ADD, *pIndex, value );
			   ret = TCMRET_SUCCESS;
               TR069_DEBUG  ( "===end refresh table RULE_ADD===" );
			   break;
            }

            if (   gMatchParameters[i].conflibAppID == ID_CHANNELPLAN || \
                   gMatchParameters[i].conflibAppID == ID_HNSP || \
                   gMatchParameters[i].conflibAppID == ID_CAPL || \
                   gMatchParameters[i].conflibAppID == ID_RAPL )
            {
               TR069_DEBUG  ( "===start refresh wimax action===" );
               config_act_refresh_apps ( gMatchParameters[i].conflibAppID, APP_RESTART );
			   config_act_refresh_apps(ID_AUTOCM, APP_RESTART);
			   ret = TCMRET_SUCCESS;
               TR069_DEBUG  ( "===end refresh wimax action===" );
			   break;

            }
         }
      //config_api_write_to_flash ( );
      }
   }
   if( ret != 0 )
   {
       TR069_NOTICE( "Add object failed, ret == %d\n", ret );
   }
   else
   {
       TR069_DEBUG( "Add object OK\n", ret );       
   }
   return ret;
}

/***********************************************************************
Function: tcmCB_delete_object
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to delete the instance of multi-node from the configuration in demo.

Return: ApplyStatus
Input:   name
            index
Output:
Others:
************************************************************************/
ApplyStatus tcmCB_delete_object ( char *name, int index )
{
   TR069_DEBUG  ( "Begin delete object\tname: %s", name );
   int i;
   ApplyStatus ret = APPLY_ERROR;
   char rowIDStr[ MAX_INSTANCE_ID_LEN + 1 ];
   int rowID;
   int rowIDLen;/*assume max instance id index is 99*/

   /*"index" is not the real instance id index, but a strange num*/

   if ( NULL == name )
   {
      return ret;
   }

   /*instance index id is <10*/
   if ( * ( name + strlen ( name ) - TEMPLATE_LEN ) == '.' )
   {
      rowIDLen = MAX_INSTANCE_ID_LEN - 1 ;
      memcpy ( rowIDStr, name + strlen ( name ) - TEMPLATE_LEN + 1, rowIDLen );
      rowIDStr[rowIDLen] = '\0';
   }

   /*instance id index is >=10*/
   else if ( * ( name + strlen ( name ) - ( TEMPLATE_LEN + 1 ) ) == '.' )
   {
      rowIDLen = MAX_INSTANCE_ID_LEN ;
      memcpy ( rowIDStr, name + strlen ( name ) - ( TEMPLATE_LEN + 1 ) + 1, rowIDLen );
      rowIDStr[rowIDLen] = '\0';
   }
   else
   {
      TR069_NOTICE ( "Wrong name format" );
      return ret;
   }

   if ( !string_to_num ( rowIDStr, &rowID ) )
   {
      TR069_NOTICE ( "rowIDStr cannot be converted to number" );
      return ret;
   }

   for ( i = 0; i < MAX_NUM_OF_MATCH_PARAMETERS ; i++ )
   {
      if ( gMatchParameters[i].conflibParID == FLAG_INSTANCE_ID  && \
	  	   memcmp ( gMatchParameters[i].tr069Name, name, strlen ( name ) - rowIDLen - 1 ) == 0 && \
	  	   ( strlen( gMatchParameters[i].tr069Name ) == strlen ( name ) - rowIDLen - 1 ) && \
	  	   gMatchParameters[i].conflibAppID != ID_ACCOUNT )
      {
         if ( gMatchParameters[i].conflibAppID == ID_PORTFORWARD || \
               gMatchParameters[i].conflibAppID == ID_FIREWALL || \
               gMatchParameters[i].conflibAppID == ID_ROUTE || \
               gMatchParameters[i].conflibAppID == ID_IPCONTROL )
         {

            char* pValueTmp[MAX_NUM_OF_ROW_PARA];
            char valueTmp[MAX_NUM_OF_ROW_PARA][MAX_LEN_OF_VALUE];
            int k;

            for ( k = 0; k < MAX_NUM_OF_ROW_PARA; k++ )
            {
               memset ( valueTmp[k], 0, MAX_LEN_OF_VALUE );
               pValueTmp[k] = valueTmp[k];
            }

            if ( 0 != config_api_read_table_row ( gMatchParameters[i].conflibAppID, rowID, pValueTmp ) )
            {
               TR069_ERROR ( "config_api_read_table_row Error" );
               return ret;
            }
            TR069_DEBUG  ( "===refresh no-wimax action begin==="  );
            config_act_refresh_tables ( gMatchParameters[i].conflibAppID, RULE_DELETE, rowID, pValueTmp );
            TR069_DEBUG  ( "===refresh no-wimax action end===" );
         }

         if (  gMatchParameters[i].conflibAppID == ID_CHANNELPLAN || \
               gMatchParameters[i].conflibAppID == ID_HNSP || \
               gMatchParameters[i].conflibAppID == ID_CAPL || \
               gMatchParameters[i].conflibAppID == ID_RAPL )
         {
            TR069_DEBUG  ( "===start refresh wimax action===" );
            config_act_refresh_apps ( gMatchParameters[i].conflibAppID, APP_RESTART );
			config_act_refresh_apps(ID_AUTOCM, APP_RESTART);
            TR069_DEBUG  ( "===end refresh wimax action===" );
         }

         if ( 0 > config_api_delete_table_row ( gMatchParameters[i].conflibAppID, rowID ) )
         {
            TR069_ERROR ( "config_api_delete_row Error for appID:%d  rowID: %d", gMatchParameters[i].conflibAppID, rowID );
            return ret;
         }
         ret = TCMRET_SUCCESS;
		 break;
      //config_api_write_to_flash ( );
      }
   }
   if( ret != 0 )
   {
       TR069_NOTICE( "Delete object failed! ret == %d\n", ret );
   }
   else
   {
       TR069_DEBUG( "Delete object OK\n" );   
   }
   return ret;
}

/***********************************************************************
Function: tcmCB_upload
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to get the file to upload to the acs.

Return: 0 - success other - failure
Input:   fileType - log or configuration
Output: content - the file content
            fileSize - the file Size
Others:
************************************************************************/
int tcmCB_upload ( char * fileType, char **content, unsigned int *fileSize )
{

TR069_DEBUG  ( "Enter tcmCB_upload fileType %s\n", fileType );

   unsigned int size;
   unsigned int maxFileSize;
   FILE *fp;
   char fileName[MAX_FILENAME_LEN + 1];

   struct stat buf;

   if ( NULL == fileType, NULL == content || NULL == fileSize )
   {
      return -1;
   }

   if ( 0 == strcmp ( UPLOAD_CONFIG_TYPE, fileType ) )
   {
      strcpy ( fileName, configName );
      if( TCMRET_SUCCESS != actUtil_sendMsgToSched(TCM_MSG_CONFIGURATION_BACKUP_WEB, 0, NULL, 0) )
      {
          TR069_ERROR ( " Send TCM_MSG_CONFIGURATION_BACKUP to sched error\n" );
          return -1;
      }
	  sleep(2);

   }
   else if ( 0 == strcmp ( UPLOAD_LOG_TYPE, fileType ) )
   {
      strcpy ( fileName, logName );
	  if( TCMRET_SUCCESS != actUtil_sendMsgToSched(TCM_MSG_LOG_BACKUP, 0, NULL, 0) )
      {
          TR069_ERROR ( " Send TCM_MSG_LOG_BACKUP to sched error\n" );
          return -1;
      }
	  sleep(2);
   }
   else
   {
      TR069_ERROR ( "error file type: %d\n", fileType );
      return -1;
   }

   stat ( fileName, &buf );

   if ( ( unsigned int ) buf.st_size > MAX_FILE_SIZE )
   {
      maxFileSize = MAX_FILE_SIZE;
   }
   else
   {
      maxFileSize = ( unsigned int ) buf.st_size;
   }

   TR069_DEBUG ( "filename == %s\tmaxFileSize == %d\n", fileName, maxFileSize );

   if ( NULL == ( *content = ( char * ) malloc ( maxFileSize ) ) )
   {
      TR069_ERROR ( "malloc failure!\n" );
      return -1;
   }

   if ( NULL == ( fp = fopen ( fileName, "rb" ) ) )
   {
      TR069_ERROR ( "open file %s failure!\n", fileName );
      free ( *content );
      return -1;
   }
   else
   {
      if ( 0 > ( size = fread ( *content, 1, maxFileSize, fp ) ) )
      {
         TR069_ERROR ( "read file %s failure\n", fileName );
         free ( *content );
         fclose ( fp );
         return -1;
      }
      //else
      //   TR069_DEBUG ( "read content[%s]==\n", *content );
   }

   fclose ( fp );
   *fileSize = size;
   TR069_DEBUG ( "*fileSize == %d\n", *fileSize );
   return 0;
}

void stopProcess()
{
    int ret=0;
    ret=config_act_refresh_apps(ID_DHCPD,APP_STOP);
    TR069_DEBUG("ID_DHCPD,APP_STOP ret %s\n",ret);
    //ret=config_act_refresh_apps(ID_TR069,APP_STOP);
    //TR069_DEBUG("ID_TR069,APP_STOP ret %s\n",ret);
    ret=config_act_refresh_apps(ID_SNTP,APP_STOP);
    TR069_DEBUG("ID_SNTP,APP_STOP ret %s\n",ret);
    /*
    ret=config_act_refresh_apps(ID_SDKD,APP_STOP);
    wcm_printf("ID_SDKD,APP_STOP ret %s\n",ret);
    ret=config_act_refresh_apps(ID_AUTOCM,APP_STOP);
    wcm_printf("ID_AUTOCM,APP_STOP ret %s\n",ret);
    */
}
/**************************For wimax cert download **************/
tcmRet cert_clear_app(int cflID, int cflPara)
{
    tcmRet ret = TCMRET_SUCCESS;
	char tmpFileName[BUFLEN_64];
	char cmd_buf[1024];
	
    if ( config_api_read_parameter( cflID, cflPara, tmpFileName ) == TCMRET_SUCCESS )
	{
		/* Remove files under AUTH_DIR */
		sprintf(cmd_buf, "rm -f %s/%s"
				, AUTH_DIR
				, tmpFileName
			);

		system(cmd_buf);
	
	}
	else
    {
        tcmLog_error( "Fail to read the certs configuration");
        return TCMRET_CONFLIB_READ_ERROR;
    }
	
	return ret;
}

#if 1
void removeBlank(char* pfString)
{
    int i = 0;
    int j = 0;
    for ( i = 0; pfString[i] != '\0'; i++ )
    {
        if ( pfString[i] == ' ' )
        {
            continue;
        }
        else
        {
            pfString[j] = pfString[i];
            j++;
        }
    }
}

tcmRet parseTables(char* pfString, char** pfvalue, int maxnum)
{
   if ( pfString == NULL || pfvalue == NULL )
   {
      return TCMRET_INVALID_ARGUMENTS;
   }

   removeBlank(pfString);

   int i = 0;
   char* p = pfString;
   char* tok = NULL;

   while(1)
   {
       tok = strchr(p, ',');
       if ( tok == NULL )
       {
           strcpy(pfvalue[i], p);
           TR069_DEBUG("value[%d] = %s\n", i, p);
           break;
       }
       else
       {
            *tok = '\0';
            strcpy(pfvalue[i], p);
            TR069_DEBUG("value[%d] = %s\n", i, p);
            i++;
            p = tok + 1;
       }
   }

   return TCMRET_SUCCESS;
}

tcmRet ca_add_talbe(int cflID, int cflPara, char *savedValue)
{
    char actString[BUFLEN_128] = {0};
    int i = 0;
    tcmRet ret = TCMRET_SUCCESS;
    char pftmp[PARA_TAB_CERTS_END - 1][BUFLEN_32];
    char* pfvalue[PARA_TAB_CERTS_END - 1];
    /* For reading tables */
    for ( i = 0; i < PARA_TAB_CERTS_END -1; i++ )
    {
        pfvalue[i] = pftmp[i];
        memset(pftmp[i], 0, BUFLEN_32);
    }


	strncpy ( actString, savedValue, sizeof ( actString ) - 1 );
	if (( ret = parseTables(actString, pfvalue, PARA_TAB_CERTS_END) ) != TCMRET_SUCCESS )
	{
		return ret;
	}
	TR069_DEBUG("Start config_act_advCheck_tables(%d)\n", ret);
	ret = config_act_advCheck_tables(cflID, pfvalue);
	if ( ret == TCMRET_INVALID_SAME )
	{
	    TR069_NOTICE( "There is the same file under /etc/sysconfig/os/certs dir" );
		/*remove cert file under /etc/sysconfig/os/cert dir*/
	    if( (ret = config_act_refresh_tables(cflID, RULE_DELETE, 0, pfvalue)) != TCMRET_SUCCESS )
	    {
	        TR069_ERROR( "config_act_refresh_tables for RULE_DELETE " );
			return ret;
	    }
		return ret;
	}

	if( ret == TCMRET_SUCCESS )
	{
	    TR069_DEBUG("There is NO the same file under /etc/sysconfig/os/certs dir Start config_api_add_table_row(%d)\n", ret);
	    /*add it to os.conf*/
	    if ( (ret = config_api_add_table_row(cflID, 0, pfvalue)) != TCMRET_SUCCESS )
	    {
		    return ret;
	    }
	    TR069_DEBUG("Start config_act_refresh_tables(%d)\n", ret);
		/*No real act*/
	    if (( ret = config_act_refresh_tables(cflID, RULE_ADD, 0, pfvalue)) != TCMRET_SUCCESS )
	    {
		    return ret;
	    }
	}
	return ret;

}
#endif
/****************************************************************/

/***********************************************************************
Function: tcmCB_download
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to notify the demo the download file.

Return: 0 - success other - failure
Input:   fileType - configuration file or firmware
            targetFileName - the target file name
            content - the file content
            fileSize - the file size.
Output:
Others:
************************************************************************/

int tcmCB_download ( char * fileType, char *targetFileName, char *content, unsigned int fileSize )
{
   TR069_DEBUG ( "Enter tcmCB_download fileType %s\t targetFileName %s\t fileSize %d\n", fileType, targetFileName, fileSize );
   FILE *fp;
   char fileName[MAX_FILENAME_LEN + 1];
   char softwareVersion[SOFTWARE_VERSION_LEN + 1];
   char currentTime[32];
   char demoConfigFile[128];
   int len, currentPos = -1, *tl;
   char cl[256], *pcl;
   char sendBuf[MAX_SEND_MSG_LEN], *pBuf;

   int certType = 0;
   if ( NULL == content || NULL == fileType )
   {
      return -1;
   }

   if ( 0 == strcmp ( DOWNLOAD_FIRMWARE_TYPE, fileType ) )
   {
      TR069_DEBUG ( "Download file type is fireware!\n" );
      strcpy ( fileName, FIRMWARE_DIR );

         strcat ( fileName, FIRMWARE_NAME );

      #if 0  /* Donot care firmware version*/
      /* Typically, the CPE should parse the software version information
       * from the firmware. In this demo implementation, we just simulate
       * a fake software version number according to current time. */
      pal_datetime ( currentTime );

      strncpy ( softwareVersion, "firmware_", SOFTWARE_VERSION_LEN );

      len = SOFTWARE_VERSION_LEN - strlen ( softwareVersion );

      strncat ( softwareVersion, currentTime, len );


      /* notify acs server */
      tl = ( int * ) sendBuf; /* the first 4 bytes stands for the length of the msg */

      memset ( sendBuf, 0, MAX_SEND_MSG_LEN );

      pBuf = sendBuf + sizeof ( int );

      put_msg_type ( tl, &pBuf, SET_VALUE );

      put_msg_name ( tl, &pBuf, "InternetGatewayDevice.DeviceInfo.SoftwareVersion"
                     , strlen ( "InternetGatewayDevice.DeviceInfo.SoftwareVersion" ) + 1 );

      put_msg_value ( tl, &pBuf, TCMAGENT_STRING, softwareVersion );

      notify_tcmAgent ( sendBuf );
	  #endif

   }
   else if ( 0 == strcmp ( DOWNLOAD_CONFIG_TYPE, fileType ) )
   {
      strcpy ( fileName, CONFIG_DIR );
      strcat ( fileName, DEFAULT_CONFIG_NAME );/*Don't care targetFileName*/
   }
   else if ( 0 == strcmp ( DOWNLOAD_WEBCONTENT_TYPE, fileType ) )
   {
      TR069_DEBUG ( "===enter webcontent=[%d]=fileSize[%d]=\n", strlen ( targetFileName ), fileSize );
      strcpy ( fileName, WEBCONTENT_DIR );

      if ( strlen ( targetFileName ) > 0 )
      {
         strcat ( fileName, targetFileName );
		 int fileNameLen = strlen(targetFileName);
		 int caCertSuffixLen = strlen(CA_CERT_SUFFIX);
		 int clientCertSuffixLen = strlen(CLIENT_CERT_SUFFIX);
		 int keyCertSuffixLen = strlen(KEY_CERT_SUFFIX);
		 if( strcmp( targetFileName + ( fileNameLen - caCertSuffixLen ), CA_CERT_SUFFIX ) == 0 )
		 {
		    certType = CA_CERT_FLAG;
		 }
		 else if( strcmp( targetFileName + ( fileNameLen - clientCertSuffixLen ), CLIENT_CERT_SUFFIX ) == 0 )
		 {
		    certType = CLIENT_CERT_FLAG;
		 }
		 else if( strcmp( targetFileName + ( fileNameLen - keyCertSuffixLen ), KEY_CERT_SUFFIX ) == 0 )
		 {
		    certType = KEY_CERT_FLAG;
		 }
		 else
		 {
            TR069_ERROR ( "Target location(optional) format is wrong, as its suffix must be one of 'ca.pem' ,'client.pem' and 'key.pem'");
            return -1;
		 }
		 TR069_DEBUG( "fileName %s certType %d", fileName, certType  );
		 	
      }
      else
      {
          TR069_ERROR ( "Target location(optional) is requried, and its suffix must be one of 'ca.pem' ,'client.pem' and 'key.pem'");
          return -1;
      }
      TR069_DEBUG ( "===exit webcontent==\n" );
   }
   else
   {
      TR069_ERROR ( "error file type: %d\n", fileType );
      return -1;
   }

   if ( NULL == ( fp = fopen ( fileName, "wb" ) ) )
   {
      TR069_ERROR ( "open file %s failure!\n",  fileName );
      return -1;
   }
   else
   {
      if ( 0 > fwrite ( content, fileSize, 1, fp ) )
      {
         TR069_ERROR ( "write file %s failure\n", fileName );
         fclose ( fp );
         return -1;
      }
   }

   fclose ( fp );

   if ( 0 == strcmp ( DOWNLOAD_FIRMWARE_TYPE, fileType ) )
   {
       char img_path[64]="/var/update/GCT-FSSU.img";
       
	   if( 0 != actUtil_sendMsgToSched(TCM_MSG_SYSTEM_UPDATE, 0,img_path, strlen(img_path) + 1) )
       {
           TR069_ERROR ( "Send TCM_MSG_SYSTEM_UPDATE Error\n" );
		   return -1;
       }

   }
   else if ( 0 == strcmp ( DOWNLOAD_CONFIG_TYPE, fileType ) )
   {
       char restoreFileName[64]="/var/update/os.conf";
       if(TCMRET_SUCCESS==actUtil_sendMsgToSched(TCM_MSG_CONFIGURATION_RESTORE, 0, restoreFileName, strlen(restoreFileName) + 1 ))
	   {
	       config_api_uninit_library();
	   }
	   else
	   {
		   TR069_ERROR("Send TCM_MSG_CONFIGURATION_RESTORE error\n");
		   return -1;
	   }
   }
   else if ( 0 == strcmp ( DOWNLOAD_WEBCONTENT_TYPE, fileType ) )
   {
       char cmd_buf[1024];
       if( certType == CA_CERT_FLAG )
       {
           /*add table:check whether the file is existed in /etc/sysconfig/os/certs dir
		   if yes,remove the file.or add it to os.conf*/
		   if( TCMRET_SUCCESS != ca_add_talbe( ID_CERTS, PARA_TAB_CERTS_CA_CERT, targetFileName ) )
		   {
		       return -1;
		   }
		   

       }
	   else if( certType == CLIENT_CERT_FLAG )
	   {
	       /*First step: remove cert file under /etc/sysconfig/os/certs dir*/
           if( TCMRET_SUCCESS != cert_clear_app( ID_CERTS, PARA_APP_CERTS_CLIENT_CERT ) )
           {
               TR069_ERROR( "cert_clear_app for PARA_APP_CERTS_CLIENT_CERT failed!" );
               return -1;
           }
		   /*Second step:write parameter to conflib*/
		   if( 0 != config_api_write_parameter( ID_CERTS, PARA_APP_CERTS_CLIENT_CERT, targetFileName ) )
		   {
		       TR069_ERROR( "config_api_write_parameter for PARA_APP_CERTS_CLIENT_CERT failed,and value %s", targetFileName );
		       return -1;
		   }		   
	   }
	   else if( certType == KEY_CERT_FLAG )
	   {
	       /*First step: remove cert file under /etc/sysconfig/os/certs dir*/
           if( TCMRET_SUCCESS != cert_clear_app( ID_CERTS, PARA_APP_CERTS_PRIVATE_KEY ) )
           {
               TR069_ERROR( "cert_clear_app for PARA_APP_CERTS_PRIVATE_KEY failed!" );
               return -1;
           }
		   /*Second step:write parameter to conflib*/
		   if( 0 != config_api_write_parameter( ID_CERTS, PARA_APP_CERTS_PRIVATE_KEY, targetFileName ) )
		   {
		       TR069_ERROR( "config_api_write_parameter for PARA_APP_CERTS_PRIVATE_KEY failed,and value %s", targetFileName );
		       return -1;
		   }
	   }
           /*These two steps are communal*/
		   /*Third step: mv /var/update/filename to /etc/sysconfig/wimax/certs/filename*/
		   	sprintf(cmd_buf, "mv -f %s%s %s/%s", WEBCONTENT_DIR, targetFileName, AUTH_DIR, targetFileName );
		    system(cmd_buf);
			sleep(3);/*wait for system*/
		   /*Fourth step: restart app*/
		   if( TCMRET_SUCCESS != config_act_refresh_apps(ID_SDKD, APP_RESTART) )
		   {
		       TR069_ERROR( "config_act_refresh_apps restart failed" );
			   return -1;
		   }	   
   }
   else
   {
   
   }
   TR069_DEBUG ( "Leave tcmCB_download\n" );
   return 0;
}

/***********************************************************************
Function: tcmCB_reboot
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to notify the demo the reboot command sended by the acs.

Return: 0 - success other - failure
Input:
Output:
Others:
************************************************************************/
int tcmCB_reboot ( void )
{
   TR069_DEBUG ( "tcm agent reboot system!\n" );
   return actUtil_sendMsgToSched(TCM_MSG_SYSTEM_REBOOT, 0, NULL, 0);
}

/***********************************************************************
Function: tcmCB_factory_reset
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to notify the demo the factory reset command sended by the acs.

Return: 0 - success other - failure
Input:
Output:
Others:
************************************************************************/
int tcmCB_factory_reset ( void )
{
   TR069_DEBUG ( "tcm agent factory reset!\n" );
   if(TCMRET_SUCCESS==actUtil_sendMsgToSched(TCM_MSG_CONFIGURATION_RESET, 0, NULL, 0))
   {
       config_api_uninit_library();   
   }
   else
   {
	   TR069_ERROR("Send TCM_MSG_CONFIGURATION_RESET error");
	   return -1;
   }
   return 0;
}

/***********************************************************************
Function: tcmCB_get_wan_ipaddress
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to get the wan ip address.

Return: 0 - success other - failure
Input:
Output: ipaddr
Others:
************************************************************************/
int tcmCB_get_wan_ipaddress ( char *ipaddr )
{
   int ret = 0;
#ifdef VXWORKS
   TC_GET_LOCAL_IP_ADDRESS ( ipaddr );

#else
#if 0
   register int fd, intrface;

   struct ifreq buf[MAXINTERFACES];

   struct arpreq arp;

   struct ifconf ifc;

   if ( ( fd = socket ( AF_INET, SOCK_DGRAM, 0 ) ) >= 0 )
   {
      ifc.ifc_len = sizeof buf;
      ifc.ifc_buf = ( caddr_t ) buf;

      if ( !ioctl ( fd, SIOCGIFCONF, ( char * ) &ifc ) )
      {
         intrface = ifc.ifc_len / sizeof ( struct ifreq );

         if ( intrface-- > 0 )
         {
            if ( ! ( ioctl ( fd, SIOCGIFADDR, ( char * ) &buf[intrface] ) ) )
            {
               strcpy ( ipaddr, inet_ntoa ( ( ( struct sockaddr_in* ) ( &buf[intrface].ifr_addr ) )->sin_addr ) );
            }
            else
            {
               ret = -1;
            }
         }
         else
         {
            ret = -1;
         }
      }
      else
      {
         ret = -1;
      }

      close ( fd );
   }
   else
   {
      ret = -1;
   }
#else
   // Get the management inreface IP address
   char wanManageIfc[BUFLEN_32];
   memset(wanManageIfc, 0, sizeof(wanManageIfc));
   if (actUtil_GetManageWanIfcName(wanManageIfc) == TCMRET_SUCCESS && tcm_getIfAddr(wanManageIfc, ipaddr))
   {
      TR069_DEBUG("Get the wan management interface name %s, and ip %s", wanManageIfc, ipaddr);
      ret = 0;
   }
   else
   {
      ret = -1;
   }
#endif
#endif
   return ret;
}

/***********************************************************************
Function: tcmCB_kill_other_apps
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to kill other apps.

Return: 0 - success other - failure
Input:
Output: ipaddr
Others:
************************************************************************/
int tcmCB_kill_other_apps ()
{
   TR069_DEBUG ( "===kill other non important apps==\n" );
   stopProcess();
   return 0;
}

/***********************************************************************
Function: tcmCB_get_clientkey_passwd
Description:  the callback function which is used by tcmAgent.
                   tcmAgent calls the function to get the client key password.

Return: 0 - success other - failure
Input:
Output: password
            verify
Others:
************************************************************************/
int tcmCB_get_clientkey_passwd ( char *password, int *verify )
{
   /* Please use the correct client key password, the following just are the sample */
   strcpy ( password, "123456" );
   *verify = 0;
   return 0;
}

/***********************************************************************
Function: put_signle_node
Description:  put the single node of configuration to the message buffer.

Return:
Input:   msgType
            name
            valueType
            value
            bufPtr
            totalLen
Output: bufPtr
            totalLen
Others:
************************************************************************/
void put_single_node ( MsgType msgType, char *name, MsgValueType valueType, char *value, char **bufPtr, int *totalLen )
{
   /* put msg type */
   put_msg_type ( totalLen, bufPtr, msgType );

   /* put msg name */
   put_msg_name ( totalLen, bufPtr, name, strlen ( name ) + 1 );

   /* put msg value */

   put_msg_value ( totalLen, bufPtr, valueType, value );


}

/***********************************************************************
Function: notify_tcmAgent
Description:  send the message to the tcmAgent and get the response.

Return: 0 - success other - failure
Input:   buf - message buffer
Output:
Others:
************************************************************************/
int notify_tcmAgent ( char *buf )
{
   TR069_DEBUG ( "Enter notify_tcmAgent" );
   unsigned int *totalLen;
   char recvBuf[MAX_RECV_MSG_LEN];
   char errorName[MAX_TR069_NAME_LEN + 1];
   char *bufPtr;
   MsgType type;


   if ( send_msg_to_tcmAgent ( buf, MAX_SEND_MSG_LEN ) < 0 )
   {
      TR069_ERROR ( "send msg to tcm agent error!" );
      return -1;
   }
   else
   {
      TR069_DEBUG ( "send_msg_to_tcmAgent succeed!" );
   }
#if 0
   if ( get_msg_from_tcmAgent ( recvBuf, MAX_RECV_MSG_LEN ) < 0 )
   {
      TR069_ERROR ( "client recv msg from tcm agent error!\n" );
      return -1;
   }
   else
   {
      TR069_DEBUG ( "get_msg_from_tcmAgent succeed!\n" );
   }

   totalLen = recvBuf;

   bufPtr = ( recvBuf + sizeof ( unsigned int ) );

   while ( *totalLen > 0 )
   {
      get_msg_type ( totalLen, &bufPtr, &type );

      if (  MSG_OK != type )
      {
         TR069_ERROR ( " client get response error %d\n", ( int ) type );
         break;
      }
      else if ( *totalLen > 0 )
      {
         unsigned int index;
         char name[MAX_TR069_NAME_LEN + 1];

         get_msg_name ( &bufPtr, totalLen, name );
         get_msg_value ( &bufPtr, totalLen, &index );

         TR069_DEBUG ( " client get response name-%s, value-%d\n", name, index );

      }
      else
      {
         TR069_DEBUG ( "client get response ok!\n" );
      }
   }
#endif
   TR069_DEBUG ( " Leave notify_tcmAgent" );

   return 0;
}

void idle ( void )
{
   while ( 1 )
   {
      sleep ( 60 );
   }
}

int  send_init_end ( void )
{
   char sendBuf[MAX_SEND_MSG_LEN];
   unsigned int *totalLen = ( unsigned int * ) sendBuf;
   char *bufPtr = sendBuf + sizeof ( int ); /* now , i assume the int is 4 bytes */

   *totalLen = 0;

   *bufPtr = ( int ) INIT_END;
   *totalLen += sizeof ( MsgType );
   return  notify_tcmAgent ( sendBuf );
}


void put_object ( MsgType msgType, char *name, char **bufPtr, int *totalLen )
{
   /* put msg type */
   put_msg_type ( totalLen, bufPtr, msgType );

   /* put msg name */
   put_msg_name ( totalLen, bufPtr, name, strlen ( name ) + 1 );
}


int judge_msg_total_length ( int usedLength, char* name, char* value )
{
   int totalLength = usedLength + MAX_MSG_TYPE_LEN + MAX_LEN_FOR_MSG_NAME_LENGTH + strlen ( name ) + 1 + MAX_LEN_FOR_MSG_VALUE_TYPE + strlen ( value ) + 1;

   if (  totalLength < MAX_SEND_MSG_LEN )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}


/***********************************************************************
Function: constructInstanceName
Description:
instanceTr069Name:InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.
instanceIndex:8
resultNameStr:InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.8.

Return: 0 - success other - failure
Input:
Output:
Others:
************************************************************************/

int constructInstanceName ( char* instanceTr069Name, int instanceIndex, char* resultNameStr )
{
   if ( instanceTr069Name == NULL || resultNameStr == NULL || * ( instanceTr069Name + strlen ( instanceTr069Name ) - 1 ) != '.' )
   {
      return -1;
   }

   sprintf ( resultNameStr, "%s%d.", instanceTr069Name, instanceIndex );

   return 0;
}

/***********************************************************************
Function: constructInstanceName
Description:
instanceTr069Name:InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.0.Name
instanceIndex:8
resultNameStr:InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.8.Name

Return: 0 - success other - failure
Input:
Output:
Others:
************************************************************************/

int getInstanceParName ( char* instanceTr069Name, int instanceIndex, char* resultNameStr )
{
   if ( instanceTr069Name == NULL || resultNameStr == NULL )
   {
      return -1;
   }

   char* ptempStart = strstr ( instanceTr069Name, ".0." );

   if ( ptempStart != NULL )
   {
      strncpy ( resultNameStr, instanceTr069Name, ptempStart - instanceTr069Name );
      resultNameStr[ptempStart - instanceTr069Name] = '\0';
      sprintf ( resultNameStr, "%s.%d.%s", resultNameStr, instanceIndex, ptempStart + 3 );
   }
   else
   {
      return -1;
   }

   return 0;
}


int build_tcmAgent_parameters ( void )
{

   TR069_DEBUG ( "Enter build_tcmAgent_parameter" );

   char sendBuf[MAX_SEND_MSG_LEN];
   unsigned int *totalLen = ( unsigned int * ) sendBuf;
   char *bufPtr = sendBuf + sizeof ( int );
   *totalLen = 0;

   int i;
   char result[MAX_LEN_OF_VALUE];
   char valueResult[MAX_LEN_OF_VALUE];

   for ( i = 0; i < MAX_NUM_OF_MATCH_PARAMETERS ; i++ )
   {
      memset ( result, 0, MAX_LEN_OF_VALUE );

      /******Add Default Object********/

      if ( gMatchParameters[i].conflibAppID == FLAG_ADD_DEFAULT_OBJECT )
      {
         if ( !judge_msg_total_length ( ( int ) *totalLen, gMatchParameters[i].tr069Name, "" ) )
         {
            TR069_NOTICE ( "Msg total length will exceed its max when this msg is added" );
            notify_tcmAgent ( sendBuf );
            *totalLen = 0;
            bufPtr = sendBuf + sizeof ( int );
         }

         TR069_DEBUG ( "Add Default Object\t%s", gMatchParameters[i].tr069Name );

         put_object ( ADD_OBJECT, gMatchParameters[i].tr069Name, &bufPtr, totalLen );
      }

      /******Add Instance Object*********/
      if ( gMatchParameters[i].conflibParID == FLAG_INSTANCE_ID )
      {
         int k;
         int j;
         int index;

         for ( k = 1; k < gMatchParameters[i].maxNumInstance + 1; k++ )
         {
            /*Check whether the row exists in os.conf by reading the 1st parameter of this row*/
            if ( 0 != config_api_read_table_parameter ( gMatchParameters[i].conflibAppID, k, 1, result ) )
            {
               continue;
            }
            else
            {
               index = k;
               char instanceNameStr[MAX_LEN_OF_NAME];

               if ( 0 != constructInstanceName ( gMatchParameters[i].tr069Name, index, instanceNameStr ) )
               {
                  TR069_ERROR ( "constructInstanceName Error for %s", gMatchParameters[i].tr069Name );
                  return -1;
               }

               if ( !judge_msg_total_length ( ( int ) *totalLen, gMatchParameters[i].tr069Name, "" ) )
               {
                  TR069_NOTICE ( "Msg total length will exceed its max when this msg is added" );
                  notify_tcmAgent ( sendBuf );
                  *totalLen = 0;
                  bufPtr = sendBuf + sizeof ( int );
               }

               TR069_DEBUG ( "Add Object\t%s", instanceNameStr );

               put_object ( ADD_OBJECT, instanceNameStr, &bufPtr, totalLen );
            }
         }
      }


      /*Set values of parameters which donot belong to object*/

      if ( gMatchParameters[i].conflibRowID == FLAG_NO_ROW && gMatchParameters[i].conflibParID != FLAG_STATUS_STYLE )
      {
         if ( 0 != config_api_read_parameter ( gMatchParameters[i].conflibAppID, gMatchParameters[i].conflibParID, result ) )
         {
            continue;
         }
         else
         {
            if ( !judge_msg_total_length ( ( int ) *totalLen, gMatchParameters[i].tr069Name, result ) )
            {
               TR069_NOTICE ( "Msg total length will exceed its max when this msg is added" );
               notify_tcmAgent ( sendBuf );
               *totalLen = 0;
               bufPtr = sendBuf + sizeof ( int );
            }

            if ( ( gMatchParameters[i].conflibAppID == ID_DHCPD && gMatchParameters[i].conflibParID == PARA_DHCPD_STATUS ) || \
                  ( gMatchParameters[i].conflibAppID == ID_TR069 && gMatchParameters[i].conflibParID == PARA_TR069_INFORMSTATUS ) || \
                  gMatchParameters[i].conflibAppID == ID_SERVICE || \
                  ( gMatchParameters[i].conflibAppID == ID_ACCESSMODE && gMatchParameters[i].conflibParID == 1  ) || \
                  gMatchParameters[i].conflibAppID == ID_UPNPD || \
                  ( gMatchParameters[i].conflibAppID == ID_SDKD && gMatchParameters[i].conflibParID == PARA_SDKD_PKM ) || \
                  ( gMatchParameters[i].conflibAppID == ID_SDKD && gMatchParameters[i].conflibParID == PARA_SDKD_ALPHA_DELIM ) || \
                  ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_WM_AUTO_CON ) || \
                  ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_DHCPCD_AUTO_START ) || \
                  ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_GW_PING_CHECK ) || \
                  ( gMatchParameters[i].conflibAppID == ID_IGMPPROXY && gMatchParameters[i].conflibParID == PARA_IGMP_STATUS  ) || \
                  ( gMatchParameters[i].conflibAppID == ID_LOG && gMatchParameters[i].conflibParID == PARA_LOG_STATUS ) || \
                  ( gMatchParameters[i].conflibAppID == ID_WAN && gMatchParameters[i].conflibParID == PARA_MNG_STATUS )  || \
                  ( gMatchParameters[i].conflibAppID == ID_AUTOCM && gMatchParameters[i].conflibParID == PARA_AUTOCM_MNG_DHCPCD_AUTO_START ) )
            {
               if ( strcmp ( result, "enable" ) == 0  )
               {
                  strcpy ( result, "1" );
               }
               else if ( strcmp ( result, "disable" ) == 0  )
               {
                  strcpy ( result, "0" );
               }
            }



            TR069_DEBUG ( "put_single_node of no row for %s=%s", gMatchParameters[i].tr069Name, result );

            if ( TCMAGENT_STRING == gMatchParameters[i].type )
               put_single_node ( SET_VALUE, gMatchParameters[i].tr069Name, gMatchParameters[i].type, result, &bufPtr, totalLen );
            else
            {
               int num;

               if ( string_to_num ( result, &num ) )
               {
                  put_single_node ( SET_VALUE, gMatchParameters[i].tr069Name, gMatchParameters[i].type, &num, &bufPtr, totalLen );
               }
               else
               {
                  TR069_ERROR ( "string_to_num Error for %s", result );
                  return -1;
               }
            }

            continue;
         }
      }

      /*Set values of row parameters from os.conf which belong to object*/
      if ( gMatchParameters[i].conflibRowID == FLAG_TEMPLATE_ROW && gMatchParameters[i].conflibParID != FLAG_INSTANCE_ID && gMatchParameters[i].conflibParID != FLAG_STATUS_STYLE )
      {
         int k;
         int j;
         int index;

         for ( k = 0; k < gMatchParameters[i].maxNumInstance; k++ )
         {
            memset ( result, 0, MAX_LEN_OF_VALUE );

            if ( 0 != config_api_read_table_parameter ( gMatchParameters[i].conflibAppID, k, gMatchParameters[i].conflibParID, result ) )
            {
               continue;
            }
            else
            {
               /*Find a row in os.conf which ID is k*/
               index = k;
               char instanceParNameStr[MAX_LEN_OF_NAME];

               if ( gMatchParameters[i].conflibAppID == ID_ROUTE && gMatchParameters[i].conflibParID == 3 )
               {
                  if ( strcmp ( result, "true" ) == 0 )
                  {
                     strcpy ( result, "1" );
                  }
                  else if ( strcmp ( result, "false" ) == 0 )
                  {
                     strcpy ( result, "0" );
                  }
               }


               if( gMatchParameters[i].conflibAppID == ID_ACCOUNT && gMatchParameters[i].conflibParID == PARA_ACCOUNT_CPASSWORD )
               {
                   char resultTmp[MAX_LEN_OF_VALUE];
                   revertPSW ( result, resultTmp );
				   memset( result, 0, MAX_LEN_OF_VALUE );
				   strcpy( result, resultTmp );
               }
			
               if ( 0 != getInstanceParName ( gMatchParameters[i].tr069Name, k, instanceParNameStr ) )
               {
                  TR069_ERROR ( "getInstanceParName Error for %s",  gMatchParameters[i].tr069Name );
                  return -1;
               }

               if ( !judge_msg_total_length ( ( int ) *totalLen, gMatchParameters[i].tr069Name, result ) )
               {
                  TR069_NOTICE ( "Msg total length will exceed its max when this msg is added" );
                  notify_tcmAgent ( sendBuf );
                  *totalLen = 0;
                  bufPtr = sendBuf + sizeof ( int );
               }

               TR069_DEBUG ( "put_single_node of row for %s=%s", instanceParNameStr, result );

               if ( TCMAGENT_STRING == gMatchParameters[i].type )
               {
                  put_single_node ( SET_VALUE, instanceParNameStr, gMatchParameters[i].type, result, &bufPtr, totalLen );
               }
               else
               {

                  int num;

                  if ( string_to_num ( result, &num ) )
                  {
                     put_single_node ( SET_VALUE, instanceParNameStr, gMatchParameters[i].type, &num, &bufPtr, totalLen );
                  }
                  else
                  {
                     TR069_ERROR ( "string_to_num Error for %s", result );
                     return -1;
                  }
               }
            }
         }

      }
   }

    char wanIp[16];
	if ( tcmCB_get_wan_ipaddress (wanIp) >= 0)
	{
	    char url[32];
		sprintf( url, "http://%s:%d/crq", wanIp, CPE_CRQ_PORT );
	    if ( !judge_msg_total_length ( ( int ) *totalLen, "InternetGatewayDevice.ManagementServer.ConnectionRequestURL", url ) )
        {
            TR069_NOTICE ( "Msg total length will exceed its max when this msg is added" );
            notify_tcmAgent ( sendBuf );
            *totalLen = 0;
            bufPtr = sendBuf + sizeof ( int );
        }
        put_single_node ( SET_VALUE, "InternetGatewayDevice.ManagementServer.ConnectionRequestURL", TCMAGENT_STRING, url, &bufPtr, totalLen );
	}
	
   if ( -1 == notify_tcmAgent ( sendBuf ) )
   {
      TR069_ERROR ( "Notify_tcmAgent Error!" );
      return -1;
   }
   else
   {
      TR069_DEBUG ( "Notify_tcmAgent Success!" );
   }

   send_init_end();
   TR069_DEBUG( "build_tcmAgent_parameters successfully!!!" );
   return 0;

}

void* msgHandle = NULL;
const tcmEntityId myId = ID_TR069;

int main ( int argc, char *argv[] )
{
   tcmRet ret = TCMRET_FAILURE;

   tcmLog_init ( myId );
  // tcmLog_setDestination ( LOG_DEST_STDERR );
  // tcmLog_setLevel ( LOG_LEVEL_DEBUG );

   if ( access ( TCMAGENT_LOG_SWITCH_FILE, R_OK ) == 0 )
   {
       init_tcmAgent_log();
   }
   
   if ( ( ret = tcmMsg_init ( myId, &msgHandle ) ) != TCMRET_SUCCESS )
   {
      tcmLog_cleanup();
      return ret;
   }

   //config_api_release_library();
   if ( ( ret = config_api_init_library() ) < 0 )
   {
      tcmMsg_cleanup ( &msgHandle );
      tcmLog_cleanup();
      return ret;
   }

   config_api_disable_send_ACS();

   /* Init the act library, save the app ID and app message,
   so you can use the act library to send message and call other functions */

   if ( ( ret = actLib_init ( myId, msgHandle ) ) != TCMRET_SUCCESS )
   {
      config_api_uninit_library();
      tcmMsg_cleanup ( &msgHandle );
      tcmLog_cleanup();
      return ret;
   }

   stop_tcmAgent ();

   TcmAgentCB cb;

   struct stat buf, bufBak;

   memset ( &cb, 0, sizeof ( cb ) );

   cb.session_begin = tcmCB_session_begin;

   cb.session_end = tcmCB_session_end;

   cb.set_value = tcmCB_set_value;

   cb.get_value = tcmCB_get_value;

   cb.add_object = tcmCB_add_object;

   cb.delete_object = tcmCB_delete_object;

   cb.do_upload = tcmCB_upload;

   cb.do_download = tcmCB_download;

   cb.do_reboot = tcmCB_reboot;

   cb.factory_reset = tcmCB_factory_reset;

   cb.get_wan_ipaddress = tcmCB_get_wan_ipaddress;

   cb.get_clientkey_passwd = tcmCB_get_clientkey_passwd;

   cb.kill_other_apps = tcmCB_kill_other_apps;



   if ( access ( "/bin/conf/configuration.xml.bak", R_OK ) == 0 )
   {
      if ( access ( "/bin/conf/configuration.xml", R_OK ) == 0 )
      {
         stat ( "/bin/conf/configuration.xml", &buf );
         stat ( "/bin/conf/configuration.xml.bak", &bufBak );

         if ( buf.st_size < MIN_TEMPLATE_FILE_SIZE && bufBak.st_size > MIN_TEMPLATE_FILE_SIZE )
         {
            system ( "cp /bin/conf/configuration.xml.bak /bin/conf/configuration.xml" );
         }
      }
      else
      {
         system ( "cp /bin/conf/configuration.xml.bak /bin/conf/configuration.xml" );
      }
   }

   if ( start_tcmAgent ( &cb ) )
   {
      TR069_ERROR ( "\nstart tcm agent failure\n" );
      return -1;
   }


   TR069_DEBUG  ( "\n==========Begin set tcmAgent tree values with global parameters========\n" );
   sleep(5);
   build_tcmAgent_parameters();

   idle ();

   /* Uninit the act lib, conflib, msg lib, log lib */
   actLib_cleanup();
   config_api_uninit_library();
   tcmMsg_cleanup ( &msgHandle );
   uninit_tcmAgent_log();
   tcmLog_cleanup();

   return 0;
}
