#include <iostream>
#include "clock.h"

using namespace::std;


int main()
{
	int64_t mils = 0;
	int64_t mics = 0;
	int64_t ntp_mils = 0;
	
	while (1)
	{
	    mils = cp::Clock::GetRealTimeClock()->TimeInMilliseconds();
	    mics = cp::Clock::GetRealTimeClock()->TimeInMicroseconds();
	    ntp_mils = cp::Clock::GetRealTimeClock()->CurrentNtpInMilliseconds();
	    cout << "mils = " << mils << "mics = " << mics << "ntp_mils = " << ntp_mils << endl;
	}

	return 0;
}