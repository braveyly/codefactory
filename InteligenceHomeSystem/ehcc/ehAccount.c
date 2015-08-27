#include <tcm_log.h>
#include "ehAccount.h"
int phoneNum = 2001;

void getRandPasswd ( char* s, int length )
{
    char str[62] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i;
    char ss[2];
    srand ( ( unsigned int ) time ( ( time_t * ) NULL ) );
    for ( i = 1;i <= length;i++ )
    {
        sprintf ( ss, "%c", str[ ( rand() %62 ) ] );
        s[i-1] = ss[0];
    }
    s[i] = '\0';
}

#define MIN_PHONE_NUMBER 100
#define MAX_PHONE_NUMBER 125

tcmRet generateAccount ( IN char* mac, IN char* ip, IN DEVICETYPE type,
                         OUT char* userName,
                         OUT char* passWord )
{
    tcmLog_debug ( "start generating account for mac %s ip %s type %d\n",
                   mac,
                   ip, type );
    char phoneNumber[MAX_VALUE_LEN];
    int i;
    for ( i = MIN_PHONE_NUMBER ; i < MAX_PHONE_NUMBER + 1; i++ )
    {
        memset ( phoneNumber, 0 , sizeof ( phoneNumber ) );
        sprintf ( phoneNumber, "%d", i );
        if ( !actDevice_checkPhoneNumerExist ( phoneNumber, 0 ) )
        {
            break;
        }
    }

    memcpy ( userName, phoneNumber, strlen ( phoneNumber ) );
    * ( userName + strlen ( phoneNumber ) ) = '\0';
    getRandPasswd ( passWord, ACCOUNT_PASSWORD_LENGTH );
    * ( passWord + ACCOUNT_PASSWORD_LENGTH ) = '\0';

    tcmLog_debug ( "Finish generating account username %s password %s\n",
                   userName,
                   passWord );
    return TCMRET_SUCCESS;
}
