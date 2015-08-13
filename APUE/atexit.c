 #include <stdlib.h>
#include <signal.h>

int uninit_flag = 0;

void func ( void )
{
    if ( 0 == uninit_flag )
    {
        printf ( "uninit some libs\n" );
        uninit_flag = 1;
    }
}


int test()
{
    signal ( SIGTERM, func );
    atexit ( func );
    sleep ( 100 );
    return 0;
}

int main()
{
    test();
}
