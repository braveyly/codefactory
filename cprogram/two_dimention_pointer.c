#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_two_pointers ( char** eachValue )
{
    strcpy ( eachValue[0], "test 0000000" );
    strcpy ( eachValue[1], "test 1111111" );
    strcpy ( eachValue[2], "test 2222222" );
    strcpy ( eachValue[3], "test 3333333" );
    strcpy ( eachValue[4], "test 4444444" );
    strcpy ( eachValue[5], "test 5555555" );
    strcpy ( eachValue[6], "test 6666666" );
}
int main ( int argc, char *argv[] )
{
    /****test case 1: *********/
    char** p;
    p = argv;
    int n = 8;
    while ( n > 0 )
    {
        printf ( " while argv *p =%s \t *p = %d \n ", *p, *p );
        p++;
        n--;
    }
    printf ( "\n***********************************\n" );

    /*****test case 2:**********/
    char* parray[4] = {"aaa1111", "bbbbb", "cwecc", "daadd"};
    p = parray;
    printf ( " before p = %s \n", p );
    printf ( "p[0]=%s\tp[1]=%s\tp[2]=%s\n", p[0], p[1], p[2] );
    printf ( "strlen( p ) = %d\n", strlen ( *p ) );
    p++;
    printf ( "strlen( p ) =%d \n", strlen ( *p ) );
    printf ( "\n***********************************\n" );

    /******test case 3:**********/
    p = parray;
    while ( *p != 0 )  //if parray is defined to have more than 4 elements,this end condition will work
    {
        printf ( " while parray *p =%s \t *p = %d \n ", *p, *p );
        p++;
    }
    printf ( "\n***********************************\n" );

    /*******test case 4:right usage*********/
    char value[7][100];
    char* pResult[7];
    int i;
    for ( i = 0; i < 7; i++ )
    {
        printf ( "sizeof(value) is %d\n", sizeof ( value[i] ) );
        memset ( value[i], 0, sizeof ( value[i] ) );
        pResult[i] = value[i];
    }
    test_two_pointers ( pResult );
    for ( i = 0; i < 7;i++ )
    {
        printf ( "show pResult[%d] %s\n", i, pResult[i] );
    }
    printf ( "\n***********************************\n" );
    /*********test case 5:wrong usage***********/
    test_two_pointers ( value );//segment error
    for ( i = 0; i < 7;i++ )
    {
        printf ( "show pResult[%d] %s\n", i, pResult[i] );
    }
    printf ( "\n***********************************\n" );

    return 0;
}

/****************output******************
while argv *p =./two   *p = -1076929652
while argv *p =(null)   *p = 0
while argv *p =HOSTNAME=d01.tecomtech.com   *p = -1076929646
while argv *p =TERM=xterm   *p = -1076929619
while argv *p =SHELL=/bin/bash   *p = -1076929608
while argv *p =HISTSIZE=1000   *p = -1076929592
while argv *p =KDE_NO_IPV6=1   *p = -1076929578
while argv *p =SSH_CLIENT=172.16.1.122 4065 22   *p = -1076929564

***********************************
before p = u.}...?Ͽ
p[0]=aaa1111 p[1]=bbbbb p[2]=cwecc
strlen( p ) = 7
strlen( p ) =5

***********************************
while parray *p =aaa1111   *p = 134514805
while parray *p =bbbbb   *p = 134514813
while parray *p =cwecc   *p = 134514819
while parray *p =daadd   *p = 134514825
while parray *p =?Ͽ   *p = -1076932940

***********************************
sizeof(value) is 100
sizeof(value) is 100
sizeof(value) is 100
sizeof(value) is 100
sizeof(value) is 100
sizeof(value) is 100
sizeof(value) is 100
show pResult[0] test 0000000
show pResult[1] test 1111111
show pResult[2] test 2222222
show pResult[3] test 3333333
show pResult[4] test 4444444
show pResult[5] test 5555555
show pResult[6] test 6666666

***********************************
Segmentation fault
*********************end output**************/

