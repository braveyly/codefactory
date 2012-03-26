#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include	<net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
	int			       sockfd, len;
	char				*ptr, buf[2048];
	struct ifconf		ifc;
	struct ifreq		*ifr;
	struct sockaddr_in	*sinptr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_req = (struct ifreq *) buf;
	ioctl(sockfd, SIOCGIFCONF, &ifc);

	for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
		ifr = (struct ifreq *) ptr;
		len = sizeof(struct sockaddr);
		
		ptr += sizeof(ifr->ifr_name) + len;	/* for next one in buffer */

		switch (ifr->ifr_addr.sa_family) {
		case AF_INET:
			sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
			printf("%s\t%s\n", ifr->ifr_name,
				   inet_ntop(AF_INET, &sinptr->sin_addr, NULL, 4) );
			break;

		default:
			printf("%s\n", ifr->ifr_name);
			break;
		}
	}
	exit(0);
}
