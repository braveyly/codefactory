/*v1.0
*basic version
*by braveyly 
*2012-8-10
*/
#include <stdio.h>
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

//Function parameter input and output
#define IN
#define OUT

//String macro definition
#define STR_HTTP_CONTENT_TYPE "Content-Type: "
#define STR_HTTP_CONTENT_LENGTH "Content-Length: "
#define STR_HTTP_IMG_START "<img"
#define STR_HTTP_IMG_SRC_START "src=\""
#define STR_HTTP_URL_START "http://"
#define STR_HTTP_DEFAULT_PORT "80"
#define STR_HTTP_RESPONSE_HEAD "HTTP/1.1 "
#define STR_HTTP_RESPONSE_200_OK "200"
#define STR_HTTP_RESPONSE_301_MOVED "301"
#define STR_HTTP_RESPONSE_301_LOCATION "Location: "

//Symbol macro definition
#define HTTP_NEW_LINE "\r\n"
#define HTTP_DOUBLE_NEW_LINE "\r\n\r\n"
#define HTTP_IMG_SRC_END "\""

//Length macro definition
#define LENGTH_IP 16
#define LENGTH_HOST_NAME 200
#define LENGTH_RESOURCE 1000
#define LENGTH_URL LENGTH_RESOURCE + LENGTH_HOST_NAME
#define LENGTH_PORT 6
#define LENGTH_TYPE 20
#define LENGTH_LEN 10
#define LENGTH_ONCE_READ 204800
#define LENGTH_HEAD_WRITE 1024
#define LENGTH_FILE_NAME 200
#define LENGTH_HTTP_CODE 3

int to_null_terminating_string( char* str )
{
	if( '\n' == *(str + strlen(str) -1 ) )
	{
		*(str + strlen(str) -1) = 0;
		if( '\r' == *(str + strlen(str) - 1) )
		{
			*(str + strlen(str) - 1) = 0;
		}
		
		return 0;
	}
	else
		return -1;
}

int to_new_line_string( char* str )
{
	if( '\n' == *(str + strlen(str) -1 ) && '\r' == *(str + strlen(str) - 2) )
	{
		return 1;
	}
	else if( '\n' == *(str + strlen(str) -1 ) && '\r' != *(str + strlen(str) - 2) )
	{
		*(str + strlen(str) -1 ) = '\r';
		*(str + strlen(str) -2 ) = '\n';
	}
	else
	{
		strcat( str, "\r\n" );
	}
	
	return 0;
}

int  sock_create_tcp_socket(SOCKET* sock)
{
	SOCKET	sSocket = INVALID_SOCKET;
	int ret = 0;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( INVALID_SOCKET == sSocket )
	{
		printf( "socket create failed\n" );
		printf( "WSAGetLastError():[%d]\n", WSAGetLastError() );
		ret = -1;
	}
	else
	{
		*sock = sSocket;
	}
	return ret;
}

int sock_connect( const SOCKET sock_fd, const char* ip, const int port )
{
	int nRet = 0;
	SOCKADDR_IN stSvrAddrIn = {0}; 
	stSvrAddrIn.sin_family      = AF_INET;
	stSvrAddrIn.sin_port        = htons(port); 
	stSvrAddrIn.sin_addr.s_addr = inet_addr(ip);
	nRet = connect(sock_fd, (SOCKADDR*)&stSvrAddrIn, sizeof(SOCKADDR));
	if( SOCKET_ERROR == nRet )
	{
		printf( "socket connect to [%s:%d] failed\n", ip, port );
		printf( "WSAGetLastError():[%d]\n", WSAGetLastError() );
		nRet = -1;
	}
	return nRet;
}

