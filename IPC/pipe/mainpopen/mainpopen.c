#include	"myheader.h"

int
main(int argc, char **argv)
{
	size_t	n;
	char	buff[MAXLINE], command[MAXLINE];
	FILE	*fp;

		/* 4read pathname */
	Fgets(buff, MAXLINE, stdin);
	n = strlen(buff);		/* fgets() guarantees null byte at end */
	if (buff[n-1] == '\n')
		n--;				/* delete newline from fgets() */

printf("buff==%s\n",buff);
	snprintf(command, sizeof(command), "cat %s", buff);
printf("command==%s\n",command);
	fp = Popen(command, "r");

		/* 4copy from pipe to standard output */
	while (Fgets(buff, MAXLINE, fp) != NULL)
{
printf("buff after popen==%s\n",buff);
		Fputs(buff, stdout);
}

	Pclose(fp);
	exit(0);
}
