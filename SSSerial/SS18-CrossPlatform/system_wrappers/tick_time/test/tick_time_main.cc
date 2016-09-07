#include <iostream>
#include "tick_util.h"

using namespace::std;

int main()
{
	int64_t  ticks = 0;

	int64_t msticks = 0;


	for (int i = 0; i < 5; i++)
	{
		ticks = cp::TickTime::Now().Ticks();

		msticks = cp::TickTime::MillisecondTimestamp();

		cout << "ticks = " << ticks << " msticks=" << msticks << endl;
	}

	while (1)
	{
	}
	return 0;
	
}