void sock_error_str( int errno )
{
	if( WSANOTINITIALISED == errno )
	{
		printf( "errno WSANOTINITIALISED:A successful WSAStartup call must occur before using this function.\n" );
	}
	else if( WSAENETDOWN == errno )
	{
		printf( "errno WSANOTINITIALISED:The network subsystem has failed.\n" );
	}
	else if( WSAHOST_NOT_FOUND == errno )
	{
		printf( "errno WSANOTINITIALISED:Authoritative answer host not found.\n" );
	}
	else if( WSATRY_AGAIN == errno )
	{
		printf( "errno WSANOTINITIALISED:Nonauthoritative host not found, or server failure.\n" );
	}
	else if( WSANO_RECOVERY == errno )
	{
		printf( "errno WSANOTINITIALISED:A nonrecoverable error occurred.\n" );
	}
	else if( WSANO_DATA == errno )
	{
		printf( "errno WSANO_DATA:Valid name, no data record of requested type.n" );
	}
	else if( WSAEINPROGRESS == errno )
	{
		printf( "errno WSAEINPROGRESS:A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n" );
	}
	else if( WSAEFAULT == errno )
	{
		printf( "errno WSAEFAULT:The name parameter is not a valid part of the user address space.\n" );
	}
	else if( WSAEINTR == errno )
	{
		printf( "errno WSAEINTR:A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n" );
	}
	else
	{
		printf( "Other errno\n" );
	}
}

int sock_get_ip_by_host( const char* host_name, char* ip )
{
	HOSTENT *host_entry = 0;
	int WSA_return;
	WSADATA WSAData;
	
	WSA_return=WSAStartup(0x0101,&WSAData);
	host_entry=gethostbyname(host_name);
	if(0==WSA_return)
	{
		if(0!=host_entry)
		{
			sprintf(ip, "%d.%d.%d.%d",
			(host_entry->h_addr_list[0][0]&0x00ff),
			(host_entry->h_addr_list[0][1]&0x00ff),
			(host_entry->h_addr_list[0][2]&0x00ff),
			(host_entry->h_addr_list[0][3]&0x00ff));
			printf("Host:[%s]  host length:[%d]  ip:[%s]\n", host_name, strlen(host_name), ip);
		}
		else
		{
			printf( "gethostbyname failed for Host:[%s]  host length:[%d]\n", host_name, strlen(host_name) );
			printf( "WSAGetLastError():[%d]\n", WSAGetLastError() );
			sock_error_str( WSAGetLastError() );
			return -1;
		}
	}
	else
	{
		printf( "WSAStartup failed\n" );
		printf( "WSAGetLastError():[%d]\n", WSAGetLastError() );		
		sock_error_str( WSAGetLastError() );
		return -1;
	}

	return 0;
}

int sock_send_buf( const SOCKET sock_fd, const char* snd_buf, const int  snd_length )
{
	int nRet = 0;
	int total = 0;
	
	while( total < snd_length )
	{
		nRet = send( sock_fd, snd_buf + total, snd_length - total, 0 );
		if( SOCKET_ERROR == nRet )
		{
			printf( "socket [%d] send error\n", sock_fd );
			printf( "WSAGetLastError():[%d]\n", WSAGetLastError() );	
			nRet = -1;
			break;
		}
		else
		{
			total += nRet;
			printf( "Send [%d] bytes this time total [%d] bytes left [%d] bytes\n", nRet, total, snd_length - total );
		}	
	}
	
	if( total == snd_length )
		nRet = total;
	
	return nRet;
}



int sock_rcv_buf( const SOCKET sock_fd,  char* buf, int length )
{
	int nRet = recv( sock_fd, buf, length, 0 );
	if( 0 == nRet )
	{
		printf( "Peer socket is closed while receiving\n" );
		printf( "WSAGetLastError():[%d]\n", WSAGetLastError() );	
		nRet = -1;
	}
	else if( SOCKET_ERROR == nRet )
	{
		printf( "Socket received error\n" );
		printf( "WSAGetLastError():[%d]\n", WSAGetLastError() );	
		nRet = -2;
	}
	else
	{
		printf( "Socket receive [%d] bytes expected for [%d] bytes\n", nRet , length  );
	}
	return nRet;
}

int http_parse_url_simple( IN const char* url, OUT char* host_name, OUT char* file_name )
{
	char ip[LENGTH_IP] = {0};
	int port  = 0;
	char resource[LENGTH_RESOURCE] = {0};
	char* file_name_start = 0;
	
	 file_name_start = url;
	 while(1)
	 {
		file_name_start = strstr( file_name_start, "/" );
		if( 0 == strstr( file_name_start + 1, "/" ) )
			break;
		file_name_start += 1;
	 }
	 strcpy( file_name, file_name_start + 1 );
	 http_parse_url(url,host_name,ip,&port,resource);
	 return 0;
}

