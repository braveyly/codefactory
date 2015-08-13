#include <stdio.h>

static int top[10] = {0,1,2,3,4,5,6,7,8,9};
#define N 10
static int bottom[10] = {0,0,0,0,0,0,0,0,0,0};
int getcount(int top)
{
  int i = 0;
  int count = 0;
  for( i = 0; i < N; i++ )
  {
    if( top == bottom[i] )
    {
      count++;
    }
  }
  return count;
}

int main()
{
  int i = 0;
  int ready = 1;
  int count = 0;
  while(ready)
  {
    ready = 0;
    for( i = 0; i < N; i++ )
    {
      count = getcount(top[i]);
      if( bottom[i] != count )
      {
        ready = 1;
        bottom[i] = count;
      }
    }
  }
  
    printf( "top\t" );
  for( i = 0 ; i < N; i++ )
  {
    printf( "%d\t",top[i] );
  }
  printf( "\n" );
  
    printf( "bottom\t" );
  for( i = 0; i< N; i++ )
  {
    printf( "%d\t",bottom[i] );
  }
}