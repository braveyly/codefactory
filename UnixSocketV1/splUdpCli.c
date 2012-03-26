#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <stdio.h>
#define PORT_SERV 8889

#define BUFF_LEN 1024
#define SERV_IP "192.168.31.34"
static void UDP_client_process(int s,struct sockaddr *to)
{
    char RecBuff[BUFF_LEN]=" ",SendBuff[BUFF_LEN];
    int n;
    struct sockaddr_in from;
    int len = sizeof(*to);
    printf("please input message!\n ");
    while(1){
        n = read(0,SendBuff,BUFF_LEN);
        SendBuff[n++] = '\0';
        sendto(s,SendBuff,n,0,to,len);
    //    recvfrom(s,RecBuff,BUFF_LEN,0,(struct sockaddr*)&from,&len);
    //    printf("recved:%s ",RecBuff);
    }
}

int main(int argc,char **argv)
{
    int s;
    struct sockaddr_in addr_serv,addr_clie;
    char *msg;
    if(argc != 3){
        printf("parameter is error! ");
        printf("format:ip port ");
        return -1;
    }
    
    s = socket(AF_INET,SOCK_DGRAM,0);
    memset(&addr_serv,0,sizeof(addr_clie));
    addr_serv.sin_family = AF_INET;
    
    /*inet_addr(SERV_IP);htonl(INADDR_ANY);*/    
    addr_serv.sin_addr.s_addr = inet_addr(argv[1]);
    
    /*htons(PORT_SERV);*/
    addr_serv.sin_port = htons(atoi(argv[2]));

    UDP_client_process(s,(struct sockaddr*)&addr_serv);

    close(s);
    return 0;
}