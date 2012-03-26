#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <stdio.h>

#define PORT_SERV 8889

#define BUFF_LEN 1024
static void UDP_server_process(int s,struct sockaddr * client)
{
    int n,len,PORT_INT,count = 0;
    char buff[BUFF_LEN];
    char *IP_STR;
    struct sockaddr_in *ip;
    ip = (struct sockaddr_in *)client;
    len = sizeof(*client);
    while(1){
        n = recvfrom(s,buff,BUFF_LEN,0,client,&len);
        IP_STR = (char *)inet_ntoa(ip->sin_addr);
        PORT_INT = ntohs(ip->sin_port);
        printf("%d Received Address:%s:%d \n",count,IP_STR,PORT_INT);
        printf("%d Received Message:%s \n",count++,buff);
        printf(" ");
        sendto(s,buff,n,0,client,len);
    }
}

int main(int argc,char **argv)
{
    int s;
    struct sockaddr_in addr_serv,addr_clie;

    s = socket(AF_INET,SOCK_DGRAM,0);

    memset(&addr_serv,0,sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_serv.sin_port = htons(PORT_SERV);
    bind(s,(struct sockaddr *)&addr_serv,sizeof(addr_serv));

    UDP_server_process(s,(struct sockaddr*)&addr_clie);

    return 0;
}