int http_parse_url( IN const char* url, OUT char* host_name, OUT char* ip, OUT int* port, OUT char* resource)
{
	char portStr[LENGTH_PORT] = {0};
	char* hostStart = 0;
	char* hostEnd = 0;
	char* portStart = 0;
	char* portEnd = 0;
	char* resourceStart = 0;
	char* resourceEnd = 0;
	
	/*1 step: get host name*/
	if( 0 == memcmp( url, STR_HTTP_URL_START, strlen(STR_HTTP_URL_START) ) )
	{
		hostStart = url + strlen(STR_HTTP_URL_START);
	}
	else
	{
		hostStart = url;
	}

	hostEnd = strstr( hostStart, "/" );
	if( 0 != hostEnd )
	{
		memcpy( host_name, hostStart, hostEnd - hostStart );
	}
	else
	{
		strcpy( host_name, hostStart );
	}
	
	/*2 step: get resource*/
	strcpy( resource, hostEnd );

	/*3 step: get port*/
	portStart = strstr( host_name, ":" );
	if( 0 != portStart )
	{
		portStart += strlen(":");
		strcpy( portStr, portStart );
		host_name[portStart-host_name-1] = '\0';
	}
	else
	{
		strcpy( portStr, STR_HTTP_DEFAULT_PORT );
	}
	*port = atoi(portStr);

	/*4 step: get ip*/
	if( 0 != sock_get_ip_by_host(host_name,ip) )
	{
		return -1;
	}

	return 0;
}

int http_parse_response( IN const char* response, OUT char* type, OUT int* length, OUT char* url )
{
	  char* typeStart = 0;
	  char* typeEnd = 0;
	  char* lenStart = 0;
	  char* lenEnd = 0;
	  char lenStr[LENGTH_LEN] = {0};
	  char response_code[4] = {0};
	  char* locationStart = 0;
	  char* locationEnd = 0;

	  strncpy( response_code, response + strlen(STR_HTTP_RESPONSE_HEAD),  LENGTH_HTTP_CODE  );
	  if( strcmp( response_code, STR_HTTP_RESPONSE_200_OK )  )
	  {
		  printf("http response is not 200 OK\n");
		  //printf( "%c %c %c %c %c %c %c %c %c %c\n", response[0], response[1], response[2], response[3], response[4] ,response[5], response[6], response[7], response[8], response[9] );
		  printf( "%s\n", response );
	 	  if( 0 == strcmp( response_code, STR_HTTP_RESPONSE_301_MOVED )  )
	 	  {
	 	      printf( "reponse code is 301\n" );
		      locationStart = strstr( response, STR_HTTP_RESPONSE_301_LOCATION );
		      if( !locationStart )
		      	{
		      		return -1;
		      	}
			locationEnd = strstr( locationStart, HTTP_DOUBLE_NEW_LINE );
			memset( url, 0, LENGTH_URL );
			strncpy( url, locationStart + strlen(STR_HTTP_RESPONSE_301_LOCATION), locationEnd - locationStart - strlen(STR_HTTP_RESPONSE_301_LOCATION));
			to_null_terminating_string(url);
			printf( " new url [%s] length [%d]\n", url ,strlen(url) );
			return -2;
	 	  }
		  return -1;
	  }

	  typeStart =  strstr(response,STR_HTTP_CONTENT_TYPE);
	  lenStart  = strstr(response,STR_HTTP_CONTENT_LENGTH);
	  if( !typeStart || !lenStart )
	  {
	  	  printf( "cantnot find %s or %s\n", STR_HTTP_CONTENT_TYPE,  STR_HTTP_CONTENT_LENGTH );
		  return -1;
	  }
	 
	  typeEnd = strstr(typeStart,HTTP_NEW_LINE);
	  lenEnd = strstr(lenStart,HTTP_NEW_LINE);	  
	  if( !typeEnd || !lenEnd )
	  {
	  	 printf( "cannot find the end of %s or %s\n",  STR_HTTP_CONTENT_TYPE,  STR_HTTP_CONTENT_LENGTH );
		  return -1;
	  }
	  strncpy(lenStr,lenStart+strlen(STR_HTTP_CONTENT_LENGTH),lenEnd-lenStart-strlen(STR_HTTP_CONTENT_LENGTH));
	  *length = atoi(lenStr);

	  typeStart = strchr( typeStart,'/');
	  strncpy(type,typeStart+1,typeEnd-typeStart-1);
	  type[typeEnd-typeStart-1] = '\0';

	  printf( "Get type %s length [%d] from http response\n", type, *length );
	  return 0;
}

