// Module Name: Ping.c
//
// Description:
//    This sample illustrates how an ICMP ping app can be written
//    using the SOCK_RAW socket type and IPPROTO_ICMP protocol.
//    By creating a raw socket, the underlying layer does not change
//    the protocol header so that when we submit the ICMP header
//    nothing is changed so that the receiving end will see an 
//    ICMP packet. Additionally, we use the record route IP option
//    to get a round trip path to the endpoint. Note that the size
//    of the IP option header that records the route is limited to
//    nine IP addresses.
//
// Compile:
//     cl -o Ping Ping.c ws2_32.lib /Zp1
//
// Command Line Options/Parameters:
//     Ping [host] [packet-size]
//     
//     host         String name of host to ping
//     packet-size  Integer size of packet to send 
//                      (smaller than 1024 bytes)
//
//#pragma pack(1)
#include "comDef.h"
#include "ping.h"

BOOL  bRecordRoute;
int   datasize;
char *lpdest;
char *startIp;
char *endIp;


//
// Function: usage
//
// Description:
//    Print usage information
//
void usage(char *progname)
{
    printf("usage: ping <start IP>  <end IP>\n");
    ExitProcess(-1);
}

// 
// Function: FillICMPData
//
// Description:
//    Helper function to fill in various fields for our ICMP request
//
void FillICMPData(char *icmp_data, int datasize)
{
    IcmpHeader *icmp_hdr = NULL;
    char       *datapart = NULL;

    icmp_hdr = (IcmpHeader*)icmp_data;
    icmp_hdr->i_type = ICMP_ECHO;        // Request an ICMP echo
    icmp_hdr->i_code = 0;
    icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
    icmp_hdr->i_cksum = 0;
    icmp_hdr->i_seq = 0;
  
    datapart = icmp_data + sizeof(IcmpHeader);
    //
    // Place some junk in the buffer
    //
    memset(datapart,'E', datasize - sizeof(IcmpHeader));
}

