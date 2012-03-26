#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#if 0
struct in_addr{
	in_addr_t s_addr; /*32-bit IPV4 address , network byte ordered*/
};

struct sockaddr_in{
	uint8_t sin_len; /*length of structrue(16)*/
	sa_family_t sin_family; /*AF_INET*/
	in_port_t sin_port; /*16-bit TCP or UDP port number, network byte ordered*/
	struct in_addr sin_addr; /*32-bit IPV4 address, network byte ordered*/
	char sin_zero[8]; /*unused*/
};

struct sockaddr{
	uint8_t sa_len;
	sa_family_t sa_family; /*address family:AF_XXX value*/
	char sa_data[14]; /*protocol-specific address*/
};
#endif
/*********************
Definition:int inet_aton(const char *cp, struct in_addr *inp);
Function Description:135.16.1.122 -----> int 
**********************/
#define MAX_IP_V4_LENGTH 16
void test_inet_aton()
{
   char ipstr[MAX_IP_V4_LENGTH];
   struct in_addr inaddr;
   int ret = 0;
   
   memset( ipstr, 0, sizeof(ipstr) );
   memset( &inaddr, 0, sizeof(inaddr) );

   strcpy( ipstr, "135.16.1.122" );
   ret = inet_aton( ipstr, &inaddr );

   if( ret == 0 )
   {
      printf( "ip address %s is valid\n", ipstr );
   }

   if( ret == 1 )
   {
      printf( "[%d][%d][%d][%d]\n",*( (char *)&(inaddr.s_addr) + 0),*( (char *)&(inaddr.s_addr)+1), *( (char *)&(inaddr.s_addr)+2), *( (char *)&(inaddr.s_addr)+3) );
   }
}


/*********************
Definition:in_addr_t inet_addr(const char *cp);
Function Description:135.16.1.122 -----> int 
**********************/
void test_inet_addr()
{
   char ipstr[MAX_IP_V4_LENGTH]; 
   in_addr_t addrt;

   memset( ipstr, 0, sizeof( ipstr ) );
   memset( &addrt, 0, sizeof( addrt ) );

   strcpy( ipstr, "145.11.1.a22" );
   addrt = inet_addr( ipstr );

   if( addrt == INADDR_NONE )
   {
      printf( "ip address %s is valid\n", ipstr );
   }
   else
   {
      printf( "[%d][%d][%d][%d]\n",*( (char *)&addrt + 0),*( (char *)&addrt+1), *( (char *)&addrt+2), *( (char *)&addrt+3) );      
   }
}

/*********************
Definition:char *inet_ntoa(struct in_addr in);
Function Description:int ----->172.16.12.40
**********************/
void test_inet_ntoa()
{
   struct in_addr inaddr;
   char* ipstr = NULL;
  
   inaddr.s_addr  = inet_addr("192.168.0.10");

   ipstr = inet_ntoa( inaddr );

   printf( "ipstr is %s\n", ipstr );
   
}

/*********************
Definition:int inet_pton(int af, const char *src, void *dst);
Function Description:172.16.12.40----->int
**********************/
void test_inet_pton()
{
   char ipstr[MAX_IP_V4_LENGTH];
   in_addr_t addrt;
   int ret = 0;
	
   memset( ipstr, 0, sizeof(ipstr) );
   memset( &addrt, 0, sizeof( addrt ) );
   
   strcpy( ipstr, "123.16.1.119" );

   ret = inet_pton( AF_INET, ipstr, (void*)&addrt );

   if( 1 != ret )
   {
      printf( "ipstr %s is valid\n", ipstr );
   }

   if( ret == 1 )
   {
      printf( "[%d][%d][%d][%d]\n",*( (char *)&addrt + 0),*( (char *)&addrt+1), *( (char *)&addrt+2), *( (char *)&addrt+3) );             
   }

   
}

/*********************
Definition:const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
Function Description: int -----> 172.16.12.40
cnt is the size of the buffer dst in case that this function over the caller's buffer, because function just receive one char pointer
**********************/
void test_inet_ntop()
{
  struct in_addr inaddr;
   char ipstr[MAX_IP_V4_LENGTH];
   char* result = NULL;

   memset( ipstr, 0, sizeof(ipstr) );
   
   inaddr.s_addr = inet_addr("172.16.1.120");

   result = inet_ntop( AF_INET, (void*)(&inaddr), ipstr, sizeof(ipstr) );

   if( NULL != result )
  {
     printf( "ipstr is [%s], len is [%d]\n", ipstr, sizeof(ipstr) );
  }
  else
  {
     printf( "inet_ntop is error\n" );
  } 
   
}

int main()
{
   test_inet_aton();
   test_inet_addr();
   test_inet_ntoa();
   test_inet_pton();
   test_inet_ntop();
}