int produce_task_from_buffer( const char* buf, const int len, const char* host_name, const FILE* stream )
{
	char* bufTmp = 0;
	char* imgStart = 0;
	char* srcStart = 0;
	char* srcEnd = 0;
	char imgUrl[LENGTH_URL] = {0};
	
	bufTmp = buf;
	while(1)
	{
		memset( imgUrl, 0, sizeof(imgUrl) );
		imgStart = strstr( bufTmp, STR_HTTP_IMG_START );
		if( 0 == imgStart )
			return -1;
		srcStart = strstr( imgStart, STR_HTTP_IMG_SRC_START );
		if( 0 == srcStart )
			return -1;
		srcEnd   = strstr( srcStart + strlen(STR_HTTP_IMG_SRC_START), HTTP_IMG_SRC_END );
		if( 0 == srcEnd )
			return -1;

		if( '/' == *(srcStart + strlen(STR_HTTP_IMG_SRC_START)) )
		{
			strncpy( imgUrl, host_name, strlen(host_name) );
			strncpy( imgUrl + strlen(host_name)  , srcStart + strlen(STR_HTTP_IMG_SRC_START) , srcEnd - srcStart - strlen(STR_HTTP_IMG_SRC_START)  );
		}
		else
		{
			strncpy( imgUrl, srcStart + strlen(STR_HTTP_IMG_SRC_START) , srcEnd - srcStart - strlen(STR_HTTP_IMG_SRC_START)  );			
		}

		printf( "imgUrl is %s\n", imgUrl );
		to_new_line_string(imgUrl);
		fwrite( imgUrl, sizeof(char), strlen(imgUrl),  stream );
		fflush(stream);
		bufTmp = srcEnd;
	}

   	 return 0;
}

int produce_task_from_file( const char* htmlFile,const char* host_name, const FILE* taskStream )
{
	FILE* stream = fopen( htmlFile, "rb" );
	char rdBuf[LENGTH_ONCE_READ] = {0};
	int rdLen = 0;

	if( 0 == stream || 0 == taskStream )
		return -1;

	memset( rdBuf, 0, sizeof(rdBuf) );
	rdLen = fread( rdBuf, sizeof(char), sizeof(rdBuf), stream );
	produce_task_from_buffer( rdBuf, rdLen, host_name, taskStream );
		
	return 0;
}


int run_task(const char* taskFile)
{
	FILE* stream;
	char lineStr[LENGTH_URL] = {0};
	int len = LENGTH_URL;
	
	stream = fopen( taskFile, "rb" );
	while( fgets( lineStr, len, stream ) )
	{
		to_null_terminating_string(lineStr);
		printf( "lineStr = %s\n", lineStr );
		download_file(lineStr);
		memset( lineStr, 0, sizeof(lineStr) );
	}

	return 0;
}


int download_file(char* url)
{
	char host_name[LENGTH_HOST_NAME] = {0};
	char ip[LENGTH_IP] = {0};
	int port = 0;
	char resource[LENGTH_RESOURCE] = {0};
	int nRet = 0;
	do
	{
		http_parse_url( url, host_name, ip , &port, resource );
		nRet = download( host_name, ip, port, resource, url );
		printf( " nRet = [%d] url [%s] strlen(url) [%d]\n", nRet, url, strlen(url) );
	}
	while( -2 == nRet );
	return 0;
}



