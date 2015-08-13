#include <stdio.h>
#include <string.h>

int calculate(int* a, int len)
{
  int cursum = 0, maxsum = 0;
  int i = 0;
  
  for( i = 0; i < len; i++ )
  {
    cursum += a[i];
    if( cursum < 0 )
    {
      cursum = 0;
    }
    if( cursum > maxsum )
    {
      maxsum = cursum;
    }
  }

  if( 0 == maxsum )
  {
    maxsum = a[0];
    for( i = 1; i < len; i++ )
    {
      if( a[i] > maxsum )
      {
        maxsum = a[i];
      }
    }
  }

  return maxsum;
}


int main()
{
  int a[10] = { 1,-6,3,8,12,33,-19,20,-22,99 };
  int sum = 0;
  sum = calculate( a, 10 );
  printf( "max sub array sum is %d\n", sum );
  return 0;
}