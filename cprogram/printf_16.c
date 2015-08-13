#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
void dtoh(uint8_t * hex,uint8_t n)
{
    static const char HEX[] = "0123456789ABCDEF";
    hex[0] = HEX[n / 16];
    hex[1] = HEX[n % 16];
}

void dump_block(char * in,char * out,int len)
{
	
    int cur;
    for (cur = 0; cur < len; cur++)
    {
        dtoh(out + 2 * cur,in[cur]);
    }
}

void hex_disp(char * in,int len,int size)
{
    int cur = 0;
    int row_cnt = 0;
    char bufTmp[10000];
    char* pbufTmp = bufTmp;
    do {
        sprintf( pbufTmp + strlen(bufTmp) ,"%-3.2s ",in + size * cur);
        cur++;
        if (cur % 8 == 0)
        {

        }      
    } while(cur < len);
printf( "%s\n", bufTmp  );
}

void dump (char * in,int len)
{
    unsigned char * out = (unsigned char *)malloc(len * 2 + 1);
    dump_block(in,out,len);
    hex_disp(out,len,2);
    free(out);
}

int main()
{
char a[10]="abcdef";
dump( a, strlen(a));
printf( "%x", a );
}