int download(  char* host_name, char* ip, int port, char* resource, char* url )
{
       SOCKET      sSocket      = INVALID_SOCKET;
       char        sndBuf[LENGTH_HEAD_WRITE] = {0};
       char        rcvBuf[LENGTH_ONCE_READ] = {0};
       char       *pRcv         = rcvBuf;
       int         num          = 0;
       int         nRet         = -1;
	int len = 0;
	char type[LENGTH_TYPE] = {0};
	char fileName[LENGTH_FILE_NAME] = {0};
	char *fileNameStart = 0;
	char* writeFileBuf = 0;
	int writeFileLen = 0;
	int parseHeadFlag = 0;
	int totalLen = 0;
	FILE *getFile = 0;
	int curLen = 0;
		
	fileNameStart = resource;
	while(1)
	{
		fileNameStart = strstr( fileNameStart, "/" );
		if( 0 == strstr( fileNameStart + 1, "/" ) )
			break;
		fileNameStart += 1;
	}
	strcpy( fileName, fileNameStart + 1 );
	sprintf( sndBuf, "GET %s  HTTP/1.1\nHost: %s\n\r\n", resource, host_name );

	 if( sock_create_tcp_socket(&sSocket) )
	 {
	 	return -1;
	 }
	  
	if ( sock_connect( sSocket, ip, port ) )
	{
		return -1;
	}

	nRet = sock_send_buf( sSocket, sndBuf, strlen(sndBuf) );
	if( -1 == nRet )
	{
		return -1;
	}
	
	while(1)
	{
		memset( pRcv, 0, LENGTH_ONCE_READ) ;
		num = sock_rcv_buf( sSocket, pRcv, LENGTH_ONCE_READ);
		if( num <= 0 )
		{
			break ;
		}
		writeFileBuf = pRcv;
		writeFileLen = num;
		
		if( 0 == parseHeadFlag )
		{
			nRet = http_parse_response(pRcv,type,&len, url);
		 	if( 0 ==  nRet )
		 	{
		 		printf( "type is %s\n", type );
		 		writeFileBuf = strstr(pRcv,HTTP_DOUBLE_NEW_LINE) + strlen(HTTP_DOUBLE_NEW_LINE);
		 		writeFileLen = num - ( strstr(pRcv,HTTP_DOUBLE_NEW_LINE) - pRcv + strlen(HTTP_DOUBLE_NEW_LINE) );
		 		parseHeadFlag = 1;
		 		if( 0 == getFile )
		 			getFile = fopen( fileName, "wb" );
		 	}
		 	else if(  nRet )
		 	{
				break;
		 	}
		}

		totalLen += writeFileLen;
		fwrite( writeFileBuf, sizeof(char), writeFileLen, getFile );
		if( totalLen >= len )
			break;
	}
       if( getFile )
		fclose(getFile);

	return nRet;
}
/*
Three commands
1、exe -s url
2、exe -m url1 url2 url3
3、exe -r 211 344 http://www.huawei.com/index*.html
*/
int main(int argc,char* args[])
{
	FILE* taskStream;
	int urlNum = 0;
	int rangeStart = 0;
	int rangeEnd = 0;
	char url[LENGTH_URL] = {0};
	char urlTmp[LENGTH_URL] = {0};
	char* starStart = 0;
	char host_name[LENGTH_HOST_NAME] = {0};
	char file_name[LENGTH_FILE_NAME] = {0};
	int i = 0;

	if( argc < 3 )
	{
		printf( "arguments number is invalid\n" );
		return -1;
	}

	//1、create task file
	taskStream = fopen( "task.txt", "wb" );
	
	//2、produce tasks from cmd
	if( 0 == strcmp( args[1], "-s" ) )
	{
		printf( "download one single web picture\n" );
		http_parse_url_simple( args[2], host_name, file_name );
		download_file(args[2]);
		produce_task_from_file( file_name, host_name, taskStream );
	}
	else if( 0 == strcmp( args[1], "-m" ) )
	{
		printf( "download multiple webs picture\n" );
		for( i = 2; i < argc; i++  )
		{
			memset( host_name , 0, sizeof(host_name) );
			memset( file_name, 0, sizeof(host_name) );
			http_parse_url_simple( args[i], host_name, file_name );
			download_file(args[i]);
			produce_task_from_file( file_name, host_name, taskStream );
		}
	}
	else if( 0 == strcmp( args[1], "-r" ) )
	{
		printf( "download webs by regulare expression\n" );
		rangeStart = atoi( args[2] );
		rangeEnd   = atoi( args[3] );
		strcpy( url, args[4] );
		starStart = strstr( url, "*" );
		for( i = rangeStart ; i < rangeEnd; i++ )
		{
			memset( urlTmp, 0, sizeof(urlTmp) );
			strncpy( urlTmp, url, starStart - url );
			sprintf( urlTmp + (starStart - url), "%d", i );
			strcat( urlTmp, starStart + 1 );
			printf( "urlTmp is %s\n", urlTmp );
			memset( host_name , 0, sizeof(host_name) );
			memset( file_name, 0, sizeof(host_name) );
			http_parse_url_simple( urlTmp, host_name, file_name );
			download_file(urlTmp);
			produce_task_from_file( file_name, host_name, taskStream );
		}	
	}
	
	//3、execute tasks
	run_task("task.txt");
	
	return 0;
}

