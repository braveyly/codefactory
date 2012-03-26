#include "StrategyA.h"
#include "StrategyB.h"
#include "Context.h"

int main()
{
	Context* pcontext = new Context();
	StrategyA* psa = new StrategyA();
	pcontext->set_strategy(psa);
	pcontext->context_algorithm_interface();
}
