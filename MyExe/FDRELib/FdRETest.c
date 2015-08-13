#include "FdRE.h"

int main()
{
   uint8_t re[] = {0x01, 0x02, LEN_BYTE,LEN_BYTE,LEN_BYTE,LEN_BYTE,0x23,MUL_BYTES};
   uint8_t src[100]={ 0x01, 0x01,0x02,0x06,0x00,0x00,0x00,0x23,0x11,0x12,0x13,0x14,0x15,0x16 };
   uint8_t resStr[100]={0};
   int ret;
   int resLen= 0;
   int offSet= 0;
   int reLen = 8;
   int srcLen = 14;
   int i, j;

   uint8RESetExpression(re,reLen);
   uint8RESetValueLenOffSet( 0 );
   ret = uint8REParse( src, srcLen, &resStr, sizeof(resStr), &resLen, &offSet );

   printf( "offSet is %d resLen is %d, ret is %d\n", offSet, resLen,ret );
   i = resLen;
   while(i)
   {
	   printf( "resStr[%d]%x\n", resLen - i, resStr[resLen-i] );
	   i--;
   }
   
}

