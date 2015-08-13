#include <stdio.h>
#include <stdint.h>
#include <string.h>

void initProtocolMsgTwo( uint8_t** buf, uint16_t* totalLen)
{	
   printf( "before memset *buf\n" );
   memset( *buf, 0, 100 );
   printf( "after memset *buf\n" );
   *totalLen = 123;
   strcpy( *buf, "abcdefghi" );
}

void initProtocolMsgOne( uint8_t* buf, uint16_t* totalLen)
{	
   printf( "before memset *buf\n" );
   memset( buf, 0, 100 );
   printf( "after memset *buf\n" );
   *totalLen = 123;
   strcpy( buf, "abcdefghi" );
}


int main()
{
    uint8_t *msgbuf1 = ( uint8_t *) malloc(1000);
    uint16_t msglen1 = 0;
    memset( msgbuf1, 0, 1000 );
    initProtocolMsgTwo((char **)&msgbuf1,&msglen1);
    printf( "msglen1:[%d], msgbuf1:[%s]\n", msglen1, msgbuf1 );
    
    uint8_t msgbuf2[1000];
    uint16_t msglen2 = 0;
    memset( msgbuf2, 0, 1000 );
    initProtocolMsgTwo((char **)&msgbuf2,&msglen2);    
}
/*
before memset *buf
after memset *buf
msglen1:[123], msgbuf1:[abcdefghi]
before memset *buf
Segmentation fault
*/
/*
char array pointer !=  two dimentionary char pointer
*/
int main2()
{
    uint8_t *msgbuf1 = ( uint8_t *) malloc(1000);
    uint16_t msglen1 = 0;
    memset( msgbuf1, 0, 1000 );
    initProtocolMsgOne(msgbuf1,&msglen1);
    printf( "msglen1:[%d], msgbuf1:[%s]\n", msglen1, msgbuf1 );
    
    uint8_t msgbuf2[1000];
    uint16_t msglen2 = 0;
    memset( msgbuf2, 0, 1000 );
    initProtocolMsgOne(msgbuf2,&msglen2);  
    printf( "msglen2:[%d], msgbuf2:[%s]\n", msglen2, msgbuf2 );
}
/*
before memset *buf
after memset *buf
msglen1:[123], msgbuf1:[abcdefghi]
before memset *buf
after memset *buf
msglen2:[123], msgbuf2:[abcdefghi]
*/
/*
char array = one dimentionary pointer
*/
