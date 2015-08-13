#include <stdio.h>


int main()
{
   char a1[100] = "aa";
   char a2[100] = "bb";
   char a3[100] = "cc";
   char *p = NULL;

   printf( "a is [%d][%s], b [%d][%s], c[%d][%s]\n", a1,a1,a2,a2,a3,a3 );

   p = a1;
   printf( "p is[%d] [%s]\n",p, p );

   *(void **)p = a3;
   printf( "p is[%d] [%s], and *p is[%d] [%s]\n", p,p, *(void **)p,*(void**)p );
 }
 
 /*
 void double pointer has another function
 */


/*
[nick@d02 doublevoidpointer]$ ./main
a is [-1075484820][aa], b [-1075484920][bb], c[-1075485020][cc]
p is[-1075484820] [aa]
p is[-1075484820] [¤få¿], and *p is[-1075485020] [cc]
*/