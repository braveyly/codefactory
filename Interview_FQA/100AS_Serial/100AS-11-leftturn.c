#include <stdio.h>

int leftturn(char * s,int n)
{
  int i = 0,j = 0;
  char tmp = ' ';
  for( i = 0; i< n; i++)
  {
    tmp = s[0];
    for( j = 0; j < strlen(s) - 1; j++ )
    {
      s[j] = s[j+1];
    }
    s[j] = tmp;
  }
}

int main()
{
  char s[100] = "abcdefg";
  leftturn(s,3);
  printf( "s is %s\n",s );
}