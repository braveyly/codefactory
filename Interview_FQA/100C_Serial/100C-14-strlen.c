#include <stdio.h>

int my_strlen(char* str)
{
	if(NULL==str)
		return -1;
	if('\0' == (*str) )
	{
		return 0;
	}
	else
	{
		printf("*str is %c\n", *str);
		return (1+my_strlen(++str));
	}
}

int main()
{
	char* str = (char*)malloc(10*sizeof(char));
	memset(str,0,10);
	strcpy(str,"china");
	printf("str %s length = %d\n", str, my_strlen(str));

}