#include <stdio.h>

int onenum(int num)
{
  int count;
  while(num)
  {
    if(num&1)
    {
      count++;
    }
    num = num >> 1;
  }
  return count;
}

int main()
{
  printf( "1 number is %d\n", onenum(11) );
}