// 
// Function: checksum
//
// Description:
//    This function calculates the 16-bit one's complement sum
//    of the supplied buffer (ICMP) header
//
USHORT checksum(USHORT *buffer, int size) 
{
    unsigned long cksum=0;

    while (size > 1) 
    {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size) 
    {
        cksum += *(UCHAR*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (USHORT)(~cksum);
}

//
// Function: DecodeIPOptions
//
// Description:
//    If the IP option header is present, find the IP options
//    within the IP header and print the record route option
//    values
//
void DecodeIPOptions(char *buf, int bytes)
{
    IpOptionHeader *ipopt = NULL;
    IN_ADDR         inaddr;
    int             i;
    HOSTENT        *host = NULL;

    ipopt = (IpOptionHeader *)(buf + 20);

    printf("RR:   ");
    for(i = 0; i < (ipopt->ptr / 4) - 1; i++)
    {
        inaddr.S_un.S_addr = ipopt->addr[i];
        if (i != 0)
            printf("      ");
        host = gethostbyaddr((char *)&inaddr.S_un.S_addr,
                    sizeof(inaddr.S_un.S_addr), AF_INET);
        if (host)
            printf("(%-15s) %s\n", inet_ntoa(inaddr), host->h_name);
        else
            printf("(%-15s)\n", inet_ntoa(inaddr));
    }
    return;
}

//
// Function: DecodeICMPHeader
//
// Description:
//    The response is an IP packet. We must decode the IP header to
//    locate the ICMP data.
//
void DecodeICMPHeader(char *buf, int bytes, 
    struct sockaddr_in *from)
{
    IpHeader       *iphdr = NULL;
    IcmpHeader     *icmphdr = NULL;
    unsigned short  iphdrlen;
    DWORD           tick;
    static   int    icmpcount = 0;
    OnLineHost oneHost;
    iphdr = (IpHeader *)buf;
	// Number of 32-bit words * 4 = bytes
    iphdrlen = iphdr->h_len * 4;
    tick = GetTickCount();

    if ((iphdrlen == MAX_IP_HDR_SIZE) && (!icmpcount))
        DecodeIPOptions(buf, bytes);

    if (bytes  < iphdrlen + ICMP_MIN) 
    {
        printf("Too few bytes from %s\n", 
            inet_ntoa(from->sin_addr));
    }
    icmphdr = (IcmpHeader*)(buf + iphdrlen);

    if (icmphdr->i_type != ICMP_ECHOREPLY) 
    {
        printf("nonecho type %d recvd\n", icmphdr->i_type);
        return;
    }
    // Make sure this is an ICMP reply to something we sent!
    //
    if (icmphdr->i_id != (USHORT)GetCurrentProcessId()) 
    {
        printf("someone else's packet!\n");
        return ;
    }
    printf("%d bytes from %s:", bytes, inet_ntoa(from->sin_addr));
    printf(" icmp_seq = %d. ", icmphdr->i_seq);
    printf(" time: %d ms", tick - icmphdr->timestamp);
    printf("\n");
    strcpy( oneHost.ip, inet_ntoa(from->sin_addr) );
	oneHost.icmp_seq = icmphdr->i_seq;
	oneHost.time = tick - icmphdr->timestamp;
	hostList.push_back(oneHost);
	printf( "hostList size is %d\n", hostList.size() );
	for( vector<OnLineHost>::size_type ix = 0 ; ix != hostList.size(); ++ix )
    {
       printf( "%s %d %d\n", hostList[ix].ip, hostList[ix].icmp_seq, hostList[ix].time );
    }
    icmpcount++;
	eventUpdateView.SetEvent();
    return;
}

void ValidateArgs(int argc, char **argv)
{
    int                i;

    bRecordRoute = FALSE;
    lpdest = NULL;
    datasize = DEF_PACKET_SIZE;

	if( 3 == argc )
	{
	   lpdest = argv[1];
       startIp = argv[1];
	   endIp = argv[2];
    }
	else
    {
		
	}
}

unsigned long getHostOdNumByIpStr( char* ipStr )
{
   return ntohl ( inet_addr(ipStr) );
} 

int getIpStrByHostOdNum( unsigned long ipNum, char* ipStr )
{
  	struct in_addr ipInAddr;
	unsigned long ipNNum;
	memset(&ipInAddr, 0, sizeof(struct in_addr));
	ipNNum = htonl(ipNum);
	memcpy( &ipInAddr, &ipNNum, 4 );
	strcpy( ipStr, inet_ntoa(ipInAddr) );
	return 1;
}


//
// Function: main
//
// Description:
//    Setup the ICMP raw socket, and create the ICMP header. Add
//    the appropriate IP option header, and start sending ICMP
//    echo requests to the endpoint. For each send and receive,
//    we set a timeout value so that we don't wait forever for a 
//    response in case the endpoint is not responding. When we
//    receive a packet decode it.
//
int demo_main(int argc, char **argv)
{
    WSADATA            wsaData;
    SOCKET             sockRaw = INVALID_SOCKET;
    struct sockaddr_in dest,
                       from;
    int                bread,
                       fromlen = sizeof(from),
                       timeout = 300,
                       ret;
    char              *icmp_data = NULL,
                      *recvbuf = NULL;
    unsigned int       addr = 0;
    USHORT             seq_no = 0;
    struct hostent    *hp = NULL;
    IpOptionHeader     ipopt;
	unsigned long startIpNum = 0;
	unsigned long endIpNum = 0;
	unsigned long i = 0;
	char destIp[18];
	int nCount = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup() failed: %d\n", GetLastError());
        return -1;
    }
    ValidateArgs(argc, argv);


	startIpNum = getHostOdNumByIpStr(startIp);
	endIpNum = getHostOdNumByIpStr(endIp);
	printf( "startIpNum %u endIp %u\n", startIpNum, endIpNum );
	printf( "startIP is %s\n", startIp );
	printf( "endIP is %s\n", endIp );
	
    sockRaw = WSASocket (AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,
                         WSA_FLAG_OVERLAPPED);
    if (sockRaw == INVALID_SOCKET) 
    {
        printf("WSASocket() failed: %d\n", WSAGetLastError());
        return -1;
    }
    
    // Set the send/recv timeout values
    //
    bread = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, 
                (char*)&timeout, sizeof(timeout));
    if(bread == SOCKET_ERROR) 
    {
        printf("setsockopt(SO_RCVTIMEO) failed: %d\n", 
            WSAGetLastError());
        return -1;
    }
    timeout = 300;
    bread = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, 
                (char*)&timeout, sizeof(timeout));
    if (bread == SOCKET_ERROR) 
    {
        printf("setsockopt(SO_SNDTIMEO) failed: %d\n", 
            WSAGetLastError());
        return -1;
    }
    

    // 
    // Create the ICMP packet
    //       
    datasize += sizeof(IcmpHeader);  

    icmp_data = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                  MAX_PACKET);
    recvbuf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                  MAX_PACKET);
    if (!icmp_data) 
    {
        printf("HeapAlloc() failed: %d\n", GetLastError());
        return -1;
    }
    memset(icmp_data,0,MAX_PACKET);
    FillICMPData(icmp_data,datasize);
    //
    // Start sending/receiving ICMP packets
    //
    for( i = startIpNum; i < endIpNum + 1; i++ )
    {
		if( i == endIpNum )
		{
			finishFlag = SCAN_LAN_HOST_FINISH;
			eventUpdateView.SetEvent();
		}
        nCount = 0;
        memset(&dest, 0, sizeof(dest));
		getIpStrByHostOdNum(i,destIp);
		printf( "i %u destIp1 %s\n", i, destIp);

        //
        // Resolve the endpoint's name if necessary
        //
        dest.sin_family = AF_INET;
        if ((dest.sin_addr.s_addr = inet_addr(destIp)) == INADDR_NONE)
        {
            if ((hp = gethostbyname(destIp)) != NULL)
            {
                memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
                dest.sin_family = hp->h_addrtype;
                printf("dest.sin_addr = %s\n", inet_ntoa(dest.sin_addr));
            }
            else
            {
                printf("gethostbyname() failed: %d\n", 
                    WSAGetLastError());
                return -1;
            }
        } 
		
		while(1)
	    {
           
           int        bwrote;
           printf( "nCount %d", nCount );        
           if (nCount++ == MAX_TRY_PING_TIMES) 
               break;
                   
           ((IcmpHeader*)icmp_data)->i_cksum = 0;
           ((IcmpHeader*)icmp_data)->timestamp = GetTickCount();
           ((IcmpHeader*)icmp_data)->i_seq = seq_no++;
           ((IcmpHeader*)icmp_data)->i_cksum = 
               checksum((USHORT*)icmp_data, datasize);
   
           bwrote = sendto(sockRaw, icmp_data, datasize, 0, 
                        (struct sockaddr*)&dest, sizeof(dest));
           if (bwrote == SOCKET_ERROR)
           {
               if (WSAGetLastError() == WSAETIMEDOUT) 
               {
                   printf("timed out\n");
                   continue;
               }
               printf("sendto() failed: %d\n", WSAGetLastError());
               return -1;
           }
           if (bwrote < datasize) 
           {
               printf("Wrote %d bytes\n", bwrote);
           }
           bread = recvfrom(sockRaw, recvbuf, MAX_PACKET, 0, 
                       (struct sockaddr*)&from, &fromlen);
           if (bread == SOCKET_ERROR)
           {
               if (WSAGetLastError() == WSAETIMEDOUT) 
               {
                   printf("timed out\n");
                   continue;
               }
               printf("recvfrom() failed: %d\n", WSAGetLastError());
               return -1;
           }
           DecodeICMPHeader(recvbuf, bread, &from);
   
           Sleep(10);
	    }
    }
    // Cleanup
    //
    if (sockRaw != INVALID_SOCKET) 
        closesocket(sockRaw);
    HeapFree(GetProcessHeap(), 0, recvbuf);
    HeapFree(GetProcessHeap(), 0, icmp_data);

    WSACleanup();
    return 0;
}