/*the oldest test case*/
int main1(int argc, char args[])
{

       SOCKET      sSocket      = INVALID_SOCKET;
       SOCKADDR_IN stSvrAddrIn = {0}; /* 服务器端地址 */
       char        sndBuf[1024] = {0};
       char        rcvBuf[2048] = {0};
       char       *pRcv         = rcvBuf;
       int         num          = 0;
       int         nRet         = SOCKET_ERROR;
	  int len = 0;
	  char type[20] = {0};
	  char* writeFileBuf = 0;
	  int writeFileLen = 0;
	   WSADATA     wsaData;
	   int parseHeadFlag = 0;
	   int totalLen = 0;
	   FILE *getFile = fopen("1.html", "wb");

       /* HTTP 消息构造开始，这是程序的关键之处 */
       //sprintf(sndBuf, "GET /ucmf/groups/public/documents/webasset/logo_cn.jpg HTTP/1.1\n");
       //strcat(sndBuf, "Host: www.huawei.com\n\r\n");
       sprintf(sndBuf, "GET /cn/about-huawei/corporate-info/vision-mission/index.htm HTTP/1.1\n");
       strcat(sndBuf, "Host: www.huawei.com\n\r\n");
       /* HTTP 消息构造结束 */

       /* socket DLL初始化 */
	   WSAStartup(MAKEWORD(2, 0), &wsaData);

       stSvrAddrIn.sin_family      = AF_INET;
		stSvrAddrIn.sin_port        = htons(80); 
       stSvrAddrIn.sin_addr.s_addr = inet_addr("10.82.49.59");

       sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

       /* 连接 */
       nRet = connect(sSocket, (SOCKADDR*)&stSvrAddrIn, sizeof(SOCKADDR));
       if (SOCKET_ERROR == nRet)
       {
              printf("connect fail!\n");
              return -1;
       }

       /* 发送HTTP请求消息 */
       send(sSocket, (char*)sndBuf, strlen(sndBuf), 0);
 
       while(1)
       {
			  memset( pRcv, 0, 2048) ;
              num = recv(sSocket, pRcv, 2048, 0);

			  if((0 == num) || (-1 == num))
              {
                     break ;
              }

			  writeFileBuf = pRcv;
			  writeFileLen = num;
			 if( 0 == parseHeadFlag && 0 == http_parse_response(pRcv,type,&len, 0) )
			 {
				 printf( "type is %s\n", type);
				 printf( "len is %d\n", len );
				 writeFileBuf = strstr(pRcv,HTTP_DOUBLE_NEW_LINE) + strlen(HTTP_DOUBLE_NEW_LINE);
				 writeFileLen = num - ( strstr(pRcv,HTTP_DOUBLE_NEW_LINE) - pRcv + strlen(HTTP_DOUBLE_NEW_LINE) );
				 parseHeadFlag = 1;
			 }

			  printf( "num is %d pRcv is \n\n%s", num, pRcv );
			  totalLen += writeFileLen;
              fwrite( writeFileBuf, sizeof(char), writeFileLen, getFile );
			  if( totalLen >= len )
				  break;
       }

	   fflush(getFile);
	   fclose(getFile);
       return 0;
}
