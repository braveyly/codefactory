#include <stdio.h>

int reverse( char* begin, char* end )
{
  char tmp = ' ';
  if( begin >= end )
  {
    return -1;
  }
  while( begin < end )
  {
    tmp = *end;
    *end = *begin;
    *begin = tmp;
    end--;
    begin++;
  } 
  return 0;
}

int reverseperword( char* begin, char* end )
{
  char* wordbegin = NULL;
  char* wordend = NULL;
  while( begin < end )
  {
    wordbegin = begin;
    wordend = begin;

    while( ' ' != *(wordend + 1) )
    {
      wordend++;
    }

    reverse( wordbegin,wordend );

    wordend++;
    while( ' ' == *wordend && wordend <= end )
    {
      wordend++;
    }

    begin = wordend;
  }
}

int main()
{
  char a[100] = "I AM A STUDENT";
  int i = 0;
  char* p = a;
  reverse(p,p+strlen(a)-1);
  for( i = 0; i< strlen(a); i++ )
  {
    printf("%c",a[i]);
  }
  printf("\n");
  reverseperword(p,p+strlen(a)-1);
  
  for( i = 0; i< sizeof(a); i++ )
  {
    printf("%c",a[i]);
  }
  printf("\n");
}