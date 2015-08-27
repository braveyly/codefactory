#include "tcmAgentApi.h"
#include "define.c"

char *gParamName = NULL;
PAL_MSG_T gMsgUser[MAX_MODULE];
int flag_Cli_Init = 0;
#define ENABLE_TCMAGENTAPI_LOG
#define TCMAGENTAPI_LOG_SWITCH "/tmp/tcmAgentapilogswitch"
int flag_init_log = 0;
/***********************************************************************
Function: cli_msg_init
Description: message init.
             Be used in get_msg_from_tcmAgent and send_msg_to_tcmAgent

Return:
Input:
Output:
Others:
************************************************************************/
static void cli_msg_init()
{
   PAL_INT32_T i;

   for ( i = 0; i < MAX_MODULE; i++ )
   {
      gMsgUser[i] = pal_create_msg ( i );
   }

   flag_Cli_Init = 1;
}

void init_log()
{
#ifdef ENABLE_TCMAGENTAPI_LOG
   if ( access ( TCMAGENTAPI_LOG_SWITCH, R_OK ) == 0 )
   {
       tcmLog_init ( 0 );
       tcmLog_setDestination ( LOG_DEST_STDERR );
       tcmLog_setLevel ( LOG_LEVEL_DEBUG );
   }
#endif
flag_init_log = 1;
}
/***********************************************************************
Function: get_msg_from_tcmAgent
    send_msg_to_tcmAgent
    msg_send
    msg_recv

Description: Receive and Send message.
Return:
Input:
Output:
Others:
************************************************************************/
PAL_INT32_T get_msg_from_tcmAgent ( char *buf,  PAL_SIZE_T bufLen )
{
   if ( flag_Cli_Init == 0 )
      cli_msg_init();

   Module src;

   return msg_recv ( CLI, &src, buf, bufLen );
}

PAL_INT32_T send_msg_to_tcmAgent ( char *buf, PAL_SIZE_T bufLen )
{
   if ( flag_Cli_Init == 0 )
      cli_msg_init();

   return msg_send ( HOST_HANDLER, CLI, buf, bufLen );
}

PAL_INT32_T msg_send ( Module dest, Module src, char *buf, PAL_SIZE_T bufLen )
{
   return pal_send_msg ( gMsgUser[ ( PAL_INT32_T ) dest], gMsgUser[ ( PAL_INT32_T ) src], buf, bufLen, 0 );
}

