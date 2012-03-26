#include "ehTimer.h"

int sendSynTime ( void* arg1, void* arg2 )
{
	printf( "send syn time\n" );
	return 0;
}

int main( int argc, char* argv[] )
{
	struct timeval tmv;
    tmv.tv_sec = 3;
    tmv.tv_usec = 0;
    int timerID = installTimer ( sendSynTime , tmv, PERIOD );
    if ( TCMRET_SUCCESS != startTimer ( timerID ) )
    {
        tcmLog_notice ( "Start timer id %d error", timerID );
    }
	
	sleep(10);
	stopTimer( timerID );
	
	while(1)
	{
		sleep(1);
	}
}
