#include <stdio.h>

int step( int n )
{
  if( 1 == n )
  {
    return 1;
  }

  if( 2 == n )
  {
    return 2;
  }

  return step( n -1 ) + step( n - 2 );
}

int main()
{
  printf( "step(1) = %d\n", step(1) );
  printf( "step(2) = %d\n", step(2) );
  printf( "step(5) = %d\n", step(5) );
}