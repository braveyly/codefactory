#include <tcm_log.h>
#include "ehHex.h"

void dtoh ( uint8_t * hex, uint8_t n )
{
    static const uint8_t HEX[] = "0123456789ABCDEF";
    hex[0] = HEX[n / 16];
    hex[1] = HEX[n % 16];
}

void dump_block ( char * in, char * out, int len )
{
    int cur;
    for ( cur = 0; cur < len; cur++ )
    {
        dtoh ( out + 2 * cur, in[cur] );
    }
}

void hex_disp ( char * in, int len, int size )
{
    int cur = 0;
    int row_cnt = 0;
    //tcmLog_debug("%08dh:",row_cnt++);
    char* bufTmp = ( unsigned char * ) malloc ( len * 4 + 1 );
    memset ( bufTmp , 0, len*4 + 1 );
    char* pbufTmp = bufTmp;
    do
    {
        sprintf ( pbufTmp + strlen ( bufTmp ) , "%-3.2s " , in + size * cur );
        cur++;

        if ( cur % 8 == 0 )
        {
            //tcmLog_debug("\n%08dh:",row_cnt++);
        }
    }
    while ( cur < len );
    pbufTmp[strlen ( bufTmp ) ] = '\0';
    tcmLog_debug ( "%s", pbufTmp );
    free ( bufTmp );
}

#ifdef _NOT_PRINTF_HEX_
void dump ( char * in, int len )
{
    unsigned char * out = ( unsigned char * ) malloc ( len * 2 + 1 );
    dump_block ( in, out, len );
    hex_disp ( out, len, 2 );
    free ( out );
}
#else
void dump ( char * in, int len )
{
}
#endif