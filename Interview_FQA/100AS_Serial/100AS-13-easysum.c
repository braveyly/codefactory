#include <stdio.h>

int easysum( int n )
{
  int sum = 0;
  ( n > 0 )&&( sum = n+ easysum(n-1) );
  return sum;
}

int main()
{
  int sum = easysum(100);
  printf("sum is %d\n", sum);
}