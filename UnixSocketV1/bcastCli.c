#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define	MAXLINE		4096	/* max text line length */

void
dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen)
{
	int				n;
	const int		on = 1;
	char			sendline[MAXLINE], recvline[MAXLINE + 1];
	socklen_t		len;
	struct sockaddr	*preply_addr;

	memset( sendline, 0, sizeof( sendline ) );
	memset( recvline, 0, sizeof( recvline ) );
	
	preply_addr = (struct sockaddr	*) malloc (servlen);

	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

	strcpy( sendline, "what is the time" );
	while (1) {
              sleep(1);
		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		printf( "have send data %s\n", sendline );
		
		for ( ; ; ) {
			len = servlen;
			printf( "before recvfrom\n" );
			n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
			printf( "after recvfrom %s\n", recvline );
			if (n < 0) {
				if (errno == EINTR)
					break;		/* waited long enough for replies */
				else
					printf("recvfrom error");
			} else {
				recvline[n] = 0;	/* null terminate */
				break;
				//printf("from %s: %s",
				//		sock_ntop_host(preply_addr, servlen), recvline);
			}
		}
	}
}

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 3)
		printf("usage: udpcli01 <IPaddress> <Port>");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));		/* standard daytime server */
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	dg_cli(stdin, sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	exit(0);
}