PAL_INT32_T msg_recv ( Module dest, Module *src, char *buf, PAL_SIZE_T bufLen )
{
   PAL_INT32_T ret;
   PAL_UINT32_T eventType;
   PAL_INT32_T i;

   ret = pal_recv_msg ( gMsgUser[ ( PAL_INT32_T ) dest], &eventType, buf, bufLen, 0 );

   for ( i = 0; i < MAX_MODULE; i++ )
   {
      if ( gMsgUser[i] == eventType )
      {
         *src = i;
      }
   }

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


int notify_tcmAgent ( char *buf )
{
   TCMAGENTAPI_DEBUG ( "Enter notify_tcmAgent\n" );
   unsigned int *totalLen;
   char recvBuf[MAX_RECV_MSG_LEN];
   char errorName[MAX_TR069_NAME_LEN + 1];
   char *bufPtr;
   MsgType type;

   if ( send_msg_to_tcmAgent ( buf, MAX_SEND_MSG_LEN ) < 0 )
   {
      TCMAGENTAPI_ERROR ( "send msg to tcm agent error!\n" );
      return -1;
   }
   else
   {
      TCMAGENTAPI_DEBUG ( "send_msg_to_tcmAgent succeed!\n" );
   }
#if 0
   if ( get_msg_from_tcmAgent ( recvBuf, MAX_RECV_MSG_LEN ) < 0 )
   {
      TCMAGENTAPI_ERROR ( "client recv msg from tcm agent error!\n" );
      return -1;
   }
   else
   {
      TCMAGENTAPI_DEBUG ( "get_msg_from_tcmAgent succeed!\n" );
   }

   totalLen = recvBuf;

   bufPtr = ( recvBuf + sizeof ( unsigned int ) );

   while ( *totalLen > 0 )
   {
      get_msg_type ( totalLen, &bufPtr, &type );

      if (  MSG_OK != type )
      {
         TCMAGENTAPI_ERROR ( " client get response error %d\n", ( int ) type );
         break;
      }
      else if ( *totalLen > 0 )
      {
         unsigned int index;
         char name[MAX_TR069_NAME_LEN + 1];

         get_msg_name ( &bufPtr, totalLen, name );
         get_msg_value ( &bufPtr, totalLen, &index );

         TCMAGENTAPI_NOTICE ( " client get response name-%s, value-%d\n", name, index );

      }
      else
      {
         TCMAGENTAPI_DEBUG ( "client get response ok!\n" );
      }
      
   }
#endif

   TCMAGENTAPI_DEBUG ( " Leave notify_tcmAgent\n" );


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

void put_single_node ( MsgType msgType, char *name, MsgValueType valueType, char *value, char **bufPtr, int *totalLen )
{
   /* put msg type */
   put_msg_type ( totalLen, bufPtr, msgType );

   /* put msg name */
   put_msg_name ( totalLen, bufPtr, name, strlen ( name ) + 1 );

   /* put msg value */

   put_msg_value ( totalLen, bufPtr, valueType, value );


}

void put_object ( MsgType msgType, char *name, char **bufPtr, int *totalLen )
{
   /* put msg type */
   put_msg_type ( totalLen, bufPtr, msgType );

   /* put msg name */
   put_msg_name ( totalLen, bufPtr, name, strlen ( name ) + 1 );
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

int tcmAgent_api_set_value ( char* name, char* value, int type )
{
   char sendBuf[MAX_SEND_MSG_LEN];
   unsigned int *totalLen = ( unsigned int * ) sendBuf;
   char *bufPtr = sendBuf + sizeof ( int ); /* now , i assume the int is 4 bytes */

   *totalLen = 0;

   if ( type == TCMAGENT_STRING )
   {
      put_single_node ( SET_VALUE, name, type, value, &bufPtr, totalLen );
   }
   else
   {
      int num;

      if ( string_to_number ( value, &num ) )
      {
         put_single_node ( SET_VALUE, name, type, &num, &bufPtr, totalLen );
      }
      else
      {
         return -1;
      }

   }

   return notify_tcmAgent ( sendBuf );
}

int tcmAgent_api_add_object ( char* name )
{
   char sendBuf[MAX_SEND_MSG_LEN];
   unsigned int *totalLen = ( unsigned int * ) sendBuf;
   char *bufPtr = sendBuf + sizeof ( int ); /* now , i assume the int is 4 bytes */

   *totalLen = 0;
   put_object ( ADD_OBJECT, name, &bufPtr, totalLen );

   return notify_tcmAgent ( sendBuf );
}

int tcmAgent_api_delete_object ( char* name )
{
   char sendBuf[MAX_SEND_MSG_LEN];
   unsigned int *totalLen = ( unsigned int * ) sendBuf;
   char *bufPtr = sendBuf + sizeof ( int ); /* now , i assume the int is 4 bytes */

   *totalLen = 0;
   put_object ( DELETE_OBJECT, name, &bufPtr, totalLen );

   return notify_tcmAgent ( sendBuf ) ;
}

/***********************************************************************
Function: create_parameter_name
Description:
   tr069Name:InternetGatewayDevice.Layer3Forwarding.Forwarding.0.DestIPAddress
   index   :8


Return:
  Success 0 ; Fail -1
Input:
Output:
  resultStr:
      InternetGatewayDevice.Layer3Forwarding.Forwarding.8.DestIPAddress
Others:
************************************************************************/
int create_parameter_name ( char* tr069Name, int index, char* resultStr )
{
   if ( index < 1 || index > 99 )
   {
      return -1;
   }

   char* tmp = strstr ( tr069Name, ".0." );

   char indexStr[5];/*5 at least*/
   int indexStrLen;

   if ( index < 10 && index > 0 )
   {
      indexStrLen = 3;
      sprintf ( indexStr, ".%d.", index );
   }
   else if ( index >= 10 && index < 100 )
   {
      indexStrLen = 4;
      sprintf ( indexStr, ".%d.", index );/*".12." length is 5, so make sure indexStr length is 5 at least*/
   }

   if ( tmp != NULL )
   {

      TCMAGENTAPI_DEBUG ( "tr069Name = %s\t size = %d\n", tr069Name, tmp - tr069Name );
      memcpy ( resultStr, tr069Name, ( int ) ( tmp - tr069Name ) );
      memcpy ( resultStr + ( tmp - tr069Name ), indexStr, indexStrLen );
      memcpy ( resultStr + ( tmp - tr069Name ) + indexStrLen, tmp + 3, strlen ( tmp + 3 ) + 1 );
   }
   else
   {
      return -1;
   }

   return 0;
}

static int string_to_number ( char *str, int *retValue )
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
      ret = 0;

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
Function: judge_validity_of_value
Description:Judge message value.

Return:
  Invalid 0; valid 1
Input:
Output:
Others:
************************************************************************/
int judge_validity_of_value ( ConflibTR069MatchParameter* parameter, char* value )
{
   if ( parameter == NULL || value == NULL )
   {
      return 0;
   }


   if ( TCMAGENT_STRING == parameter->type )
   {
      if ( strlen ( value ) < parameter->min || strlen ( value ) > parameter->max )
      {
         return 0;
      }
      else
      {
         return 1;
      }
   }
   else
   {
      int num;

      if ( string_to_number ( value, &num ) )
      {
         if ( num < parameter->min || num > parameter->max )
         {
            return 0;
         }
         else
         {
            return 1;
         }
      }
      else
      {
         return 0;
      }

   }

}

/***********************************************************************
Function: create_instance_name
Description:
   tr069Name:InternetGatewayDevice.Layer3Forwarding.Forwarding.
   index  : 8

Return:
  Success 0; Fail  -1
Input:
Output:
  resultName: InternetGatewayDevice.Layer3Forwarding.Forwarding.8.
Others:
************************************************************************/
int create_instance_name ( char* tr069Name, int index, char* resultName )
{
   char indexStr[5];
   int indexLen = 0;

   if ( tr069Name == NULL || index < 1 || index > 99 || * ( tr069Name + strlen ( tr069Name ) - 1 ) != '.' )
   {
      return -1;
   }

   if ( index < 10 )
   {
      sprintf ( indexStr, "%d.", index );
      indexLen = 3;
   }
   else
   {
      sprintf ( indexStr, "%d.", index );
      indexLen = 4;
   }

   indexStr[ indexLen ] = '\0';

   memcpy ( resultName, tr069Name, strlen ( tr069Name ) );
   memcpy ( resultName + strlen ( tr069Name ), indexStr,  indexLen + 1 );
   return 0;
}


tcmRet tcmAgent_api_set_value_by_index ( tcmEntityId appID, int rowID, int parID, char* value )
{
   if( flag_init_log == 0 )
   	init_log ();
   TCMAGENTAPI_DEBUG ( "tcmAgent_api_set_value_by_index appID = %d\t rowID = %d\t parID =%d\t size = %d\t value = %s\n", appID, rowID, parID, sizeof ( gMatchParameters ) / sizeof ( ConflibTR069MatchParameter ), value );

   if ( appID < 1 || rowID < -1 || rowID == 0 || parID < 0 || value == NULL || appID > ID_END )
   {
      return TCMRET_INVALID_PARAMETER;
   }

   int i;
   char valueTmp[MAX_LEN_OF_VALUE];
      if( ( appID == ID_DHCPD && parID == PARA_DHCPD_STATUS ) || \
		 	appID == ID_SERVICE  ||\
		 	appID == ID_ACCESSMODE ||\
		 	appID == ID_UPNPD ||\
		  (	appID == ID_TR069 && parID == PARA_TR069_INFORMSTATUS ) || \
		  ( appID == ID_SDKD && parID == PARA_SDKD_PKM ) || \
		  ( appID == ID_SDKD && parID == PARA_SDKD_ALPHA_DELIM ) || \
		  ( appID == ID_AUTOCM && parID == PARA_AUTOCM_WM_AUTO_CON ) || \
		  ( appID == ID_AUTOCM && parID == PARA_AUTOCM_DHCPCD_AUTO_START ) || \
		  ( appID == ID_AUTOCM && parID == PARA_AUTOCM_GW_PING_CHECK ) || \
		  ( appID == ID_LOG && parID == PARA_LOG_STATUS ) || \
		  ( appID == ID_IGMPPROXY && parID == PARA_IGMP_STATUS ) || \
		  ( appID == ID_WAN && parID == PARA_MNG_STATUS ) || \
		  ( appID == ID_AUTOCM && parID == PARA_AUTOCM_MNG_DHCPCD_AUTO_START ))
         {
             if( strcmp( value, "enable" ) == 0 )
             {
                 strcpy( valueTmp, "1" );
             }
			 else if( strcmp( value, "disable" ) == 0 )
			 {
			     strcpy( valueTmp, "0" );
			 }
         }
	     else if(  appID == ID_ROUTE && parID == PARA_ROUTE_NEXTHOPTYPE )
	     {
             if( strcmp( value, "true" ) == 0 )
             {
                 strcpy( valueTmp, "1" );
             }
			 else if( strcmp( value, "false" ) == 0 )
			 {
			     strcpy( valueTmp, "0" );
			 }
			 else
			 {
     		     strcpy( valueTmp, value );			 
			 }
	     }
		 else if( appID == ID_ACCOUNT && parID == PARA_ACCOUNT_CPASSWORD )
		 {
   		     strcpy( valueTmp, value );			 
		     char resultTmp[MAX_LEN_OF_VALUE];
             revertPSW ( valueTmp, resultTmp );
			 memset( valueTmp, 0, MAX_LEN_OF_VALUE );
			 strcpy( valueTmp, resultTmp );
		 }
		 else
		 {
		     strcpy( valueTmp, value );
		 }
		 
   for ( i = 0; i < sizeof ( gMatchParameters ) / sizeof ( ConflibTR069MatchParameter ); i++ )
   {
      if ( rowID == -1 && gMatchParameters[i].conflibRowID == FLAG_NO_ROW && \
            gMatchParameters[i].conflibAppID == appID && gMatchParameters[i].conflibParID == parID )
      {
   
         if ( judge_validity_of_value ( &gMatchParameters[i], valueTmp ) )
         {
            if ( 0 == tcmAgent_api_set_value ( gMatchParameters[i].tr069Name, valueTmp, gMatchParameters[i].type ) )
            {
               if( appID == ID_TR069 )
               {
                   send_init_end();
               }
               return TCMRET_SUCCESS;
            }
            else
            {
               TCMAGENTAPI_ERROR ( "Error: set app value for appID %d, parID %d, value %s\n", appID, parID, valueTmp );
               return TCMRET_FAILURE;
            }
         }
      }

      if ( rowID != -1 && gMatchParameters[i].conflibRowID == FLAG_TEMPLATE_ROW && \
            gMatchParameters[i].conflibAppID == appID && gMatchParameters[i].conflibParID == parID )
      {
         if ( judge_validity_of_value ( &gMatchParameters[i], valueTmp ) )
         {
            char parName[ MAX_LEN_OF_NAME ];

            if ( !create_parameter_name (  gMatchParameters[i].tr069Name, rowID, parName ) )
            {
               if ( 0 == tcmAgent_api_set_value ( parName, valueTmp, gMatchParameters[i].type ) )
               {
                  return TCMRET_SUCCESS;
               }
               else
               {
                  TCMAGENTAPI_ERROR ( "Error: set table value for appID %d, rowID %d, parID %d, value %s\n", appID, rowID, parID, valueTmp );
                  return TCMRET_FAILURE;
               }
            }
            else
            {
               TCMAGENTAPI_ERROR ( "Error: create parameter name for tr069Name %s\n", gMatchParameters[i].tr069Name );
               return  TCMRET_INTERNAL_ERROR;
            }
         }
      }
   }

   TCMAGENTAPI_ERROR ( "Fail to set value and Leave\n" );

   return TCMRET_FAILURE;
}

tcmRet tcmAgent_api_add_object_by_index ( tcmEntityId appID, int rowID, char** value )
{
   if( flag_init_log == 0 )
   	init_log ();
   TCMAGENTAPI_DEBUG ( "tcmAgent_api_add_object_by_index appID = %d\t parID =%d\t size = %d\n", appID, rowID, sizeof ( gMatchParameters ) / sizeof ( ConflibTR069MatchParameter ) );
  
   if ( appID < 1 || rowID < 1 || appID > ID_END )
   {
      return TCMRET_INVALID_PARAMETER;
   }

   int i;

   char valueConvert[MAX_NUM_OF_ROW_PARA][MAX_LEN_OF_VALUE];

   if ( appID == ID_ROUTE )
   {
      if ( strcmp ( value[2], "true" ) == 0 )
      {
         strcpy ( valueConvert[0], value[0] );
         strcpy ( valueConvert[1], value[1] );
         strcpy ( valueConvert[2], "1" );
         strcpy ( valueConvert[3], value[3] );
         strcpy ( valueConvert[4], value[4] );
         strcpy ( valueConvert[5], value[5] );
      }
      else if ( strcmp ( value[2], "false" ) == 0 )
      {
         strcpy ( valueConvert[0], value[0] );
         strcpy ( valueConvert[1], value[1] );
         strcpy ( valueConvert[2], "0" );
         strcpy ( valueConvert[3], value[3] );
         strcpy ( valueConvert[4], value[4] );
         strcpy ( valueConvert[5], value[5] );
      }
   }

   for ( i = 0; i < sizeof ( gMatchParameters ) / sizeof ( ConflibTR069MatchParameter ); i++ )
   {
      if ( gMatchParameters[i].conflibRowID == FLAG_TEMPLATE_ROW && \
            gMatchParameters[i].conflibParID == FLAG_INSTANCE_ID && \
            gMatchParameters[i].conflibAppID == appID )
      {
         char instanceName[MAX_LEN_OF_NAME];

         if ( !create_instance_name ( gMatchParameters[i].tr069Name, rowID, instanceName ) )
         {
            TCMAGENTAPI_DEBUG ( "tcmAgent_api_add_object for %s\n", instanceName );

            if ( 0 != tcmAgent_api_add_object ( instanceName ) )
            {
               TCMAGENTAPI_ERROR ( "Error: add object for instanceName %s\n", instanceName );
               return TCMRET_FAILURE;
            }
         }
         else
         {
            TCMAGENTAPI_ERROR ( "Error: create instance name for tr069Name %s\n", gMatchParameters[i].tr069Name );
            return TCMRET_INTERNAL_ERROR;
         }
      }

      if ( gMatchParameters[i].conflibRowID == FLAG_TEMPLATE_ROW && \
            gMatchParameters[i].conflibAppID == appID && \
            gMatchParameters[i].conflibParID != FLAG_INSTANCE_ID && \
            gMatchParameters[i].conflibParID != FLAG_STATUS_STYLE )
      {
         TCMAGENTAPI_DEBUG ( "parID = %d\n", gMatchParameters[i].conflibParID );

         /*  Use default parameter values if valueNum is 0*/

         if ( appID == ID_ROUTE )
         {
            if ( TCMRET_SUCCESS != tcmAgent_api_set_value_by_index ( appID, rowID, gMatchParameters[i].conflibParID, valueConvert[gMatchParameters[i].conflibParID - 1] ) )
            {
               TCMAGENTAPI_ERROR ( "Error: set value in add object for appID %d, rowID %d, parID %d, value %s\n", appID, rowID, gMatchParameters[i].conflibParID, valueConvert[gMatchParameters[i].conflibParID - 1] );               
               return TCMRET_FAILURE;
            }
         }
         else if ( TCMRET_SUCCESS != tcmAgent_api_set_value_by_index ( appID, rowID, gMatchParameters[i].conflibParID, value[gMatchParameters[i].conflibParID - 1] ) )
         {
            TCMAGENTAPI_ERROR ( "Error: set value in add object for appID %d, rowID %d, parID %d, value %s\n", appID, rowID, gMatchParameters[i].conflibParID, value[gMatchParameters[i].conflibParID - 1] );               
            return TCMRET_FAILURE;
         }


      }
   }

   return TCMRET_SUCCESS;
}

tcmRet tcmAgent_api_del_object_by_index ( tcmEntityId appID, int rowID )
{
    if( flag_init_log == 0 )
   	init_log ();
   TCMAGENTAPI_DEBUG ( "tcmAgent_api_del_object_by_index appID = %d\t rowID = %d\t size = %d\n", appID, rowID, sizeof ( gMatchParameters ) / sizeof ( ConflibTR069MatchParameter ) );

   if ( appID < 1 || rowID < 1 || rowID > ID_END )
   {
      return TCMRET_INVALID_PARAMETER;
   }

   int i;

   for ( i = 0; i < sizeof ( gMatchParameters ) / sizeof ( ConflibTR069MatchParameter ); i++ )
   {
      if ( gMatchParameters[i].conflibAppID == appID && \
            gMatchParameters[i].conflibRowID == FLAG_TEMPLATE_ROW && \
            gMatchParameters[i].conflibParID == FLAG_INSTANCE_ID )
      {
         char instanceName[MAX_LEN_OF_NAME];

         if ( 0 == create_instance_name ( gMatchParameters[i].tr069Name, rowID, instanceName ) )
         {
            TCMAGENTAPI_DEBUG ( "tcmAgent_api_add_object for %s\n", instanceName );

            if ( -1 == tcmAgent_api_delete_object ( instanceName ) )
            {
               TCMAGENTAPI_ERROR ( "Error: delete object for instanceName %s\n", instanceName );               
               return TCMRET_FAILURE;
            }
         }
         else
         {
            TCMAGENTAPI_ERROR ( "Error: create instance name for tr069Name %s\n", gMatchParameters[i].tr069Name );                     
            return TCMRET_INTERNAL_ERROR;
         }
      }
   }

   return TCMRET_SUCCESS;
}


