#include "tcmAgentApi.h"

static int string_to_num2 ( char *str, int *retValue )
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

int myId = ID_SNTP;
int main ( int argc, char *argv[] )
{
   int appID;
   int rowID;
   int parID;

   tcmLog_init ( myId );
   tcmLog_setDestination ( LOG_DEST_STDERR );
   tcmLog_setLevel ( LOG_LEVEL_DEBUG );

   if ( strcmp ( argv[1], "set" ) == 0 )
   {
      string_to_num2 ( argv[2], &appID );
      string_to_num2 ( argv[3], &rowID );
      string_to_num2 ( argv[4], &parID );
      tcmAgent_api_set_value_by_index ( appID, rowID, parID, argv[5] );
   }

   else if ( strcmp ( argv[1], "add" ) == 0 )
   {
      string_to_num2 ( argv[2], &appID );
      string_to_num2 ( argv[3], &rowID );
	      string_to_num2 ( argv[4], &parID );   
      tcmAgent_api_add_object_by_index ( appID, rowID, &argv[5]);
   }

   else if ( strcmp ( argv[1], "del" ) == 0 )
   {
      string_to_num2 ( argv[2], &appID );
      string_to_num2 ( argv[3], &rowID );
      tcmAgent_api_del_object_by_index ( appID, rowID );
   }
   else
   {
   printf( "./tcmAgentApiTest set appID rowID/-1 parID value\n" );
   printf( "./tcmAgentApiTest add appID rowID    parNum/0 value1 value2.....\n" );
   printf( "./tcmAgentApiTest del appID rowID\n" );
   return 0;
   }

}
