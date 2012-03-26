#include "SimpleAlgorithm.h"
#include "ComplicatedAlgorithm.h"

int main()
{
	SimpleAlgorithm* psimple = new SimpleAlgorithm();
	psimple->algorithm_operate();

	ComplicatedAlgorithm* pcomplicated = new ComplicatedAlgorithm();
	pcomplicated->algorithm_operate();
}
