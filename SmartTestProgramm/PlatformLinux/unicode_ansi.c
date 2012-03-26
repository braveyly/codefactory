
#include <stdio.h>
#include <locale.h>

int m2w ( const char* mbs, char* buf )
{
   int len;
   len = mbstowcs ( NULL, mbs, 0 );

   if ( len == 0 )
      return NULL;

   memset ( buf, 0, 2* ( len + 1 ) );

   len = mbstowcs ( buf, mbs, len + 1 );

   return 0;
}

void ansitoUnicode ( char* ansiStr, int ansiStrLen, char* unicodeStr, int* unicodeStrLen )
{
   int i = 0;

   while ( i < ansiStrLen )
   {
      * ( unicodeStr + 2*i ) = 0x00;
      * ( unicodeStr + 2*i + 1 ) = * ( ansiStr + i );
      printf ( "out %dth: %d   %dth: %d\n ", 2*i, * ( unicodeStr + 2*i ), 2*i + 1, * ( unicodeStr + 2*i + 1 ) );
      i++;
   }

   if ( i >= ansiStrLen )
   {
      * ( unicodeStr + 2*i ) = '\0';
      *unicodeStrLen = 2 * ansiStrLen;
   }
}

void unicodeToansi ( char* unicodeStr, int unicodeStrLen, char* ansiStr, int *ansiStrLen )
{
   int i = 0;

   while ( i < unicodeStrLen / 2 )
   {
      * ( ansiStr + i ) = * ( unicodeStr + 2 * i + 1 );
      ++i;
   }

   *ansiStrLen = unicodeStrLen / 2;
}

int main()
{
   char in[10] = "abc";
   char out[20];
   int length;
   ansitoUnicode ( in, strlen ( in ), out, &length );
   printf ( "in  %d  %d %d %d %d %d %d\n", in[0], in[1], in[2], in[3], in[4], in[5], in[6] );
   printf ( "out %d  %d %d %d %d %d %d %d %d %d %d %d %d %d \n", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11], out[12], out[13] );
   printf ( "out length is %d\n", length );
   char outT[20];
   int lengthT;
   unicodeToansi ( out, length, outT, &lengthT );
   printf ( "ansi length is %d\n", lengthT );
   printf ( "%d %d %d %d %d %d %d %d %d %d\n ", outT[0], outT[1], outT[2], outT[3], outT[4], outT[5], outT[6], outT[7], outT[8] );

   char hanzi[100] = "ÄãºÃÂðÄãºÃÂð";
   printf ( "%s  the length is %d\n", hanzi, strlen ( hanzi ) );
   printf ( "%1x  %1x  %1x  %1x  %1x  %1x  %1x  %1x  %1x  %1x  %1x  %1x   %1x   %1x\n", hanzi[0], hanzi[1], hanzi[2], hanzi[3], hanzi[4], hanzi[5], hanzi[6], hanzi[7], hanzi[8], hanzi[9], hanzi[10], hanzi[11], hanzi[12], hanzi[13], hanzi[14] );
}

