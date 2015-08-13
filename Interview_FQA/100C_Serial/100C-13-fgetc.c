#include <stdio.h>

int main()
{
	/*unsigned char: circle forever
	  signed char  : quit normally, but quit earlily with 0xff in*/
	unsigned char c;
	FILE* fp = NULL;
	fp = fopen("book.txt", "rb" );
	if(fp)
	while( ( c = fgetc(fp) ) != EOF )
	{
		fputc(c,stdout);
		printf("\n[]");
	}
}