#include <stdio.h>

static int a1[10] = { 4,4,5,4,7,4,6,4,2,4 }; // 1/2 1/2

static int a2[20] = { 1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,5 }; // 1/4 1/4 1/4 1/4


int find_half_king( int* a, int N)
{
   int candicate = 0;
   int times = 0;
   int i = 0;
   
   for( i = 0; i < N; i++ )
   {
     if( 0 == times )
     {
        candicate = a[i];
        times = 1;
     }
     else
     {
        if( a[i] == candicate )
        {
          times++;
        }
        else
        {
          times--;
        }
     }
   }
   return candicate;
}

/*
int find_quartor_king( int* a, int N )
{
   int candicate[3] = {0};
   int times[3] = {0};
   int i = 0;
  
   for( i = 0; i < N; i++ )
   {
     if( 0 == times[0] )
     {
       if( 0 == times[1] && 0 == times[2] )
       {
          candicate[0] = a[i];
       }
       
       
     }  
   }
}
*/

int main()
{
  int king = 0;
  king = find_half_king( a1, 10 );
  
  printf( "the bbs half king is %d\n", king );
  return 0;
}